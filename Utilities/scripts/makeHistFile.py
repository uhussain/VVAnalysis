#!/usr/bin/env python
import ROOT
from python import SelectorTools
from python import UserInput
from python import OutputTools
from python import ConfigureJobs
from python import HistTools
import os
import sys

def getComLineArgs():
    parser = UserInput.getDefaultParser()
    parser.add_argument("--proof", "-p", 
        action='store_true', help="Don't use proof")
    parser.add_argument("--lumi", "-l", type=float,
        default=35.87, help="luminosity value (in fb-1)")
    parser.add_argument("--output_file", "-o", type=str,
        default="test.root", help="Output file name")
    parser.add_argument("--test", action='store_true',
        help="Run test job (no background estimate)")
    parser.add_argument("--output_selection", type=str,
        default="", help="Selection stage of output file "
        "(Same as input if not give)")
    parser.add_argument("-b", "--hist_names", 
                        type=lambda x : [i.strip() for i in x.split(',')],
                        default=["all"], help="List of histograms, "
                        "as defined in AnalysisDatasetManager, separated "
                        "by commas")
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
    "AnalysisDatasetManager", "Utilities/python"]))
import ConfigHistTools 

tmpFileName = args['output_file']
fOut = ROOT.TFile(tmpFileName, "recreate")

fScales = ROOT.TFile('data/scaleFactors.root')
mCBTightFakeRate = fScales.Get("mCBTightFakeRate")
eCBTightFakeRate = fScales.Get("eCBTightFakeRate")
useSvenjasFRs = False
useJakobsFRs = False
if useSvenjasFRs:
    mCBTightFakeRate = fScales.Get("mCBTightFakeRate_Svenja")
    eCBTightFakeRate = fScales.Get("eCBTightFakeRate_Svenja")
elif useJakobsFRs:
    mCBTightFakeRate = fScales.Get("mCBTightFakeRate_Jakob")
    eCBTightFakeRate = fScales.Get("eCBTightFakeRate_Jakob")
# For medium muons
#mCBMedFakeRate.SetName("fakeRate_allMu")
if mCBTightFakeRate:
    mCBTightFakeRate.SetName("fakeRate_allMu")
if eCBTightFakeRate:
    eCBTightFakeRate.SetName("fakeRate_allE")

muonIsoSF = fScales.Get('muonIsoSF')
muonIdSF = fScales.Get('muonTightIdSF')
electronTightIdSF = fScales.Get('electronTightIdSF')
electronGsfSF = fScales.Get('electronGsfSF')
pileupSF = fScales.Get('pileupSF')

#fPrefireEfficiency = ROOT.TFile('data/Map_Jet_L1FinOReff_bxm1_looseJet_JetHT_Run2016B-H.root')
fPrefireEfficiency = ROOT.TFile('data/Map_Jet_L1FinOReff_bxm1_looseJet_SingleMuon_Run2016B-H.root')
prefireEff = fPrefireEfficiency.Get('prefireEfficiencyMap')

fr_inputs = [eCBTightFakeRate, mCBTightFakeRate,]
sf_inputs = [electronTightIdSF, electronGsfSF, muonIsoSF, muonIdSF, pileupSF, prefireEff]
selection = args['output_selection'].split("_")[0]
if selection == "Inclusive2Jet":
    selection = "Wselection"
    print "Info: Using Wselection for hist defintions"
analysis = "/".join([args['analysis'], selection])
hists = ConfigHistTools.getAllHistNames(manager_path, analysis) \
    if "all" in args['hist_names'] else args['hist_names']

hists = [h for h in hists if "unrolled" not in h and "wCR" not in h and h not in  ["YieldByChannel", "CutFlow"]]
hist_inputs = [getHistExpr(hists, analysis)]
tselection = [ROOT.TNamed("selection", args['output_selection'])]
nanoAOD = True
channels = ["Inclusive"] if nanoAOD else ["eee", "eem", "emm", "mmm"]

if args['proof']:
    ROOT.TProof.Open('workers=12')

if "FakeRate" not in args['output_selection'] and not args['test']:
    background = SelectorTools.applySelector(["WZxsec2016data"] +
        ConfigureJobs.getListOfEWKFilenames() + ["wz3lnu-powheg"] +
        ConfigureJobs.getListOfNonpromptFilenames(), 
            "WZBackgroundSelector", args['selection'], fOut, 
            extra_inputs=sf_inputs+fr_inputs+hist_inputs+tselection, 
            channels=channels,
            addSumweights=False,
            nanoAOD=nanoAOD,
            proof=args['proof'])
mc = SelectorTools.applySelector(args['filenames'], "WZSelector", args['selection'], fOut, 
        extra_inputs=sf_inputs+hist_inputs+tselection, 
        channels=channels,
        nanoAOD=nanoAOD,
        addSumweights=True, proof=args['proof'])
if args['test']:
    exit(0)

alldata = HistTools.makeCompositeHists(fOut,"AllData", 
    ConfigureJobs.getListOfFilesWithXSec(["WZxsec2016data"], manager_path), args['lumi'],
    underflow=False, overflow=False)
OutputTools.writeOutputListItem(alldata, fOut)
alldata.Delete()

nonpromptmc = HistTools.makeCompositeHists(fOut, "NonpromptMC", ConfigureJobs.getListOfFilesWithXSec( 
    ConfigureJobs.getListOfNonpromptFilenames(), manager_path), args['lumi'],
    underflow=False, overflow=False)
nonpromptmc.Delete()

OutputTools.writeOutputListItem(nonpromptmc, fOut)
ewkmc = HistTools.makeCompositeHists(fOut,"AllEWK", ConfigureJobs.getListOfFilesWithXSec(
    ConfigureJobs.getListOfEWKFilenames(), manager_path), args['lumi'],
    underflow=False, overflow=False)
OutputTools.writeOutputListItem(ewkmc, fOut)
ewkmc.Delete()

ewkcorr = HistTools.getDifference(fOut, "DataEWKCorrected", "AllData", "AllEWK")
OutputTools.writeOutputListItem(ewkcorr, fOut)
ewkcorr.Delete()
