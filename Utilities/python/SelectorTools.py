#!/usr/bin/env python
import ROOT
import glob
import datetime
import ConfigureJobs, OutputTools
import sys
import os
import multiprocessing
import subprocess

def applySelector(filelist, selector_name, selection, 
        rootfile,
        analysis, channels=["eee", "eem", "emm", "mmm"], 
        extra_inputs = [],
        nanoAOD=False,
        addSumweights=True):
    ntuple = ROOT.TNamed("ntupleType", "NanoAOD" if nanoAOD else "UWVV")
    extra_inputs.append(ntuple)
    outfile_name = rootfile.GetName()
    for i, chan in enumerate(channels):
        print "INFO: Processing channel %s" % chan
        inputs = ROOT.TList()
        for inp in extra_inputs:
            inputs.Add(inp)
        tchannel = ROOT.TNamed("channel", chan)
        inputs.Add(tchannel)
        tname = ROOT.TNamed("name", "")
        inputs.Add(tname)
        tree_name = getTreeName(nanoAOD, chan)
        datasets = ConfigureJobs.getListOfFiles(filelist, selection)
        #for dataset in datasets:
        #    print "Processing channel %s for dataset %s" % (chan, dataset)
        #    processDataset(dataset, analysis, selection, tree_name, selector_name, inputs, addSumweights, i)
        # TODO: Stop passing a bazillion arguments around. Make a class
        processParallelByDataset(datasets, analysis, selection, tree_name, selector_name, inputs, addSumweights, rootfile, i)
    # Store arrays in temp files, since it can get way too big to keep around in memory
    rval = subprocess.call(["hadd", "-f", outfile_name] + ["%s_temp.root" % d for d in datasets])
    if rval == 0:
        for d in datasets:
            os.remove("%s_temp.root" % d)


def processDataset(dataset, analysis, selection, tree_name, selector_name, inputs, addSumweights, outfile, chanNum=0):
    print "Processing dataset %s" % dataset
    select = getattr(ROOT, selector_name)()
    select.SetInputList(inputs)
    name = select.GetInputList().FindObject("name")
    if name:
        name.SetTitle(dataset)
    try:
        file_path = ConfigureJobs.getInputFilesPath(dataset, 
            selection, analysis)
        # Only add for one channel
        addSumweights = addSumweights and chanNum == 0 and "data" not in dataset
        if addSumweights:
            ROOT.gROOT.cd()
            sumweights_hist = ROOT.TH1D("sumweights", "sumweights", 100, 0, 100)
        processLocalFiles(select, file_path, tree_name, addSumweights)
    except ValueError as e:
        print e
        return
    output_list = select.GetOutputList()
    dataset_list = output_list.FindObject(dataset)
    if not dataset_list or dataset_list.ClassName() != "TList":
        print "WARNING: No output found for dataset %s" % dataset
        dataset_list = output_list.FindObject("Unknown")
        if dataset_list and dataset_list.ClassName() == "TList":
            print 'WARNING: Falling back to dataset "Unknown"'
        else:
            print 'WARNING: Skipping dataset %s' % dataset
            return
    if addSumweights:
        dataset_list.Add(ROOT.gROOT.FindObject("sumweights"))
    #if len(multiprocessing.active_children()) > 0:
    if True:
        outfile.Close()
        outfile = ROOT.TFile.Open("%s_temp.root" % dataset, "recreate" if chanNum == 0 else "update")
    OutputTools.writeOutputListItem(dataset_list, outfile)
    if True:
        outfile.Close()
    output_list.Delete()

def getFileNames(file_path):
    xrootd = "/store/user" in file_path
    if not (xrootd or os.path.isfile(file_path) or os.path.isdir(file_path.rsplit("/", 1)[0])):
        raise ValueError("Invalid path! Skipping dataset. Path was %s" 
            % file_path)

    # Assuming these are user files on HDFS, otherwise it won't work
    filenames =  glob.glob(file_path) if not xrootd else \
            ConfigureJobs.getListOfHDFSFiles(file_path)
    filenames = ['root://cmsxrootd.hep.wisc.edu/' + f if "/store/user" in f[0:12] else f for f in filenames]
    return filenames

def getTreeName(nanoAOD, chan):
    return "Events" if nanoAOD else "%s/ntuple" % chan

def processParallelByDataset(datasets, analysis, selection, tree_name, selector_name, inputs, addSumweights, outfile, i):
    numCores = min(10, len(datasets))
    p = multiprocessing.Pool(processes=numCores)
    p.map(processDatasetHelper, [
        [d, analysis, selection, tree_name, selector_name, inputs, addSumweights, outfile, i] for d in datasets])

# Pool.map can only take in one argument, so expand the array
def processDatasetHelper(args):
    processDataset(*args)

def processLocalFiles(selector, file_path, tree_name, addSumweights):
    filenames = getFileNames(file_path)
    for i, filename in enumerate(filenames):
        processFile(selector, filename, tree_name, addSumweights, i+1)

def processFile(selector, filename, tree_name, addSumweights, filenum=1):
        rtfile = ROOT.TFile.Open(filename)
        tree = rtfile.Get(tree_name)
        if not tree:
            raise ValueError(("tree %s not found for file %s. " \
                    "Probably the file is corrupted") % (tree_name, filename)
            )

        tree.Process(selector, "")
        if addSumweights:
            fillSumweightsHist(rtfile, filenum, tree_name)
        rtfile.Close()

# You can use filenum to index the files and sum separately, but it's not necessary
def fillSumweightsHist(rtfile, filenum, tree_name):
    # NanoAOD
    if tree_name == "Events":
        sumweights_branch = "genEventSumw"
        meta_tree_name = "Runs"
    else:
        sumweights_branch = "summedWeights"
        meta_tree_name = "metaInfo/metaInfo"
    meta_tree = rtfile.Get(meta_tree_name)
    ROOT.gROOT.cd()
    sumweights_hist = ROOT.gROOT.FindObject("sumweights")
    tmplabel = sumweights_hist.GetName()+"_i"
    tmpweights_hist = sumweights_hist.Clone(tmplabel)
    meta_tree.Draw("%i>>%s" % (filenum, tmplabel), sumweights_branch)
    sumweights_hist.Add(tmpweights_hist)
