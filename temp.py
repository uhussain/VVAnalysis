# coding: utf-8
import ROOT
import glob

def writeOutputListItem(item, directory):
    if item.ClassName() == "TList":
        d = directory.mkdir(item.GetName())
        ROOT.SetOwnership(d, False)
        for subItem in item:
            writeOutputListItem(subItem, d)
    elif item.InheritsFrom("CategorizedHist"):
        directory.cd()
        for h in item.GetHists():
            h.Write()
    elif hasattr(item, 'Write'):
        directory.cd()
        item.Write()
    else:
        print "Couldn't write output item:"
        print repr(item)

chain = ROOT.TChain("eee/ntuple")
for directory in glob.glob("/data/kelong/DibosonAnalysisData/3LooseLeptons/2017-02-03-data_*Electron*"):
    print directory + "/*"
    print chain.Add(directory + "/*")
ROOT.gROOT.LoadMacro("Selectors/SelectorBase.cc+")
ROOT.gROOT.LoadMacro("Selectors/FakeRateSelector.cc+")
select = ROOT.FakeRateSelector()
chain.Process(select)

tmpFileName = "temp.root" 
fOut = ROOT.TFile(tmpFileName, "recreate")
for item in select.GetOutputList():
    writeOutputListItem(item, fOut)

testdir = fOut.Get("test")
passingTight = testdir.Get("passingTight")
print "Passing tight has %i entries" % passingTight.GetEntries()
passingLoose = testdir.Get("passingLoose")
print "Failing tight has %i entries" % passingLoose.GetEntries()
canvas = ROOT.TCanvas("canvas")
ratio = passingTight.Clone()
ratio.Divide(passingLoose)
ratio.Draw("colz")
canvas.Print("~/www/WZFakeRate/2Dratio.pdf")
passingTight_ptonly = passingTight.ProjectionX("passingTight_pt")
passingTight_ptonly.SetBinContent(10, passingTight_ptonly.GetBinContent(11))
passingLoose_ptonly = passingLoose.ProjectionX("passingLoose_pt")
passingLoose_ptonly.SetBinContent(10, passingLoose_ptonly.GetBinContent(11))
pt_ratio = passingTight_ptonly.Clone()
pt_ratio.Divide(passingLoose_ptonly)
passingTight_ptonly.Draw("hist")
canvas.Print("~/www/WZFakeRate/1Dpt_passingTight.pdf")
passingLoose_ptonly.Draw("hist")
canvas.Print("~/www/WZFakeRate/1Dpt_passingLoose.pdf")
pt_ratio.Draw("hist")
canvas.Print("~/www/WZFakeRate/1Dpt_ratio.pdf")

