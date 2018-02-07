#!/usr/bin/env python
import ROOT
from python import SelectorTools
from python import UserInput
from python import OutputTools
from python import ConfigureJobs
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
    parser.add_argument("--output_selection", type=str,
        default="", help="Selection stage of output file "
        "(Same as input if not give)")
    parser.add_argument("-b", "--hist_names", 
                        type=lambda x : [i.strip() for i in x.split(',')],
                        default=["all"], help="List of histograms, "
                        "as defined in AnalysisDatasetManager, separated "
                        "by commas")
    return vars(parser.parse_args())

def getDifference(name, dir1, dir2, addRatios=True):
    differences = ROOT.TList()
    differences.SetName(name)
    for histname in [i.GetName() for i in fOut.Get(dir1).GetListOfKeys()]:
        if histname == "sumweights": continue
        hist1 = fOut.Get("/".join([dir1, histname]))
        hist2 = fOut.Get("/".join([dir2, histname]))
        if hist1 and hist2:
            diff = hist1.Clone()
            diff.Add(hist2, -1)
            hist1.Delete()
            hist2.Delete()
        elif not hist1:
            print "WARNING: Hist %s was not produced for " \
                "dataset(s) %s" % (histname, dir1)
        elif not hist2:
            print "WARNING: Hist %s was not produced for " \
                "dataset(s) %s" % (histname, dir2)
        differences.Add(diff)
    if addRatios:
        ratios = getRatios(differences)
        for ratio in ratios:
            differences.Add(ratio) 
    return differences

def makeCompositeHists(name, members, lumi):
    composite = ROOT.TList()
    composite.SetName(name)
    for directory in [str(i) for i in members.keys()]:
        if not fOut.Get(directory):
            print "Skipping invalid filename %s" % directory
            continue
        sumweights = 0
        for histname in [i.GetName() for i in fOut.Get(directory).GetListOfKeys()]:
            if histname == "sumweights": continue
            hist = fOut.Get("/".join([directory, histname]))
            if hist:
                sumhist = composite.FindObject(hist.GetName())
                if "data" not in directory and hist.GetEntries() > 0:
                    if sumweights == 0:
                        sumweights_hist = fOut.Get("/".join([directory, "sumweights"]))
                        sumweights = sumweights_hist.Integral()
                        sumweights_hist.Delete()
                    hist.Scale(members[directory]*1000*lumi/sumweights)
            else:
                raise RuntimeError("hist %s was not produced for "
                    "dataset %s!" % (histname, directory))
            if not sumhist:
                sumhist = hist.Clone()
                composite.Add(sumhist)
            else:
                sumhist.Add(hist)
            hist.Delete()
    return composite

def getHistExpr(hist_names, selection):
    info = ROOT.TList()
    info.SetName("histinfo")
    for hist_name in hist_names:
        bin_info = ConfigHistTools.getHistBinInfo(manager_path, selection, hist_name)
        bin_expr = "{nbins}, {xmin}, {xmax}".format(**bin_info)
        info.Add(ROOT.TNamed(hist_name, "%s $ %s" % (hist_name, bin_expr))
        )
    return info

ROOT.gROOT.SetBatch(True)

args = getComLineArgs()
manager_path = ConfigureJobs.getManagerPath()
sys.path.append("/".join([manager_path, 
    "AnalysisDatasetManager", "Utilities/python"]))
import ConfigHistTools 

tmpFileName = args['output_file']
#fOut = ROOT.TFile(tmpFileName, "recreate")
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
mCBTightFakeRate.SetName("fakeRate_allMu")
eCBTightFakeRate.SetName("fakeRate_allE")

muonIsoSF = fScales.Get('muonIsoSF')
muonIdSF = fScales.Get('muonTightIdSF')
electronTightIdSF = fScales.Get('electronTightIdSF')
pileupSF = fScales.Get('pileupSF')

fr_inputs = [eCBTightFakeRate, mCBTightFakeRate,]
sf_inputs = [electronTightIdSF, muonIsoSF, muonIdSF, pileupSF]
selection = args['selection'].replace("LooseLeptons", "") \
    if args['output_selection'] == "" else args['output_selection'].split("_")[0]
if selection == "Inclusive2Jet":
    selection = "Wselection"
    print "Info: Using Wselection for hist defintions"
analysis = "/".join([args['analysis'], selection])
hists = ConfigHistTools.getAllHistNames(manager_path, analysis) \
    if "all" in args['hist_names'] else args['hist_names']
    
hist_inputs = [getHistExpr(hists, analysis)]
tselection = [ROOT.TNamed("selection", args['output_selection'])]

if args['proof']:
    ROOT.TProof.Open('workers=12')

if "FakeRate" not in args['output_selection']:
    background = SelectorTools.applySelector(["WZxsec2016data"] +
        ConfigureJobs.getListOfEWKFilenames() + ["wz3lnu-powheg"] +
        ConfigureJobs.getListOfNonpromptFilenames(), 
            "WZBackgroundSelector", args['selection'], fOut, 
            extra_inputs=fr_inputs+hist_inputs+tselection, proof=args['proof'])
mc = SelectorTools.applySelector(["WZxsec2016"], "WZSelector", args['selection'], fOut, 
        extra_inputs=sf_inputs+hist_inputs+tselection, addsumweights=True, proof=args['proof'])
alldata = makeCompositeHists("AllData", 
    ConfigureJobs.getListOfFilesWithXSec(["WZxsec2016data"], manager_path), args['lumi'])
OutputTools.writeOutputListItem(alldata, fOut)

nonpromptmc = makeCompositeHists("NonpromptMC", ConfigureJobs.getListOfFilesWithXSec( 
    ConfigureJobs.getListOfNonpromptFilenames(), manager_path), args['lumi'])
OutputTools.writeOutputListItem(nonpromptmc, fOut)

ewkmc = makeCompositeHists("AllEWK", ConfigureJobs.getListOfFilesWithXSec(
    ConfigureJobs.getListOfEWKFilenames(), manager_path), args['lumi'])
OutputTools.writeOutputListItem(ewkmc, fOut)

ewkcorr = getDifference("DataEWKCorrected", "AllData", "AllEWK", False)
OutputTools.writeOutputListItem(ewkcorr, fOut)
