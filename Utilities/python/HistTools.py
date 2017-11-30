import array
import ROOT

def getDifference(name, dir1, dir2, addRatios=True):
    differences = ROOT.TList()
    differences.SetName(name)
    for histname in [i.GetName() for i in fOut.Get(dir1).GetListOfKeys()]:
        if histname == "sumweights": continue
        hist1 = fOut.Get("/".join([dir1, histname]))
        hist2 = fOut.Get("/".join([dir2, histname]))
        if hist1 and hist2:
            diff = hist1.Clone()
            diff.Add(hist2, -1)
        elif not hist1:
            print "WARNING: Hist %s was not produced for " \
                "dataset(s) %s" % (histname, dir1)
        elif not hist2:
            print "WARNING: Hist %s was not produced for " \
                "dataset(s) %s" % (histname, dir2)
        differences.Add(diff)
    if addRatios:
        ratios = getRatios(differences)
        for ratio in ratios:
            differences.Add(ratio) 
    return differences

def makeUnrolledHist(init_2D_hist, xbins, ybins, name=""):
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
        hists_half_rolled.append(mjj_hist)

    if name is "":
        name = init_2D_hist.GetName().replace("2D", "unrolled")
    unrolled_hist = ROOT.TH1F(name, "Unrolled", nbins, 0, nbins)
    unrolled_hist.SetDirectory(init_2D_hist.GetDirectory())
    for i, hist in enumerate(hists_half_rolled):
        for j in range(1, hist.GetNbinsX()+1):
            entry = i*(hist.GetNbinsX()) + j
            content = hist.GetBinContent(j)
            if j == hist.GetNbinsX():
                content += hist.GetBinContent(j+1)
            unrolled_hist.SetBinContent(entry, content)

    return unrolled_hist

def getTransformedHists(orig_file, folders, input_hists, transformation, transform_inputs):
    output_folders = []
    for folder in folders:
        output_list = ROOT.gROOT.FindObject(folder)
        if not output_list:
            output_list = ROOT.TList()
            output_list.SetName(folder)
        ROOT.SetOwnership(output_list, False)
        for input_hist_name in input_hists:
            orig_hist = orig_file.Get("/".join([folder, input_hist_name]))
            new_hist = transformation(orig_hist, *transform_inputs)
            ROOT.SetOwnership(new_hist, False)
            output_list.Add(new_hist)
        output_folders.append(output_list)
    return output_folders 
