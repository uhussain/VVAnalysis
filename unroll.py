import array
import ROOT

initial_eta_min = 0
initial_eta_max = 8
initial_eta_step = 0.25

initial_mjj_min = 0
initial_mjj_max = 2500
initial_mjj_step = 50

canvas = ROOT.TCanvas("canvas", "canvas")

input_file = ROOT.TFile("/eos/user/k/kelong/WZAnalysisData/HistFiles/Wselection-30Nov2017-TightFrom3LooseLeptons.root")
mjj_etajj_2D_hist = input_file.Get("wlljj-ewk/mjj_etajj_2D_eee")
print mjj_etajj_2D_hist.GetNbinsY()

first_bin_etajj = 7
nbins_etajj = 4
group_eta = 6

group_mjj = 10
nbins_mjj = 4

ybins = [2.5, 4, 5.5, 7.5]
xbins = array.array('d', [500, 1000, 1500, 2000, 2500])
nbins = (len(xbins)-1)*nbins_etajj

etajj_upper = (first_bin_etajj-1)*initial_eta_step
etajj_lower = etajj_upper

hists_half_rolled = []
for i in range(first_bin_etajj, mjj_etajj_2D_hist.GetNbinsY()+1, group_eta):
    etajj_upper += initial_eta_step*group_eta
    hist_name = ("mjj_detajj_%0.1fTo%0.1f" % (etajj_lower, etajj_upper)).replace(".","p")
    print hist_name
    upper_range = i + group_eta
    mjj_hist = mjj_etajj_2D_hist.ProjectionX(hist_name, i, upper_range, "e")
    mjj_hist = mjj_hist.Rebin(len(xbins)-1, hist_name+"_rebin", xbins)
    if i > 0:
        mjj_hist.Draw("same")
    else:
        mjj_hist.Draw()
    hists_half_rolled.append(hist_name+"_rebin")
    etajj_lower += initial_eta_step*group_eta

unrolled_hist = ROOT.TH1F("mjj_etajj_unrolled", "unrolled", nbins, 0, nbins)
for i, hist_name in enumerate(hists_half_rolled):
    print hist_name
    hist = canvas.GetListOfPrimitives().FindObject(hist_name)
    print "Number of bins is",hist.GetNbinsX()
    for j in range(1, hist.GetNbinsX()+1):
        entry = i*(hist.GetNbinsX()) + j
        print "entry will be", entry
        print "pulling from bin", j, "in old hist. Value: ", hist.GetBinContent(j)
        content = hist.GetBinContent(j)
        if j == hist.GetNbinsX():
            content += hist.GetBinContent(j+1)
        unrolled_hist.SetBinContent(entry, content)

unrolled_hist.Draw()
