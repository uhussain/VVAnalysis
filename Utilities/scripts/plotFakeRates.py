# coding: utf-8
import ROOT
import datetime
import os
ROOT.gROOT.SetBatch(True)
canvas = ROOT.TCanvas("canvas", "canvas")

def getTGraphAsymmErrors(frfile, folder, param, obj):
    tight_hist = frfile.Get("%s/passingTight%s_all%s" % (folder, param, obj))
    loose_hist = frfile.Get("%s/passingLoose%s_all%s" % (folder, param, obj))
    graph = ROOT.TGraphAsymmErrors(tight_hist, loose_hist)
    graph.SetMarkerStyle(6)
    graph.SetMinimum(0)
    graph.SetMaximum(0.8)
    return graph

folder_name = datetime.date.today().strftime("%Y%b")
outdir = "~/www/DibosonAnalysisData/PlottingResults/WZxsec2016/FakeRates/" + folder_name
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
        draw_opt = "PA" if "2D" not in param else "colz text"
        if "2D" in param:
            data_ewkcorr_graph.GetYaxis().SetTitle("#eta")
            data_ewkcorr_graph.SetTitle("%s Fake Rate" % ("Muon" if "Mu" in obj else "Electron"))
        else:
            data_ewkcorr_graph.GetYaxis().SetTitle("Passing Tight / Passing Loose")
            xlabel = "p_{T} [GeV]" if "Pt" in param else "#eta"
            data_ewkcorr_graph.GetXaxis().SetTitle(xlabel)
        data_ewkcorr_graph.Draw(draw_opt)

        text_box = ROOT.TPaveText(0.2, 0.92, 0.4, 0.86, "NDC")
        text_box.SetFillColor(0)
        text_box.SetLineColor(ROOT.kBlack)
        text_box.SetTextFont(42)
        text_box.AddText("%s Fake Rate" % ("#mu" if "Mu" in obj else "e"))
        text_box.SetShadowColor(0)
        text_box.Draw()
            
        
        if not "2D" in param:
            data_uncorr_graph = getTGraphAsymmErrors(frfile, "AllData", param, obj)
            data_uncorr_graph.Draw("P")

            legend = ROOT.TLegend(0.2,.85,.55,.70)
            legend.AddEntry(data_uncorr_graph, "Data", "l")
            legend.AddEntry(data_ewkcorr_graph, "Data - EWK", "l")
            legend.Draw()


        canvas.Print("%s/ratio%s_all%s.png" % (outdir, param, obj))
        canvas.Print("%s/ratio%s_all%s.pdf" % (outdir, param, obj))
#histAllratio1DPt_DY_m = frfile.Get("DYm50/ratio1DPt_allMu")
#histAllratio1DPt_DY_m.SetLineColor(ROOT.kGreen)
#histAllratio1DPt_DY_m = frfile.Get("DYm50/ratio1DPt_mmm")
#histAllratio1DPt_DY_m.Add(frfile.Get("DYm50/ratio1DPt_eem"))
#histAllratio1DPt_DY_m.SetLineColor(ROOT.kGreen)
#histAllratio1DPt_DY_m.Draw("same hist e1")
#histAllratio1DPt_DY_m.Scale(36.8*1000*5765.4/frfile.Get("DYm50/sumweights").Integral)
#histAllratio1DPt_DY_m.Scale(36.8*1000*5765.4/frfile.Get("DYm50/sumweights").Integral())
#histAllratio1DPt_DY_m.Draw("same hist e1")
