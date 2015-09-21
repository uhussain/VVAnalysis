#!/usr/bin/env python
import ROOT
import json
import argparse
from collections import OrderedDict
import time


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
    print "cuts_json is %s" % cuts_json
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
def applySelection(tree, state, selection, selection_json):
    print "______________________________________________________"
    listname = '_'.join(["list", state, selection])
    print selection_json
    cut_string = buildCutString(state, str(selection_json))
    num_passing = tree.Draw(">>" + listname, cut_string.getString(), "entrylist")
    print "%i events passed the cut " % num_passing
    print cut_string.getString()
    tlist = ROOT.gDirectory.FindObject(listname);
    print tlist
    tree.SetEntryList(tlist)
def applySelections(tree, state, selections):
    print "______________________________________________________"
    print "______________________________________________________"
    start_time = time.time()
    setAliases(tree, state, "Cuts/aliases.json")
    for selection, json_file in selections.iteritems():
        applySelection(tree, state, selection, json_file)
        print "Run time was %s seconds" % ((time.time() - start_time))
def getComLineArgs():
    parser = argparse.ArgumentParser()
    parser.add_argument("-s", "--selections", 
                        type=lambda x: OrderedDict((i.strip(),  '')  for i in x.split(",")),
                        required=True, help="List of selections you wish to make "
                        "(as defined in Cuts/definitions.json), separated by commas")
    args = vars(parser.parse_args())
    definitions_json = readJson("Cuts/definitions.json")
    for selection in args['selections']:
        if selection not in definitions_json.keys():
            raise ValueError("Cut name must correspond to a definition in " 
                "Cuts/definitions.json")
        args['selections'][selection] = definitions_json[selection]
    print args
    return args
def main():
    ROOT.gROOT.SetBatch(True)
    #For testing on single file
    #file_path = "/hdfs/store/user/dntaylor/data/2015-09-13-13TeV-WZ/WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8/make_ntuples_cfg-008E7FBF-9218-E511-81E0-001E675A5244.root"
    file_path = "/hdfs/store/user/dntaylor/data/2015-09-13-13TeV-WZ/WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8/*"
    try:
        user_input = getComLineArgs()
    except ValueError as err:
        print(err)
        exit(0)
    for state in ["eee", "eem", "emm", "mmm"]:
        tree = ROOT.TChain("%s/final/Ntuple" % state)
        tree.Add(file_path)
        cuts_table = applySelections(tree, state, user_input['selections'])
if __name__ == "__main__":
    main()
