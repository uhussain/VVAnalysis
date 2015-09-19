#!/usr/bin/env python
import ROOT
import json

class CutString(object):
    def __init__(self):
        self.cuts = ["((eVetoTight + muVetoTight) == 0)"]
    def append(self, append):
        if self.cuts != "" and append != "":
            self.cuts += [append]
    def getString(self):
        return " && ".join(self.cuts)

def getObjectCuts(json_file_name):
    object_cuts = {}
    with open(json_file_name) as json_file:
        try:
            object_cuts = json.load(json_file)
        except ValueError as err:
            print "Error reading JSON file. The (likely unhelpful) error message was: "
            print(err)
    return object_cuts
def getSelectedEntries(tree, entrylist):
    #tree.SetEntryList(entrylist)
    for i in xrange( tree.GetEntries() ):
        tree.GetEntry( tree.GetEntryNumber( i ) )
        yield tree
    #for i in xrange(entrylist.GetN()):
     #   n = entrylist.Next()
      #  tree.GetEntry(n)
       # yield tree
def buildCuts(state):
    cut_string = CutString()
    object_cuts = getObjectCuts("Cuts/preselection.json")
    counts = dict((lep, state.count(lep)) for lep in state)
    current = dict((lep, 0) for lep in state)
    for lep in state:
        current[lep] += 1
        lep_name = ''.join([lep, "" if counts[lep] == 1 else str(current[lep])])
        print "lep is %s lep_name is %s" % (lep, lep_name)
        for cut in object_cuts[lep]:
            cut_string.append(cut.format(object=lep_name))
    return cut_string.getString()

root_file = ROOT.TFile.Open(
    "/hdfs/store/user/dntaylor/data/2015-09-13-13TeV-WZ/WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8/make_ntuples_cfg-008E7FBF-9218-E511-81E0-001E675A5244.root")
num = 0
entry_map = {}
for state in ["eee", "eem", "emm", "mmm"]:
    ROOT.gROOT.SetBatch(True)
    tree = root_file.Get("%s/final/Ntuple" % state)
    #tree = ROOT.TChain("%s/final/Ntuple" % state)
    #tree.Add(file_name)
    plotvar = "e1Pt" if "ee" in state else "m1Pt"
   
    selector = ROOT.TSelector.GetSelector("disambiguateFinalStates.C+") 
    zcand_name = "e1_e2_Mass" if state.count('e') >= 2 else "m1_m2_Mass"
    selector.setZCandidateBranchName(zcand_name)
    print selector
    #ROOT.gSystem.Load("disambiguateFinalStates.C+")
    #selector = ROOT.disambiguateFinalStates()
    listname = "list" + state
    cut_string = buildCuts(state)
    print cut_string
    print tree.Draw(">>" + listname, cut_string, "entrylist")
    tlist = ROOT.gDirectory.FindObject(listname);
    print tlist
    tree.SetEntryList(tlist)
    
    canvas = ROOT.TCanvas("canvas" + state, "canvas", 600, 800)
    #hist = ROOT.TH1F("test" + state, "test", 100, 0, 100)
    #tree.Process("disambiguateFinalStates.C+")
    print "Number of entries after draw string is: %i" % tree.Draw("evt>>test" + state, "")
    print "Now calling selector"
    tree.Process(selector) #, "Mass")
    print "DONE"
    entryList = selector.GetOutputList().FindObject('bestCandidates')
    tree.SetEntryList(entryList)
    count = 0
    print "With this loop count is %i " % count
    entries = tree.Draw("evt", "")
    hist = ROOT.gDirectory.FindObject("test" + state);
    hist.Draw("hist")
    canvas.Print("test" + state + ".pdf")
    print "Number of entries after selector is: %i" % entries
    count = 0
    with open("selectedEvents_%s.out" % state, "w") as outfile:
        #for entry in getSelectedEntries(tree, entryList):
        for i in xrange(entryList.GetN()):    
            tree.GetEntry(entryList.Next())
            entry_name = ':'.join([str(i) for i in [tree.run, tree.lumi, tree.evt]])
            outfile.write(entry_name)
            outfile.write('\n')
            if entry_name not in entry_map:
                entry_map[entry_name] = 1
            else:
                entry_map[entry_name] += 1    
            count += 1
    with open("multiplicies_%s.out" % state, "w") as outfile:
        for entry in entry_map:
            outfile.write(str(entry_map[entry]))
            outfile.write('\n')

    print "Number of events looped over was: %i " % count
    num += entries
    print cut_string
print num
