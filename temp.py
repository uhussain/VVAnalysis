# coding: utf-8
import ROOT
import glob

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

ROOT.gROOT.LoadMacro("Selectors/SelectorBase.cc+")
tmpFileName = "temp.root" 
fOut = ROOT.TFile(tmpFileName, "recreate")
#for directory in glob.glob("/data/kelong/DibosonAnalysisData/3LooseLeptons/2017-02-03-data_*Electron*"):
for directory in glob.glob("/data/kelong/DibosonAnalysisData/3LooseLeptons/2017-02-03-data_SingleElectron_Run2016H-PromptReco-v3-WZxsec2016-3LooseLeptons-*"):
    name = directory.split("/")[-1]
    for chan in ["eee", "eem", "emm", "mmm"]:
        chain = ROOT.TChain("%s/ntuple" % chan)
        chain.Add(directory + "/*")
        selector_name = "FakeRateSelector"+chan.upper()
        ROOT.gROOT.LoadMacro("Selectors/%s.cc+" % selector_name)
        select = getattr(ROOT, "FakeRateSelector"+chan.upper())(name)
        chain.Process(select)
        for item in select.GetOutputList():
            writeOutputListItem(item, fOut)
        filedir = fOut.Get(name)
        filedir.cd()
        passingLoose2D = filedir.Get("passingLoose2D_"+ chan)
        passingTight2D = filedir.Get("passingTight2D_"+ chan)
        ratio2D = passingTight2D.Clone("ratio2D_"+chan)
        ratio2D.Divide(passingLoose2D) 
        ratio2D.Write()
        passingLoose1DPt = filedir.Get("passingLoose1DPt_"+ chan)
        passingTight1DPt = filedir.Get("passingTight1DPt_"+ chan)
        ratio1DPt = passingTight1DPt.Clone("ratio1DPt_"+chan)
        ratio1DPt.Divide(passingLoose1DPt) 
        ratio1DPt.Write()
        passingLoose1DEta = filedir.Get("passingLoose1DEta_"+ chan)
        passingTight1DEta = filedir.Get("passingTight1DEta_"+ chan)
        ratio1DEta = passingTight1DEta.Clone("ratio1DEta_"+chan)
        ratio1DEta.Divide(passingLoose1DEta) 
        ratio1DEta.Write()
        fOut.cd()

#testdir = fOut.Get(name)
#print "Passing tight has %i entries" % passingTight.GetEntries()
#print "Failing tight has %i entries" % passingLoose.GetEntries()
#canvas = ROOT.TCanvas("canvas")
#ratio = passingTight.Clone()
#ratio.Divide(passingLoose)
#ratio.Draw("colz")
#canvas.Print("~/www/WZFakeRate/2Dratio.pdf")
#passingTight_ptonly = passingTight.ProjectionX("passingTight_pt")
#passingTight_ptonly.SetBinContent(10, passingTight_ptonly.GetBinContent(11))
#passingLoose_ptonly = passingLoose.ProjectionX("passingLoose_pt")
#passingLoose_ptonly.SetBinContent(10, passingLoose_ptonly.GetBinContent(11))
#pt_ratio = passingTight_ptonly.Clone()
#pt_ratio.Divide(passingLoose_ptonly)
#passingTight_ptonly.Draw("hist")
#canvas.Print("~/www/WZFakeRate/1Dpt_passingTight.pdf")
#passingLoose_ptonly.Draw("hist")
#canvas.Print("~/www/WZFakeRate/1Dpt_passingLoose.pdf")
#pt_ratio.Draw("hist")
#canvas.Print("~/www/WZFakeRate/1Dpt_ratio.pdf")

