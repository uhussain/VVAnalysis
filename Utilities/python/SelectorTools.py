#!/usr/bin/env python
import ROOT
import glob
import datetime
import ConfigureJobs, OutputTools
import sys
import os
def applySelector(filelist, channels,selector_name, selection, 
        rootfile,
        analysis="ZZ4l2018", 
        extra_inputs = [],
        addSumweights=True,
        proof=False):
    for i, chan in enumerate(channels):
        #inputs = ROOT.TList()
        #for inp in extra_inputs:
        #    inputs.Add(inp)
        for dataset in ConfigureJobs.getListOfFiles(filelist, selection):
            select = getattr(ROOT, selector_name)()
            inputs = ROOT.TList()
            select.SetInputList(inputs)
            #This part is introduced to work out the fact that I dont have "extra_inputs" such as SFs yet.
            tchan=ROOT.TNamed("channel",chan)
            tname=ROOT.TNamed("name",dataset) 
            inputs.Add(tname)
            inputs.Add(tchan)
            ROOT.gROOT.cd()
            #So line 38 doesn't give a false because of null list?
            print "Processing channel %s for dataset %s" % (chan, dataset)
            try:
                #print dataset
                file_path = ConfigureJobs.getInputFilesPath(dataset, 
                    selection, analysis)
                print file_path
                countfiles=processLocalFiles(select, file_path, chan)
                print countfiles
            except ValueError as e:
                print e
                continue
            output_list = select.GetOutputList()
            print "Output_list: ", output_list
            #dataset_list = output_list.FindObject(dataset)
            #if not dataset_list or dataset_list.ClassName() != "TList":
            #    print "WARNING: No output found for dataset %s" % dataset
            #    continue
            # Only add for one channel
            if addSumweights and i == 0:
                meta_chain = ROOT.TChain("metaInfo/metaInfo")
                meta_chain.Add(file_path)
                sumweights = ROOT.TH1D("sumweights", "sumweights", 1, 0, 10)
                meta_chain.Draw("1>>sumweights", "summedWeights")
                dataset_list = output_list.FindObject(dataset)
                dataset_list.Add(ROOT.gROOT.FindObject("sumweights"))
                ROOT.SetOwnership(sumweights,False)
                dataset_list.SetOwner()
            for item in output_list:
                if "PROOF" in item.GetName() or item.GetName() == "MissingFiles":
                    continue
                OutputTools.writeOutputListItem(item, rootfile)
                ROOT.SetOwnership(item,False)
                item.Delete()
            if hasattr(sumweights,"Delete"):
                sumweights.Delete()
            #OutputTools.writeOutputListItem(dataset_list, rootfile)
            #output_list.Delete()
            #ROOT.gROOT.GetList().Delete()
        #if chain.GetEntries() != 0 and proof:
        #proof_path = "_".join([analysis, selection+("#/%s/ntuple" % chan)])
        #ROOT.gProof.Process(proof_path, select, "")

def processLocalFiles(selector, file_path, chan):
    if not os.path.isdir(file_path.rsplit("/", 1)[0]):
        raise ValueError("Invalid path! Path was %s" 
            % file_path)
    countfiles=0
    for filename in glob.glob(file_path):
        print "filename: ", filename
        rtfile = ROOT.TFile(filename)
        tree = rtfile.Get("%s/ntuple" % chan)
        print "tree exists"
        if not tree:
            raise ValueError(("tree %s/ntuple not found for file %s. " \
                    "Probably it is corrupted") % (chan, filename)
            )

        tree.Process(selector, "")
        countfiles=countfiles+1
    return countfiles
