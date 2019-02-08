#!/usr/bin/env python
import ROOT
import glob
import datetime
import ConfigureJobs, OutputTools
import sys
import os
def applySelector(filelist, channels,selector_name, selection, 
        rootfile,
        analysis, 
        extra_inputs = [],
        addSumweights=True,
        proof=False):
    print "channels: ",channels
    for i, chan in enumerate(channels):
        print "channel length: ",len(chan)
        #if len(chan)==4:
            #inputs = ROOT.TList()
            #for inp in extra_inputs:
            #    inputs.Add(inp)
                #print "inputs getting added: ",inp
        for dataset in ConfigureJobs.getListOfFiles(filelist, selection):
            select = getattr(ROOT, selector_name)()
            #This is the original implementation with inputLists and for ZZSelector hist_inputs is still there
            #even if there are no fake rates or scale factors as extra_inputs
            if len(chan)==4:
                inputs = ROOT.TList()
                select.SetInputList(inputs)
                #I have to do this here for all datasets as my extra_inputs only have hist_info which is
                #indpenedent of the datasets so no dataset info was added if you do it before the for loop as commented above.
                #Ultimately once extra_inputs has fake rates, we won't need to add tname for dataset. 
                for inp in extra_inputs:
                    inputs.Add(inp)
                    print "inputs getting added: ",inp
                tchan=ROOT.TNamed("channel",chan)
                tname=ROOT.TNamed("name",dataset) 
                inputs.Add(tname)
                inputs.Add(tchan)
                ROOT.gROOT.cd()
                #print "inputs: ",inputs.ls()
                print "Processing channel %s for dataset %s" % (chan, dataset)
                #print select
                #If its a real issue don't just skip the files, especially data and that's why this try and except is important
                #try:
                file_path = ConfigureJobs.getInputFilesPath(dataset, 
                        selection, analysis)
                processLocalFiles(select, file_path, chan)
                    #countfiles=processLocalFiles(select, file_path, chan)
                    #print "CountFiles: ",countfiles
                #except ValueError as e:
                    #print e
                    #print "CountFiles: ",countfiles
                    #continue
                output_list = select.GetOutputList()
                print "Output_list: ", output_list
                #dataset_list = output_list.FindObject(dataset)
                #if not dataset_list or dataset_list.ClassName() != "TList":
                #    print "WARNING: No output found for dataset %s" % dataset
                #    continue
            #This is a workaround for FakeRateSelector until there are no SFs as inputs
            else:
                inputs = ROOT.TList()
                select.SetInputList(inputs)
                #This part is introduced to work out the fact that I dont have "extra_inputs" such as SFs yet.
                #It doesn't hurt to have this put in by hand
                for inp in extra_inputs:
                    inputs.Add(inp)
                    print "inputs getting added: ",inp
                tchan=ROOT.TNamed("channel",chan)
                tname=ROOT.TNamed("name",dataset) 
                inputs.Add(tname)
                inputs.Add(tchan)
                #print "inputs: ",inputs.Print()
                ROOT.gROOT.cd()
                #So line 56 doesn't give a false because of null list?
                print "Processing channel %s for dataset %s" % (chan, dataset)
                try:
                    #print dataset
                    file_path = ConfigureJobs.getInputFilesPath(dataset, 
                        selection, analysis)
                    #print "file_path",file_path
                    #print file_path
                    processLocalFiles(select, file_path, chan)
                    #print "CountFiles: ",countfiles
                except ValueError as e:
                    print e
                    #print "CountFiles: ",countfiles
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
                print "dataset_list: ", dataset_list
                dataset_list.Add(ROOT.gROOT.FindObject("sumweights"))
                ROOT.SetOwnership(sumweights,False)
                dataset_list.SetOwner()
            for item in output_list:
                #if "PROOF" in item.GetName() or item.GetName() == "MissingFiles":
                #    continue
                OutputTools.writeOutputListItem(item, rootfile)
                ROOT.SetOwnership(item,False)
                item.Delete()
            #if hasattr(sumweights,"Delete"):
            #    sumweights.Delete()
            #OutputTools.writeOutputListItem(dataset_list, rootfile)
            output_list.Delete()
            ROOT.gROOT.GetList().Delete()
        #if chain.GetEntries() != 0 and proof:
        #proof_path = "_".join([analysis, selection+("#/%s/ntuple" % chan)])
        #ROOT.gProof.Process(proof_path, select, "")
#This is a version that has extra_inputs in the shape of fakeRates for Bkgs. This will be the function used for everything once I have sfs setup.
def applyBkgSelector(filelist, channels,selector_name, selection, 
        rootfile,
        analysis="ZZ4l2018", 
        extra_inputs = [],
        addSumweights=True,
        proof=False):
    print "Background Selector"
    for i, chan in enumerate(channels):
        inputs = ROOT.TList()
        for inp in extra_inputs:
            inputs.Add(inp)
            print "inputs getting added: ",inp
        for dataset in ConfigureJobs.getListOfFiles(filelist, selection):
            select = getattr(ROOT, selector_name)()
            select.SetInputList(inputs)
            print "inputs: ",inputs.Print()
            print "Processing channel %s for dataset %s" % (chan, dataset)
            print select
            #If its a real issue don't just skip the files, especially data and that's why this try and except is important
            try:
                file_path = ConfigureJobs.getInputFilesPath(dataset, 
                    selection, analysis)
                processLocalFiles(select, file_path, chan)
                #countfiles=processLocalFiles(select, file_path, chan)
                #print "CountFiles: ",countfiles
            except ValueError as e:
                print e
                continue
            output_list = select.GetOutputList()
            print "BkgSelector Output_list: ", output_list
            dataset_list = output_list.FindObject(dataset)
            if not dataset_list or dataset_list.ClassName() != "TList":
                print "WARNING: No output found for dataset %s" % dataset
                continue
            # Only add for one channel
            if addSumweights and i == 0:
                meta_chain = ROOT.TChain("metaInfo/metaInfo")
                meta_chain.Add(file_path)
                sumweights = ROOT.TH1D("sumweights", "sumweights", 1, 0, 10)
                meta_chain.Draw("1>>sumweights", "summedWeights")
                dataset_list.Add(ROOT.gROOT.FindObject("sumweights"))
            OutputTools.writeOutputListItem(dataset_list, rootfile)
            output_list.Delete()
            ROOT.gROOT.GetList().Delete()
def processLocalFiles(selector, file_path, chan):
    if not os.path.isdir(file_path.rsplit("/", 1)[0]):
        raise ValueError("Invalid path! Path was %s" 
            % file_path)
    #countfiles=0
    for filename in glob.glob(file_path):
        #print "filename: ", filename
        rtfile = ROOT.TFile(filename)
        if(chan=="eemm" or chan=="mmee"):
            tree = rtfile.Get("eemm/ntuple")
        else:
            tree = rtfile.Get("%s/ntuple" % chan)
            #print "tree exists"
        ##Catching the exception and continue here means that even if you find a corrupt/bad file you continue processing, which is not good practice
        #try:
        if not tree:
            raise ValueError(("tree %s/ntuple not found for file %s. " \
                "Probably it is corrupted") % (chan, filename)
            )
        #except ValueError as e:
        #    print e
        #    continue
        tree.Process(selector, "")
        #countfiles=countfiles+1
    #return countfiles
