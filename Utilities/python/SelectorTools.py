#!/usr/bin/env python
import ROOT
import glob
import datetime
import ConfigureJobs, OutputTools
import sys
import os

def applySelector(filelist, selector_name, selection, 
        rootfile,
        analysis="WZxsec2016", channels=["eee", "eem", "emm", "mmm"], 
        extra_inputs = [],
        proof=False):
    for chan in channels:
        select = getattr(ROOT, selector_name)()
        inputs = ROOT.TList()
        for inp in extra_inputs:
            inputs.Add(inp)
        select.SetInputList(inputs)
        for dataset in ConfigureJobs.getListOfFiles(filelist, selection):
            try:
                file_path = ConfigureJobs.getInputFilesPath(dataset, 
                    selection, analysis)
                processLocalFiles(select, file_path, chan)
            except ValueError as e:
                print e
                continue
            output_list = select.GetOutputList()
            dataset_list = output_list.FindObject(dataset)
            if not dataset_list or dataset_list.ClassName() != "TList":
                print "WARNING: No output found for dataset %s" % dataset
                continue
            OutputTools.writeOutputListItem(dataset_list, rootfile)
            output_list.Delete()
            ROOT.gROOT.GetList().Delete()

def processLocalFiles(selector, file_path, chan):
    if not os.path.isdir(file_path.rsplit("/", 1)[0]):
        raise ValueError("Invalid path for dataset"
            "%s. Path was %s" % (dataset, file_path))
    for filename in glob.glob(file_path):
        rtfile = ROOT.TFile(filename)
        tree = rtfile.Get("%s/ntuple" % chan)
        tree.Process(selector, "")
        #if chain.GetEntries() != 0 and proof:
        #    proof_path = "_".join([dataset, analysis, 
        #        selection+("#/%s/ntuple" % chan)])
        #    ROOT.gProof.Process(proof_path, select, "")
