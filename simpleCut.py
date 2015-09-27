#!/usr/bin/env python
import ROOT
import argparse
from collections import OrderedDict
import os
import sys
from Utilities import UserInput
import time

class CutString(object):
    def __init__(self):
        self.cuts = []
    def append(self, append):
        if self.cuts != "" and append != "":
            self.cuts += append if isinstance(append, list) else [append]
    def getString(self):
        return " && ".join(self.cuts)

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
    preselection_cuts = UserInput.readJson(cuts_json)
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
    aliases = UserInput.readJson(aliases_json)
    for name, value in aliases["State"][state].iteritems():
        tree.SetAlias(name, value)

def applySelection(tree, state, selection, user_input):
    #order = ["fsa", "preselection", "Zselection", "Wselection"] 
    setAliases(tree, state, "Cuts/aliases.json")
    cut_string = buildCutString(state, user_input['selections'][selection])
    
    print tree
    #tree.SetProof()
    #num_passing = proof.DrawSelect(tree, ">>" + listname, cut_string.getString(), "entrylist")
    listname = '_'.join(["list", state, selection])
    num_passing = tree.Draw(">>" + listname, cut_string.getString(), "entrylist")
    print "%i events passed the cut " % num_passing
    print cut_string.getString()
    #tlist = ROOT.gProof.GetOutputList().FindObject(listname) 
    tlist = ROOT.gDirectory.FindObject(listname);
    print tlist
    tree.SetEntryList(tlist)
        
def getComLineArgs():
    parser = argparse.ArgumentParser()
    parser.add_argument("-a", "--save_all", action='store_true',
                        required=False, help="Save output of step separately")
    parser.add_argument("-s", "--selections", 
                        type=lambda x: OrderedDict((i.strip(),  '')  for i in x.split(",")),
                        required=True, help="List of selections you wish to make "
                        "(as defined in Cuts/definitions.json), separated by commas")
    args = vars(parser.parse_args())
    definitions_json = UserInput.readJson("Cuts/definitions.json")
    for selection in args['selections']:
        if selection not in definitions_json.keys():
            raise ValueError("Cut name must correspond to a definition in " 
                "Cuts/definitions.json")
        args['selections'][selection] = definitions_json[selection]
    return args
def createOutputFile(name, selection):
    output_files = UserInput.readJson("MetaData/output_files.json") 
    print output_files
    file_name = output_files[name]['file_paths'][selection].format(DATE=time.strftime("%Y_%m_%d"))
    print "This file_name is %s " % file_name
    folder = file_name.rsplit("/", 1)[0]
    print "The folder is %s" % folder
    if not os.path.isdir(folder):
        os.mkdir(folder)
    output_file = ROOT.TFile(file_name, "RECREATE")
    return output_file
def writeNtupleToFile(output_file, tree, state):
    state_dir = output_file.mkdir(state)
    state_dir.cd()
    ntuple_dir = state_dir.mkdir("final")
    ntuple_dir.cd()
    save_tree = tree.CloneTree()
    save_tree.Write()
    print "Writing TREE!"
    ROOT.gROOT.cd()
    del save_tree

def main():
    current_path = os.getcwd()
    os.chdir(sys.path[0])
    ROOT.gROOT.SetBatch(True)
    #For testing on single file
    #file_path = "/hdfs/store/user/dntaylor/data/2015-09-13-13TeV-WZ/WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8/make_ntuples_cfg-008E7FBF-9218-E511-81E0-001E675A5244.root"
    #file_path = "/hdfs/store/user/dntaylor/data/2015-09-13-13TeV-WZ/WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8/*"
    #proof = ROOT.gProof
    try:
        user_input = getComLineArgs()
    except ValueError as err:
        print(err)
        exit(0)
    #ROOT.TProof.Open('workers=8')

    input_files = UserInput.readJson("MetaData/input_files.json")
    for name, input_info in input_files.iteritems(): 
        for selection in user_input['selections']:
            output_file = createOutputFile(name, selection)
            ROOT.gROOT.cd()
            for state in ["eee", "eem", "emm", "mmm"]:
                tree = ROOT.TChain("%s/final/Ntuple" % state)
                print "The file is %s" % input_info['file_paths'][selection]
                tree.Add(input_info['file_paths'][selection])
                #tree.Add(file_path)
                applySelection(tree, state, selection, user_input)
                writeNtupleToFile(output_file, tree, state)
                os.chdir(current_path)
if __name__ == "__main__":
    main()
