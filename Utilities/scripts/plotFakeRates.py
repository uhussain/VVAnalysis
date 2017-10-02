# coding: utf-8
import ROOT
import datetime
import os
ROOT.gROOT.SetBatch(True)
canvas = ROOT.TCanvas("canvas", "canvas")

def getTGraphAsymmErrors(frfile, folder, param, obj):
    tight_hist = frfile.Get("%s/passingTight%s_all%s" % (folder, param, obj))
    print tight_hist
    print "%s/passingTight%s_all%s" % (folder, param, obj)
    loose_hist = frfile.Get("%s/passingLoose%s_all%s" % (folder, param, obj))
    print loose_hist
    return ROOT.TGraphAsymmErrors(tight_hist, loose_hist)

today = datetime.date.today().strftime("%d%b%Y")
outdir = "~/www/WZFakeRate/%s-TightLeps" % today
frfile = ROOT.TFile("/eos/user/k/kelong/WZAnalysisData/FakeRates/fakeRate02Oct2017-TightLepsFrom3LooseLeptons.root")

try:
    os.mkdir(os.path.expanduser(outdir))
except OSError as e:
    print e
    pass
for param in ["1DPt", "1DEta", "2D"]:
    for obj in ["Mu", "E"]:
        data_ewkcorr_graph = frfile.Get("DataEWKCorrected/ratio%s_all%s" % (param, obj)) \
            if "2D" in param else getTGraphAsymmErrors(frfile, "DataEWKCorrected", param, obj)
        data_ewkcorr_graph.SetLineColor(ROOT.kRed)
        data_ewkcorr_graph.SetMarkerStyle(6)
        data_ewkcorr_graph.SetMinimum(0)
        data_ewkcorr_graph.SetMaximum(0.8)
        data_ewkcorr_graph.GetYaxis().SetTitle("Passing Tight / Passing Loose")
        draw_opt = "PA" if "2D" not in param else "colz text e1"
        data_ewkcorr_graph.Draw(draw_opt)
        
        if not "2D" in param:
            data_uncorr_graph = getTGraphAsymmErrors(frfile, "AllData", param, obj)
            data_uncorr_graph.SetMinimum(0)
            data_uncorr_graph.SetMaximum(0.8)
            data_uncorr_graph.SetMarkerStyle(6)
            data_uncorr_graph.Draw("P")
            
            legend = ROOT.TLegend(0.2,.9,.5,.75)
            legend.AddEntry(data_uncorr_graph, "Data", "l")
            legend.AddEntry(data_ewkcorr_graph, "Data - EWK", "l")
            legend.Draw()

        canvas.Print("%s/ratio%s_all%s_correrr.png" % (outdir, param, obj))
#histAllratio1DPt_DY_m = frfile.Get("DYm50/ratio1DPt_allMu")
#histAllratio1DPt_DY_m.SetLineColor(ROOT.kGreen)
#histAllratio1DPt_DY_m = frfile.Get("DYm50/ratio1DPt_mmm")
#histAllratio1DPt_DY_m.Add(frfile.Get("DYm50/ratio1DPt_eem"))
#histAllratio1DPt_DY_m.SetLineColor(ROOT.kGreen)
#histAllratio1DPt_DY_m.Draw("same hist e1")
#histAllratio1DPt_DY_m.Scale(36.8*1000*5765.4/frfile.Get("DYm50/sumweights").Integral)
#histAllratio1DPt_DY_m.Scale(36.8*1000*5765.4/frfile.Get("DYm50/sumweights").Integral())
#histAllratio1DPt_DY_m.Draw("same hist e1")
