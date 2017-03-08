# coding: utf-8
import ROOT
from Utilities.python import SelectorTools
from Utilities.python import ConfigureJobs

def getDifference(name, dir1, dir2, addRatios=True):
    differences = ROOT.TList()
    differences.SetName(name)
    channels = ["eee", "eem", "emm", "mmm"]
    for histname in [i.GetName() for i in fOut.Get(dir1).GetListOfKeys()]:
        if histname == "sumweights": continue
        hist1 = fOut.Get("/".join([dir1, histname]))
        hist2 = fOut.Get("/".join([dir2, histname]))
        if hist1 and hist2:
            diff = hist1.Clone()
            diff.Add(hist2, -1)
        else:
            raise RuntimeError("hist %s was not produced for "
                "dataset(s) %s and/or %s!" % (histname, dir1, dir2))
        differences.Add(diff)
    if addRatios:
        ratios = getRatios(differences)
        for ratio in ratios:
            differences.Add(ratio) 
    return differences

def makeCompositeHists(name, members):
    composite = ROOT.TList()
    composite.SetName(name)
    lumi = 35.87
    for directory in [str(i) for i in members.keys()]:
        for histname in [i.GetName() for i in fOut.Get(directory).GetListOfKeys()]:
            if histname == "sumweights": continue
            hist = fOut.Get("/".join([directory, histname]))
            if hist:
                sumhist = composite.FindObject(hist.GetName())
                if "data" not in directory and hist.GetEntries() > 0:
                    sumweights_hist = fOut.Get("/".join([directory, "sumweights"]))
                    sumweights = sumweights_hist.Integral()
                    hist.Scale(members[directory]*1000*lumi/sumweights)
            else:
                raise RuntimeError("hist %s was not produced for "
                    "dataset %s!" % (histname, directory))
            if not sumhist:
                sumhist = hist.Clone()
                composite.Add(sumhist)
            else:
                sumhist.Add(hist)
    return composite

def writeOutputListItem(item, directory):
    if item.ClassName() == "TList":
        d = directory.Get(item.GetName())
        if not d:
            d = directory.mkdir(item.GetName())
            ROOT.SetOwnership(d, False)
        for subItem in item:
            writeOutputListItem(subItem, d)
    elif hasattr(item, 'Write'):
        directory.cd()
        item.Write()
    else:
        print "Couldn't write output item:"
        print repr(item)
    directory.cd()

tmpFileName = "test.root"
fOut = ROOT.TFile(tmpFileName, "recreate")

fScales = ROOT.TFile('data/scaleFactors.root')
#mCBMedFakeRate = fScales.Get("mCBMedFakeRate_Svenja")
#eCBTightFakeRate = fScales.Get("eCBTightFakeRate_Svenja")
mCBMedFakeRate = fScales.Get("mCBTightFakeRate")
eCBTightFakeRate = fScales.Get("eCBTightFakeRate")
mCBMedFakeRate.SetName("fakeRate_allMu")
eCBTightFakeRate.SetName("fakeRate_allE")
extra_inputs = [eCBTightFakeRate, mCBMedFakeRate]

#background = SelectorTools.applySelector(["WZxsec2016-data"], "MakeBackgroundEstimate", "3LooseLeptons", extra_inputs=extra_inputs)
background = SelectorTools.applySelector(["WZxsec2016"], "MakeBackgroundEstimate", "WselectionLooseLeps", extra_inputs=extra_inputs)
for item in background:
    if "PROOF" in item.GetName() or item.GetName() == "MissingFiles":
        continue
    writeOutputListItem(item, fOut)
mc = SelectorTools.applySelector(["WZxsec2016"], "WZSelector", "Wselection", addsumweights=True)
for item in mc:
    if "PROOF" in item.GetName() or item.GetName() == "MissingFiles":
        continue
    writeOutputListItem(item, fOut)
path = ConfigureJobs.getManagerPath()
alldata = makeCompositeHists("AllData", ConfigureJobs.getListOfFilesWithXSec(["WZxsec2016data"], path))
writeOutputListItem(alldata, fOut)
nonpromptmc = makeCompositeHists("NonpromptMC", ConfigureJobs.getListOfFilesWithXSec( 
    ConfigureJobs.getListOfNonpromptFilenames(), path))
writeOutputListItem(nonpromptmc, fOut)
ewkmc = makeCompositeHists("AllEWK", ConfigureJobs.getListOfFilesWithXSec(
    ConfigureJobs.getListOfEWKFilenames(), path))
writeOutputListItem(ewkmc, fOut)
ewkcorr = getDifference("DataEWKCorrected", "AllData", "AllEWK", False)
writeOutputListItem(ewkcorr, fOut)
