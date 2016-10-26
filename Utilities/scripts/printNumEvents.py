#!/usr/bin/env python
import ROOT
import argparse
import os
from python import ConfigureJobs

parser = argparse.ArgumentParser() 
parser.add_argument("-f", "--filelist", 
                    type=lambda x : [i.strip() for i in x.split(',')],
                    required=True, help="List of input file names "
                    "to be processed (separated by commas)")
parser.add_argument("-s", "--selection", required=True)
parser.add_argument("-m", "--cut_string", required=False, type=str,
                    default="")
parser.add_argument("-c", "--channels", required=False, type=str,
                    default="eee,mmm,eem,emm")
args = parser.parse_args()
path = "/cms/kdlong" if "hep.wisc.edu" in os.environ['HOSTNAME'] else \
        "/afs/cern.ch/user/k/kelong/work"
filelist = ConfigureJobs.getListOfFiles(args.filelist, path)
states = [x.strip() for x in args.channels.split(",")]
totals = dict((i,0) for i in states)
total = 0
for name in filelist:
    if ".root" not in name:
        file_path = ConfigureJobs.getInputFilesPath(name, path,
            args.selection, "WZxsec2016") + "/*"
    else:
        file_path = name
    print "Results for file %s" % name
    print "File path is %s" % file_path
    for state in states:
        state = state.strip()
        chain = ROOT.TChain("%s/ntuple" % state)
        chain.Add(file_path)
        cut_tree = chain.CopyTree(args.cut_string) if args.cut_string != "" \
            else chain
        num_events = cut_tree.GetEntries()
        print "Number of events in state %s is %i" % (state, num_events)
        totals[state] += num_events
    print "Number of events in all states is %i" % total
print ""
print "Results for all files:"
total = 0
for state, count in totals.iteritems():
    print "Summed events for all files in %s state is %i" % (state, count)
    total += count
print "Summed events for all files in all states is %i" % total

