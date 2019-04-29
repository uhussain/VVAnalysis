#!/usr/bin/env python
import ROOT
import glob
import math
from python import SelectorTools
from python import UserInput
from python import OutputTools
from python import ConfigureJobs
from python import helpers
import makeSimpleHtml
from PlotTools import PlotStyle as Style, pdfViaTex
from PlotTools import makeLegend, addPadsBelow, makeRatio, fixRatioAxes, makeErrorBand
import os
import sys
import datetime
import array
from ROOT import vector as Vec
VFloat = Vec('float')

style = Style()
ROOT.gStyle.SetLineScalePS(1.8)

#channels = ["eeee", "eemm","mmmm"]
channels = ["eemm"]
Z_MASS = 91.1876
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
    #parser.add_argument('--variables', type=str, nargs='*',
    #                    default="Mass",
    #                    help=('Names of variables to use. Options are: {}. '
    #                          'If not specified, all are used except massFull'
    #                          ).format(', '.join(varList)))
    parser.add_argument('--noNorm', action='store_true',
                        help='Leave differential cross sections in abolute normalization rather than normalizing to unity area.')
    parser.add_argument('--logy', '--logY', '--log', action='store_true',
                        help='Put vertical axis on a log scale.')
    parser.add_argument('--plotDir', type=str, nargs='?',
                        default='/afs/cern.ch/user/u/uhussain/www/UWVVPlots/DummyUnfolding/TUnfoldMat2017',
                        help='Directory to put response and covariance plots in')
    parser.add_argument('--unfoldDir', type=str, nargs='?',
                        default='/afs/cern.ch/user/u/uhussain/www/UWVVPlots/DummyUnfolding/TUnfoldedDistributions2017',
                        help='Directory to put response and covariance plots in')
    parser.add_argument('--nIter', type=int, nargs='?', default=1,
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
fOut = ROOT.TFile(tmpFileName, "recreate")

fScales = ROOT.TFile(args['scalefactors_file'])
mZZTightFakeRate = fScales.Get("mZZTightFakeRate")
eZZTightFakeRate = fScales.Get("eZZTightFakeRate")
#fakeRateFile=ROOT.TFile('data/fakeRate18Dec2018-ZplusLSkimZZ2018Data.root')
#eZZTightFakeRate=fakeRateFile.Get('DataEWKCorrected/ratioE2D_allE')
#mZZTightFakeRate=fakeRateFile.Get('DataEWKCorrected/ratioMu2D_allMu')
#useSvenjasFRs = False
#useJakobsFRs = False
#if useSvenjasFRs:
#    mZZTightFakeRate = fScales.Get("mZZTightFakeRate_Svenja")
#    eZZTightFakeRate = fScales.Get("eZZTightFakeRate_Svenja")
#elif useJakobsFRs:
#    mZZTightFakeRate = fScales.Get("mZZTightFakeRate_Jakob")
#    eZZTightFakeRate = fScales.Get("eZZTightFakeRate_Jakob")
## For medium muons
##mZZMedFakeRate.SetName("fakeRate_allMu")
if mZZTightFakeRate:
    #print "Yes muon fake rates"
    mZZTightFakeRate.SetName("fakeRate_allMu")
if eZZTightFakeRate:
    #print "Yes electron fake rates"
    eZZTightFakeRate.SetName("fakeRate_allE")
#
#muonIsoSF = fScales.Get('muonIsoSF')
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
variables = {
    'pt' : {c:'Pt' for c in channels},
    'mass' : {c:'Mass' for c in channels},
    'massFull' : {c:'Mass' for c in channels},
    'eta' : {c:'abs(Eta)' for c in channels},
    'z1mass' : {'eeee':'e1_e2_Mass', 'mmmm':'m1_m2_Mass',
                'eemm':['e1_e2_Mass','m1_m2_Mass']},
    'z2mass' : {'eeee':'e3_e4_Mass', 'mmmm':'m3_m4_Mass',
                'eemm':['m1_m2_Mass','e1_e2_Mass']},
    'z1pt' : {'eeee':'e1_e2_Pt', 'mmmm':'m1_m2_Pt',
              'eemm':['e1_e2_Pt','m1_m2_Pt']},
    'z2pt' : {'eeee':'e3_e4_Pt', 'mmmm':'m3_m4_Pt',
              'eemm':['m1_m2_Pt','e1_e2_Pt']},
    'zHigherPt' : {
        'eeee' : ['e1_e2_Pt', 'e3_e4_Pt'],
        'mmmm' : ['m1_m2_Pt', 'm3_m4_Pt'],
        'eemm' : ['e1_e2_Pt','m1_m2_Pt']
        },
    'zLowerPt' : {
        'eeee' : ['e1_e2_Pt', 'e3_e4_Pt'],
        'mmmm' : ['m1_m2_Pt', 'm3_m4_Pt'],
        'eemm' : ['e1_e2_Pt','m1_m2_Pt']
        },
    'zPt' : {
        'eeee' : ['e1_e2_Pt', 'e3_e4_Pt'],
        'mmmm' : ['m1_m2_Pt', 'm3_m4_Pt'],
        'eemm' : ['e1_e2_Pt','m1_m2_Pt']
        },
    'leppt' : {
        'eeee' : ['e1Pt', 'e2Pt', 'e3Pt', 'e4Pt'],
        'eemm' : ['e1Pt', 'e2Pt', 'm1Pt', 'm2Pt'],
        'mmmm' : ['m1Pt', 'm2Pt', 'm3Pt', 'm4Pt'],
        },
    'l1Pt' : {
        'eeee' : 'max(e1Pt, e3Pt)',
        'eemm' : 'max(e1Pt, m1Pt)',
        'mmmm' : 'max(m1Pt, m3Pt)',
        },
    }


_binning = {
    'pt' : [25.*i for i in range(4)] + [100., 150., 200., 300.],
    #'mass' : [100.+100.*i for i in range(12)],
    'mass' : [100.] + [200.+50.*i for i in range(5)] + [500.,600.,800.],
    'massFull' : [80.,100.,120.,130.,150.,180.,200.,240.,300.,400.,1000],
    'eta' : [6,0.,6.],
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
    #'zHigherPt' : {c:'Pt' for c in channels},
    #'zLowerPt' : {c:'Pt' for c in channels},
    'leppt' : {c:'Pt' for c in channels},
    #'l1Pt' : {c:'Pt' for c in channels},
    #'zPt' : {c:'Pt' for c in channels},
}

# list of variables not counting systematic shifts
varList=['Mass','ZZPt','ZZEta','Z1Mass','Z1Pt','Z2Mass','Z2Pt','ZPt','LepPt']

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
    h.sumw2()

def unnormalizeBins(h):
    binUnit = 1 # min(h.GetBinWidth(b) for b in range(1,len(h)+1))
    for ib in xrange(1,len(h)+1):
        w = h.GetBinWidth(ib)
        h.SetBinContent(ib, h.GetBinContent(ib) * w / binUnit)
        h.SetBinError(ib, h.GetBinError(ib) * w / binUnit)
        if h.GetBinError(ib) > h.GetBinContent(ib):
            h.SetBinError(ib, h.GetBinContent(ib))
    h.sumw2()

def createRatio(h1, h2):
    Nbins = h1.GetNbinsX()
    Ratio = h1.Clone("Ratio")
    hStackLast = h2.Clone("hStackLast")
    for i in range(Nbins+1):
        stackcontent = hStackLast.GetBinContent(i)
        #stackerror = hStackLast.GetBinError(i)
        datacontent = h1.GetBinContent(i)
        #dataerror = h1.GetBinError(i)
        print "stackcontent: ",stackcontent," and data content: ",datacontent
        ratiocontent=0
        if(datacontent!=0):
            ratiocontent = datacontent/stackcontent
        #if(datacontent!=0):
        #    error = ratiocontent*(math.sqrt(math.pow((dataerror/datacontent),2) + math.pow((stackerror/stackcontent),2)))
        #else:
        #    error = 2.07
        print "ratio content: ",ratiocontent
        Ratio.SetBinContent(i,ratiocontent)
        #Ratio.SetBinError(i,error)

    Ratio.GetYaxis().SetRangeUser(0.0,2.2)
    Ratio.SetStats(0)
    Ratio.GetYaxis().CenterTitle()
    Ratio.SetMarkerStyle(20)
    Ratio.SetMarkerSize(0.7)

    line = ROOT.TLine(h1.GetXaxis().GetXmin(), 1.,h1.GetXaxis().GetXmax(), 1.)
    line.SetLineStyle(8)

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

ROOT.gSystem.Load('Utilities/scripts/ResponseMatrixMaker_cxx')
def generateResponseClass(varName, channel,sumW):
    
    className = responseClassNames[varName][channel]

    #for example C=<class 'ROOT.BranchValueResponseMatrixMaker<float>'>
    C = getattr(ROOT, className)

    #sigFileNames = {s.name : [f for f in s.getFileNames()]
    #                for s in samples['reco'].values()[0].getBaseSamples()}
    
    filelist= args['filenames']
    #sigFileNames = {dataset : ConfigureJobs.getInputFilesPath(dataset,selection,analysis)  
    #                for dataset in ConfigureJobs.getListOfFiles(filelist, selection)}
    
    #sigConstWeights = {dataset : s.xsec * s.intLumi * float(s.kFactor) / s.sumW
    #                   for dataset in ConfigureJobs.getListOfFiles(filelist, args['selection'])}
    #mcinfo = ConfigureJobs.getMCInfo()

    sigConstWeights = {sample : (1.256*41500*1.0835)/sumW
                       for sample in ConfigureJobs.getListOfFiles(filelist, selection)}

    binning = _binning[varName]
    vBinning = VFloat()
    if len(binning) == 3:
        binningTemp = [binning[1] + i * (binning[2] - binning[1])/float(binning[0]) for i in xrange(binning[0]+1)]
        for b in binningTemp:
            vBinning.push_back(b)
    else:
        for b in binning:
            vBinning.push_back(b)

    responseMakers = {}
    #for sample, file_path in sigFileNames.items():
    for sample in ConfigureJobs.getListOfFiles(filelist,selection):
        file_path = ConfigureJobs.getInputFilesPath(sample,selection,analysis)  
        resp = C(channel, varNamesForResponseMaker[varName][channel], vBinning)
        print "sample:", sample
        file_path=file_path.encode("utf-8")
        print "file_path: ",type(file_path)
        fileList=glob.glob(file_path)
        print "type in fileList should be str: ",type(fileList[0])
        for fName in fileList:
            resp.registerFile(fName)

        resp.setConstantScale(sigConstWeights[sample])

        responseMakers[sample] = resp
    
    return responseMakers

_printCounter = 0
#Load the RooUnfold library into ROOT
ROOT.gSystem.Load("RooUnfold/libRooUnfold")
#Load the TUnfold library into ROOT
ROOT.gSystem.Load("TUnfold/libunfold")
#def getResponse(varName,chan,sigHists,sumW,trueHists,nIter,plotDir='',unfoldDir=''):
def getResponse(varName,chan,sigHists,sumW,trueHists,plotDir=''):
    global _printCounter
    #get responseMakers from the function above- this is the whole game.
    responseMakers = generateResponseClass(varName, chan,sumW)
    
    print "responseMakers: ",responseMakers
    hResponseNominal = {s:resp for s,resp in responseMakers.items()}
    print "hResponseNominal:",hResponseNominal
    hResponseNominalTotal = hResponseNominal
    #hResponseNominalTotal = sum(resp for resp in hResponseNominal.values())
    
    hResponse = hResponseNominalTotal["zz4l-powheg"].getResponse()

    #print "type of hResp: " ,hResponse
    # But its better to use RooUnfoldResponse here
    #RooUnfoldResponse constructor - create from already-filled histograms
    # "response" gives the response matrix, measured X truth.
    #  "measured" and "truth" give the projections of "response" onto the X-axis and Y-axis respectively,
    #   but with additional entries in "measured" for measurements with no corresponding truth (fakes/background) and
    #    in "truth" for unmeasured events (inefficiency).
    #     "measured" and/or "truth" can be specified as 0 (1D case only) or an empty histograms (no entries) as a shortcut
    #      to indicate, respectively, no fakes and/or no inefficiency.

    Response = getattr(ROOT,"RooUnfoldResponse")

    #hTrue = {}


    ## Give hSig and hTrue in the form of histograms
    
    varNames={'mass': 'Mass','pt':'ZZPt','eta':'ZZEta','z1mass':'Z1Mass','z1pt':'Z1Pt','z2mass':'Z2Mass','z2pt':'Z2Pt','zpt':'ZPt','leppt':'LepPt'}

    hSig = sigHists[chan][varNames[varName]]
    #Obviously you have to scale the reco histograms
    hSig.Scale((1.256*41500*1.0835)/sumW)
    hTrue = trueHists[chan][varNames[varName]]
   
    #No need to rebin certain variables
    if varNames[varName] not in ['ZZEta']:
        bins=array.array('d',_binning[varName])
        hSig=hSig.Rebin(len(bins)-1,"",bins)
        hTrue=hTrue.Rebin(len(bins)-1,"",bins)
    
    xaxisSize = hSig.GetXaxis().GetTitleSize()
    yaxisSize = hTrue.GetXaxis().GetTitleSize()
    #print "xaxisSize: ",xaxisSize
    response = Response(hSig.Clone(), hTrue.Clone(), hResponse)
    
    #Response matrix as a 2D-histogram: (x,y)=(measured,truth)
    hResp = response.Hresponse()
    #hResp = hResponse
    #print "hResp out of response: ",hResp

    TUnfoldDensity = getattr(ROOT,"TUnfoldDensity")

    #RooUnfoldInv = getattr(ROOT,"RooUnfoldInvert")

    try:
        svd = ROOT.TDecompSVD(response.Mresponse())
        rows = svd.GetNrows()
        cols = svd.GetNcols()
        sig = svd.GetSig()
        try:
            condition = sig.Max() / max(0., sig.Min())
        except ZeroDivisionError:
            condition = float('inf')
            raise

        print "channel: ",chan
        print "variable: ",varNames[varName]
        print "hResp out of response: ",hResp
        print "rows x cols: ",rows," x ",cols
        print ''
        print 'condition: {}'.format(condition)
        print ''

    except:
        print "It broke! Printing debug info"
        print "Sig: {}, true: {}, response: {}".format(hSig.Integral(), hTrue.Integral(), hResponse.Integral())
        c = ROOT.TCanvas("c1","canvas",1200,1200)
        hSig.Draw()
        style.setCMSStyle(c, '', dataType='Debug', intLumi=41500.)
        c.Print("DebugPlots/sig{}.png".format(_printCounter))
        hTrue.Draw()
        style.setCMSStyle(c, '', dataType='Debug', intLumi=41500.)
        c.Print("DebugPlots/true{}.png".format(_printCounter))
        draw_opt = "colz text45"
        hResponse.Draw(draw_opt)
        style.setCMSStyle(c, '', dataType='Debug', intLumi=41500.)
        c.Print("DebugPlots/resp{}.png".format(_printCounter))
        c.Print("DebugPlots/resp{}.root".format(_printCounter))
        _printCounter += 1


    #c = ROOT.TCanvas("c1","canvas",1200,1200)
    #hSig.GetXaxis().SetTitle('Reco '+prettyVars[varName]+''+units[varName])
    #hSig.Draw()
    #style.setCMSStyle(c, '', dataType='Debug', intLumi=41500.)
    #c.Print("DebugPlots/sig{}.png".format(_printCounter))
    #hTrue.GetXaxis().SetTitle('True '+prettyVars[varName]+''+units[varName])
    #hTrue.Draw()
    #style.setCMSStyle(c, '', dataType='Debug', intLumi=41500.)
    #c.Print("DebugPlots/true{}.png".format(_printCounter))
    #draw_opt = "colz text45"
    #hResponse.GetXaxis().SetTitle('Reco '+prettyVars[varName]+''+units[varName])
    #hResponse.GetYaxis().SetTitle('True '+prettyVars[varName]+''+units[varName])
    #hResponse.Draw(draw_opt)
    #style.setCMSStyle(c, '', dataType='Debug', intLumi=41500.)
    #c.Print("DebugPlots/resp{}.png".format(_printCounter))
    #c.Print("DebugPlots/resp{}.root".format(_printCounter))

    #unf = RooUnfoldIter(response, hSig, nIter)
    
    #Constructor for TUnfoldDensity.
    #Ingredients
    #truth level on y-axis of the response matrix
    histMap = TUnfoldDensity.kHistMapOutputVert
    #RegMode->no regularisation, or defined later by RegularizeXXX() methods
    RegMode = TUnfoldDensity.kRegModeNone
    #constraint (default=kEConstraintArea) type of constraint
    #preserve the area
    Constraint = TUnfoldDensity.kEConstraintArea
    #density flags - bin content is divided by the bin width
    densityFlags=TUnfoldDensity.kDensityModeBinWidth
    unf = TUnfoldDensity(hResp, histMap,RegMode,Constraint,densityFlags)
    unf.SetInput(hSig.Clone())
    
    #Probability matrix
    name="ProbMatrix"+varNames[varName] 
    ProResp = unf.GetProbabilityMatrix(name)
    #This is the unfolded "true" distribution
    #hOut = unf.Hreco()
    #if not hOut:
    #    print hOut
    #    raise ValueError("The unfolded histogram got screwed up somehow!")
    #
    #UnfHists=[]
    #TrueHists=[]
    #if unfoldDir:
    #    cUnfold = ROOT.TCanvas("c","canvas",1200,1200)
    #    hOut.GetXaxis().SetTitle('UnfTrue '+prettyVars[varName]+''+units[varName])
    #    hOut.GetXaxis().SetTitleSize(0.75*xaxisSize)
    #    hOut.Draw("HIST")
    #    UnfHists.append(hOut)
    #    leg = makeLegend(cUnfold,*(UnfHists),**legParams[varName])
    #    leg.SetFillStyle(1001)
    #    leg.Draw("SAME")
    #    style.setCMSStyle(cUnfold, '', dataType='  Work in Progress', intLumi=41500.)
    #    print "unfoldDir: ",unfoldDir
    #    cUnfold.Print("%s/UnfDist_%s.png" % (unfoldDir,varName))
    #    cUnfold.Print("%s/UnfDist_%s.pdf" % (unfoldDir,varName))
    #    del cUnfold

    #    cTrue = ROOT.TCanvas("c","canvas",1200,1200)
    #    hTrue.GetXaxis().SetTitle('True '+prettyVars[varName]+''+units[varName])
    #    hTrue.GetXaxis().SetTitleSize(0.75*yaxisSize)
    #    hTrue.SetLineColor(ROOT.kRed)
    #    hTrue.Draw("HIST")
    #    TrueHists.append(hTrue)
    #    leg = makeLegend(cTrue,*(TrueHists),**legParams[varName])
    #    leg.SetFillStyle(1001)
    #    leg.Draw("SAME")
    #    style.setCMSStyle(cTrue, '', dataType='  Work in Progress', intLumi=41500.)
    #    print "unfoldDir: ",unfoldDir
    #    cTrue.Print("%s/TrueDist_%s.png" % (unfoldDir,varName))
    #    cTrue.Print("%s/TrueDist_%s.pdf" % (unfoldDir,varName))
    #    del cTrue
    #    
    #    #Create a ratio plot
    #    c,pad1 = createCanvasPads()

    #    Unfmaximum = hOut.GetMaximum()
    #    hTrue.SetFillColor(ROOT.kRed)
    #    print "Total Truth Integral",hTrue.Integral()
    #    print "Total Unf Truth Integral",hOut.Integral()
    #    Truthmaximum = hTrue.GetMaximum()
    #    hTrue.Draw("HIST")
    #    if(Unfmaximum > Truthmaximum):
    #        hTrue.SetMaximum(Unfmaximum*1.2)
    #    else:
    #        hTrue.SetMaximum(Truthmaximum*1.2)

    #    hTrue.GetXaxis().SetTitle("")

    #    hTrue.GetXaxis().SetLabelSize(0)
    #    hTrue.GetXaxis().SetTitleSize(0)
    #    hTrue.GetYaxis().SetTitle("Events")
    #    hTrue.GetYaxis().SetTitleOffset(1.0)
    #    hTrue.Draw("HIST")
    #    hOut.SetLineColor(ROOT.kBlack)
    #    hOut.SetMarkerStyle(20)
    #    hOut.SetMarkerSize(0.7)
    #    hOut.GetXaxis().SetTitle("")
    #    hOut.GetXaxis().SetLabelSize(0)
    #    hOut.GetXaxis().SetTitleSize(0)
    #    hOut.Draw("px0same")
    #    
    #    leg = ROOT.TLegend(0.68,0.54,0.88,0.84,"")
    #    leg.AddEntry(hOut,"Unf Gen")
    #    leg.AddEntry(hTrue, "True Gen")
    #    leg.SetFillColor(ROOT.kWhite)
    #    leg.SetFillStyle(0)
    #    leg.SetTextSize(0.025)
    #    leg.Draw()

    #    #SecondPad
    #    pad2 = createPad2(c)
    #    #Starting the ratio proceedure
    #    Ratio,line = createRatio(hOut, hTrue)
    #    Ratio.Draw("px0")
    #    line.SetLineColor(ROOT.kBlack)
    #    line.Draw("same")

    #    #redraw axis
    #    xaxis = ROOT.TGaxis(hOut.GetXaxis().GetXmin(),0,hOut.GetXaxis().GetXmax(),0,hOut.GetXaxis().GetXmin(),hOut.GetXaxis().GetXmax(),510)
    #    xaxis.SetTitle(prettyVars[varName]+''+units[varName])
    #    xaxis.SetLabelFont(42)
    #    xaxis.SetLabelSize(0.10)
    #    xaxis.SetTitleFont(42)
    #    xaxis.SetTitleSize(0.12)
    #    xaxis.SetTitleOffset(0.70)
    #    xaxis.Draw("SAME")

    #    yaxis = ROOT.TGaxis(hOut.GetXaxis().GetXmin(),0,hOut.GetXaxis().GetXmin(),2.2,0,2.2,6,"")
    #    yaxis.SetTitle("UnfGen/TrueGen")
    #    yaxis.SetLabelFont(42)
    #    yaxis.SetLabelSize(0.10)
    #    yaxis.SetTitleFont(42)
    #    yaxis.SetTitleSize(0.12)
    #    yaxis.SetTitleOffset(0.35)
    #    yaxis.Draw("SAME")
    #    style.setCMSStyle(c, '', dataType='  Work in Progress', intLumi=41500.)
    #    c.Print("%s/Ratio_%s.png" % (unfoldDir,varName))
    #    c.Print("%s/Ratio_%s.pdf" % (unfoldDir,varName))
    #    c.Update()

    #    del c

    ##Returns covariance matrices for error calculation of type withError
    ##0: Errors are the square root of the bin content
    ##1: Errors from the diagonals of the covariance matrix given by the unfolding
    ##2: Errors from the covariance matrix given by the unfolding => We use this one for now
    ##3: Errors from the covariance matrix from the variation of the results in toy MC tests
    #hCov = unf.Ereco(0)

    # plot covariance and response
    if plotDir:
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
        print "plotDir: ",plotDir
        cRes.Print("%s/response_%s.png" % (plotDir,varName))
        cRes.Print("%s/response_%s.pdf" % (plotDir,varName))
    
        del cRes

        ProRes = ROOT.TCanvas("c","canvas",1200,1200)
        if varName == 'massFull':
            ProRes.SetLogx()
            ProRes.SetLogy()
        draw_opt = "colz text45"
        ProResp.GetXaxis().SetTitle('Reco '+prettyVars[varName]+''+units[varName])
        ProResp.GetYaxis().SetTitle('True '+prettyVars[varName]+''+units[varName])
        ProResp.GetXaxis().SetTitleSize(0.75*xaxisSize)
        ProResp.GetYaxis().SetTitleSize(0.75*yaxisSize)
        ProResp.Draw(draw_opt)
        style.setCMSStyle(ProRes, '', dataType='  Internal', intLumi=41500.)
        print "plotDir: ",plotDir
        ProRes.Print("%s/ProbResponse_%s.png" % (plotDir,varName))
        ProRes.Print("%s/ProbResponse_%s.pdf" % (plotDir,varName))
    
        del ProRes
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
#if __name__ == "__main__":

def mkdir(plotDir):
    for outdir in [plotDir]:
        try:
            os.makedirs(os.path.expanduser(outdir))
        except OSError as e:
            print e
            pass

plotDir=args['plotDir']
#UnfoldDir=args['unfoldDir']
#nIterations=args['nIter']

#varNames={'mass': 'Mass'}

varNames={'mass': 'Mass','pt':'ZZPt','eta':'ZZEta','z1mass':'Z1Mass','z1pt':'Z1Pt','z2mass':'Z2Mass','z2pt':'Z2Pt','zpt':'ZPt','leppt':'LepPt'}

mc,sumW = SelectorTools.applySelector(varList,args['filenames'],channels, "ZZSelector", args['selection'], fOut,args['analysis'],
        extra_inputs=sf_inputs+hist_inputs+tselection, 
        addSumweights=True, proof=args['proof'])
#Get the Gen Histograms
gen = SelectorTools.applyGenSelector(varList,args['filenames'],channels, "ZZGenSelector", args['selection'], fOut,args['analysis'],
        extra_inputs=hist_inputs+tselection, 
        addSumweights=False, proof=args['proof'])

#print the sum for a sample (zz4l-powheg)
#print "sumW (zz4l-powheg): ",sumW

for varName in varNames.keys():
    print "varName:", varNames[varName]

    for chan in channels:
        print "sigHist: ", mc[chan][varNames[varName]]
        print "trueHist: ",gen[chan][varNames[varName]]
        OutputDir=plotDir+"/"+chan+"/plots"
        if not os.path.exists(OutputDir):
            mkdir(OutputDir)
        #UnfoldOutDir=UnfoldDir+"/"+chan+"/plots"
        #if not os.path.exists(UnfoldOutDir):
        #    mkdir(UnfoldOutDir)
        #getResponse(varName,chan,mc,sumW,gen,nIterations,OutputDir,UnfoldOutDir)
        getResponse(varName,chan,mc,sumW,gen,OutputDir)
print(os.path.expanduser(OutputDir.replace("/plots", "")))
print(os.path.expanduser(OutputDir.replace("/plots", "")).split("/")[-1])
makeSimpleHtml.writeHTML(os.path.expanduser(OutputDir.replace("/plots", "")), "2D ResponseMatrices (from MC)")
#makeSimpleHtml.writeHTML(os.path.expanduser(UnfoldOutDir.replace("/plots", "")), "Unfolded Distributions (from MC)")

if args['test']:
    exit(0)


