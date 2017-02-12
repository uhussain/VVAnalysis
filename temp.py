# coding: utf-8
import ROOT
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
chain = ROOT.TChain("eee/ntuple")
chain.Add("/data/kelong/DibosonAnalysisData/3LooseLeptons/2017-02-09-data_DoubleEG_Run2016D-23Sep2016-v1-WZxsec2016-3LooseLeptons-v1/*")
select = ROOT.MakeBackgroundEstimate()
channel = ROOT.TNamed("channel", "eee")
name = ROOT.TNamed("name", "data_DoubleEG_Run2016D-23Sep2016-v1")
inputs = ROOT.TList()
inputs.Add(name)
inputs.Add(channel)

fScales = ROOT.TFile('data/scaleFactors.root')
mCBMedFakeRate = fScales.Get("mCBMedFakeRate_Svenja")
mCBMedFakeRate.SetName("fakeRate_allMu")
eCBTightFakeRate = fScales.Get("eCBTightFakeRate_Svenja")
eCBTightFakeRate.SetName("fakeRate_allE")
inputs.Add(eCBTightFakeRate)
inputs.Add(mCBMedFakeRate)

select = ROOT.MakeBackgroundEstimate()
select.SetInputList(inputs)
chain.Process(select, "")

for item in select.GetOutputList():
    if "PROOF" in item.GetName() or item.GetName() == "MissingFiles":
        continue
    writeOutputListItem(item, fOut)
