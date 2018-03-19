#!/usr/bin/env python
import ROOT
import glob
import datetime
import ConfigureJobs, OutputTools
import sys
import os

def runSelector(dataset, selector, selection, rootfile, 
        chan, analysis="WZxsec2016", extra_inputs = [],
        addsumweights=False, proof=False):
    inputs = ROOT.TList()
    selector.SetInputList(inputs)
    tchan = ROOT.TNamed("channel", chan)
    tname = ROOT.TNamed("name", dataset)
    inputs.Add(tname)
    inputs.Add(tchan)
    for inp in extra_inputs:
        inputs.Add(inp)
    ROOT.gROOT.cd()
    sumweights_hist = 0
    chain = ROOT.TChain("%s/ntuple" % chan)
    meta_chain = ROOT.TChain("metaInfo/metaInfo")
    try:
        file_path = ConfigureJobs.getInputFilesPath(dataset, 
            selection, analysis)
        if not os.path.isdir(file_path.rsplit("/", 1)[0]):
            raise ValueError("file_path is not a valid path for "
                "dataset %s" % dataset)
        chain.Add(file_path)
        if chain.GetEntries() != 0 and proof:
            proof_path = "_".join([dataset, analysis, 
                selection+("#/%s/ntuple" % chan)])
            ROOT.gProof.Process(proof_path, selector, "")
        else: 
            chain.Process(selector, "")
        if "data" not in dataset and addsumweights and chan == "eee":
            sumweights_hist = ROOT.TH1D("sumweights", "sumweights", 1,0,100)
            meta_chain.Add(file_path)
            meta_chain.Draw("1>>sumweights", "summedWeights")
            if sumweights_hist.Integral() <= 0:
                raise ValueError("Sum of weights <= 0 found for file"
                    "%s. Probably the file is empty." % dataset)
            sumweights_hist.SetDirectory(0)
    except ValueError as e:
        print e
        if sumweights_hist:
            sumweights_hist.Delete()
        return
    output_list = selector.GetOutputList()
    dataset_list = output_list.FindObject(dataset)
    if not dataset_list or dataset_list.ClassName() != "TList":
        raise RuntimeError("WARNING: No output found for dataset %s" % dataset)
    if sumweights_hist:
        dataset_list.Add(sumweights_hist)
    OutputTools.writeOutputListItem(dataset_list, rootfile)
    output_list.Delete()

def applySelector(filelist, selector_name, selection, 
        rootfile,
        analysis="WZxsec2016", channels=["eee", "eem", "emm", "mmm"], 
        extra_inputs = [],
        addsumweights=False, proof=False):
    for dataset in ConfigureJobs.getListOfFiles(filelist, selection):
        select = getattr(ROOT, selector_name)()
        for chan in channels:
            try:
                runSelector(dataset, select, selection, rootfile,
                        chan, analysis, extra_inputs, addsumweights, proof)
                ROOT.gROOT.GetList().Delete()
            except RuntimeError as e:
                print e
