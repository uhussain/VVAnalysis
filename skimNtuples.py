#!/usr/bin/env python
import ROOT
import argparse
import os
import sys
from Utilities.python import ApplySelection

def getComLineArgs():
    parser = argparse.ArgumentParser()
    parser.add_argument("-s", "--selection_json", type=str,
                        required=True, help="Name of json file containing" 
                        " cuts to make, e.g. Cuts/preselection.json")
    parser.add_argument("-f", "--filelist", type=str,
                        required=True, help="List of input file names "
                        "to be processed (separated by commas)")
    parser.add_argument("-o", "--output_file_name", type=str,
                        required=True, help="Name of output file")
    return vars(parser.parse_args())
def writeNtupleToFile(output_file, tree, state, cut_string):
    state_dir = output_file.mkdir(state)
    state_dir.cd()
    ntuple_dir = state_dir.mkdir("final")
    ntuple_dir.cd()
    save_tree = tree.CopyTree(cut_string)
    print "save_tree has %i entries" % save_tree.GetEntries()
    save_tree.Write()
    # Remove AutoSaved trees
    output_file.Purge()
    ROOT.gROOT.cd()
    del save_tree
def skimNtuple(selection_json, filelist, output_file_name):
    current_path = os.getcwd()
    os.chdir(sys.path[0])
    ROOT.gROOT.SetBatch(True)
    #For testing on single file
    #file_path = "/hdfs/store/user/dntaylor/data/2015-09-13-13TeV-WZ/WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8/*"
    output_file = ROOT.TFile(output_file_name, "RECREATE")
    ROOT.gROOT.cd()
    for state in ["eee", "eem", "emm", "mmm"]:
        tree = ROOT.TChain("%s/final/Ntuple" % state)
        with open(filelist) as input_file:
            for file_path in input_file:
                file_path = ('/hdfs' if 'xrootd' not in file_path else '') + file_path.strip()
                tree.Add(file_path)
        print "Now the tree has %i entries" % tree.GetEntries()
        #ApplySelection.applySelection(tree, state, selection_json)
        cut_string = ApplySelection.buildCutString(state, selection_json).getString()
        writeNtupleToFile(output_file, tree, state, cut_string)
    os.chdir(current_path)

def main():
    args = getComLineArgs()
    skimNtuple(args['selection_json'], args['filelist'], args['output_file_name'])
if __name__ == "__main__":
    main()
