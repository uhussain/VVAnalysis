# coding: utf-8
import ROOT
import glob
from Utilities.python import UserInput
from Utilities.python import ConfigureJobs

def getComLineArgs():
    parser = UserInput.getDefaultParser()
    parser.add_argument("--proof", "-p", 
        action='store_true', help="Don't use proof")
    return vars(parser.parse_args())

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
args = getComLineArgs()
proof = 0
if args['proof']:
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
        tname = ROOT.TNamed("name", dataset)
        inputs.Add(tname)
        inputs.Add(tchan)
        if proof:
            proof_path = "_".join([dataset, args['analysis'], 
                args['selection']+("#/%s/ntuple" % chan)])
            proof.Process(proof_path, select, "")
        else: 
            chain = ROOT.TChain("%s/ntuple" % chan)
            try:
                file_path = ConfigureJobs.getInputFilesPath(dataset, 
                    path, args['selection'], args['analysis'])
                chain.Add(file_path)
                chain.Process(select, "")
            except ValueError as e:
                print e
                continue
        for item in select.GetOutputList():
            if "PROOF" in item.GetName() or item.GetName() == "MissingFiles":
                continue
            writeOutputListItem(item, fOut)

alldata = ROOT.TList()
alldata.SetName("AllData")
hists = [x+"_"+y for x in ["passingLoose1DPt", "passingLoose1DEta", "passingLoose2D",
            "passingTight1DPt", "passingTight1DEta", "passingTight2D",] \
    for y in ["eee", "eem", "emm", "mmm"]]
for directory in fOut.GetListOfKeys():
    if "data" in directory.GetName():
        for histname in hists:
            hist = fOut.Get("/".join([directory.GetName(), histname]))
            if hist:
                sumhist = alldata.FindObject(hist.GetName())
            else:
                raise RuntimeError("hist %s was not produced for "
                    "dataset %s!" % (histname, directory.GetName()))
            if not sumhist:
                sumhist = hist.Clone()
                alldata.Add(sumhist)
            else:
                sumhist.Add(hist)
for hist_name in ["passingLoose1DPt", "passingLoose1DEta", "passingLoose2D", 
        "passingTight1DPt", "passingTight1DEta", "passingTight2D"]:
    etot = alldata.FindObject(hist_name+"_eee")
    etot.SetName(hist_name+"_allE")
    etot.Add(alldata.FindObject(hist_name+"_emm"))
    alldata.Add(etot)
    mtot = alldata.FindObject(hist_name+"_mmm")
    mtot.SetName(hist_name+"_allMu")
    mtot.Add(alldata.FindObject(hist_name+"_eem"))
    alldata.Add(mtot)
ratios = []
for hist in alldata:
    if "Tight" not in hist.GetName():
        continue
    ratio = hist.Clone()
    ratio.SetName(hist.GetName().replace("passingTight", "ratio"))
    ratio.Divide(alldata.FindObject(hist.GetName().replace("Tight", "Loose")))
    ratios.append(ratio)
for ratio in ratios:
    alldata.Add(ratio) 
writeOutputListItem(alldata, fOut)
