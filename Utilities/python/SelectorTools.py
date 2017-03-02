#!/usr/bin/env python
import ROOT
import glob
import datetime
import ConfigureJobs


def applySelector(filelist, selector_name, selection, 
        analysis="WZxsec2016", channels=["eee", "eem", "emm", "mmm"], 
        extra_inputs = [],
        addsumweights=False, proof=False):
    path = ConfigureJobs.getManagerPath()
    output = []
    for dataset in ConfigureJobs.getListOfFiles(filelist, path):
        for chan in channels:
            select = getattr(ROOT, selector_name)()
            inputs = ROOT.TList()
            select.SetInputList(inputs)
            tchan = ROOT.TNamed("channel", chan)
            tname = ROOT.TNamed("name", dataset)
            inputs.Add(tname)
            inputs.Add(tchan)
            for inp in extra_inputs:
                inputs.Add(inp)
            ROOT.gROOT.cd()
            sumweights_hist = 0
            if proof:
                proof_path = "_".join([dataset, analysis, 
                    selection+("#/%s/ntuple" % chan)])
                proof.Process(proof_path, select, "")
                proof_meta_path = "_".join([dataset, analysis, 
                    selection+"#/metaInfo/metaInfo"])
                ## TODO proof draw command for meta tree
                #proof.DrawSelect(proof_path, "1>>sumweights", "")
            else: 
                chain = ROOT.TChain("%s/ntuple" % chan)
                meta_chain = ROOT.TChain("metaInfo/metaInfo")
                try:
                    file_path = ConfigureJobs.getInputFilesPath(dataset, 
                        path, selection, analysis)
                    chain.Add(file_path)
                    chain.Process(select, "")
                    if "data" not in dataset and addsumweights and chan == "eee":
                        sumweights_hist = ROOT.TH1D("sumweights", "sumweights", 1,0,100)
                        meta_chain.Add(file_path)
                        meta_chain.Draw("1>>sumweights", "summedWeights")
                        sumweights_hist.SetDirectory(0)
                except ValueError as e:
                    print e
                    if sumweights_hist:
                        sumweights_hist.Delete()
                    continue
            out = select.GetOutputList()
            if  sumweights_hist:
                outputlist = out.FindObject(dataset)
                outputlist.Add(sumweights_hist)
            output.extend(out)
    return output
