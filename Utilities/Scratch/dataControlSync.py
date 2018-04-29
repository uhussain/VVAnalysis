import ROOT
import json
from python import ApplySelection

selection = "Zlep1IsTight && Zlep2IsTight && WlepIsTight && abs(nLooseLep -3) < 0.1 && metFiltersDataNoBadMuon && mjj > 100 && (dEtajj < 2.5 || mjj < 500)"
with open("controlevent.txt", "w") as outfile:
    file_info = json.load(open("/afs/cern.ch/user/k/kelong/work/AnalysisDatasetManager/FileInfo/WZxsec2016/WselectionMediumLeptons.json"))
    for i, chan in enumerate(["emm",]):
    #for i, chan in enumerate(["mmm", "eem", "emm", "eee"]):
        chain = ROOT.TChain("%s/ntuple" % chan)
        for key, value in file_info.iteritems():
            #if "data" in key:
            if "data_DoubleMuon_Run2016H-03Feb2017_ver2" in key:
                print "Including %s!!!" % key
                chain.Add(value["file_path"])
        print "Chan is", chan
        ApplySelection.setAliases(chain, chan, "Cuts/WZxsec2016/aliases.json")
        cut_chain = chain.CopyTree(selection) 
        print cut_chain.GetEntries()
        for row in cut_chain:
            outfile.write("DATA "+" ".join([str(v) for v in [row.run,row.lumi,row.evt]]) + " " +
                " ".join([str(round(v,2)) for v in [row.jetPt[0],row.jetEta[0],row.jetPhi[0],
                    row.jetPt[1],row.jetEta[1], row.jetPhi[1],row.mjj,abs(row.jetEta[0]-row.jetEta[1]),len(row.jetPt)]]))
            if chan == "eee":
                outfile.write(" " + " ".join([str(round(v,2)) for v in [row.e1Pt,row.e1Eta,row.e2Pt,row.e2Eta]]) + " %i\n" %i)
            elif chan == "eem":
                outfile.write(" " + " ".join([str(round(v,2)) for v in [row.e1Pt,row.e1Eta,row.e2Pt,row.e2Eta]]) + " %i\n" %i)
            elif chan == "emm":
                outfile.write(" " + " ".join([str(round(v,2)) for v in [row.m1Pt,row.m1Eta,row.m2Pt,row.m2Eta]]) + " %i\n" %i)
            elif chan == "mmm":
                outfile.write(" " + " ".join([str(round(v,2)) for v in [row.m1Pt,row.m1Eta,row.m2Pt,row.m2Eta]]) + " %i\n" %i)
