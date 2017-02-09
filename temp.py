# coding: utf-8
import ROOT
import glob
from Utilities.python import UserInput
from Utilities.python import ConfigureJobs

def getComLineArgs():
    parser = UserInput.getDefaultParser()
    parser.add_argument("--withoutProof", "-l", 
        action='store_true', help="Don't use proof")
    return vars(parser.parse_args())

def writeOutputListItem(item, directory):
    print item
    if item.ClassName() == "TList":
        d = directory.Get(item.GetName())
        if not d:
            d = directory.mkdir(item.GetName())
            ROOT.SetOwnership(d, False)
        for subItem in item:
            writeOutputListItem(subItem, d)
            print "Subitem", subItem
    elif hasattr(item, 'Write'):
        print directory
        directory.cd()
        item.Write()
    else:
        print "Couldn't write output item:"
        print repr(item)
args = getComLineArgs()
proof = 0
if not args['withoutProof']:
    ROOT.TProof.Open("workers=12")
    proof = ROOT.gProof
tmpFileName = "temp.root" 
fOut = ROOT.TFile(tmpFileName, "recreate")
selection = "WZxsec2016/3LooseLeptons"
selector_name = "FakeRateSelector"
path = ConfigureJobs.getManagerPath()
for dataset in ConfigureJobs.getListOfFiles(args['filenames'], path):
    print dataset
    for chan in ["eee", "eem", "emm", "mmm"]:
        select = getattr(ROOT, selector_name)()
        inputs = ROOT.TList()
        select.SetInputList(inputs)
        tchan = ROOT.TNamed("channel", chan)
        inputs.Add(tchan)
        if proof:
            proof_path = "_".join([dataset, args['analysis'], 
                args['selection']+("#/%s/ntuple" % chan)])
            tname = ROOT.TNamed("name", dataset)
            inputs.Add(tname)
            proof.Process(proof_path, select, "")
        else: 
            chain = ROOT.TChain("%s/ntuple" % chan)
            try:
                file_path = ConfigureJobs.getInputFilesPath(dataset, 
                    path, args['selection'], args['analysis'])
                tname = ROOT.TNamed("name", file_path.split("/")[-2])
                inputs.Add(tname)
                chain.Add(file_path)
                chain.Process(select, "")
            except ValueError as e:
                print e
                continue
    for item in select.GetOutputList():
        if "PROOF" in item.GetName() or item.GetName() == "MissingFiles":
            continue
        writeOutputListItem(item, fOut)

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

