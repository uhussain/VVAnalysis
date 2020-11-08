#!/usr/bin/env python
import ROOT
import glob
import math
#from python import ConfigureJobs
from python import UserInput
import makeSimpleHtml
import os
import subprocess
import sys
import datetime
import array
from ROOT import vector as Vec

VFloat = Vec('float')
#from PlotTools import PlotStyle as Style, pdfViaTex

#style = Style()
#ROOT.gStyle.SetLineScalePS(1.8)
ROOT.gStyle.SetOptDate(False)
#channels = ["eeee","eemm","mmmm"]
channels = []
def getComLineArgs():
    parser = UserInput.getDefaultParser()
    parser.add_argument("--lumi", "-l", type=float,
        default=41.5, help="luminosity value (in fb-1)")
    parser.add_argument('--thesis', action='store_true',
                        help='For making CMS thesis plots')
    parser.add_argument('--preliminary', action='store_true',
                        help='For making CMS Preliminary plots')
    parser.add_argument("--legend_left", action="store_true",
                        help="Put legend left or right")
    parser.add_argument("--titleOffset", type=float, default=1.0,
                        help="Scale default ymax by this amount")
    parser.add_argument("--scaleymax", type=float, default=1.0,
                        help="Scale default ymax by this amount")
    parser.add_argument("--scaleymin", type=float, default=1.0,
                        help="Scale default ymin by this amount")
    parser.add_argument("--scalelegy", type=float, default=1.0,
                        help="Scale default legend entry size by this amount")
    parser.add_argument("--scalelegx", type=float, default=1.0,
                        help="Scale default legend entry wdith by this amount")
    parser.add_argument("--output_file", "-o", type=str,
        default="", help="Output file name")
    parser.add_argument("--test", action='store_true',
        help="Run test job (no background estimate)")
    parser.add_argument("--variable", "-vr", type=str,
        default="all", help="variableName")
    parser.add_argument('--noNorm', action='store_true',
                        help='Leave differential cross sections in abolute normalization rather than normalizing to unity area.')
    parser.add_argument('--NormFb', action='store_true',
                        help='Normalize differential cross sections to the luminosity')
    parser.add_argument('--logy', '--logY', '--log', action='store_true',
                        help='Put vertical axis on a log scale.')
    parser.add_argument('--makeTotals', action='store_true',
                        help='plot total unfolded with uncertainities.')
    parser.add_argument('--unfoldDir', type=str, nargs='?',
                        default='/afs/cern.ch/user/u/uhussain/www/ZZFullRun2/PlottingResults/ZZ4l2018/ZZSelectionsTightLeps/ANPlots/FullRun2/DiffDistWMatrix_19Feb2020/',
                        help='Directory to put response and covariance plots in')
    return vars(parser.parse_args())

args = getComLineArgs()

today = datetime.date.today().strftime("%d%b%Y")

#manager_path = ConfigureJobs.getManagerPath()

analysis=args['analysis']
_binning = {
    'pt' : [25.*i for i in range(4)] + [100., 150., 200., 300.],
    #'mass' : [100.+100.*i for i in range(12)],
    'mass' : [100.] + [200.+50.*i for i in range(5)] + [500.,600.,800.,1000.],
    'massFull' : [80.,100.,120.,130.,150.,180.,200.,240.,300.,400.,1000],
    'eta' : [6,0.,6.],
    'zmass' : [60., 80., 84., 86.] + [87.+i for i in range(10)] + [98., 102., 120.], #[12, 60., 120.],
    'z1mass' : [60., 80., 84., 86.] + [87.+i for i in range(10)] + [98., 102., 120.], #[12, 60., 120.],
    'z2mass' : [60., 75., 83.] + [84.+i for i in range(14)] + [105., 120.],#[12, 60., 120.],
    'z1pt' : [i * 25. for i in range(7)] + [200., 300.],
    'z2pt' : [i * 25. for i in range(7)] + [200., 300.],
    'zpt' : [i * 25. for i in range(7)] + [200., 300.],
    'zHigherPt' : [i * 25. for i in range(7)] + [200., 300.],
    'zLowerPt' : [i * 25. for i in range(7)] + [200., 300.],
    'leppt' : [i * 15. for i in range(11)],
    'l1Pt' : [0.,15.,30.,40.,50.]+[60.+15.*i for i in range(9)]+[195.,225.],#[14,0.,210.],#[15, 0., 150.],
    'dphiz1z2': [0.,1.5,2.0,2.25,2.5,2.75,3.0,3.25],
    'drz1z2': [0.,1.0,2.0,3.0,4.0,5.0,6.0]
    }
units = {
    'pt' : '[GeV]',
    'mass' : '[GeV]',
    'massFull' : '[GeV]',
    'eta' : '',
    'zmass' : '[GeV]',
    'z1mass' : '[GeV]',
    'z2mass' : '[GeV]',
    'zpt' : '[GeV]',
    'z1pt' : '[GeV]',
    'z2pt' : '[GeV]',
    'zHigherPt' : '[GeV]',
    'zLowerPt' : '[GeV]',
    'leppt' : '[GeV]',
    'l1Pt' : '[GeV]',
    'dphiz1z2': '',
    'drz1z2':'',
    }

yaxisunits = {
    'pt' : 'GeV',
    'mass' : 'GeV',
    'massFull' : 'GeV',
    'eta' : '',
    'zmass' : 'GeV',
    'z1mass' : 'GeV',
    'z2mass' : 'GeV',
    'zpt' : 'GeV',
    'z1pt' : 'GeV',
    'z2pt' : 'GeV',
    'zHigherPt' : 'GeV',
    'zLowerPt' : 'GeV',
    'leppt' : 'GeV',
    'l1Pt' : 'GeV',
    'dphiz1z2': '',
    'drz1z2':'',
    }

prettyVars = {
    'pt' : 'p_{T}^{ZZ}',
    'mass' : 'm_{ZZ}',
    'massFull' : 'm_{4\\ell}',
    'eta' : '\\eta_{4\\ell}',
    'zmass' : 'm_{Z}',
    'z1mass' : 'm_{Z_{1}}',
    'z2mass' : 'm_{Z_{2}}',
    'z1pt' : 'p_{T}^{Z_{1}}',
    'z2pt' : 'p_{T}^{Z_{2}}',
    'zpt' : 'p_{T}^{Z}',
    'zHigherPt' : 'p_\\text{T}^{\\text{Z}_{\\text{lead}}}',
    'zLowerPt' : 'p_\\text{T}^{\\text{Z}_{\\text{sublead}}}',
    'leppt' : '\\text{p}_\\text{T}^{\\ell}',
    'l1Pt' : 'p_\\text{T}^{\\ell_1}', 
    'dphiz1z2': '\\Delta\\varphi_{Z_{1},Z_{2}}',
    'drz1z2':'\\Delta\\text{R}_{Z_{1},Z_{2}}}',
    }

_xTitle = {}
_yTitle = {}
_yTitleNoNorm = {}

_yTitleTemp = '{prefix} \\frac{{d\\sigma_{{\\text{{fid}}}}}}{{d{xvar}}} {units}'
#_xTitleTemp = '\\text{xvar} {units}'
for var, prettyVar in prettyVars.iteritems():
    xt = prettyVar
    if yaxisunits[var]:
        #xt = _xTitleTemp.format(xvar=prettyVar,units = ' \\, \\left(\\text{{{}}}\\right)'.format(yaxisunits[var]))
        xt += ' \\, \\left(\\text{{{}}}\\right)'.format(yaxisunits[var])
        yt = _yTitleTemp.format(xvar=prettyVar,
                                prefix='\\frac{1}{\\sigma_{\\text{fid}}}',
                                units='\\, \\left[ \\frac{{1}}{{\\text{{{unit}}}}} \\right]'.format(unit=yaxisunits[var]))
        ytnn = _yTitleTemp.format(xvar=prettyVar, prefix='',
                                  units='\\, \\left( \\frac{{\\text{{fb}}}}{{\\text{{{unit}}}}} \\right)'.format(unit=yaxisunits[var]))
    else:
        yt = _yTitleTemp.format(prefix='\\frac{1}{\\sigma_{\\text{fid}}}',
                                xvar=prettyVar, units='')
        ytnn = _yTitleTemp.format(prefix='', xvar=prettyVar, units='\\left( \\text{fb} \\right)')

    _xTitle[var] = xt
    _yTitle[var] = yt
    _yTitleNoNorm[var] = ytnn

# list of variables not counting systematic shifts
varList=['Mass','ZZPt','ZPt','LepPt','dPhiZ1Z2','dRZ1Z2']

# Sometimes need to more or resize legend
legDefaults = {
    'textsize' : 0.034, #.027,#2,
    'leftmargin' : 0.35,
    'entryheight' : 0.037,
    'rightmargin' : 0.03,
    }
legParams = {v.lower():legDefaults.copy() for v in varList}
legParams['z1mass'] = {
    'textsize' : .026,
    'leftmargin' : .03,
    'rightmargin' : .46,
    'entryheight' : .034,#23
    'entrysep' : .007,
    }
legParams['pt'] = legParams['zzpt'].copy()
legParams['zmass'] = legParams['z1mass'].copy()
legParams['z2mass'] = legParams['z1mass'].copy()
legParams['deltaEtajj'] = legParams['z1mass'].copy()
legParams['deltaEtajj']['leftmargin'] = .5
legParams['deltaEtajj']['rightmargin'] = .03
legParams['deltaEtajj']['topmargin'] = .05
legParams['eta'] = legParams['deltaEtajj'].copy()
#legParams['massFull']['leftmargin'] = 0.25

legParamsLogy = {v:p.copy() for v,p in legParams.iteritems()}
#legParamsLogy['l1Pt']['topmargin'] = 0.65
#legParamsLogy['l1Pt']['leftmargin'] = 0.2
#legParamsLogy['l1Pt']['rightmargin'] = 0.18
legParamsLogy['mass']['topmargin'] = 0.075
legParamsLogy['mass']['leftmargin'] = 0.35
legParamsLogy['mass']['rightmargin'] = 0.025
legParamsLogy['mass']['textsize'] = 0.033
legParamsLogy['leppt']['topmargin'] = 0.05
#legParamsLogy['zHigherPt']['topmargin'] = 0.045
#legParamsLogy['massFull']['topmargin'] = 0.035

def normalizeBins(h):
    binUnit = 1 # min(h.GetBinWidth(b) for b in range(1,len(h)+1))
    for ib in range(1,h.GetNbinsX()+1):
        w = h.GetBinWidth(ib)
        h.SetBinContent(ib, h.GetBinContent(ib) * binUnit / w)
        h.SetBinError(ib, h.GetBinError(ib) * binUnit / w)
        if h.GetBinError(ib) > h.GetBinContent(ib):
            h.SetBinError(ib, h.GetBinContent(ib))
    h.Sumw2()

def unnormalizeBins(h):
    binUnit = 1 # min(h.GetBinWidth(b) for b in range(1,len(h)+1))
    for ib in range(1,h.GetNbinsX()+1):
        w = h.GetBinWidth(ib)
        h.SetBinContent(ib, h.GetBinContent(ib) * w / binUnit)
        h.SetBinError(ib, h.GetBinError(ib) * w / binUnit)
        if h.GetBinError(ib) > h.GetBinContent(ib):
            h.SetBinError(ib, h.GetBinContent(ib))
    h.Sumw2()

def createRatio(h1, h2):
    Nbins = h1.GetNbinsX()
    Ratio = h1.Clone("Ratio")
    hStackLast = h2.Clone("hStackLast")
    try:
        Ratio.Sumw2()
    except AttributeError:
        pass
    try:
        hStackLast.Sumw2()
    except AttributeError:
        pass
    for i in range(1,Nbins+1):
        stackcontent = hStackLast.GetBinContent(i)
        stackerror = hStackLast.GetBinError(i)
        datacontent = h1.GetBinContent(i)
        dataerror = h1.GetBinError(i)
        #print "stackcontent: ",stackcontent," and data content: ",datacontent
        ratiocontent=0
        if(datacontent!=0):
            ratiocontent = datacontent/stackcontent
        if(datacontent!=0):
            error = ratiocontent*(math.sqrt(math.pow((dataerror/datacontent),2) + math.pow((stackerror/stackcontent),2)))
        else:
            error = 2.07
        #print "ratio content: ",ratiocontent
        #print "stat error: ", error
        Ratio.SetBinContent(i,ratiocontent)
        Ratio.SetBinError(i,error)

    Ratio.GetYaxis().SetRangeUser(0.4,1.8)
    Ratio.SetStats(0)
    Ratio.GetYaxis().CenterTitle()
    Ratio.SetMarkerStyle(20)
    Ratio.SetMarkerSize(1.5)#0.7

    line = ROOT.TLine(h1.GetXaxis().GetXmin(), 1.,h1.GetXaxis().GetXmax(), 1.)
    line.SetLineStyle(7)

    Ratio.GetYaxis().SetLabelSize(0.14)
    Ratio.GetYaxis().SetTitleSize(0.16)
    Ratio.GetYaxis().SetLabelFont(42)
    Ratio.GetYaxis().SetTitleFont(42)
    Ratio.GetYaxis().SetTitleOffset(0.25)
    Ratio.GetYaxis().SetNdivisions(100)
    Ratio.GetYaxis().SetTickLength(0.05)

    Ratio.GetXaxis().SetLabelSize(0)
    Ratio.GetXaxis().SetTitleSize(0)

    return Ratio,line

def getPrettyLegend(hTrue, data_hist, hAltTrue,hMat, error_hist, coords):
    legend = ROOT.TLegend(coords[0], coords[1], coords[2], coords[3])
    ROOT.SetOwnership(legend, False)
    legend.SetName("legend")
    #legend.SetFillStyle(0)
    legend.SetFillColor(ROOT.kWhite)
    legend.SetBorderSize(0)
    legend.SetTextSize(0.028)
    legend.SetTextFont(62)
    legend.SetTextColor(ROOT.kBlack)
    sigLabel = "POWHEG+MCFM+Pyth." 
    sigLabelAlt = "MG5_aMC@NLO+MCFM+Pyth."
    MatLabel = "MATRIX"
    if data_hist:
        legend.AddEntry(data_hist, "#scale[1.2]{Data + stat. unc.}", "lep")
    legend.AddEntry(error_hist, "#scale[1.2]{Stat. #oplus syst. unc.}", "f")
    legend.AddEntry(hMat, MatLabel,"l")
    legend.AddEntry(hTrue, sigLabel,"lf")
    legend.AddEntry(hAltTrue, sigLabelAlt,"l")
    return legend

def createCanvasPads(varName):
    canvas_dimensions = [1000, 1450]
    c = ROOT.TCanvas("c", "canvas",*canvas_dimensions)
    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetLegendBorderSize(0)
    # Upper histogram plot is pad1
    pad1 = ROOT.TPad("pad1", "pad1", 0.01, 0.34, 0.99, 0.995)
    pad1.Draw()
    pad1.cd()
    if varName!="drz1z2" and not args['noNorm']:
        pad1.SetLogy()
    pad1.SetFillColor(0)
    pad1.SetFrameLineWidth(3)
    pad1.SetFrameBorderMode(0)
    pad1.SetBorderMode(0)
    #pad1.SetTopMargin(0)  # play with this if -1 is out of frame in superscript of 137
    pad1.SetBottomMargin(0)  # joins upper and lower plot
    #pad1.SetGridx()
    #pad1.Draw()
    return c,pad1

def createPad1a(canvas):
    # Lower ratio plot is pad2
    canvas.cd()  # returns to main canvas before defining pad2
    pad1a = ROOT.TPad("pad1a", "pad1a", 0.01, 0.25125, 0.99, 0.34)
    pad1a.Draw()
    pad1a.cd()
    pad1a.SetFillColor(0)
    pad1a.SetFrameLineWidth(3)
    pad1a.SetFrameBorderMode(0)
    pad1a.SetBorderMode(0)#bordermode = -1 box looks as it is behind the screen
   # bordermode = 0 no special effects
   # bordermode = 1 box looks as it is in front of the screen
    pad1a.SetTopMargin(0)  # joins upper and lower plot
    pad1a.SetBottomMargin(0)
    #pad1a.SetGridx()
    #pad1a.Draw()
    return pad1a

def createPad2(canvas):
    # Lower ratio plot is pad2
    canvas.cd()  # returns to main canvas before defining pad2
    pad2 = ROOT.TPad("pad2", "pad2", 0.01, 0.1625, 0.99, 0.25125)
    pad2.Draw()
    pad2.cd()
    pad2.SetFillColor(0)
    pad2.SetFrameLineWidth(3)
    pad2.SetFrameBorderMode(0)
    pad2.SetBorderMode(0)#bordermode = -1 box looks as it is behind the screen
   # bordermode = 0 no special effects
   # bordermode = 1 box looks as it is in front of the screen
    pad2.SetTopMargin(0)  # joins upper and lower plot
    pad2.SetBottomMargin(0)
    #pad2.SetGridx()
    #pad2.Draw()
    return pad2

def createPad3(canvas):
    # Lower ratio plot is pad3
    canvas.cd()  # returns to main canvas before defining pad3
    pad3 = ROOT.TPad("pad3", "pad3", 0.01, 0.01, 0.99, 0.1625)
    pad3.Draw()
    pad3.cd()
    pad3.SetFillColor(0)
    pad3.SetFrameLineWidth(3)
    pad3.SetFrameBorderMode(0)
    #pad3.SetFrameFillStyle(4000)
    pad3.SetBorderMode(0)
    pad3.SetTopMargin(0)  # joins upper and lower plot
    pad3.SetBottomMargin(0.42)
    #pad3.SetGridx()
    #pad3.Draw()
    return pad3

#rebin histos and take care of overflow bins
def rebin(hist,varName):
    ROOT.SetOwnership(hist, False)
    #No need to rebin certain variables but still might need overflow check
    if varName not in ['eta']:
        bins=array.array('d',_binning[varName])
        Nbins=len(bins)-1 
        hist=hist.Rebin(Nbins,"",bins)
    else:
        Nbins = hist.GetSize() - 2
    add_overflow = hist.GetBinContent(Nbins) + hist.GetBinContent(Nbins + 1)
    hist.SetBinContent(Nbins, add_overflow)
    hist.SetBinContent(Nbins+1,0)
    return hist

def getLumiTextBox():
    texS = ROOT.TLatex(0.68,0.960, str(int(args['lumi']))+" fb^{-1} (13 TeV)")
    texS.SetNDC()
    texS.SetTextFont(62)
    texS.SetTextSize(0.043)
    texS.SetTextColor(ROOT.kBlack)
    texS.Draw()
    texS1 = ROOT.TLatex(0.14,0.965,"#bf{CMS}")
    texS1.SetNDC()
    texS1.SetTextFont(42)
    texS1.SetTextColor(ROOT.kBlack)
    texS1.SetTextSize(0.050)#0.045
    texS1.Draw()
    return texS,texS1

def getSigTextBox(x,y,sigLabel,size):
    if sigLabel=="POWHEG+MCFM+Pythia8":
        texS = ROOT.TLatex(x,y, "#bf{POWHEG+MCFM+Pythia8}")
    elif sigLabel=="MG5_aMC@NLO+MCFM+Pythia8":
        texS = ROOT.TLatex(x,y, "#bf{MG5_aMC@NLO+MCFM+Pythia8}")
    else:
        texS = ROOT.TLatex(x,y, "#bf{MATRIX}")
    texS.SetNDC()
    texS.SetTextFont(42)
    texS.SetTextColor(ROOT.kBlack)
    texS.SetTextSize(size)
    texS.Draw()
    return texS

def getAxisTextBox(x,y,axisLabel,size,rotated):
    texS = ROOT.TLatex(x,y,axisLabel)
    texS.SetNDC()
    #rotate for y-axis
    if rotated:
        texS.SetTextAngle(90)
    texS.SetTextFont(42)
    #texS.SetTextColor(ROOT.kBlack)
    texS.SetTextSize(size)
    texS.Draw()
    return texS

def RatioErrorBand(Ratio,hUncUp,hUncDn,hTrueNoErrs,varName):
        ratioGraph=ROOT.TGraphAsymmErrors(Ratio)
        ROOT.SetOwnership(ratioGraph,False)
        tmpData = Ratio.Clone("tmp")
        for i in range(1, tmpData.GetNbinsX()+1):
            if hTrueNoErrs.GetBinContent(i)==0:
                continue
            eUp=hUncUp.GetBinContent(i)
            eDn=hUncDn.GetBinContent(i)
            tru=hTrueNoErrs.GetBinContent(i)
            #print "eUp: ",eUp, "","eDn: ",eDn
            errorUp = tmpData.GetBinContent(i) + math.sqrt(math.pow(tmpData.GetBinError(i),2) + math.pow((eUp/tru),2))
            errorUp -= Ratio.GetBinContent(i) 
            errorDn = max(tmpData.GetBinContent(i) - math.sqrt(math.pow(tmpData.GetBinError(i),2) + math.pow((eDn/tru),2)),0)
            errorDn = Ratio.GetBinContent(i) - errorDn
            #print "stat. error: ",tmpData.GetBinError(i)
            #print "eUp/tru: ",eUp/tru
            #print "eDn/tru: ",eDn/tru
            #print "errorUp: ",errorUp, "","errorDn: ",errorDn
            ratioGraph.SetPointEYhigh(i-1, errorUp)
            ratioGraph.SetPointEYlow(i-1, errorDn)
        ratioGraph.SetFillColorAlpha(1,0.5)
        ratioGraph.SetFillStyle(3001)
        ratioGraph.GetXaxis().SetLabelSize(0)
        ratioGraph.GetXaxis().SetTitleSize(0)
        #ratioGraph.GetYaxis().SetLabelSize(0)
        #ratioGraph.GetYaxis().SetTitleSize(0)
        ratioGraph.GetXaxis().SetLimits(Ratio.GetXaxis().GetXmin(),Ratio.GetXaxis().GetXmax())
        if varName=="drz1z2":
            ratioGraph.SetMaximum(1.8)
            ratioGraph.SetMinimum(0.3)
        else:
            ratioGraph.SetMaximum(1.8)
            ratioGraph.SetMinimum(0.3)
        return ratioGraph

def MainErrorBand(hMain,hUncUp,hUncDn,varName,norm,normFb):
        MainGraph=ROOT.TGraphAsymmErrors(hMain)
        ROOT.SetOwnership(MainGraph,False)
        tmpData = hMain.Clone("tmp")
        for i in range(1, tmpData.GetNbinsX()+1):
            if hMain.GetBinContent(i)==0:
                continue
            eUp=hUncUp.GetBinContent(i)
            eDn=hUncDn.GetBinContent(i)
            #print "eUp: ",eUp, "","eDn: ",eDn
            errorUp = tmpData.GetBinContent(i) + math.sqrt(math.pow(tmpData.GetBinError(i),2) + math.pow(eUp,2))
            errorUp -= hMain.GetBinContent(i) 
            errorDn = max(tmpData.GetBinContent(i) - math.sqrt(math.pow(tmpData.GetBinError(i),2) + math.pow(eDn,2)),0)
            errorDn = hMain.GetBinContent(i) - errorDn
            #print "errorUp: ",errorUp, "","errorDn: ",errorDn
            MainGraph.SetPointEYhigh(i-1, errorUp)
            MainGraph.SetPointEYlow(i-1, errorDn)
        MainGraph.SetFillColorAlpha(1,0.7)
        MainGraph.SetFillStyle(3001)
        if norm:
            drawyTitle = _yTitle[varName]
        elif normFb:
            drawyTitle = _yTitleNoNorm[varName]
        else:
            drawyTitle = "Events"
        MainGraph.GetYaxis().SetTitle(drawyTitle)

        MainGraph.GetYaxis().SetTitleSize(1.3*hMain.GetYaxis().GetTitleSize())
        MainGraph.GetYaxis().SetLabelSize(1.3*hMain.GetYaxis().GetLabelSize())
        if varName=="drz1z2":
            MainGraph.GetYaxis().SetTitleOffset(1.0)
        else:
            MainGraph.GetYaxis().SetTitleOffset(hMain.GetYaxis().GetTitleOffset()*args['titleOffset'])
        MainGraph.GetYaxis().SetLabelOffset(0.01)
        #MainGraph.GetXaxis().SetLabelSize(0)
        #MainGraph.GetXaxis().SetTitleSize(0)
        #MainGraph.GetYaxis().SetLabelSize(0)
        #MainGraph.GetYaxis().SetTitleSize(0)
        if varName=="mass":
            MainGraph.GetYaxis().ChangeLabel(1,-1,0)
            MainGraph.GetYaxis().ChangeLabel(4,-1,0)
        if varName=="pt" or varName=="zpt" or varName=="leppt" or varName=="dphiz1z2":
            MainGraph.GetYaxis().ChangeLabel(2,-1,0)
        if varName=="drz1z2":
            MainGraph.GetYaxis().ChangeLabel(3,-1,0)
        MainGraph.GetXaxis().SetLimits(hMain.GetXaxis().GetXmin(),hMain.GetXaxis().GetXmax())
        MainGraph.GetXaxis().SetLimits(hMain.GetXaxis().GetXmin(),hMain.GetXaxis().GetXmax())
        #MainGraph.SetMaximum(1.5)

        #MainGraph.SetMaximum(1.2*(hMain.GetMaximum())*args["scaleymax"])
        MainGraph.SetMinimum(args['scaleymin']*(hMain.GetMinimum()))
        #if varName=="drz1z2":
        #    MainGraph.SetMinimum(0.0)
        #else:
            #MainGraph.SetMinimum(0.5*(hMain.GetMinimum()))
        return MainGraph

def PredErrorBand(hMain,hUncUp,hUncDn,varName,norm,normFb):
        PredGraph=ROOT.TGraphAsymmErrors(hMain)
        ROOT.SetOwnership(PredGraph,False)
        tmpData = hMain.Clone("tmp")
        for i in range(1, tmpData.GetNbinsX()+1):
            if hMain.GetBinContent(i)==0:
                continue
            eUp=hUncUp.GetBinContent(i)
            eDn=hUncDn.GetBinContent(i)
            #print "eUp: ",eUp, "","eDn: ",eDn
            errorUp = tmpData.GetBinContent(i) + math.sqrt(math.pow(tmpData.GetBinError(i),2) + math.pow(eUp,2))
            errorUp -= hMain.GetBinContent(i) 
            errorDn = max(tmpData.GetBinContent(i) - math.sqrt(math.pow(tmpData.GetBinError(i),2) + math.pow(eDn,2)),0)
            errorDn = hMain.GetBinContent(i) - errorDn
            #print "errorUp: ",errorUp, "","errorDn: ",errorDn
            PredGraph.SetPointEYhigh(i-1, errorUp)
            PredGraph.SetPointEYlow(i-1, errorDn)
        #PredGraph.SetFillColorAlpha(1,0.7)
        PredGraph.SetFillStyle(3001)
        if norm:
            drawyTitle = _yTitle[varName]
        elif normFb:
            drawyTitle = _yTitleNoNorm[varName]
        else:
            drawyTitle = "Events"
        PredGraph.GetYaxis().SetTitle(drawyTitle)

        #PredGraph.GetYaxis().SetTitleSize(1.3*hMain.GetYaxis().GetTitleSize())
        #PredGraph.GetYaxis().SetLabelSize(1.3*hMain.GetYaxis().GetLabelSize())
        #if varName=="drz1z2":
        #    PredGraph.GetYaxis().SetTitleOffset(1.0)
        #else:
        #    PredGraph.GetYaxis().SetTitleOffset(hMain.GetYaxis().GetTitleOffset()*args['titleOffset'])
        #PredGraph.GetYaxis().SetLabelOffset(0.01)
        ##PredGraph.GetXaxis().SetLabelSize(0)
        ##PredGraph.GetXaxis().SetTitleSize(0)
        ##PredGraph.GetYaxis().SetLabelSize(0)
        ##PredGraph.GetYaxis().SetTitleSize(0)
        #if varName=="mass":
        #    PredGraph.GetYaxis().ChangeLabel(1,-1,0)
        #    PredGraph.GetYaxis().ChangeLabel(4,-1,0)
        #if varName=="pt" or varName=="zpt" or varName=="leppt" or varName=="dphiz1z2":
        #    PredGraph.GetYaxis().ChangeLabel(2,-1,0)
        #if varName=="drz1z2":
        #    PredGraph.GetYaxis().ChangeLabel(3,-1,0)
        #PredGraph.GetXaxis().SetLimits(hMain.GetXaxis().GetXmin(),hMain.GetXaxis().GetXmax())
        #PredGraph.GetXaxis().SetLimits(hMain.GetXaxis().GetXmin(),hMain.GetXaxis().GetXmax())
        #PredGraph.SetMaximum(1.5)

        #PredGraph.SetMaximum(1.2*(hMain.GetMaximum())*args["scaleymax"])
        PredGraph.SetMinimum(args['scaleymin']*(hMain.GetMinimum()))
        #if varName=="drz1z2":
        #    PredGraph.SetMinimum(0.0)
        #else:
            #PredGraph.SetMinimum(0.5*(hMain.GetMinimum()))
        return PredGraph
def generatePlots(hUnfolded,hUncUp,hUncDn,hTruth,hTruthAlt,hMatrix,hMatrix_Up,hMatrix_Dn,varName,norm,normFb,lumi,unfoldDir):
    UnfHists=[]
    TrueHists=[]
    # for normalization if needed
    nomArea = hUnfolded.Integral(1,hUnfolded.GetNbinsX())
    # Make uncertainties out of the unfolded histos
    ### plot
    hUnf = hUnfolded.Clone()
    hTrue = hTruth.Clone()
    #Alt Signal 
    hTrueAlt = hTruthAlt.Clone()
    #hTrueLeg = hTruthAlt.Clone()
    #Matrix
    hMatDist = hMatrix.Clone()
    hMatDistUp = hMatrix_Up.Clone()
    hMatDistDn = hMatrix_Dn.Clone()
    #hMatLeg = hMatrix.Clone()
    #lumi provided already in fb-1
    lumifb = lumi

    if norm:
        hUnf.Scale(1.0/(hUnf.Integral(1,hUnf.GetNbinsX())))
    elif normFb:
        hUnf.Scale(1.0/lumifb)
    else:
        print "no special normalization"

    print ("hTrue histo here: ",hTrue)
    print ("unfoldDir: ",unfoldDir)
    xaxisSize = hUnf.GetXaxis().GetTitleSize()
    yaxisSize = hTrue.GetXaxis().GetTitleSize()
    if unfoldDir:
        #Create a ratio plot
        c,pad1 = createCanvasPads(varName)
        c.SetCanvasSize(1000, 1450);
        #c.SetBottomMargin(0.10);
        Unfmaximum = hUnf.GetMaximum()
        #hTrue.SetFillColor(ROOT.TColor.GetColor("#99ccff"))
        #hTrue.SetLineColor(ROOT.TColor.GetColor('#000099')) 
        hTrue.SetFillColor(ROOT.TColor.GetColor("#add8e6"))
        hTrue.SetLineColor(ROOT.TColor.GetColor('#377eb8'))
        hTrue.SetLineStyle(1)
        hTrue.SetFillStyle(3010)
        #AltSignal
        hTrueAlt.SetFillColor(2)
        hTrueAlt.SetLineStyle(9)#dashes
        hTrueAlt.SetFillStyle(0)#hollow
        hTrueAlt.SetLineColor(ROOT.kRed)
        #Matrix
        hMatDist.SetFillColor(8)
        hMatDist.SetLineStyle(2)#dashes
        hMatDist.SetFillStyle(0)#hollow
        hMatDist.SetLineColor(ROOT.kMagenta)
        Truthmaximum = hTrue.GetMaximum()
        hTrue.SetLineWidth(2*hTrue.GetLineWidth())
        hTrueAlt.SetLineWidth(4*hTrueAlt.GetLineWidth())
        hMatDist.SetLineWidth(4*hMatDist.GetLineWidth())
        print "Total Truth Integral",hTrue.Integral()

        if not norm and normFb:
            print "Inclusive fiducial cross section = {} fb".format(hUnf.Integral(1,hUnf.GetNbinsX()))
        if norm or normFb:
            normalizeBins(hUnf)

        if norm:
            hUncUp.Scale(1.0/hUnfolded.Integral(1,hUnfolded.GetNbinsX()))
            hUncDn.Scale(1.0/hUnfolded.Integral(1,hUnfolded.GetNbinsX()))
        elif normFb:
            hUncUp.Scale(1.0/lumifb)
            hUncDn.Scale(1.0/lumifb)
        else:
            print "no special normalization"

        if norm or normFb:
            normalizeBins(hUncUp)
            normalizeBins(hUncDn)

        if norm:
            trueInt = hTrue.Integral(1,hTrue.GetNbinsX())
            hTrue.Scale(1.0/trueInt)
            #hTrueUncUp /= trueInt # (trueInt + hTrueUncUp.Integral(1,hTrueUncUp.GetNbinsX()))
            #hTrueUncDn /= trueInt # (trueInt - hTrueUncDn.Integral(1,hTrueUncDn.GetNbinsX()))
            #Alt Signal
            AltTrueInt = hTrueAlt.Integral(1,hTrueAlt.GetNbinsX())
            hTrueAlt.Scale(1.0/AltTrueInt)
            #Matrix
            MatInt = hMatDist.Integral(1,hMatDist.GetNbinsX())
            hMatDist.Scale(1.0/MatInt)
            MatIntUp = hMatDistUp.Integral(1,hMatDistUp.GetNbinsX())
            hMatDistUp.Scale(1.0/MatIntUp)
            MatIntDn = hMatDistDn.Integral(1,hMatDistDn.GetNbinsX())
            hMatDistDn.Scale(1.0/MatIntDn)
        elif normFb:
            hTrue.Scale(1.0/lumifb)
            #hTrueUncUp /= lumifb
            #hTrueUncDn /= lumifb
            hTrueAlt.Scale(1.0/lumifb)
            hMatDist.Scale(1.0/lumifb)
            hMatDistUp.Scale(1.0/lumifb)
            hMatDistDn.Scale(1.0/lumifb)
        else:
            print "no special normalization"

        print "Total Alt Truth Integral",hTrueAlt.Integral()
        print "Total MATRIX Integral",hMatDist.Integral()
        print "Total Unf Data Integral",hUnf.Integral()
       
        #subtract hMat from Up and Dn
        #hMatDistUp.Add(hMatDist,-1)
        #hMatDistDn.Add(hMatDist,-1)


        if norm or normFb:
            normalizeBins(hTrue)
            #normalizeBins(hTrueUncUp)
            #normalizeBins(hTrueUncDn)
            normalizeBins(hTrueAlt)
            normalizeBins(hMatDist)
            normalizeBins(hMatDistUp)
            normalizeBins(hMatDistDn)


        print "Total MATRIX Up Integral",hMatDistUp.Integral()
        print "Total MATRIX Down Integral",hMatDistDn.Integral()
        #hTrue.Draw("HIST")
        #hTrueAlt.Draw("HIST") 
        #hMatDist.Draw("HIST")
        print "Total UnfUnc Up Integral",hUncUp.Integral()
        print "Total UnfUnc Down Integral",hUncDn.Integral()

        if(Unfmaximum > Truthmaximum):
            hTrue.SetMaximum(Unfmaximum*args["scaleymax"])
        else:
            hTrue.SetMaximum(Truthmaximum*args["scaleymax"])

        hTrue.GetXaxis().SetTitle("")

        UnfErrBand = MainErrorBand(hUnf,hUncUp,hUncDn,varName,norm,normFb)

        MatErrBand = PredErrorBand(hMatDist,hMatDistUp,hMatDistDn,varName,norm,normFb)
        MatErrBand.SetFillColorAlpha(6,0.7)
        
        if norm:
            if varName=="mass":
                UnfErrBand.SetMaximum(0.01*args['scaleymax'])
            elif varName=="pt":
                UnfErrBand.SetMaximum(0.02*args['scaleymax'])
            elif varName=="zpt":
                UnfErrBand.SetMaximum(0.02*args['scaleymax'])
            elif varName=="dphiz1z2":
                UnfErrBand.SetMaximum(1.4*args['scaleymax'])
            elif varName=="drz1z2":
                UnfErrBand.SetMaximum(0.59*args['scaleymax'])
            elif varName=="leppt":
                UnfErrBand.SetMaximum(0.02*args['scaleymax'])
        UnfErrBand.Draw("a2")
        MatErrBand.Draw("a2SAME")
        hTrue.GetXaxis().SetLabelSize(0)
        hTrue.GetXaxis().SetTitleSize(0)
        #hTrue.GetYaxis().SetTitle("Events")
        #hTrue.GetYaxis().SetTitleOffset(1.0)
        hTrueAlt.GetXaxis().SetLabelSize(0)
        hTrueAlt.GetXaxis().SetTitleSize(0)
        hMatDist.GetXaxis().SetLabelSize(0)
        hMatDist.GetXaxis().SetTitleSize(0)
        hTrueAlt.Draw("HISTSAME")
        hTrue.Draw("HISTSAME")
        hMatDist.Draw("HISTSAME")
        box = ROOT.TBox(ROOT.gPad.GetLeftMargin()-0.1,ROOT.gPad.GetBottomMargin(),ROOT.gPad.GetRightMargin()-0.1,ROOT.gPad.GetTopMargin()-0.1);
        MatMax=hMatDist.GetMaximum()
        MatMin=args['scaleymin']*hUnf.GetMinimum()
        if varName=="dphiz1z2":
            dline = ROOT.TLine(hUnf.GetXaxis().GetXmax(),MatMin,hUnf.GetXaxis().GetXmax(),MatMax)
            dline.SetLineColor(ROOT.kBlack)
            dline.SetLineWidth(3);
            dline.Draw("same");
        eline = ROOT.TLine(hUnf.GetXaxis().GetXmin(),MatMin,hUnf.GetXaxis().GetXmin(),MatMax)
        eline.SetLineColor(ROOT.kBlack)
        eline.SetLineWidth(3);
        eline.Draw("same");
        #hUnf.Sumw2(False)
        #hUnf.SetBinErrorOption(ROOT.TH1.kPoisson)
        hUnf.SetLineColor(ROOT.kBlack)
        hUnf.SetMarkerStyle(20)
        hUnf.SetMarkerSize(2.5)#1.3
        hUnf.GetXaxis().SetTitle("")
        hUnf.GetXaxis().SetLabelSize(0)
        hUnf.GetXaxis().SetTitleSize(0)
        hUnf.Draw("PE1SAME")
      
        offset = ROOT.gPad.GetLeftMargin() - 0.07 if args['legend_left'] else \
            ROOT.gPad.GetRightMargin() - 0.07
        width = .33
        width *= args['scalelegx']
        xdist = 0.1 if args['legend_left'] else 0.91
        if varName=="leppt":
            xdist=0.15
        xcoords = [xdist+offset, xdist+width+offset] if args['legend_left'] \
            else [xdist-width-offset, xdist-offset]
        unique_entries = min(2, 8)
        ymax = 0.30 if varName=="leppt" else 0.915
        #ymax = 0.915
        ycoords = [ymax, ymax - 0.08*unique_entries*args['scalelegy']]
        coords = [xcoords[0], ycoords[0], xcoords[1], ycoords[1]]
        legend = getPrettyLegend(hTrue, hUnf, hTrueAlt, hMatDist, UnfErrBand, coords)
        legend.Draw()
        texS,texS1=getLumiTextBox()
        sigLabel = "POWHEG+MCFM+Pythia8" 
        sigLabelAlt = "MG5_aMC@NLO+MCFM+Pythia8"
        MatLabel = "MATRIX"
        #if varName=="dphiz1z2" or varName=="drz1z2":
        #    leg = ROOT.TLegend(0.15,0.60,0.15+0.015*len(sigLabelAlt),0.90,"")
        #elif varName=="leppt":
        #    leg = ROOT.TLegend(0.20,0.18,0.20+0.015*len(sigLabelAlt),0.48,"")
        #else:
        #    leg = ROOT.TLegend(0.55,0.60,0.55+0.015*len(sigLabelAlt),0.90,"")
        #leg.AddEntry(hUnf,"Data + stat. unc.","lep")
        #leg.AddEntry(UnfErrBand, "Stat. #oplus syst. unc.","f")
        #leg.AddEntry(hTrue, sigLabel,"lf")

        #hTrueLeg.SetFillColor(2)
        #hTrueLeg.SetLineStyle(10)#dashes
        #hTrueLeg.SetFillColorAlpha(2,0.4)
        #hTrueLeg.SetFillStyle(3001)#solid
        #hTrueLeg.SetLineColor(ROOT.kRed)
        #hTrueLeg.SetLineWidth(4*hTrueLeg.GetLineWidth())
        #MatLeg
        #hMatLeg.SetFillColor(8)
        #hMatLeg.SetLineStyle(10)#dashes
        #hMatLeg.SetFillColorAlpha(8,0.4)
        #hMatLeg.SetFillStyle(3001)#solid
        #hMatLeg.SetLineColor(ROOT.kGreen)
        #hMatLeg.SetLineWidth(4*hMatLeg.GetLineWidth())

        #leg.AddEntry(hTrueLeg, sigLabelAlt,"l")
        #leg.AddEntry(hMatLeg, MatLabel,"l")
        #leg.SetFillColor(ROOT.kWhite)
        #leg.SetBorderSize(1)
        #leg.SetFillStyle(1001)
        #leg.SetTextSize(0.025)
        #leg.Draw()

        #Pad1a for Data/MATRIX ratio
        pad1a = createPad1a(c)

        hMatDistNoErrs = hMatDist.Clone() # need central value only to keep ratio uncertainties consistent

        #hMatDistNoErrs.SetError(array.array('d',[0.]*nbins))
        #Starting the ratio proceedure
        MatRatio,Matline = createRatio(hUnf, hMatDistNoErrs)
        MatRatioErrorBand = RatioErrorBand(MatRatio,hUncUp,hUncDn,hMatDistNoErrs,varName)
        MatRatioErrorBand.GetYaxis().SetLabelSize(0)
        MatRatioErrorBand.GetYaxis().SetTitleSize(0)
        MatRatioErrorBand.Draw("a2")
        
        MatTex = getSigTextBox(0.20,0.8,MatLabel,0.20)
        MatRatio.Draw("PE1SAME")
        Matline.SetLineColor(ROOT.kMagenta)
        Matline.Draw("same")

        Matyaxis = ROOT.TGaxis(hUnf.GetXaxis().GetXmin(),MatRatioErrorBand.GetMinimum(),hUnf.GetXaxis().GetXmin(),MatRatioErrorBand.GetMaximum(),MatRatioErrorBand.GetMinimum(),MatRatioErrorBand.GetMaximum())
        Matyaxis.SetNdivisions(003)
        Matyaxis.SetTitle("")
        #Matyaxis.SetTitle("#scale[1.2]{Data/Theo.}")
        Matyaxis.SetLabelFont(42)
        Matyaxis.SetLabelOffset(0.01)
        Matyaxis.SetLabelSize(0.22)
        Matyaxis.SetTitleFont(42)
        Matyaxis.SetTitleSize(0.14)
        Matyaxis.SetTitleOffset(0.30)
        Matyaxis.Draw("SAME")
        
        #SecondPad
        pad2 = createPad2(c)

        hTrueNoErrs = hTrue.Clone() # need central value only to keep ratio uncertainties consistent
        nbins=hTrueNoErrs.GetNbinsX()
        print("trueNbins: ",nbins)

        Unfbins=hUnf.GetNbinsX()
        print("UnfNbins: ",Unfbins)
        #hTrueNoErrs.SetError(array.array('d',[0.]*nbins))
        #Starting the ratio proceedure
        Ratio,line = createRatio(hUnf, hTrueNoErrs)
        ratioErrorBand = RatioErrorBand(Ratio,hUncUp,hUncDn,hTrueNoErrs,varName)
        ratioErrorBand.GetYaxis().SetLabelSize(0)
        ratioErrorBand.GetYaxis().SetTitleSize(0)
        ratioErrorBand.Draw("a2")
        
        sigTex = getSigTextBox(0.20,0.8,sigLabel,0.19)
        Ratio.Draw("PE1SAME")
        line.SetLineColor(ROOT.kBlack)
        line.Draw("same")

        Altyaxis = ROOT.TGaxis(hUnf.GetXaxis().GetXmin(),ratioErrorBand.GetMinimum(),hUnf.GetXaxis().GetXmin(),ratioErrorBand.GetMaximum(),ratioErrorBand.GetMinimum(),ratioErrorBand.GetMaximum())
        Altyaxis.SetNdivisions(003)
        #Altyaxis.SetTitle("")

        #texTitle = ROOT.TLatex(0.095,0.2,"Data/Theo.")
        #texTitle.SetNDC()
        #texTitle.SetTextFont(42)
        #texTitle.SetTextColor(ROOT.kBlack)
        #texTitle.SetTextSize(0.23)
        #texTitle.SetTextAngle(90)
        #texTitle.Draw()
        #Altyaxis.SetTitle("")
        Altyaxis.CenterTitle()
        #Altyaxis.SetTitle("#scale[1.43]{Data/Theo}")#1.3
        yaxislabelTex = getAxisTextBox(0.06,0.0,"Data/Theo",0.23,True)
        Altyaxis.SetTitle("")
        Altyaxis.SetLabelFont(42)
        Altyaxis.SetLabelOffset(0.01)
        Altyaxis.SetLabelSize(0.22)
        Altyaxis.SetTitleFont(42)
        Altyaxis.SetTitleSize(0.16)
        Altyaxis.SetTitleOffset(0.348)
        Altyaxis.Draw("SAME")
        
        #ThirdPad
        pad3 = createPad3(c)


        hTrueAltNoErrs = hTrueAlt.Clone() # need central value only to keep ratio uncertainties consistent
        #nbins=hTrueNoErrs.GetNbinsX()
        #print("trueNbins: ",nbins)

        #Unfbins=hUnf.GetNbinsX()
        #print("UnfNbins: ",Unfbins)

        #hTrueNoErrs.SetError(array.array('d',[0.]*nbins))
        #Starting the ratio proceedure
        AltRatio,Altline = createRatio(hUnf, hTrueAltNoErrs)
        AltRatioErrorBand = RatioErrorBand(AltRatio,hUncUp,hUncDn,hTrueAltNoErrs,varName) 
        AltRatioErrorBand.GetYaxis().SetLabelSize(0)
        AltRatioErrorBand.GetYaxis().SetTitleSize(0)
        AltRatioErrorBand.Draw("a2")
        AltRatio.Draw("PE1SAME")
        #ratioErrorBand.Draw("p")
        Altline.SetLineColor(ROOT.kRed)
        Altline.Draw("same")
       
        #bottom margin
        #bottom_margin = 0.05
        #ROOT.gPad.SetBottomMargin(bottom_margin)

        AltTex = getSigTextBox(0.20,0.85,sigLabelAlt,0.11)
        #redraw axis
        xaxis = ROOT.TGaxis(hUnf.GetXaxis().GetXmin(),ratioErrorBand.GetMinimum(),hUnf.GetXaxis().GetXmax(),ratioErrorBand.GetMinimum(),hUnf.GetXaxis().GetXmin(),hUnf.GetXaxis().GetXmax(),510)
        #if varName=="leppt":
        #    xaxis.SetTitle(prettyVars[varName]+' '+"[GeV]")
        #else:
        #xaxis.SetTitle(prettyVars[varName]+' '+units[varName])
        xaxis.SetTitle("")
        labelTex = getAxisTextBox(0.78,0.08,prettyVars[varName]+' '+units[varName],0.20,False)
        xaxis.SetLabelFont(42)
        xaxis.SetLabelOffset(0.03)
        xaxis.SetLabelSize(0.158)
        xaxis.SetTitleFont(42)
        xaxis.SetTitleSize(0.175)
        xaxis.SetTitleOffset(1.05)
        if varName=="mass":
            xaxis.SetNoExponent(True)
        xaxis.Draw("SAME")

        yaxis = ROOT.TGaxis(hUnf.GetXaxis().GetXmin(),ratioErrorBand.GetMinimum(),hUnf.GetXaxis().GetXmin(),ratioErrorBand.GetMaximum(),ratioErrorBand.GetMinimum(),ratioErrorBand.GetMaximum())
        yaxis.SetNdivisions(003)
        yaxis.SetTitle("")
        #yaxis.SetTitle("Data/Theo.")
        yaxis.SetLabelFont(42)
        yaxis.SetLabelOffset(0.01)
        yaxis.SetLabelSize(0.13)
        yaxis.SetTitleFont(42)
        yaxis.SetTitleSize(0.12)
        yaxis.SetTitleOffset(0.438)
        yaxis.Draw("SAME")

        c.Update()
        print("CanvasWidth: ", c.GetWw())
        print("CanvasHeight: ", c.GetWh())
        plotName="Ratio_%s" % (varName)
        output_name="/".join([unfoldDir,plotName])
        #c.Print("%s/Ratio_%s.png" % (unfoldDir,varName))
        #c.Print("%s/Ratio_%s.eps" % (unfoldDir,varName))
        c.Print(output_name+".eps")
        c.Print(output_name+".png")
        c.SaveAs(output_name+".root")
        subprocess.call(["epstopdf", "--outfile=%s" % output_name+".pdf", output_name+".eps"],env={})
        #os.remove(output_name+".eps")
        del c
def mkdir(plotDir):
    for outdir in [plotDir]:
        try:
            os.makedirs(os.path.expanduser(outdir))
        except OSError as e:
            print e
            pass

varNames={'mass': 'Mass','pt':'ZZPt','zpt':'ZPt','leppt':'LepPt','dphiz1z2':'dPhiZ1Z2','drz1z2':'dRZ1Z2'}
UnfoldDir=args['unfoldDir']
UnfoldOutDirs={}
#Make differential cross sections normalizing to unity area.')
norm = not args['noNorm']
#normalize with the luminosity instead of area under the curve
normFb = args['NormFb']
#def main():
runVariables=[]
runVariables.append(args['variable'])
print "runVariables: ",runVariables
Matrix = {'mass':'m_ZZ__NNLOqq_QCDxNLOqq_EW+NLOa+NLOgg','pt':'pT_ZZ__NNLOqq_QCDxNLOqq_EW+NLOa+NLOgg','drz1z2':'dReta_Ze_Zm__NNLOqq_QCDxNLOqq_EW+NLOa+NLOgg',
'zpt':['pT_ee__NNLOqq_QCDxNLOqq_EW+NLOa+NLOgg','pT_mm__NNLOqq_QCDxNLOqq_EW+NLOa+NLOgg'],'leppt':['pT_lep1__NNLOqq_QCDxNLOqq_EW+NLOa+NLOgg',
'pT_lep2__NNLOqq_QCDxNLOqq_EW+NLOa+NLOgg','pT_lep3__NNLOqq_QCDxNLOqq_EW+NLOa+NLOgg','pT_lep4__NNLOqq_QCDxNLOqq_EW+NLOa+NLOgg'],
'dphiz1z2':'dPhi_Ze_Zm__NNLOqq_QCDxNLOqq_EW+NLOa+NLOgg'}
#Matrix distributions before Aug 6,2020
#Matrix = {'mass':'m_ZZ__NNLOqq_QCDxNLO_EW+NLOgg','pt':'pT_ZZ__NNLOqq_QCDxNLO_EW+NLOgg','drz1z2':'dReta_Ze_Zm__nNNLO_QCDxNLO_EW',
#'zpt':['pT_ee__NNLOqq_QCDxNLO_EW+NLOgg','pT_mm__NNLOqq_QCDxNLO_EW+NLOgg'],'leppt':['pT_lep1__NNLOqq_QCDxNLO_EW+NLOgg',
#'pT_lep2__NNLOqq_QCDxNLO_EW+NLOgg','pT_lep3__NNLOqq_QCDxNLO_EW+NLOgg','pT_lep4__NNLOqq_QCDxNLO_EW+NLOgg'],'dphiz1z2':'dPhi_Ze_Zm__nNNLO_QCDxNLO_EW'}
#Plot histograms from these respective root files generated wiht saveUnfolded.py
if analysis=="ZZ4l2016":
    fUse = ROOT.TFile("UnfHistsFinal-18Apr2020-ZZ4l2016.root","read")
elif analysis=="ZZ4l2017":
    fUse = ROOT.TFile("UnfHistsFinal-18Apr2020-ZZ4l2017.root","read")
elif analysis=="ZZ4l2018":
    if args['lumi'] < 100. : 
        fUse = ROOT.TFile("UnfHistsFinal-18Apr2020-ZZ4l2018.root","read")
    else:
        #LepPt,ZPt
        #fUse = ROOT.TFile("UnfHistsFinal-26Jun2020-ZZ4lFullRun2.root","read")
        fUse = ROOT.TFile("UnfHistsFinal-18Apr2020-ZZ4lFullRun2.root","read")
#fUse = ROOT.TFile.Open("UnfHistsFull-09Feb2020-FullRun2.root","read")
print("file being Used: ",fUse)
for varName in runVariables:
    print "varName:", varName
    # save unfolded distributions by channel, then systematic
    hUnfolded = {}
    hTrue = {}
    hTrueAlt = {}
    hErr = {}
    hErrTrue = {}
    if varName=='zpt' or varName=='leppt':
        fMat = ROOT.TFile.Open("data/MATRIX/%s.root" % (Matrix[varName][0]),"read")
        canvas = fMat.Get("canvas")
        #This is eemm channel
        hMat = canvas.GetListOfPrimitives().FindObject("%s" %(Matrix[varName][0]))
        unnormalizeBins(hMat)
        hMat.SetDirectory(0)
        fMat.Close()
        for i in range (1,len(Matrix[varName])):
            fileName = Matrix[varName][i]
            fSecMat = ROOT.TFile.Open("data/MATRIX/%s.root" % (fileName),"read")
            canvas = fSecMat.Get("canvas")
            #This is eemm channel
            hSecMat = canvas.GetListOfPrimitives().FindObject("%s" %(fileName))
            unnormalizeBins(hSecMat)
            hSecMat.SetDirectory(0)
            hMat.Add(hSecMat)
            fSecMat.Close()
    else:
        fMat = ROOT.TFile.Open("data/MATRIX/%s.root" % (Matrix[varName]),"read")
        canvas = fMat.Get("canvas")
        #This is eemm channel
        hMat = canvas.GetListOfPrimitives().FindObject("%s" %(Matrix[varName]))
        unnormalizeBins(hMat)
        hMat.SetDirectory(0)
        # scaleDown is for the UPPER error, scaleUp is for the LOWER
        hMat_Up = canvas.GetListOfPrimitives().FindObject("%s__scaleDown" %(Matrix[varName]))
        unnormalizeBins(hMat_Up)
        hMat_Up.SetDirectory(0)
        # scaleDown is for the UPPER error, scaleUp is for the LOWER
        hMat_Dn = canvas.GetListOfPrimitives().FindObject("%s__scaleUp" %(Matrix[varName]))
        unnormalizeBins(hMat_Dn)
        hMat_Dn.SetDirectory(0)
        fMat.Close()
    for chan in channels:
        print "channel: ",chan
        print "hUnfolded: ",hUnfolded
        print "hTrue: ",hTrue
        if chan == "eeee" or chan =="mmmm":
            hMatrix = hMat.Clone()
            hMatrix.Scale(0.5*args['lumi'])
            hMatrix_Up = hMat_Up.Clone()
            hMatrix_Up.Scale(0.5*args['lumi'])
            hMatrix_Dn = hMat_Dn.Clone()
            hMatrix_Dn.Scale(0.5*args['lumi'])
        else:
            hMatrix = hMat.Clone()
            hMatrix.Scale(args['lumi'])
            hMatrix_Up = hMat_Up.Clone()
            hMatrix_Up.Scale(args['lumi'])
            hMatrix_Dn = hMat_Dn.Clone()
            hMatrix_Dn.Scale(args['lumi'])
        #rebin hMatrix histogram
        hMatrix=rebin(hMatrix,varName)
        hMatrix_Up=rebin(hMatrix,varName)
        hMatrix_Dn=rebin(hMatrix,varName)
        UnfoldOutDir=UnfoldDir+"/"+chan+"/plots"
        if chan not in UnfoldOutDirs:
            UnfoldOutDirs[chan]=UnfoldOutDir
        if not os.path.exists(UnfoldOutDir):
            mkdir(UnfoldOutDir)
        hUnfolded[chan] = fUse.Get(chan+"_"+varName+"_unf")
        hTrue[chan] = fUse.Get(chan+"_"+varName+"_true")
        hTrueAlt[chan] = fUse.Get(chan+"_"+varName+"_trueAlt")
        print("returning unfolded? ",hUnfolded[chan])
        print("returning truth? ",hTrue[chan])
        print("returning Alt truth? ",hTrueAlt[chan])
        #Get the total UncUp and total UncDown histograms from the file as well
        hUncUp = fUse.Get(chan+"_"+varName+"_totUncUp") 
        hUncDn = fUse.Get(chan+"_"+varName+"_totUncDown")
        print "UnfoldOutDir:",UnfoldOutDir
        generatePlots(hUnfolded[chan],hUncUp,hUncDn,hTrue[chan],hTrueAlt[chan],hMatrix,hMatrix_Up,hMatrix_Dn,varName,norm,normFb,args['lumi'],UnfoldOutDir)
    
    if args['makeTotals']:
        #Now access the histograms for all channels combined together
        #While saving in makeResponseMatrix.py, make a "total" category as wel
        hUnfTot = fUse.Get("tot_"+varName+"_unf") 
        hTrueTot = fUse.Get("tot_"+varName+"_true")
        hTrueAltTot = fUse.Get("tot_"+varName+"_trueAlt") 
        hTotUncUp = fUse.Get("tot_"+varName+"_totUncUp")
        hTotUncDn = fUse.Get("tot_"+varName+"_totUncDown") 
        UnfoldOutDir=UnfoldDir+"/"+"tot"+"/plots"
        if "tot" not in UnfoldOutDirs:
            UnfoldOutDirs["tot"]=UnfoldOutDir
        if not os.path.exists(UnfoldOutDir):
            mkdir(UnfoldOutDir)
        hMatrixTot = hMat.Clone()
        hMatrixTot.Scale(2*args['lumi'])
        #Up and Down
        hMatrixTot_Up = hMat_Up.Clone()
        hMatrixTot_Up.Scale(2*args['lumi'])
        hMatrixTot_Dn = hMat_Dn.Clone()
        hMatrixTot_Dn.Scale(2*args['lumi'])
        #rebin hMatrix histogram and Up,Down
        hMatrixTot=rebin(hMatrixTot,varName)
        hMatrixTot_Up=rebin(hMatrixTot_Up,varName)
        hMatrixTot_Dn=rebin(hMatrixTot_Dn,varName)
        generatePlots(hUnfTot,hTotUncUp,hTotUncDn,hTrueTot,hTrueAltTot,hMatrixTot,hMatrixTot_Up,hMatrixTot_Dn,varName,norm,normFb,args['lumi'],UnfoldOutDir)
#Show plots nicely on my webpages
for cat in ["tot"]:   
#for cat in ["eeee","eemm","mmmm","tot"]:   
    #This is where we put all the plots in html format for quick access/debugging
    makeSimpleHtml.writeHTML(os.path.expanduser(UnfoldOutDirs[cat].replace("/plots", "")), "Unfolded Distributions (from MC)")
fUse.Close()
#exit(0)

#if __name__ == "__main__":
#    main()
