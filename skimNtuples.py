#!/usr/bin/env python
import ROOT
import argparse
import os
import sys
from Utilities import ApplySelection

def getComLineArgs():
    parser = argparse.ArgumentParser()
    parser.add_argument("-s", "--selection_json", type=str,
                        required=True, help="Name of json file containing" 
                        " cuts to make, e.g. Cuts/preselection.json")
    parser.add_argument("-f", "--filelist", 
                        type=lambda x: [i.strip() for i in x.split(",")],
                        required=True, help="List of input file names "
                        "to be processed (separated by commas)")
    parser.add_argument("-o", "--output_file_name", type=str,
                        required=True, help="Name of output file")
    return vars(parser.parse_args())
def writeNtupleToFile(output_file, tree, state):
    state_dir = output_file.mkdir(state)
    state_dir.cd()
    ntuple_dir = state_dir.mkdir("final")
    ntuple_dir.cd()
    save_tree = tree.CloneTree()
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
    #file_path = "/hdfs/store/user/dntaylor/data/2015-09-13-13TeV-WZ/WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8/make_ntuples_cfg-008E7FBF-9218-E511-81E0-001E675A5244.root"
    #file_path = "/hdfs/store/user/dntaylor/data/2015-09-13-13TeV-WZ/WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8/*"
    output_file = ROOT.TFile(output_file_name, "RECREATE")
    ROOT.gROOT.cd()
    for state in ["eee"]:#, "eem", "emm", "mmm"]:
        tree = ROOT.TChain("%s/final/Ntuple" % state)
        for file_name in filelist:
            tree.Add(file_name)
        ApplySelection.applySelection(tree, state, selection_json)
        writeNtupleToFile(output_file, tree, state)
    os.chdir(current_path)

def main():
    args = getComLineArgs()
    skimNtuple(args['selection_json'], args['filelist'], args['output_file_name'])
if __name__ == "__main__":
    main()
