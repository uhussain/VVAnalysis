#!/usr/bin/env python
import ROOT
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-f", "--file_path", required=True)

args = parser.parse_args()

for state in ["eee", "emm", "eem", "mmm"]:
    chain = ROOT.TChain("%s/final/Ntuple" % state)
    file_path = args.file_path
    if ".root" not in args.file_path:
        file_path = file_path.rstrip("/*") + "/*"
    chain.Add(file_path)
    print "Number of events in state %s is %i" % (state, chain.GetEntries())
