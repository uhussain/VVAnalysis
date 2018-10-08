# coding: utf-8
import ROOT
nocorr_file = ROOT.TFile("NoCorrection/VBSselection_Loose_Full-04Oct2018_Higgs_NoPrefireCorrection_wCR.root")
corr_file = ROOT.TFile("WithPrefiringCorrection/VBSselection_Loose_Full-04Oct2018_Higgs_WithPrefireCorrection_wCR.root")
jakobs_file = ROOT.TFile("Jakob/higgs30/wz3l3lHig_input_13TeV2016.root")

corrections_file = ROOT.TFile("VBSselection_LooseSelection_Higgs_prefiringCorrections.root", "RECREATE") 
output_file = ROOT.TFile("VBSselection_Higgs_fromJakob_withCorrections.root", "Recreate")
var = "MTWZ_wCR"

names = {"EWWZ" : "EW-WZjj", 
        "WZ" :"QCD-WZjj", 
        "VVV" : "top-ewk", 
        "ZZ" : "vv", 
        "Fake" : "DataEWKCorrected", 
        "Zg" : "zg"
}

names.update({ "Higgs_M%i" % m : "chargedHiggsWZ-m%i" % m for m in [300,400,500,600,700,800,900,1000,1500,2000]})

corrections_file.cd()
for folder in names.values():
    nonprompt = "DataEWKCorrected" in folder
    hist_name = var if not nonprompt else var + "_Fakes"
    corr_hist = corr_file.Get("/".join([folder, hist_name]))
    uncorr_hist = nocorr_file.Get("/".join([folder, hist_name]))
    int_ratio = corr_hist.Integral()/uncorr_hist.Integral()
    print "Overall correction to yield for sample %s is %s" % (folder, int_ratio)
    ratio = corr_hist.Clone(folder +"_ratio")
    ROOT.SetOwnership(ratio, False)
    # These seem too unstable to take bin-by-bin corrections
    if nonprompt:
        for i in range(ratio.GetNbinsX()+1):
            ratio.SetBinContent(i, int_ratio)
    else:
        ratio.Divide(uncorr_hist)
    ratio.Write()
    canvas_name = folder+"_canvas"
    canvas = ROOT.TCanvas(canvas_name, canvas_name)
    ratio.Draw("hist e1")
    ratio.GetXaxis().SetTitle("m_{T}(WZ) [GeV]")
    ratio.GetYaxis().SetTitle("Prefire corr. / uncorr.")
    canvas.Print("/afs/cern.ch/user/k/kelong/www/ChargedHiggs/Kenneth/PrefireCorrRatios/%s.pdf" % folder)

for hist_key in jakobs_file.GetListOfKeys():
    hist_name = hist_key.GetName()
    if "Data" in hist_name:
        continue
    hist = jakobs_file.Get(hist_name)
    output_file.cd()
    corrected_hist = hist.Clone()
    isHiggs = "Higgs" in corrected_hist.GetName()
    name = "_".join(corrected_hist.GetName().split("_")[1:2+isHiggs])
    if name not in names.keys():
        print "WARNING: Found no correction for process %s" % name
        continue
    correction_hist = corrections_file.Get(names[name]+"_ratio")
    print correction_hist.GetNbinsX()
    print hist.GetNbinsX()
    hist.Multiply(correction_hist)
    hist.Write()

print ROOT.gROOT.FindObject("EW-WZjj_ratio")
