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
        addsumweights=False, proof=False):
    for dataset in ConfigureJobs.getListOfFiles(filelist, selection):
        for chan in channels:
            select = getattr(ROOT, selector_name)()
            inputs = ROOT.TList()
            select.SetInputList(inputs)
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
                    raise ValueError("Invalid path for dataset"
                        "%s. Path was %s" % (dataset, file_path))
                chain.Add(file_path)
                if chain.GetEntries() != 0 and proof:
                    proof_path = "_".join([dataset, analysis, 
                        selection+("#/%s/ntuple" % chan)])
                    ROOT.gProof.Process(proof_path, select, "")
                else: 
                    chain.Process(select, "")
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
                continue
            output_list = select.GetOutputList()
            dataset_list = output_list.FindObject(dataset)
            if not dataset_list or dataset_list.ClassName() != "TList":
                print "WARNING: No output found for dataset %s" % dataset
                continue
            if sumweights_hist:
                dataset_list.Add(sumweights_hist)
            OutputTools.writeOutputListItem(dataset_list, rootfile)
            output_list.Delete()
            ROOT.gROOT.GetList().Delete()

