import ROOT
import glob
from python import UserInput
from python import ConfigureJobs
import os

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
def getComLineArgs():
    parser = UserInput.getDefaultParser()
    parser.add_argument("--withoutProof", "-l", 
        action='store_true', help="Don't use proof")
    return vars(parser.parse_args())

ROOT.TProof.Open("workers=12")
ROOT.gProof.SetParameter("PROOF_UseTreeCache", 0)
ROOT.gProof.Load("Selectors/SelectorBase.cc+")
#ROOT.gROOT.LoadMacro("SelectorBase.cc+")
tmpFileName = "temp.root" 
fOut = ROOT.TFile(tmpFileName, "recreate")
for chan in ["eee", "eem", "emm", "mmm"]:
    selector_name = "FakeRateSelector"+chan.upper()
    #ROOT.gROOT.LoadMacro("%s.cc+" % selector_name)
    ROOT.gProof.Load("Selectors/%s.cc+" % selector_name)
#selector_name = "FakeRateSelector"
#selector_name = "SelectorBase"
#ROOT.gProof.Load("%s.cc+" % selector_name)
#ROOT.gROOT.LoadMacro("%s.cc+" % selector_name)
args = getComLineArgs()
path = ConfigureJobs.getManagerPath()
for dataset in ConfigureJobs.getListOfFiles(args['filenames'], path):
    for chan in ["eee", "eem", "emm", "mmm"]:
        proof_path = "_".join([dataset, args['analysis'], args['selection'],
            "#%s/ntuple" % chan])
        selector_name = "FakeRateSelector"+chan.upper()
        select = getattr(ROOT, selector_name)(dataset)
        inputs = ROOT.TList()
        select.SetInputList(inputs)
        tname = ROOT.TNamed("name", dataset) 
        inputs.Add(tname)
        ROOT.gProof.Process(proof_path, select, "")
        for item in select.GetOutputList():
            if "PROOF" in item.GetName() or item.GetName() == "MissingFiles":
                continue
            writeOutputListItem(item, fOut)
        filedir = fOut.Get(dataset)
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

