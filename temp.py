# coding: utf-8
import ROOT
from Utilities.python import SelectorTools
from Utilities.python import ConfigureJobs

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
#mCBMedFakeRate.SetName("fakeRate_allMu")
#eCBTightFakeRate = fScales.Get("eCBTightFakeRate_Svenja")
mCBMedFakeRate = fScales.Get("mCBTightFakeRate")
mCBMedFakeRate.SetName("fakeRate_allMu")
eCBTightFakeRate = fScales.Get("eCBTightFakeRate")
eCBTightFakeRate.SetName("fakeRate_allE")
extra_inputs = [eCBTightFakeRate, mCBMedFakeRate]

#background = SelectorTools.applySelector(["WZxsec2016-data"], "MakeBackgroundEstimate", "3LooseLeptons", extra_inputs=extra_inputs)
background = SelectorTools.applySelector(["WZxsec2016-data", "DYm50*"], "MakeBackgroundEstimate", "WselectionLooseLeps", extra_inputs=extra_inputs)
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
nonpromptmc = makeCompositeHists("NonpromptMC", ConfigureJobs.getListOfFilesWithXSec(["DYm50-*j", 
        "tt-lep",
        "st-schan",
        "st-tchan-t",
        "st-tchan-tbar",
        "st-tw",
        "st-tbarw",
    ], path))
writeOutputListItem(nonpromptmc, fOut)
