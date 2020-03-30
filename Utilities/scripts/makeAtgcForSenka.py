import sys
import ROOT
import os
import logging
import json
import array
from python import ConfigureJobs

def addOverflow(hist):
    addOverflowAndUnderflow(hist, underflow=False, overflow=True)

def addOverflowAndUnderflow(hist, underflow=True, overflow=True):
    if not "TH1" in hist.ClassName():
        return
    if overflow:
        # Returns num bins + overflow + underflow
        num_bins = hist.GetNbinsX()
        add_overflow = hist.GetBinContent(num_bins) + hist.GetBinContent(num_bins + 1)
        hist.SetBinContent(num_bins, add_overflow)
    if underflow:
        add_underflow = hist.GetBinContent(0) + hist.GetBinContent(1)
        hist.SetBinContent(1, add_underflow)

#if manager_path not in sys.path:
#        sys.path.insert(0, "/".join([manager_path,"ZZ4lRun2DatasetManager", "Utilities/python"]))

manager_path = ConfigureJobs.getManagerPath() 
manager_name = ConfigureJobs.getManagerName()
print "manager_path: ",manager_path
print "manager_name: ",manager_name
sys.path.append("/".join([manager_path, manager_name,
    "Utilities/python"]))
#print sys.path
from ConfigHistFactory import ConfigHistFactory
config_factory = ConfigHistFactory(
    "%s/%s" % (manager_path, manager_name),
    "ZZ4l2016/LooseLeptons",
)

fileMap = { "2016" : "/afs/cern.ch/user/u/uhussain/ZZ4lRun2HistFiles/Hists23Jan2020-ZZ4l2016_Moriond.root",
    "2017" : "",
    "2018" : "/afs/cern.ch/user/u/uhussain/ZZ4lRun2HistFiles/Hists23Jan2020-ZZ4l2018_MVA.root",
    }
lumiMap = {"2016":35.9,"2017":41.5,"2018":59.7}
channels = ["eeee", "eemm", "mmmm"]
#channels = ["eemm"]
eVariations = ["CMS_eff_eUp", "CMS_eff_eDown","CMS_RecoEff_eUp", "CMS_RecoEff_eDown"]
mVariations = ["CMS_eff_mUp","CMS_eff_mDown"]
#systVariations=[]
fitvar = "Mass"
rebin = array.array('d', [100.0,200.0,300.0,400.0,500.0,600.0,700.0,800.0,900.0,1000.0,1100.0,1200.0,1300.0,1500.0,1502.0])
outputFolder = "ForSenka/%sFullRunII_2016Scaled_binv3_Mar30" % fitvar
if not os.path.isdir(outputFolder):
    os.makedirs(outputFolder)

#for year in fileMap.keys():
for year in ["2016"]:
    #dataset_file = "%s/ZZ4lRun2DatasetManager/FileInfo/ZZ4l%s/%s.json" % (manager_path,year, "atgc")
    dataset_file = "%s/ZZ4lRun2DatasetManager/FileInfo/ZZ4l%s/%s.json" % (manager_path,year, "ntuples")
    allnames = json.load(open(dataset_file))
    atgcSamples={}
    for name in allnames.keys():
        if "atgc" in name or "sherpa" in name:
            atgcSamples[str(name)]= str(allnames[name]['plot_group'])
    print atgcSamples
    print "No.of samples:", len(atgcSamples)
    plotGroupsMap = {name : config_factory.getPlotGroupMembers(name) for name in atgcSamples.values()}
    xsecs  = ConfigureJobs.getListOfFilesWithXSec([f for files in plotGroupsMap.values() for f in files])
    print "xsecs: ",xsecs
    fileName=fileMap[year]
    print "fileName: ",fileName
    if type(fileName) == str: hist_file = ROOT.TFile.Open(fileName)
    for chan in channels:
        savehists=[]
        outputFile = "ZZatgcInput_%s_%s.root" % (chan,year)
        for process in [str(i) for i in atgcSamples.keys()]:
            sumweights = 0
            if "data" not in process.lower() and "nonprompt" not in process.lower():
                sumweights_hist = hist_file.Get("/".join([process, "sumweights"]))
                if not sumweights_hist:
                    raise RuntimeError("Failed to find sumWeights for dataset %s" % process)
                sumweights = sumweights_hist.Integral(1, sumweights_hist.GetNbinsX()+2)
                sumweights_hist.Delete()
            histNames=[]
            if process=="nonprompt":
                histNames.append("Mass_Fakes_{channel}".format(channel=chan))
            else:
                histNames.append("Mass_{channel}".format(channel=chan))
            if not hist_file.Get(process):
                logging.warning("Skipping invalid filename %s" % process)
                continue
            if process not in ["data","nonprompt"]:
                if chan=="eeee":
                    for var in eVariations:
                        histNames.append("Mass_%s_%s" % (var,chan))
                elif chan=="mmmm":
                    for var in mVariations:
                        histNames.append("Mass_%s_%s" % (var,chan))
                elif chan=="eemm":
                    for var in eVariations+mVariations:
                        histNames.append("Mass_%s_%s" % (var,chan))
            #print "plot_group: ",process
            #print "histNames: ",histNames
            for name in histNames:
                tmphist = hist_file.Get("/".join([process, name]))
                if not tmphist: 
                    raise RuntimeError("Failed to produce histogram %s" % "/".join([process, name]))
                if "eemm" in name:
                    ##combine eemm and mmee channels
                    secname = name.replace("eemm","mmee")
                    tmphist2 = hist_file.Get("/".join([process, secname]))
                    #print tmphist.GetName(),": ",tmphist.Integral()
                    #print tmphist2.GetName(),": ",tmphist2.Integral()
                    tmphist.Add(tmphist2)
                hist = tmphist.Clone()
                #print "total: ",hist.Integral()
                histnew = hist.Rebin(len(rebin)-1,"histnew",rebin)
                if histnew:
                    if sumweights:
                        xsec = xsecs[process]
                        histnew.Scale(xsec*1000*lumiMap[year]/sumweights)
                addOverflow(histnew)
                tmphist.Delete()
                #if process=="zz4l-sherpa":
                #    print "this process happens in 2016"
                #    histnew.Scale(0.455985*1000*35.9*1.239/histnew.Integral())
                #if process=="zz4l-atgc_9" and year=="2018":
                #    print "chan: ",chan
                #    print "this process happens in 2018:",process
                #    print "sumweights: ",sumweights
                #    #histnew.Scale(0.455985*1000*41.5*1.239/histnew.Integral())
                newName = name.replace("Mass",str(atgcSamples[process]))
                newName = newName.replace("_"+chan,"")
                hist.Delete()
                histnew.SetName(newName)
                savehists.append(histnew)

        fOut = ROOT.TFile.Open("/".join([outputFolder, outputFile]), "RECREATE")
        fOut.cd()
        for newhists in savehists:
            newhists.Write()
        fOut.Close()
    hist_file.Close()




