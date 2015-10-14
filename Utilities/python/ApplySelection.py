#!/usr/bin/env python
import ROOT
from collections import OrderedDict
import os
import sys
import UserInput
import time

class CutString(object):
    def __init__(self):
        self.cuts = []
    def append(self, append):
        if self.cuts != "" and append != "":
            self.cuts += append if isinstance(append, list) else [append]
    def getString(self):
        return " && ".join(self.cuts)

def buildCutString(state, cuts_json, trigger):
    cut_string = CutString()
    print "cuts_json is %s" % cuts_json
    cuts = UserInput.readJson(cuts_json)
    cut_string.append(cuts["Event"])
    cut_string.append(cuts["State"][state])
    if trigger != "":
        cut_string.append(getTriggerCutString(trigger))
    counts = dict((lep, state.count(lep)) for lep in state)
    current = dict((lep, 0) for lep in state)
    for lep in state:
        current[lep] += 1
        lep_name = ''.join([lep, "" if counts[lep] == 1 else str(current[lep])])
        for cut in cuts["Object"][lep]:
            cut_string.append(cut.format(object_name=lep_name))
    return cut_string
def setAliases(tree, state, aliases_json):
    aliases = UserInput.readJson(aliases_json)
    for name, value in aliases["State"][state].iteritems():
        print "Setting alias %s to %s" % (name, value)
        tree.SetAlias(name, value)
def getTriggerCutString(trigger):
    triggers = UserInput.readJson("Cuts/triggers.json")
    return triggers[trigger]
def applySelection(tree, state, selection_json, trigger):
    setAliases(tree, state, "Cuts/aliases.json")
    cut_string = buildCutString(state, selection_json, trigger)
    #tree.SetProof()
    listname = '_'.join(["list", state])
    num_passing = tree.Draw(">>" + listname, cut_string.getString(), "entrylist")
    print "%i events passed the cut " % num_passing
    print cut_string.getString()
    #tlist = ROOT.gProof.GetOutputList().FindObject(listname) 
    tlist = ROOT.gDirectory.FindObject(listname);
    print tlist
    tree.SetEntryList(tlist)
    print tree.GetEntries()
