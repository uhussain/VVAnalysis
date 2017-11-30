import array
import ROOT

canvas = ROOT.TCanvas("canvas", "canvas")

input_file = ROOT.TFile("/eos/user/k/kelong/WZAnalysisData/HistFiles/Wselection-30Nov2017-TightFrom3LooseLeptons.root")
init_2D_hist = input_file.Get("wlljj-ewk/mjj_etajj_2D_eee")

ybins = [2.5, 4, 5.5, 20]
xbins = array.array('d', [500, 1000, 1500, 2000, 2500])
nbins = (len(xbins)-1)*(len(ybins)-1)

hists_half_rolled = []
for i in range(len(ybins)-1):
    ylower = ybins[i]
    yupper = ybins[i+1]
    hist_name = ("mjj_detajj_%0.1fTo%0.1f" % (ylower, yupper)).replace(".","p")
    # Global bin number = nbinx + nbinsx*nbiny
    lower_bin = init_2D_hist.FindBin(0,ylower)/init_2D_hist.GetNbinsX()
    upper_bin = init_2D_hist.FindBin(0,yupper)/init_2D_hist.GetNbinsX()
    mjj_hist = init_2D_hist.ProjectionX(hist_name, lower_bin, upper_bin, "e")
    mjj_hist = mjj_hist.Rebin(len(xbins)-1, hist_name+"_rebin", xbins)
    if i > 0:
        mjj_hist.Draw("same")
    else:
        mjj_hist.Draw()
    hists_half_rolled.append(hist_name+"_rebin")

name = "mjj_etajj_unrolled"
unrolled_hist = ROOT.TH1F(name, "Unrolled", nbins, 0, nbins)
for i, hist_name in enumerate(hists_half_rolled):
    hist = canvas.GetListOfPrimitives().FindObject(hist_name)
    for j in range(1, hist.GetNbinsX()+1):
        entry = i*(hist.GetNbinsX()) + j
        content = hist.GetBinContent(j)
        if j == hist.GetNbinsX():
            content += hist.GetBinContent(j+1)
        unrolled_hist.SetBinContent(entry, content)

unrolled_hist.Draw()
