#!/usr/bin/env python
import ROOT
import json

class CutString(object):
    def __init__(self):
        self.cuts = []
    def append(self, append):
        if self.cuts != "" and append != "":
            self.cuts += append if isinstance(append, list) else [append]
    def getString(self):
        return " && ".join(self.cuts)

def readJson(json_file_name):
    json_info = {}
    with open(json_file_name) as json_file:
        try:
            json_info = json.load(json_file)
        except ValueError as err:
            print "Error reading JSON file. The error message was: "
            print(err)
    return json_info
def getSelectedEntries(tree, entrylist):
    #tree.SetEntryList(entrylist)
    for i in xrange( tree.GetEntries() ):
        tree.GetEntry( tree.GetEntryNumber( i ) )
        yield tree
    #for i in xrange(entrylist.GetN()):
     #   n = entrylist.Next()
      #  tree.GetEntry(n)
       # yield tree
def buildCutString(state, cuts_json):
    cut_string = CutString()
    preselection_cuts = readJson(cuts_json)
    cut_string.append(preselection_cuts["Event"])
    cut_string.append(preselection_cuts["State"][state])
    
    counts = dict((lep, state.count(lep)) for lep in state)
    current = dict((lep, 0) for lep in state)
    for lep in state:
        current[lep] += 1
        lep_name = ''.join([lep, "" if counts[lep] == 1 else str(current[lep])])
        for cut in preselection_cuts["Object"][lep]:
            cut_string.append(cut.format(object_name=lep_name))
    return cut_string
def setAliases(tree, state, aliases_json):
    aliases = readJson(aliases_json)
    for name, value in aliases["State"][state].iteritems():
        tree.SetAlias(name, value)

#root_file = ROOT.TFile.Open(
#    "/hdfs/store/user/dntaylor/data/2015-09-13-13TeV-WZ/WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8/make_ntuples_cfg-008E7FBF-9218-E511-81E0-001E675A5244.root")
#file_path = "/hdfs/store/user/dntaylor/data/2015-09-13-13TeV-WZ/WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8/make_ntuples_cfg-008E7FBF-9218-E511-81E0-001E675A5244.root"
file_path = "/hdfs/store/user/dntaylor/data/2015-09-13-13TeV-WZ/WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8/*"
num = 0
entry_map = {}
for state in ["eee", "eem", "emm", "mmm"]:
    ROOT.gROOT.SetBatch(True)
    tree = ROOT.TChain("%s/final/Ntuple" % state)
    #tree = ROOT.TChain("%s/final/Ntuple" % state)
    tree.Add(file_path)
    plotvar = "e1Pt" if "ee" in state else "m1Pt"
   
    selector = ROOT.TSelector.GetSelector("disambiguateFinalStates.C+") 
    zcand_name = "e1_e2_Mass" if state.count('e') >= 2 else "m1_m2_Mass"
    selector.setZCandidateBranchName(zcand_name)
    listname = "list" + state
    cut_string = buildCutString(state, "Cuts/fullSelection.json")
    setAliases(tree, state, "Cuts/aliases.json")
    print tree.Draw(">>" + listname, cut_string.getString(), "entrylist")
    print cut_string.getString()
    continue
    tlist = ROOT.gDirectory.FindObject(listname);
    tree.SetEntryList(tlist)
    
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
    #canvas.Print("test" + state + ".pdf")
    print "Number of entries after selector is: %i" % entries
    count = 0
    #tree.SetAlias("ZCand_name", "e1_e2_Mass > e2_e3_Mass ? 'e1_e2' : 'e2_e3'")
    tree.Draw("evt>>test" + state)
    with open("selectedEvents_%s.out" % state, "w") as outfile:
        #for entry in getSelectedEntries(tree, entryList):
        for i in xrange(entryList.GetN()):    
            tree.GetEntry(entryList.Next())
            entry_name = ':'.join([str(i) for i in [tree.run, tree.lumi, tree.evt]])
            outfile.write(entry_name)
            outfile.write('\n')
            if entry_name not in entry_map.keys():
                entry_map[entry_name] = 1
            else:
                entry_map[entry_name] += 1    
            #print "Count is %i" % count 
            #print "Entry is %i" % entry_map[entry_name]
            count += 1
    with open("multiplicies_%s.out" % state, "w") as outfile:
        for entry in entry_map:
            outfile.write(str(entry_map[entry]))
            outfile.write('\n')
    print "Number of events looped over was: %i " % count
    num += entries
print num
