#!/usr/bin/env python
import ROOT
import glob
import datetime
import ConfigureJobs, OutputTools
import sys
import os

def applySelector(filelist, selector_name, selection, 
        rootfile,
        analysis, channels=["eee", "eem", "emm", "mmm"], 
        extra_inputs = [],
        nanoAOD=False,
        addSumweights=True,
        proof=False):
    ntuple = ROOT.TNamed("ntupleType", "NanoAOD" if nanoAOD else "UWVV")
    extra_inputs.append(ntuple)
    for i, chan in enumerate(channels):
        inputs = ROOT.TList()
        for inp in extra_inputs:
            inputs.Add(inp)
        tchannel = ROOT.TNamed("channel", chan)
        inputs.Add(tchannel)
        for dataset in ConfigureJobs.getListOfFiles(filelist, selection):
            tname = ROOT.TNamed("name", dataset)
            inputs.Add(tname)
            select = getattr(ROOT, selector_name)()
            select.SetInputList(inputs)
            print "Processing channel %s for dataset %s" % (chan, dataset)
            try:
                file_path = ConfigureJobs.getInputFilesPath(dataset, 
                    selection, analysis)
                # Only add for one channel
                addWeights = addSumweights and i == 0
                if addWeights:
                    ROOT.gROOT.cd()
                    sumweights_hist = ROOT.TH1D("sumweights", "sumweights", 100, 0, 100)
                processLocalFiles(select, file_path, chan, nanoAOD, addSumweights )
            except ValueError as e:
                print e
                continue
            output_list = select.GetOutputList()
            dataset_list = output_list.FindObject(dataset)
            if not dataset_list or dataset_list.ClassName() != "TList":
                print "WARNING: No output found for dataset %s" % dataset
                dataset_list = output_list.FindObject("Unknown")
                if dataset_list and dataset_list.ClassName() == "TList":
                    print 'WARNING: Falling back to dataset "Unknown"'
                else:
                    print 'WARNING: Skipping dataset %s' % dataset
                    continue
            if addSumweights:
                dataset_list.Add(ROOT.gROOT.FindObject("sumweights"))

            OutputTools.writeOutputListItem(dataset_list, rootfile)
            output_list.Delete()
            ROOT.gROOT.GetList().Delete()
        #if chain.GetEntries() != 0 and proof:
        #proof_path = "_".join([analysis, selection+("#/%s/ntuple" % chan)])
        #ROOT.gProof.Process(proof_path, select, "")

def processLocalFiles(selector, file_path, chan, nanoAOD, addSumweights):
    xrootd = "/store/user" in file_path
    if not (xrootd or os.path.isfile(file_path) or os.path.isdir(file_path.rsplit("/", 1)[0])):
        raise ValueError("Invalid path! Skipping dataset. Path was %s" 
            % file_path)

    # Assuming these are user files on HDFS, otherwise it won't work
    filenames =  glob.glob(file_path) if not xrootd else \
            ConfigureJobs.getListOfHDFSFiles(file_path)
    for i, filename in enumerate(filenames):
        if "/store/user" in filename:
            filename = 'root://cmsxrootd.hep.wisc.edu/' + filename
        rtfile = ROOT.TFile.Open(filename)
        tree_name = "Events" if nanoAOD else "%s/ntuple" % chan
        tree = rtfile.Get(tree_name)
        if not tree:
            raise ValueError(("tree %s not found for file %s. " \
                    "Probably the file is corrupted") % (tree_name, filename)
            )

        tree.Process(selector, "")
        if addSumweights:
            fillSumweightsHist(rtfile, i+1, nanoAOD)
        rtfile.Close()

def fillSumweightsHist(rtfile, filenum, isNanoAOD):
    if isNanoAOD:
        sumweights_branch = "genEventSumw"
        meta_tree_name = "Runs"
    else:
        sumweights_branch = "sumWeights"
        meta_tree_name = "metaInfo/metaInfo"
    meta_tree = rtfile.Get(meta_tree_name)
    ROOT.gROOT.cd()
    sumweights_hist = ROOT.gROOT.FindObject("sumweights")
    tmplabel = sumweights_hist.GetName()+"_i"
    tmpweights_hist = sumweights_hist.Clone(tmplabel)
    meta_tree.Draw("%i>>%s" % (filenum, tmplabel), sumweights_branch)
    sumweights_hist.Add(tmpweights_hist)
