#!/usr/bin/env python
import ROOT
import argparse
import os
from array import array

def addWeightsForState(original_file, state):
    tree = original_file.Get("%s/ntuple" % state)
    tree.SetBranchStatus("*", 0)
    if state == "eee":
        leps = ["e1", "e2", "e3"]
    elif state == "eem":
        leps = ["e1", "e2", "m"]
    elif state == "emm":
        leps = ["e", "m1", "m2"]
    elif state == "mmm":
        leps = ["m1", "m2", "m3"]
    else:
        print "That don't work :("
        exit(1)
    lepTightFR_weights = [array('f', [0]), array('f', [0]), array('f', [0])]
    lepTightFR_branches = []
    lepMediumFR_weights = [array('f', [0]), array('f', [0]), array('f', [0])]
    lepMediumFR_branches = []
    for i, lep in enumerate(leps):
        lepTightFR_branches.append(tree.Branch('%sCBTightIDFR' % lep, lepTightFR_weights[i], '%sCBTightIDFR/F' % lep)) 
        lepMediumFR_branches.append(tree.Branch('%sCBMediumIDFR' % lep, lepMediumFR_weights[i], '%sCBMediumIDFR/F' % lep)) 
        tree.SetBranchStatus("%sPt" % lep, 1)
        tree.SetBranchStatus("%sEta" % lep, 1)
        tree.SetBranchStatus("%sCBTightIDFR" % lep, 1)
        tree.SetBranchStatus("%sCBTightIDFR" % lep, 1)
    for entry in range(tree.GetEntries()):
        tree.GetEntry(entry)
        for i, lep in enumerate(leps):
            pt = getattr(tree, "%sPt" % lep)
            if pt > 100: pt = 100
            absEta = abs(getattr(tree, "%sEta" % lep))
            if "e" in lep:
                lepTightFR_weights[i][0] = ROOT.eCBTightFakeRate(pt, absEta)
                lepMediumFR_weights[i][0] = ROOT.eCBMedFakeRate(pt, absEta)
            else: 
                lepTightFR_weights[i][0] = ROOT.mCBTightFakeRate(pt, absEta)
                lepMediumFR_weights[i][0] = ROOT.mCBMedFakeRate(pt, absEta)
            lepTightFR_branches[i].Fill()
            lepMediumFR_branches[i].Fill()
    tree.SetBranchStatus("*", 1)
    original_file.cd(state)
    tree.Write("", ROOT.TObject.kOverwrite)

parser = argparse.ArgumentParser()
parser.add_argument("-f", "--input_file", required=True)
args = parser.parse_args()

file_path = os.path.dirname(os.path.realpath(__file__))
scalefac_path = "/".join([file_path, "../../ScaleFactors"])
ROOT.gROOT.LoadMacro("/".join([scalefac_path, "ScaleFactor.C+"]))
fScales = ROOT.TFile("/".join([scalefac_path, "scaleFactors.root"]))

mCBTightFakeRate = fScales.Get('mCBTightFakeRate')
mCBMedFakeRate = fScales.Get('mCBMedFakeRate')
eCBTightFakeRate = fScales.Get('eCBTightFakeRate')
eCBMedFakeRate = fScales.Get('eCBMedFakeRate')

mCBTightFakeRate.RegisterGlobalFunction(2)
mCBMedFakeRate.RegisterGlobalFunction(2)
eCBTightFakeRate.RegisterGlobalFunction(2)
eCBMedFakeRate.RegisterGlobalFunction(2)

original_file = ROOT.TFile(args.input_file, "UPDATE")

states = ['eee', 'eem', 'emm', 'mmm']
for state in states:
    addWeightsForState(original_file, state)
