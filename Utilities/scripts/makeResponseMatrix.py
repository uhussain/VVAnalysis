#!/usr/bin/env python
import ROOT
import glob
import math
from python import SelectorTools
from python import UserInput
from python import OutputTools
from python import ConfigureJobs
from python import HistTools
import makeSimpleHtml
from PlotTools import PlotStyle as Style, pdfViaTex
from PlotTools import makeLegend, addPadsBelow, makeRatio, fixRatioAxes 
import os
import sys
import datetime
import array
from ROOT import vector as Vec
VFloat = Vec('float')

style = Style()
ROOT.gStyle.SetLineScalePS(1.8)

channels = ["eeee","eemm","mmmm"]
#channels = ["mmmm"]
def getComLineArgs():
    parser = UserInput.getDefaultParser()
    parser.add_argument("--proof", "-p", 
        action='store_true', help="Don't use proof")
    parser.add_argument("--lumi", "-l", type=float,
        default=41.5, help="luminosity value (in fb-1)")
    parser.add_argument("--output_file", "-o", type=str,
        default="", help="Output file name")
    parser.add_argument("--scalefactors_file", "-sf", type=str,
        default="", help="ScaleFactors file name")
    parser.add_argument("--leptonSelections", "-ls", type=str,
        default="TightLeptons", help="Either All Loose or Tight")
    parser.add_argument("--test", action='store_true',
        help="Run test job (no background estimate)")
    parser.add_argument("--output_selection", type=str,
        default="", help="Selection stage of output file "
        "(Same as input if not give)")
    parser.add_argument("-b", "--hist_names", 
                        type=lambda x : [i.strip() for i in x.split(',')],
                        default=["all"], help="List of histograms, "
                        "as defined in ZZ4lAnalysisDatasetManager, separated "
                        "by commas")
    parser.add_argument("--variable", "-vr", type=str,
        default="all", help="variableName")
    #parser.add_argument('--variables', type=str, nargs='*',
    #                    default="Mass",
    #                    help=('Names of variables to use. Options are: {}. '
    #                          'If not specified, all are used except massFull'
    #                          ).format(', '.join(varList)))
    parser.add_argument('--noNorm', action='store_true',
                        help='Leave differential cross sections in abolute normalization rather than normalizing to unity area.')
    parser.add_argument('--plotResponse', action='store_true',
                        help='plot Response Matrices and covariance matrices.')
    parser.add_argument('--logy', '--logY', '--log', action='store_true',
                        help='Put vertical axis on a log scale.')
    parser.add_argument('--plotDir', type=str, nargs='?',
                        default='/afs/cern.ch/user/u/uhussain/www/UWVVPlots/DataUnfolding/Era2017_SystUncert/RooIterMat2017',
                        help='Directory to put response and covariance plots in')
    parser.add_argument('--unfoldDir', type=str, nargs='?',
                        default='/afs/cern.ch/user/u/uhussain/www/UWVVPlots/DataUnfolding/Era2017_SystUncert/RooIterDistributions2017',
                        help='Directory to put response and covariance plots in')
    parser.add_argument('--nIter', type=int, nargs='?', default=4,
                        help='Number of iterations for D\'Agostini method')
    return vars(parser.parse_args())

def getHistExpr(hist_names, selection):
    info = ROOT.TList()
    info.SetName("histinfo")
    for hist_name in hist_names:
        bin_info = ConfigHistTools.getHistBinInfo(manager_path, selection, hist_name)
        if "TH1" in ConfigHistTools.getHistType(manager_path, selection, hist_name):
            bin_expr = "{nbins}, {xmin}, {xmax}".format(**bin_info)
        else:
            bin_expr = "{nbinsx}, {xmin}, {xmax}, {nbinsy}, {ymin}, {ymax}".format(**bin_info)
        info.Add(ROOT.TNamed(hist_name, " $ ".join([hist_name, bin_expr])))
    return info

ROOT.gROOT.SetBatch(True)

args = getComLineArgs()


manager_path = ConfigureJobs.getManagerPath()
sys.path.append("/".join([manager_path, 
    "ZZ4lAnalysisDatasetManager", "Utilities/python"]))
import ConfigHistTools 

today = datetime.date.today().strftime("%d%b%Y")
tmpFileName = "HistFiles/Hists%s-%s.root" % (today, args['leptonSelections']) if args['output_file'] == "" \
        else args['output_file']
if not args['test']:
    fOut = ROOT.TFile(tmpFileName, "recreate")

fScales = ROOT.TFile(args['scalefactors_file'])
mZZTightFakeRate = fScales.Get("mZZTightFakeRate")
eZZTightFakeRate = fScales.Get("eZZTightFakeRate")
if mZZTightFakeRate:
    #print "Yes muon fake rates"
    mZZTightFakeRate.SetName("fakeRate_allMu")
if eZZTightFakeRate:
    #print "Yes electron fake rates"
    eZZTightFakeRate.SetName("fakeRate_allE")
#
muonMoriond18SF= fScales.Get('muonMoriond18SF')
#Electron Reco SF (POG) https://twiki.cern.ch/twiki/bin/viewauth/CMS/Egamma2017DataRecommendations
electronLowReco18SF = fScales.Get('electronLowReco18SF')
electronReco18SF = fScales.Get('electronReco18SF')
#Electron ID + SIP HZZ Scale factors
electronMoriond18SF = fScales.Get('electronMoriond18SF')
electronMoriond18GapSF = fScales.Get('electronMoriond18GapSF')
pileupSF = fScales.Get('pileupSF')

fr_inputs = [eZZTightFakeRate, mZZTightFakeRate,]
sf_inputs = [electronLowReco18SF,electronReco18SF,electronMoriond18SF, electronMoriond18GapSF,muonMoriond18SF,pileupSF]
#sf_inputs=[pileupSF]
#selection = args['selection'].replace("LooseLeptons", "") \
#    if args['output_selection'] == "" else args['output_selection'].split("_")[0]
selection = args['selection']
if selection == "":
    selection = "LooseLeptons"
    print "Info: Using BasicZZSelections for hist defintions"
analysis = "/".join([args['analysis'], selection])
hists = ConfigHistTools.getAllHistNames(manager_path, analysis) \
    if "all" in args['hist_names'] else args['hist_names']

hists = [h for h in hists if "unrolled" not in h and "wCR" not in h and h not in  ["YieldByChannel", "CutFlow"]]
#print "hists: ", hists
hist_inputs = [getHistExpr(hists, analysis)]
#print "hist_inputs: ",hist_inputs
tselection = [ROOT.TNamed("selection", args['output_selection'])]
#print "tselection: ",tselection

if args['proof']:
    ROOT.TProof.Open('workers=12')
# set up variables, selection, binnings etc.
# (jet-related variables and selections done later)
#variables = {
#    'pt' : {c:'Pt' for c in channels},
#    'mass' : {c:'Mass' for c in channels},
#    'massFull' : {c:'Mass' for c in channels},
#    'eta' : {c:'abs(Eta)' for c in channels},
#    'z1mass' : {'eeee':'e1_e2_Mass', 'mmmm':'m1_m2_Mass',
#                'eemm':['e1_e2_Mass','m1_m2_Mass']},
#    'z2mass' : {'eeee':'e3_e4_Mass', 'mmmm':'m3_m4_Mass',
#                'eemm':['m1_m2_Mass','e1_e2_Mass']},
#    'z1pt' : {'eeee':'e1_e2_Pt', 'mmmm':'m1_m2_Pt',
#              'eemm':['e1_e2_Pt','m1_m2_Pt']},
#    'z2pt' : {'eeee':'e3_e4_Pt', 'mmmm':'m3_m4_Pt',
#              'eemm':['m1_m2_Pt','e1_e2_Pt']},
#    'zHigherPt' : {
#        'eeee' : ['e1_e2_Pt', 'e3_e4_Pt'],
#        'mmmm' : ['m1_m2_Pt', 'm3_m4_Pt'],
#        'eemm' : ['e1_e2_Pt','m1_m2_Pt']
#        },
#    'zLowerPt' : {
#        'eeee' : ['e1_e2_Pt', 'e3_e4_Pt'],
#        'mmmm' : ['m1_m2_Pt', 'm3_m4_Pt'],
#        'eemm' : ['e1_e2_Pt','m1_m2_Pt']
#        },
#    'zPt' : {
#        'eeee' : ['e1_e2_Pt', 'e3_e4_Pt'],
#        'mmmm' : ['m1_m2_Pt', 'm3_m4_Pt'],
#        'eemm' : ['e1_e2_Pt','m1_m2_Pt']
#        },
#    'leppt' : {
#        'eeee' : ['e1Pt', 'e2Pt', 'e3Pt', 'e4Pt'],
#        'eemm' : ['e1Pt', 'e2Pt', 'm1Pt', 'm2Pt'],
#        'mmmm' : ['m1Pt', 'm2Pt', 'm3Pt', 'm4Pt'],
#        },
#    'l1Pt' : {
#        'eeee' : 'max(e1Pt, e3Pt)',
#        'eemm' : 'max(e1Pt, m1Pt)',
#        'mmmm' : 'max(m1Pt, m3Pt)',
#        },
#    }


_binning = {
    'pt' : [25.*i for i in range(4)] + [100., 150., 200., 300.],
    #'mass' : [100.+100.*i for i in range(12)],
    'mass' : [100.] + [200.+50.*i for i in range(5)] + [500.,600.,800.],
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
    }

prettyVars = {
    'pt' : 'p_{T}^{4\\ell}',
    'mass' : 'm_{4\\ell}',
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
    'leppt' : 'p_{T}^{\\ell}',
    'l1Pt' : 'p_\\text{T}^{\\ell_1}',
    }


# Names of compiled C++ classes to make response matrices fast
# (this is extremely slow in Python because it requires a combination of
# information from multiple trees, which can't be done with TTree::Draw())
responseClassNames = {
    'mass' : {c:'FloatBranchResponseMatrixMaker' for c in channels},
    #'massFull' : {c:'FullSpectrumFloatResponseMatrixMaker' for c in channels},
    'pt' : {c:'FloatBranchResponseMatrixMaker' for c in channels},
    'eta' : {c:'AbsFloatBranchResponseMatrixMaker' for c in channels},
    'z1mass' : {'eeee':'FloatBranchResponseMatrixMaker',
                'mmmm':'FloatBranchResponseMatrixMaker',
                'eemm':'Z1ByMassResponseMatrixMaker',},
    'z2mass' : {'eeee':'FloatBranchResponseMatrixMaker',
                'mmmm':'FloatBranchResponseMatrixMaker',
                'eemm':'Z2ByMassResponseMatrixMaker',},
    'z1pt' : {'eeee':'FloatBranchResponseMatrixMaker',
              'mmmm':'FloatBranchResponseMatrixMaker',
              'eemm':'Z1ByMassResponseMatrixMaker',},
    'z2pt' : {'eeee':'FloatBranchResponseMatrixMaker',
              'mmmm':'FloatBranchResponseMatrixMaker',
              'eemm':'Z2ByMassResponseMatrixMaker',},
    #'zHigherPt' : {c:'Z1ByPtResponseMatrixMaker' for c in channels},
    #'zLowerPt' : {c:'Z2ByPtResponseMatrixMaker' for c in channels},
    'leppt' : {c:'AllLeptonBranchResponseMatrixMaker' for c in channels},
    #'l1Pt' : {c:'LeptonMaxBranchResponseMatrixMaker' for c in channels},
    'zpt' : {c:'BothZsBranchResponseMatrixMaker' for c in channels},
    'zmass' : {c:'BothZsBranchResponseMatrixMaker' for c in channels},
    }

# Variable names usable by response maker classes
varNamesForResponseMaker = {
    'mass' : {c:'Mass' for c in channels},
    #'massFull' : {c:'Mass' for c in channels},
    'pt' : {c:'Pt' for c in channels},
    'eta' : {c:'Eta' for c in channels},
    'z1mass' : {'eeee':'e1_e2_Mass','mmmm':'m1_m2_Mass','eemm':'Mass'}, # 4e/4mu just use 1 variable because that's easy
    'z2mass' : {'eeee':'e3_e4_Mass','mmmm':'m3_m4_Mass','eemm':'Mass'}, # for 2e2mu, the response maker class will figure it out
    'z1pt' : {'eeee':'e1_e2_Pt','mmmm':'m1_m2_Pt','eemm':'Pt'}, # 4e/4mu just use 1 variable because that's easy
    'z2pt' : {'eeee':'e3_e4_Pt','mmmm':'m3_m4_Pt','eemm':'Pt'}, # for 2e2mu, the response maker class will figure it out
    'zpt' : {c:'Pt' for c in channels},
    'zmass' : {c:'Mass' for c in channels},
    #'zHigherPt' : {c:'Pt' for c in channels},
    #'zLowerPt' : {c:'Pt' for c in channels},
    'leppt' : {c:'Pt' for c in channels},
    #'l1Pt' : {c:'Pt' for c in channels},
    #'zPt' : {c:'Pt' for c in channels},
}

# list of variables not counting systematic shifts
varList=['Mass','ZZPt','ZZEta','Z1Mass','Z1Pt','Z2Mass','Z2Pt','ZPt','ZMass','LepPt']

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
    for ib in xrange(1,len(h)+1):
        w = h.GetBinWidth(ib)
        h.SetBinContent(ib, h.GetBinContent(ib) * binUnit / w)
        h.SetBinError(ib, h.GetBinError(ib) * binUnit / w)
        if h.GetBinError(ib) > h.GetBinContent(ib):
            h.SetBinError(ib, h.GetBinContent(ib))
    h.Sumw2()

def unnormalizeBins(h):
    binUnit = 1 # min(h.GetBinWidth(b) for b in range(1,len(h)+1))
    for ib in xrange(1,len(h)+1):
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
    for i in range(Nbins+1):
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
        Ratio.SetBinContent(i,ratiocontent)
        Ratio.SetBinError(i,error)

    Ratio.GetYaxis().SetRangeUser(0.5,1.5)
    Ratio.SetStats(0)
    Ratio.GetYaxis().CenterTitle()
    Ratio.SetMarkerStyle(20)
    Ratio.SetMarkerSize(0.7)

    line = ROOT.TLine(h1.GetXaxis().GetXmin(), 1.,h1.GetXaxis().GetXmax(), 1.)
    line.SetLineStyle(7)

    Ratio.GetYaxis().SetLabelSize(0.14)
    Ratio.GetYaxis().SetTitleSize(0.12)
    Ratio.GetYaxis().SetLabelFont(42)
    Ratio.GetYaxis().SetTitleFont(42)
    Ratio.GetYaxis().SetTitleOffset(0.25)
    Ratio.GetYaxis().SetNdivisions(100)
    Ratio.GetYaxis().SetTickLength(0.05)

    Ratio.GetXaxis().SetLabelSize(0)
    Ratio.GetXaxis().SetTitleSize(0)
    #Ratio.GetXaxis().SetLabelFont(42)
    #Ratio.GetXaxis().SetTitleFont(42)
    #Ratio.GetXaxis().SetTitleOffset(0.90)
    #Ratio.GetXaxis().SetTickLength(0.05)
    #Ratio.Draw("pex0")
    #line.SetLineColor(kBlack)
    #line.Draw("same");

    return Ratio,line

def createCanvasPads():
    c = ROOT.TCanvas("c", "canvas", 800, 800)
    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetLegendBorderSize(0)
    # Upper histogram plot is pad1
    pad1 = ROOT.TPad("pad1", "pad1", 0.01, 0.25, 0.99, 0.99)
    pad1.Draw()
    pad1.cd()
    pad1.SetFillColor(0)
    pad1.SetFrameBorderMode(0)
    pad1.SetBorderMode(0)
    pad1.SetBottomMargin(0)  # joins upper and lower plot
    #pad1.SetGridx()
    #pad1.Draw()
    return c,pad1

def createPad2(canvas):
    # Lower ratio plot is pad2
    canvas.cd()  # returns to main canvas before defining pad2
    pad2 = ROOT.TPad("pad2", "pad2", 0.01, 0.01, 0.99, 0.25)
    pad2.Draw()
    pad2.cd()
    pad2.SetFillColor(0)
    pad2.SetFrameBorderMode(0)
    pad2.SetBorderMode(0)
    pad2.SetTopMargin(0)  # joins upper and lower plot
    pad2.SetBottomMargin(0.2)
    #pad2.SetGridx()
    #pad2.Draw()
    return pad2

def generateAnalysisInputs():    
    #dictionary of SF histograms
    hSF = {}
    eLowRecoFile = ROOT.TFile.Open('data/egammaEffi.txt_EGM2D_runBCDEF_passingRECO_lowEt.root')
    hSF['eLowReco'] = eLowRecoFile.Get('EGamma_SF2D').Clone()
    hSF['eLowReco'].SetDirectory(0)
    eLowRecoFile.Close()
    
    eRecoFile = ROOT.TFile.Open('data/egammaEffi.txt_EGM2D_runBCDEF_passingRECO.root')
    hSF['eReco'] = eRecoFile.Get('EGamma_SF2D').Clone()
    hSF['eReco'].SetDirectory(0)
    eRecoFile.Close()
    
    eIdFile = ROOT.TFile.Open('data/egammaEffi.txt_EGM2D_Moriond2018v1.root')
    hSF['eSel'] = eIdFile.Get('EGamma_SF2D').Clone() 
    hSF['eSel'].SetDirectory(0)
    eIdFile.Close()

    eIdGapFile = ROOT.TFile.Open('data/egammaEffi.txt_EGM2D_Moriond2018v1_gap.root')
    hSF['eSelGap'] = eIdGapFile.Get('EGamma_SF2D').Clone() 
    hSF['eSelGap'].SetDirectory(0)
    eIdGapFile.Close()

    mIdFile = ROOT.TFile.Open('data/ScaleFactors_mu_Moriond2018_final.root')
    hSF['m'] = mIdFile.Get('FINAL').Clone()
    hSF['m'].SetDirectory(0)

    hSF['mErr'] = mIdFile.Get('ERROR').Clone()
    hSF['mErr'].SetDirectory(0)
    mIdFile.Close()

    return hSF
#ROOT.gSystem.Load('Utilities/scripts/ResponseMatrixMaker_cxx')
#sigSamples is a dictionary containing sample names and kfactor*cross-section
#sumW is a dictionary with sigsample:sumweights stored
ROOT.gSystem.Load('Utilities/scripts/ResponseMatrixMaker_cxx')
def generateResponseClass(varName, channel,sigSamples,sigSamplesPath,sumW,hSF={}):
    
    className = responseClassNames[varName][channel]

    if hSF:
        className = 'SFHist'+className

    #if not hasattr(ROOT,className):
    #        ROOT.gSystem.Load('Utilities/scripts/ResponseMatrixMaker_cxx','kTRUE')
    
    #for example C=<class 'ROOT.BranchValueResponseMatrixMaker<float>'>     
    C = getattr(ROOT, className)
    print("className:",C)
    
    #filelist=["zz4l-powheg"]
    filelist=[str(i) for i in sigSamples.keys()] 
    #improve this by getting this info from ZZDatasetManager just like its done in makeCompositeHists
    #sigConstWeights = {sample : (1.256*41500*1.0835)/sumW
    #                   for sample in ConfigureJobs.getListOfFiles(filelist, selection)}
   
    sigConstWeights = {sample : (sigSamples[sample.split("__")[0]]*1000*args['lumi'])/sumW[sample]
                       for sample in [str(i) for i in sigSamples.keys()] }
    #print "sigConstWeights: ",sigConstWeights
    #print("_binning: ",_binning)
    binning = _binning[varName]
    vBinning = VFloat()
    ROOT.SetOwnership(vBinning,False)
    #print("vBinning: ",vBinning)
    #print("Content of the ROOT vector object: {}".format([x for x in vBinning]))
    #print("binning: ",binning)
    if len(binning) == 3:
        binningTemp = [binning[1] + i * (binning[2] - binning[1])/float(binning[0]) for i in xrange(binning[0]+1)]
        #print("binningTemp: ",binningTemp)
        for b in binningTemp:
            #print("b: ",b)
            vBinning.push_back(b)
    else:
        for b in binning:
            vBinning.push_back(b)

    print("Content of the ROOT vector object: {}".format([x for x in vBinning]))
    #print("vBinning: ",vBinning)
    responseMakers = {}
    #for sample, file_path in sigFileNames.items():
    #for sample in ConfigureJobs.getListOfFiles(filelist,selection):
    for sample in sigSamplesPath.keys():
        #print "sample:", sample #expect zz4l-powheg
        #file_path = ConfigureJobs.getInputFilesPath(sample,selection,analysis)
        file_path=sigSamplesPath[sample]
        #print("where are the histos leaking")
        resp = C(channel, varNamesForResponseMaker[varName][channel], vBinning)
        file_path=file_path.encode("utf-8")
        #print "file_path: ",file_path
        fileList=glob.glob(file_path)
        #print "type in fileList should be str: ",type(fileList[0])
        for fName in fileList:
            resp.registerFile(fName)
        resp.setConstantScale(sigConstWeights[sample])
        if hSF:
            resp.registerElectronSelectionSFHist(hSF['eSel'])
            resp.registerElectronSelectionGapSFHist(hSF['eSelGap'])
            resp.registerElectronLowRecoSFHist(hSF['eLowReco'])
            resp.registerElectronRecoSFHist(hSF['eReco'])
            resp.registerMuonSFHist(hSF['m'])
            resp.registerMuonSFErrorHist(hSF['mErr'])
            #print("scale factors are being added")

        responseMakers[sample] = resp
        #print "resp: ",resp
        ROOT.SetOwnership(resp,False)
    return responseMakers

_printCounter = 0
#Load the RooUnfold library into ROOT
ROOT.gSystem.Load("RooUnfold/libRooUnfold")
def unfold(varName,chan,responseMakers,hSigDic,hSigSystDic,hTrueDic,hDataDic,hbkgDic,hbkgMCDic,hbkgMCSystDic,nIter,hSF,plotDir=''):
    global _printCounter
    #get responseMakers from the function above- this is the whole game.
    #responseMakers = generateResponseClass(varName, chan,sigSamples,sumW,hSF)

    # outputs
    hUnfolded = {}
    hTruth={}
    hResponseNominal={}
    #print "responseMakers: ",responseMakers
    hResponseNominal = {s:resp for s,resp in responseMakers.items()}
    #print "hResponseNominal:",hResponseNominal
    
    #This will pop the powheg response matrix from the hResponseNominal Dictionary
    hResponseNominalTotal = hResponseNominal.pop("zz4l-powheg")
    #print "hRespNominalTotal: ",hResponseNominalTotal
    #This gets us the response matrix as a TH2D for "zz4l-powheg"
    hResponse = hResponseNominalTotal.getResponse()
    hResponse.SetDirectory(0)
    #print("where are all the leaks") 
    #print "type of hResp: " ,hResponse
    #Now we need to add the rest of the response matrices (MCFMs) to this POHWEG matrix
    #Looping over the values of the dictionary (it doesn't have powheg anymore)
    #print "hResponseNominal after zz-pohwheg:",hResponseNominal
    for response in hResponseNominal.values():
        respMat = response.getResponse()
        #ROOT.SetOwnership(response,False)
        hResponse.Add(respMat)
        #ROOT.SetOwnership(respMat,False)  
        respMat.SetDirectory(0)
        del respMat
        #respMat.Delete()

    #print ("The leaks happen in this for loop")
    #hResponseNominalTotal = sum(resp for resp in hResponseNominal.values())
    

    print "type of Total hResp: " ,hResponse
    # But its better to use RooUnfoldResponse here
    #RooUnfoldResponse constructor - create from already-filled histograms
    # "response" gives the response matrix, measured X truth.
    #  "measured" and "truth" give the projections of "response" onto the X-axis and Y-axis respectively,
    #   but with additional entries in "measured" for measurements with no corresponding truth (fakes/background) and
    #    in "truth" for unmeasured events (inefficiency).
    #     "measured" and/or "truth" can be specified as 0 (1D case only) or an empty histograms (no entries) as a shortcut
    #      to indicate, respectively, no fakes and/or no inefficiency.


    ## Give hSig and hTrue in the form of histograms
    varNames={'zmass':'ZMass','mass': 'Mass','pt':'ZZPt','eta':'ZZEta','z1mass':'Z1Mass','z1pt':'Z1Pt','z2mass':'Z2Mass','z2pt':'Z2Pt','zpt':'ZPt','leppt':'LepPt'}

    hSigNominal = hSigDic[chan][varNames[varName]]
    print "sigHist: ", hSigNominal,", ",hSigNominal.Integral()

    hTrue = hTrueDic[chan]["Gen"+varNames[varName]]
    #histTrue.Scale((1.256*41500*1.0835)/zzSumWeights) 
    print "trueHist: ",hTrue,", ",hTrue.Integral()
    hData = hDataDic[chan][varNames[varName]]
    print "dataHist: ",hData,", ",hData.Integral()
    #Get the background hists - #Get the histName_Fakes_chan histos
    hBkgNominal = hbkgDic[chan][varNames[varName]+"_Fakes"]
    print "NonPromptHist: ",hBkgNominal,", ",hBkgNominal.Integral()
    hBkgMCNominal = hbkgMCDic[chan][varNames[varName]]
    print "VVVHist: ",hBkgMCNominal,", ",hBkgMCNominal.Integral()
    #Add the two backgrounds
    hBkgTotal=hBkgNominal.Clone()
    hBkgTotal.Add(hBkgMCNominal)
    print "TotBkgHist: ",hBkgTotal,", ",hBkgTotal.Integral()

   
    #No need to rebin certain variables
    if varNames[varName] not in ['ZZEta']:
        bins=array.array('d',_binning[varName])
        hSigNominal=hSigNominal.Rebin(len(bins)-1,"",bins)
        hTrue=hTrue.Rebin(len(bins)-1,"",bins)
        hData=hData.Rebin(len(bins)-1,"",bins)
        hBkgTotal=hBkgTotal.Rebin(len(bins)-1,"",bins)
    
    xaxisSize = hSigNominal.GetXaxis().GetTitleSize()
    yaxisSize = hTrue.GetXaxis().GetTitleSize()
    #print "xaxisSize: ",xaxisSize
    
    print "TotBkgHist after rebinning: ",hBkgTotal,", ",hBkgTotal.Integral()
    hTruth['']=hTrue

    hUnfolded[''], hCov, hResp = getUnfolded(hSigNominal,hBkgTotal,hTruth[''],hResponse,hData, nIter,True)

    #hUnfolded[''] = getUnfolded(hSig,
    #                                          hBkg,
    #                                          hTruth[''],
    #                                          hResponse,
    #                                          hData, nIter)
    print "hUnfolded['']: ",hUnfolded['']
    #print("hResp: ",hResp) 
    #del hResponse
    # plot covariance and response
    if plotDir and args['plotResponse']:
        cRes = ROOT.TCanvas("c","canvas",1200,1200)
        if varName == 'massFull':
            cRes.SetLogx()
            cRes.SetLogy()
        draw_opt = "colz text45"
        hResp.GetXaxis().SetTitle('Reco '+prettyVars[varName]+''+units[varName])
        hResp.GetYaxis().SetTitle('True '+prettyVars[varName]+''+units[varName])
        hResp.GetXaxis().SetTitleSize(0.75*xaxisSize)
        hResp.GetYaxis().SetTitleSize(0.75*yaxisSize)
        hResp.Draw(draw_opt)
        style.setCMSStyle(cRes, '', dataType='  Internal', intLumi=41500.)
        #print "plotDir: ",plotDir
        cRes.Print("%s/response_%s.png" % (plotDir,varName))
        cRes.Print("%s/response_%s.pdf" % (plotDir,varName))
    
        del cRes
        #cCov = ROOT.TCanvas("c","canvas",1200,1200)
        #if varName == 'massFull':
        #    cCov.SetLogx()
        #    cCov.SetLogy()
        #draw_opt = "colz text45"
        #hCov.Draw(draw_opt)
        #style.setCMSStyle(cCov, '', dataType='Internal', intLumi=41500.)
        #cCov.Print("%s/covariance_%s.png" % (plotDir,varName))
        #cCov.Print("%s/covariance_%s.pdf" % (plotDir,varName))
        #del cCov
    
    # lepton efficiency uncertainty
    for lep in set(chan):
        hResponseSyst = {s:resp for s,resp in responseMakers.items()}
        hRespSystTot = hResponseSyst.pop("zz4l-powheg")
        for sys in ['Up','Down']:
            hRespSyst = hRespSystTot.getResponse(lep+'Eff_'+sys)
            hRespSyst.SetDirectory(0)
            for response in hResponseSyst.values():
                respMatSyst = response.getResponse(lep+'Eff_'+sys)
                hRespSyst.Add(respMatSyst)
                respMatSyst.SetDirectory(0)
                del respMatSyst
            print "hSigSystDic: ",hSigSystDic
            hSigSyst = hSigSystDic[chan][varNames[varName]+"_CMS_eff_"+lep+sys]
            hSigSyst.SetDirectory(0)
            print lep+'Eff_'+sys 
            print "sigHist: ", hSigSyst,", ",hSigSyst.Integral()
            hBkgSyst = hbkgDic[chan][varNames[varName]+"_Fakes"]
            hBkgSyst.SetDirectory(0)
            print "NonPromptHist: ",hBkgSyst,", ",hBkgSyst.Integral()
            hBkgMCSyst = hbkgMCSystDic[chan][varNames[varName]+"_CMS_eff_"+lep+sys]
            hBkgMCSyst.SetDirectory(0)
            print "VVVHist: ",hBkgMCSyst,", ",hBkgMCSyst.Integral()
            hBkgSystTotal=hBkgSyst.Clone()
            hBkgSystTotal.Add(hBkgMCSyst)
            print "TotBkgHist: ",hBkgSystTotal,", ",hBkgSystTotal.Integral()
            #print "sigHist: ", hSigNominal,", ",hSigNominal.Integral()
            if varNames[varName] not in ['ZZEta']:
                bins=array.array('d',_binning[varName])
                hSigSyst=hSigSyst.Rebin(len(bins)-1,"",bins)
                hBkgSystTotal=hBkgSystTotal.Rebin(len(bins)-1,"",bins)
            print "TotBkgHist: ",hBkgSystTotal,", ",hBkgSystTotal.Integral()
            
            hUnfolded[lep+'Eff_'+sys] = getUnfolded(hSigSyst,
                                                     hBkgSystTotal,
                                                     hTruth[''],
                                                     hRespSyst,
                                                     hData, nIter)
            del hSigSyst
            del hBkgMCSyst
            del hBkgSyst
            if plotDir and args['plotResponse']:
                cResSyst = ROOT.TCanvas("c","canvas",1200,1200)
                if varName == 'massFull':
                    cResSyst.SetLogx()
                    cResSyst.SetLogy()
                draw_opt = "colz text45"
                hRespSyst.GetXaxis().SetTitle('Reco '+prettyVars[varName]+sys+''+units[varName])
                hRespSyst.GetYaxis().SetTitle('True '+prettyVars[varName]+sys+''+units[varName])
                hRespSyst.GetXaxis().SetTitleSize(0.75*xaxisSize)
                hRespSyst.GetYaxis().SetTitleSize(0.75*yaxisSize)
                hRespSyst.Draw(draw_opt)
                style.setCMSStyle(cResSyst, '', dataType='  Internal', intLumi=41500.)
                #print "plotDir: ",plotDir
                cResSyst.Print("%s/response_%s_%s.png" % (plotDir,varName,lep+'Eff_'+sys))
                cResSyst.Print("%s/response_%s_%s.pdf" % (plotDir,varName,lep+'Eff_'+sys))
                del cResSyst
            del hRespSyst

    # make everything local (we'll cache copies)
    for h in hUnfolded.values()+hTruth.values():
        ROOT.SetOwnership(h,False)
        print("histos: ",h) 
        #h.SetDirectory(0)

    return hUnfolded,hTruth
def getUnfolded(hSig, hBkg, hTrue, hResponse, hData, nIter,withRespAndCov=False):
    Response = getattr(ROOT,"RooUnfoldResponse")

    print "SigBeforeResponse: ", hSig,", ",hSig.Integral()
    response = Response(hSig, hTrue.Clone(), hResponse.Clone()) 
    ROOT.SetOwnership(response,False)
    #Response matrix as a 2D-histogram: (x,y)=(measured,truth)
    hResp = response.Hresponse()
    #hResp = hResponse
    #print "hResp out of response: ",hResp

    RooUnfoldIter = getattr(ROOT,"RooUnfoldBayes")

    #RooUnfoldInv = getattr(ROOT,"RooUnfoldInvert")

    #RooUnfoldBinbyBin = getattr(ROOT,"RooUnfoldBinByBin")
    try:
        svd = ROOT.TDecompSVD(response.Mresponse())
        sig = svd.GetSig()
        try:
            condition = sig.Max() / max(0., sig.Min())
        except ZeroDivisionError:
            condition = float('inf')
            raise

        print "channel: ",chan
        print "variable: ",varNames[varName]
        print "hResp out of response: ",hResp
        print ''
        print 'condition: {}'.format(condition)
        print ''

    except:
        print "It broke! Printing debug info"
        print "Sig: {}, bkg: {}, true: {}, response: {}".format(hSig.Integral(), hBkg.Integral(), hTrue.Integral(), hResponse.Integral())
        c = ROOT.TCanvas("c1","canvas",1200,1200)
        hSig.Draw()
        style.setCMSStyle(c, '', dataType='Debug', intLumi=41500.)
        c.Print("DebugPlots/sig{}.png".format(_printCounter))
        hBkg.draw()
        _style.setCMSStyle(c, '', dataType='Debug', intLumi=41500.)
        c.Print("bkg{}.png".format(_printCounter))
        hTrue.Draw()
        style.setCMSStyle(c, '', dataType='Debug', intLumi=41500.)
        c.Print("DebugPlots/true{}.png".format(_printCounter))
        hData.Draw()
        style.setCMSStyle(c, '', dataType='Debug', intLumi=41500.)
        c.Print("DebugPlots/data{}.png".format(_printCounter))
        draw_opt = "colz text45"
        hResponse.Draw(draw_opt)
        style.setCMSStyle(c, '', dataType='Debug', intLumi=41500.)
        c.Print("DebugPlots/resp{}.png".format(_printCounter))
        c.Print("DebugPlots/resp{}.root".format(_printCounter))
        _printCounter += 1

    print "hData: ", hData.Integral()
    hDataMinusBkg = hData.Clone()
    hDataMinusBkg.Add(hBkg,-1)
    #ROOT.SetOwnership(hDataMinusBkg,False)
    print "hBkg: ", hBkg.Integral()
    #hDataMinusBkg.Add(hBkg,-1)
    #HistTools.zeroNegativeBins(hDataMinusBkg)
    print "DataMinusbkgIntegral: ",hDataMinusBkg, ", ",hDataMinusBkg.Integral()
    #Unfolding using 4 iterations and then stopping
    #if varNames[varName] not in ["Z1Mass","Z2Mass"]:
    #    nIter=8
    print "No.of iterations: ",nIter
    print "response: ",response
    unf = RooUnfoldIter(response, hDataMinusBkg, nIter)
    print "unf: ",unf 
    #Simply inverting the matrix
    #unf = RooUnfoldInv(response, hSig)

    #Unfolds using the method of correction factors
    #unf = RooUnfoldBinbyBin(response, hSig)

    #This is the unfolded "data" distribution
    hOut = unf.Hreco()
    #ROOT.SetOwnership(hOut,False)
    if not hOut:
        print hOut
        raise ValueError("The unfolded histogram got screwed up somehow!")
    print("hOut: ",hOut,"",hOut.Integral()) 
    #Returns covariance matrices for error calculation of type withError
    #0: Errors are the square root of the bin content
    #1: Errors from the diagonals of the covariance matrix given by the unfolding
    #2: Errors from the covariance matrix given by the unfolding => We use this one for now
    #3: Errors from the covariance matrix from the variation of the results in toy MC tests
    hCov = unf.Ereco(2)
    #hOut.SetDirectory(0)
    #hResp.SetDirectory(0)
    #ROOT.SetOwnership(hCov,False)
    print("hCov: ",hCov) 
    print("where is the crash happening?")
    #return hCov.Clone(),hResp.Clone()
    #return hOut
    if withRespAndCov:
        return hOut,hCov.Clone(),hResp.Clone()
    
    return hOut

def RatioErrorBand(Ratio,hUncUp,hUncDn,hTrueNoErrs):
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
            #print "errorUp: ",errorUp, "","errorDn: ",errorDn
            ratioGraph.SetPointEYhigh(i-1, errorUp)
            ratioGraph.SetPointEYlow(i-1, errorDn)
        ratioGraph.SetFillColorAlpha(1,0.5)
        ratioGraph.SetFillStyle(3001)
        ratioGraph.GetXaxis().SetLabelSize(0)
        ratioGraph.GetXaxis().SetTitleSize(0)
        ratioGraph.GetYaxis().SetLabelSize(0)
        ratioGraph.GetYaxis().SetTitleSize(0)
        ratioGraph.GetXaxis().SetLimits(Ratio.GetXaxis().GetXmin(),Ratio.GetXaxis().GetXmax())
        ratioGraph.SetMaximum(1.5)
        ratioGraph.SetMinimum(0.5)
        return ratioGraph

def MainErrorBand(hMain,hUncUp,hUncDn):
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
            print "errorUp: ",errorUp, "","errorDn: ",errorDn
            MainGraph.SetPointEYhigh(i-1, errorUp)
            MainGraph.SetPointEYlow(i-1, errorDn)
        MainGraph.SetFillColorAlpha(1,0.7)
        MainGraph.SetFillStyle(3001)
        MainGraph.GetYaxis().SetTitle("Events")
        MainGraph.GetYaxis().SetTitleOffset(1.0)
        #MainGraph.GetXaxis().SetLabelSize(0)
        #MainGraph.GetXaxis().SetTitleSize(0)
        #MainGraph.GetYaxis().SetLabelSize(0)
        #MainGraph.GetYaxis().SetTitleSize(0)
        MainGraph.GetXaxis().SetLimits(hMain.GetXaxis().GetXmin(),hMain.GetXaxis().GetXmax())
        #MainGraph.SetMaximum(1.5)
        #MainGraph.SetMinimum(0.5)
        return MainGraph
def generatePlots(hUnfolded,hUncUp,hUncDn,hTruth,varName,unfoldDir=''):
    UnfHists=[]
    TrueHists=[]
    # for normalization if needed
    nominalArea = hUnfolded.Integral(0,hUnfolded.GetNbinsX()+1)
    # Make uncertainties out of the unfolded histos
    ### plot
    hUnf = hUnfolded.Clone()
    hTrue = hTruth.Clone()
    #print ("hTrue histo here: ",hTrue)
    #print ("unfoldDir: ",unfoldDir)
    xaxisSize = hUnf.GetXaxis().GetTitleSize()
    yaxisSize = hTrue.GetXaxis().GetTitleSize()
    if unfoldDir:
        #print ("What happens here?")
        #cUnfold = ROOT.TCanvas("c","canvas",1200,1200)
        #hUnf.GetXaxis().SetTitle('UnfData '+prettyVars[varName]+''+units[varName])
        #hUnf.GetXaxis().SetTitleSize(0.75*xaxisSize)
        #hUnf.Draw("HIST")
        #UnfHists.append(hUnf)
        #leg = makeLegend(cUnfold,*(UnfHists),**legParams[varName])
        #leg.SetFillStyle(1001)
        #leg.Draw("SAME")
        #style.setCMSStyle(cUnfold, '', dataType='  Work in Progress', intLumi=41500.)
        ##print "unfoldDir: ",unfoldDir
        #cUnfold.Print("%s/UnfDist_%s.png" % (unfoldDir,varName))
        #cUnfold.Print("%s/UnfDist_%s.pdf" % (unfoldDir,varName))
        #del cUnfold

        #cTrue = ROOT.TCanvas("c","canvas",1200,1200)
        #hTrue.GetXaxis().SetTitle('True '+prettyVars[varName]+''+units[varName])
        #hTrue.GetXaxis().SetTitleSize(0.75*yaxisSize)
        #hTrue.SetLineColor(ROOT.kRed)
        #hTrue.Draw("HIST")
        #TrueHists.append(hTrue)
        #leg = makeLegend(cTrue,*(TrueHists),**legParams[varName])
        #leg.SetFillStyle(1001)
        #leg.Draw("SAME")
        #style.setCMSStyle(cTrue, '', dataType='  Work in Progress', intLumi=41500.)
        ##print "unfoldDir: ",unfoldDir
        #cTrue.Print("%s/TrueDist_%s.png" % (unfoldDir,varName))
        #cTrue.Print("%s/TrueDist_%s.pdf" % (unfoldDir,varName))
        #del cTrue
        

        #Create a ratio plot
        c,pad1 = createCanvasPads()

        Unfmaximum = hUnf.GetMaximum()
        hTrue.SetFillColor(ROOT.TColor.GetColor("#99ccff"))
        hTrue.SetLineColor(ROOT.TColor.GetColor('#000099')) 
        hTrue.SetFillStyle(3010)
        print "Total Truth Integral",hTrue.Integral()
        print "Total Unf Data Integral",hUnf.Integral()
        Truthmaximum = hTrue.GetMaximum()
        hTrue.SetLineWidth(2*hTrue.GetLineWidth())
        hTrue.Draw("HIST")
        if(Unfmaximum > Truthmaximum):
            hTrue.SetMaximum(Unfmaximum*1.2)
        else:
            hTrue.SetMaximum(Truthmaximum*1.2)

        hTrue.GetXaxis().SetTitle("")

        UnfErrBand = MainErrorBand(hUnf,hUncUp,hUncDn)
        #print "UnfErrBand: ",UnfErrBand
        UnfErrBand.Draw("a2")
        #UnfErrBand.Draw("PSAME")

        hTrue.GetXaxis().SetLabelSize(0)
        hTrue.GetXaxis().SetTitleSize(0)
        #hTrue.GetYaxis().SetTitle("Events")
        #hTrue.GetYaxis().SetTitleOffset(1.0)
        hTrue.Draw("HISTSAME")
        #hUnf.Sumw2(False)
        #hUnf.SetBinErrorOption(ROOT.TH1.kPoisson)
        hUnf.SetLineColor(ROOT.kBlack)
        hUnf.SetMarkerStyle(20)
        hUnf.SetMarkerSize(0.7)
        hUnf.GetXaxis().SetTitle("")
        hUnf.GetXaxis().SetLabelSize(0)
        hUnf.GetXaxis().SetTitleSize(0)
        hUnf.Draw("PE1SAME")
        

        leg = ROOT.TLegend(0.68,0.54,0.88,0.84,"")
        leg.AddEntry(hUnf,"Data + stat. unc.","lep")
        leg.AddEntry(hTrue, "POWHEG+MCFM+Pythia8","f")
        leg.AddEntry(UnfErrBand, "Stat. #oplus syst. unc.","f")
        leg.SetFillColor(ROOT.kWhite)
        leg.SetFillStyle(0)
        leg.SetTextSize(0.025)
        leg.Draw()

        #SecondPad
        pad2 = createPad2(c)

        hTrueNoErrs = hTrue.Clone() # need central value only to keep ratio uncertainties consistent
        nbins=hTrueNoErrs.GetNbinsX()
        #print(nbins)
        hTrueNoErrs.SetError(array.array('d',[0.]*nbins))
        #Starting the ratio proceedure
        Ratio,line = createRatio(hUnf, hTrueNoErrs)

        ratioErrorBand = RatioErrorBand(Ratio,hUncUp,hUncDn,hTrueNoErrs)
        ratioErrorBand.Draw("a2")
        ratioErrorBand.Draw("p")
        line.SetLineColor(ROOT.kBlack)
        line.Draw("same")

        #redraw axis
        xaxis = ROOT.TGaxis(hUnf.GetXaxis().GetXmin(),0.5,hUnf.GetXaxis().GetXmax(),0.5,hUnf.GetXaxis().GetXmin(),hUnf.GetXaxis().GetXmax(),510)
        xaxis.SetTitle(prettyVars[varName]+''+units[varName])
        xaxis.SetLabelFont(42)
        xaxis.SetLabelSize(0.10)
        xaxis.SetTitleFont(42)
        xaxis.SetTitleSize(0.12)
        xaxis.SetTitleOffset(0.70)
        xaxis.Draw("SAME")

        yaxis = ROOT.TGaxis(hUnf.GetXaxis().GetXmin(),0.5,hUnf.GetXaxis().GetXmin(),1.5,0.5,1.5,6,"")
        yaxis.SetTitle("Data/Theo")
        yaxis.SetLabelFont(42)
        yaxis.SetLabelSize(0.10)
        yaxis.SetTitleFont(42)
        yaxis.SetTitleSize(0.12)
        yaxis.SetTitleOffset(0.35)
        yaxis.Draw("SAME")
        style.setCMSStyle(c, '', dataType='  Work in Progress', intLumi=41500.)
        c.Print("%s/Ratio_%s.png" % (unfoldDir,varName))
        c.Print("%s/Ratio_%s.pdf" % (unfoldDir,varName))
        c.Update()

        del c

def _generateUncertainties(hDict):
    hErr = {'Up':{},'Down':{}}
    for sys, h in hDict.iteritems():
        if not sys:
            continue

        he = h.Clone()

        #Subtract the nominal histogram from the SysUp or Down histogram

        he.Add(hDict[''],-1)
        sysName = sys.replace('_Up','').replace('_Down','')

        if '_Up' in sys:
            hErr['Up'][sysName] = he
        elif '_Down' in sys:
            hErr['Down'][sysName] = he
        else:
            hErr['Up'][sysName] = he
            he2 = he.Clone()
            hErr['Down'][sysName] = he2
    
    return hErr

def _sumUncertainties(errDict,varName):
    if varName == "eta":
        histbins=array.array('d',[0.,1.0,2.0,3.0,4.0,5.0,6.0])
    else:
        histbins=array.array('d',_binning[varName])
    #print "histbins: ",histbins
    hUncUp=ROOT.TH1D("hUncUp","Total Up Uncert.",len(histbins)-1,histbins)
    hUncDn=ROOT.TH1D("hUncDn","Total Dn Uncert.",len(histbins)-1,histbins)
    sysList = errDict['Up'].keys()
    #print "sysList: ",sysList
    #print "hUncUp: ",hUncUp,"",hUncUp.Integral()
    #print "hUncDown: ",hUncDn,"",hUncDn.Integral()
    totUncUp=totUncDn=0.
    UncUpHistos= [errDict['Up'][sys] for sys in sysList]
    UncDnHistos= [errDict['Down'][sys] for sys in sysList]
    for i in range(1,hUncUp.GetNbinsX()+1):
        for h1, h2 in zip(UncUpHistos,UncDnHistos):
            #print "histUp: ",h1,"",h1.Integral()
            #print "histDn: ",h2,"",h2.Integral()
            totUncUp += max(h1.GetBinContent(i),h2.GetBinContent(i))**2
            totUncDn += min(h1.GetBinContent(i),h2.GetBinContent(i))**2

        totUncUp = math.sqrt(totUncUp)
        totUncDn = math.sqrt(totUncDn)
        #print "totUncUp: ",totUncUp
        #print "totUncDn: ",totUncDn
        hUncUp.SetBinContent(i,totUncUp)
        hUncDn.SetBinContent(i,totUncDn)
    print("hUncUp: ",hUncUp,"",hUncUp.Integral()) 
    print("hUncDown: ",hUncDn,"",hUncDn.Integral())

    return hUncUp, hUncDn

def _combineChannelUncertainties(*errDicts):
    hUncTot = {}
    uncList = []
    for errDict in errDicts:
        for sys in ['Up','Down']:
            uncList += errDict[sys].keys()
    uncList = set(uncList)
    print "uncList:",uncList
    for sys in ['Up','Down']:
        hUncTot[sys] = {}
        for unc in uncList:
            if varName == "eta":
                histbins=array.array('d',[0.,1.0,2.0,3.0,4.0,5.0,6.0])
            else:
                histbins=array.array('d',_binning[varName])
            #print "histTot histbins: ",histbins
            histTot=ROOT.TH1D("histTot","Tot Uncert.",len(histbins)-1,histbins)
            ROOT.SetOwnership(histTot,False)
            hUncTot[sys][unc] = histTot
            for errDict in errDicts:
                try:
                    hUncTot[sys][unc].Add(errDict[sys][unc])
                except KeyError:
                    continue

    return hUncTot
#if __name__ == "__main__":

def mkdir(plotDir):
    for outdir in [plotDir]:
        try:
            os.makedirs(os.path.expanduser(outdir))
        except OSError as e:
            print e
            pass

plotDir=args['plotDir']
UnfoldDir=args['unfoldDir']
nIterations=args['nIter']

#varNames={'mass': 'Mass','zmass':'ZMass'}
#varNames={'pt': 'ZZPt','eta':'ZZEta'}
#varNames={'z1mass':'Z1Mass','z1pt':'Z1Pt'}
#varNames={'z2mass':'Z2Mass','z2pt':'Z2Pt'}
#varNames={'zpt':'ZPt','leppt':'LepPt'}
#varNames={'zmass':'ZMass'}
varNames={'zmass':'ZMass','mass': 'Mass','pt':'ZZPt','eta':'ZZEta','z1mass':'Z1Mass','z1pt':'Z1Pt','z2mass':'Z2Mass','z2pt':'Z2Pt','zpt':'ZPt','leppt':'LepPt'}


#Dictionary where signal samples are keys with cross-section*kfactors as values
sigSampleDic=ConfigureJobs.getListOfFilesWithXSec(ConfigureJobs.getListOfEWK())
sigSampleList=[str(i) for i in sigSampleDic.keys()]
print "sigSamples: ",sigSampleList

if not args['test']:
    selectChannels=channels
    #I need the channels split up for my Selectors and histograms
    if "eemm" in channels:
        selectChannels.append("mmee")

    print "selectorChannels: ",selectChannels
    background = SelectorTools.applySelector([args['analysis']+"data"]+ConfigureJobs.getListOfEWK(),selectChannels, 
            "ZZBackgroundSelector", args['selection'], fOut,args['analysis'],
            extra_inputs=sf_inputs+fr_inputs+hist_inputs+tselection, 
            addSumweights=False,
            proof=args['proof'])

    mc = SelectorTools.applySelector(args['filenames'],selectChannels, "ZZSelector", args['selection'], fOut,args['analysis'],
        extra_inputs=sf_inputs+hist_inputs+tselection, 
        addSumweights=True, proof=args['proof'])

    #Get the Gen Histograms
    sigSamplesPath = SelectorTools.applyGenSelector(varList,sigSampleList,selectChannels, "ZZGenSelector", args['selection'], fOut,args['analysis'],
            extra_inputs=hist_inputs+tselection, 
            addSumweights=False, proof=args['proof'])

#Replace fOut with fUse once you have run all the data samples and the backgrounds - right now unfolded data looking really big- subtract backgrounds
if args['test']:
    sigSamplesPath={}
    fUse = ROOT.TFile("HistFiles/Hists28Jun2019-DiffWUnc_full.root","update")
    fOut=fUse
    for dataset in sigSampleList:
        file_path = ConfigureJobs.getInputFilesPath(dataset,selection, analysis)
        sigSamplesPath[dataset]=file_path

#Dictionary where signal samples are keys with cross-section*kfactors as values
#sigSampleDic=ConfigureJobs.getListOfFilesWithXSec(ConfigureJobs.getListOfEWK())
#sigSampleList=[str(i) for i in sigSampleDic.keys()]
#print "sigSamples: ",sigSampleList
#Get the Gen Histograms
#sigSamplesPath = SelectorTools.applyGenSelector(varList,sigSampleList,selectChannels, "ZZGenSelector", args['selection'], fOut,args['analysis'],
#        extra_inputs=hist_inputs+tselection, 
#        addSumweights=False, proof=args['proof'])

#print "sigSamplesPath: ",sigSamplesPath
#Sum all data and return a TList of all histograms that are booked. And an empty datSumW dictionary as there are no sumWeights
alldata,dataSumW = HistTools.makeCompositeHists(fOut,"AllData", 
    ConfigureJobs.getListOfFilesWithXSec([args['analysis']+"data"],manager_path), args['lumi'],
    underflow=False, overflow=False)
if not args['test']:
    OutputTools.writeOutputListItem(alldata, fOut)

#Sum all the signal which is just zz4l-powheg for now, makeCompositeHists will also scale the histogram with cross-section*kfactor*1000*lumi/sumWeights
#allzzPowheg,zzSumW= HistTools.makeCompositeHists(fOut,"zzPowheg", ConfigureJobs.getListOfFilesWithXSec(
#    ConfigureJobs.getListOfSignalFilenames(),manager_path), args['lumi'],
#    underflow=False, overflow=False)

#all ewkmc/this is also allSignal histos, scaled properly, kind of a repeat of above but with ggZZ added
ewkmc,ewkSumW = HistTools.makeCompositeHists(fOut,"AllEWK", ConfigureJobs.getListOfFilesWithXSec(
    ConfigureJobs.getListOfEWK(), manager_path), args['lumi'],
    underflow=False, overflow=False)
if not args['test']:
    OutputTools.writeOutputListItem(ewkmc, fOut)

#all mcbkg that needs to be subtracted
allVVVmc,VVVSumW = HistTools.makeCompositeHists(fOut,"AllVVV", ConfigureJobs.getListOfFilesWithXSec(
    ConfigureJobs.getListOfVVV(), manager_path), args['lumi'],
    underflow=False, overflow=False)

#This is the non-prompt background
ewkcorr = HistTools.getDifference(fOut, "DataEWKCorrected", "AllData", "AllEWK")
#print zzSumW
#print the sum for a sample (zz4l-powheg)
#zzSumWeights = zzSumW["zz4l-powheg"]  

print ewkSumW
#print the sum for a sample (zz4l-powheg)
zzSumWeights = ewkSumW["zz4l-powheg"]  
#print "sumW (zz4l-powheg): ",zzSumWeights

#getHistInDic function also takes care of adding the histograms in eemm+mmee, hence the input here is channels=[eeee,eemm,mmmm]
#dataHists dictionary
hDataDic=OutputTools.getHistsInDic(alldata,varList,channels)

#SigHists dictionary
#hSigDic=OutputTools.getHistsInDic(allzzPowheg,varList,channels)
hSigDic=OutputTools.getHistsInDic(ewkmc,varList,channels)

#TrueHists dictionary
#hTrueDic=OutputTools.getHistsInDic(allzzPowheg,["Gen"+s for s in varList],channels)
hTrueDic=OutputTools.getHistsInDic(ewkmc,["Gen"+s for s in varList],channels)

ewkmcDic=OutputTools.getHistsInDic(ewkmc,varList,channels)

#Non-prompt background dictionary
hbkgDic=OutputTools.getHistsInDic(ewkcorr,[s+"_Fakes" for s in varList],channels)
#strange python debug
print "channels: ",channels
if "mmee" in channels:
    channels.remove("mmee")
#VVV background dictionary
hbkgMCDic=OutputTools.getHistsInDic(allVVVmc,varList,channels)

runVariables=[]
runVariables.append(args['variable'])
print "runVariables: ",runVariables

##Systematic histos
systList=[]
for chan in channels:
    for lep in set(chan):         
        for sys in ["Up","Down"]: 
            for s in runVariables:
                systList.append(varNames[s]+"_CMS_eff_"+lep+sys)

print systList
hSigSystDic=OutputTools.getHistsInDic(ewkmc,systList,channels)

hbkgMCSystDic=OutputTools.getHistsInDic(allVVVmc,systList,channels)

#dictionaries to store output paths
OutputDirs={}
UnfoldOutDirs={}

SFhistos = generateAnalysisInputs()
#So even when selector runs, run the unfolding procedure only on the variables provided.

#for varName in varNames.keys():
for varName in runVariables:
    print "varName:", varNames[varName]
    # save unfolded distributions by channel, then systematic
    hUnfolded = {}
    hTrue = {}
    hErr = {}
    hErrTrue = {}
    for chan in channels:
        print "channel: ",chan
        print "hUnfolded: ",hUnfolded
        print "hTrue: ",hTrue
        OutputDir=plotDir+"/"+chan+"/plots"
        if chan not in OutputDirs:
            OutputDirs[chan]=OutputDir
        if not os.path.exists(OutputDir):
            mkdir(OutputDir)
            OutputDirs[chan]=OutputDir
        UnfoldOutDir=UnfoldDir+"/"+chan+"/plots"
        if chan not in UnfoldOutDirs:
            UnfoldOutDirs[chan]=UnfoldOutDir
        if not os.path.exists(UnfoldOutDir):
            mkdir(UnfoldOutDir)
        #print "varName:", varNames[varName]
        ewkSig = ewkmcDic[chan][varNames[varName]]
        #print "TotSigHist: ", ewkSig,", ",ewkSig.Integral()
        responseMakers = generateResponseClass(varName, chan,sigSampleDic,sigSamplesPath,ewkSumW,SFhistos)
        hUnfolded[chan], hTrue[chan] = unfold(varName,chan,responseMakers,hSigDic,hSigSystDic,hTrueDic,hDataDic,hbkgDic,hbkgMCDic,hbkgMCSystDic,nIterations,SFhistos,OutputDir)
        print("returning unfolded? ",hUnfolded[chan])
        print("returning truth? ",hTrue[chan])
        #print ("UnfoldOutDir: ",UnfoldOutDir)

        hErr[chan]= _generateUncertainties(hUnfolded[chan])
        print "hErr[",chan,"]: ",hErr[chan]
        #(hUncUp, hUncDn) = _sumUncertainties(hErr[chan],varName)
        #hErrTrue[chan] = _generateUncertainties(hTrue[chan])
        #(hTrueUncUp, hTrueUncDn) = _sumUncertainties(hErrTrue[chan],varName)

        #generatePlots(hUnfolded[chan][''],hUncUp,hUncDn,hTrue[chan][''],varName,UnfoldOutDir)
    
    if "eeee" in channels:
        hTot = hUnfolded["eeee"]['']
        hTrueTot = hTrue["eeee"]['']
        #channels.remove("eeee")
    print("channels before adding histos: ",channels)
    for c in ["eemm","mmmm"]:
        hTot.Add(hUnfolded[c][''])
        hTrueTot.Add(hTrue[c][''])
    #Make OutputDir for total (4e+2e2m+4m) unfolded plots
    UnfoldOutDir=UnfoldDir+"/"+"tot"+"/plots"
    if "tot" not in UnfoldOutDirs:
        UnfoldOutDirs["tot"]=UnfoldOutDir
    if not os.path.exists(UnfoldOutDir):
        mkdir(UnfoldOutDir)
    print "hErr.values(): ",hErr.values()
    hErrTot = _combineChannelUncertainties(*hErr.values())
    hTotUncUp, hTotUncDn = _sumUncertainties(hErrTot,varName)
    generatePlots(hTot,hTotUncUp,hTotUncDn,hTrueTot,varName,UnfoldOutDir)

#print(OutputDirs)
#print(UnfoldOutDirs)
#for cat in channels:   
for cat in ["tot"]:   
    #This is where we save all the plots
    #print(os.path.expanduser(OutputDirs[cat].replace("/plots", "")))
    #print(os.path.expanduser(OutputDirs[cat].replace("/plots", "")).split("/")[-1])
    if args['plotResponse'] and cat!="tot":
        makeSimpleHtml.writeHTML(os.path.expanduser(OutputDirs[cat].replace("/plots", "")), "2D ResponseMatrices (from MC)")
    #print(os.path.expanduser(UnfoldOutDirs[cat].replace("/plots", "")).split("/")[-1])
    makeSimpleHtml.writeHTML(os.path.expanduser(UnfoldOutDirs[cat].replace("/plots", "")), "Unfolded Distributions (from MC)")
    #print("it crashes already")

#if args['test']:
#    exit(0)


