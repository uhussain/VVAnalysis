#!/usr/bin/env python
import ROOT
import argparse
from python import ConfigureJobs

parser = argparse.ArgumentParser() 
parser.add_argument("-f", "--filelist", 
                    type=lambda x : [i.strip() for i in x.split(',')],
                    required=True, help="List of input file names "
                    "to be processed (separated by commas)")
parser.add_argument("-s", "--selection", required=True)
args = parser.parse_args()
filelist = ConfigureJobs.getListOfFiles(args.filelist)
states = ["eee", "emm", "eem", "mmm"]
totals = dict((i,0) for i in states)
for name in filelist:
    if ".root" not in name:
        file_path = ConfigureJobs.getInputFilesPath(name,
            args.selection, False) + "/*"
    else:
        file_path = name
    print "Results for file %s" % name
    print "File path is %s" % file_path
    for state in states:
        chain = ROOT.TChain("%s/final/Ntuple" % state)
        chain.Add(file_path)
        num_events = chain.GetEntries()
        print "Number of events in state %s is %i" % (state, num_events)
        totals[state] += num_events
print ""
print "Results for all files:"
for state, count in totals.iteritems():
    print "Summed events for all files in %s state is %i" % (state, count)
