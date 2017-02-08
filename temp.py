# coding: utf-8
import ROOT
import glob

def writeOutputListItem(item, directory):
    print item
    if item.ClassName() == "TList":
        d = directory.Get(item.GetName())
        if not d:
            d = directory.mkdir(item.GetName())
            ROOT.SetOwnership(d, False)
        for subItem in item:
            writeOutputListItem(subItem, d)
            print subItem
    elif hasattr(item, 'Write'):
        print directory
        directory.cd()
        item.Write()
    else:
        print "Couldn't write output item:"
        print repr(item)

#ROOT.TProof.Open("workers=2")
#ROOT.gProof.SetParameter("PROOF_UseTreeCache", 0)
tmpFileName = "temp.root" 
fOut = ROOT.TFile(tmpFileName, "recreate")
selection = "WZxsec2016/3LooseLeptons"
for chan in ["eem"]:#, "eem", "emm", "mmm"]:
    chain = ROOT.TChain("%s/ntuple" % chan)
    for dataset in [
        #"data_DoubleMuon_Run2016F-23Sep2016-v1" : {
        "/data/kelong/DibosonAnalysisData/DYControlFakeRate/2017-02-05-data_DoubleMuon_Run2016F-23Sep2016-v1-WZxsec2016-DYControlFakeRate-v1/*",
#        #"data_DoubleMuon_Run2016G-23Sep2016-v1" : {
        "/data/kelong/DibosonAnalysisData/DYControlFakeRate/2017-02-05-data_DoubleMuon_Run2016G-23Sep2016-v1-WZxsec2016-DYControlFakeRate-v1/*",
#        #"data_DoubleMuon_Run2016H-PromptReco-v2" : {
        "/data/kelong/DibosonAnalysisData/DYControlFakeRate/2017-02-05-data_DoubleMuon_Run2016H-PromptReco-v2-WZxsec2016-DYControlFakeRate-v1/*",
#        #"data_DoubleMuon_Run2016H-PromptReco-v3" : {
        "/data/kelong/DibosonAnalysisData/DYControlFakeRate/2017-02-05-data_DoubleMuon_Run2016H-PromptReco-v3-WZxsec2016-DYControlFakeRate-v1/*",
#        #"data_MuonEG_Run2016B-23Sep2016-v3" : {
        "/data/kelong/DibosonAnalysisData/DYControlFakeRate/2017-02-05-data_MuonEG_Run2016B-23Sep2016-v3-WZxsec2016-DYControlFakeRate-v1/*",
        #"data_MuonEG_Run2016C-23Sep2016-v1" : {
        "/data/kelong/DibosonAnalysisData/DYControlFakeRate/2017-02-05-data_MuonEG_Run2016C-23Sep2016-v1-WZxsec2016-DYControlFakeRate-v1/*",
        #"data_MuonEG_Run2016D-23Sep2016-v1" : {
    ]:
        proof_path = "_".join([dataset, "%s#/%s/ntuple" % (selection.replace("/", "_"), chan)])
        print chain.Add(dataset)
    selector_name = "FakeRateSelector"+chan.upper()
    select = getattr(ROOT, selector_name)()
    inputs = ROOT.TList()
    select.SetInputList(inputs)
    tname = ROOT.TNamed("name", "test") 
    inputs.Add(tname)
    print select
    print chain.Draw("Mass")
    print chain.Process(select, "")
    for item in select.GetOutputList():
        if "PROOF" in item.GetName() or item.GetName() == "MissingFiles":
            continue
        writeOutputListItem(item, fOut)
        print item
    filedir = fOut.Get(dataset)
    if filedir:
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

