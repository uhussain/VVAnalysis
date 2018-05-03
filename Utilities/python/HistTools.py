import array
import ROOT

def getDifference(fOut, name, dir1, dir2, ratioFunc=None):
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
    if ratioFunc:
        ratios = ratioFunc(differences)
        for ratio in ratios:
            differences.Add(ratio) 
    return differences

def makeUnrolledHist(init_2D_hist, xbins, ybins, name=""):
    nbins = (len(xbins)-1)*(len(ybins)-1)
    hists_half_rolled = []
    for i in range(len(ybins)-1):
        ylower = ybins[i]
        yupper = ybins[i+1]
        hist_name = ("%s_%0.1fTo%0.1f" % (init_2D_hist.GetName(), ylower, yupper)).replace(".","p")
        # Global bin number = nbinx + nbinsx*nbiny
        lower_bin = init_2D_hist.FindBin(0,ylower)/init_2D_hist.GetNbinsX()
        # Range is inclusive, so don't count upper bin twice
        upper_bin = init_2D_hist.FindBin(0,yupper*(1-0.0001))/init_2D_hist.GetNbinsX()
        ybinned_hist = init_2D_hist.ProjectionX(hist_name, lower_bin, upper_bin, "e")
        ybinned_hist = ybinned_hist.Rebin(len(xbins)-1, hist_name+"_rebin", xbins)
        hists_half_rolled.append(ybinned_hist)

    if name is "":
        name = init_2D_hist.GetName().replace("2D", "unrolled")
    unrolled_hist = ROOT.TH1D(name, "Unrolled", nbins, 0, nbins)
    unrolled_hist.SetDirectory(init_2D_hist.GetDirectory())
    for i, hist in enumerate(hists_half_rolled):
        for j in range(1, hist.GetNbinsX()+1):
            entry = i*(hist.GetNbinsX()) + j
            content = hist.GetBinContent(j)
            error = hist.GetBinError(j)
            if j == hist.GetNbinsX():
                content += hist.GetBinContent(j+1)
                error += hist.GetBinError(j+1)
            unrolled_hist.SetBinContent(entry, content)
            unrolled_hist.SetBinError(entry, error)

    return unrolled_hist

def make1DaQGCHists(orig_file, input2D_hists, plot_info):
    output_folders = []
    for name, data in plot_info.iteritems():
        entry = data["lheWeightEntry"]
        file_name = str(data["Members"][0])

        output_list = ROOT.TList()
        output_list.SetName(name)

        for init_2D_hist_name in input2D_hists:
            init_2D_hist = orig_file.Get("/".join([file_name, init_2D_hist_name]))
            # If a histogram with the same name exisits, ROOT will return
            # that instead of creating a new one. See:
            # https://root.cern.ch/root/html532/src/TH2.cxx.html#2253
            temp = init_2D_hist.ProjectionX("temphist", entry, entry, "e")
            hist1D = temp.Clone(init_2D_hist_name.replace("lheWeights_", ""))
            temp.Delete()
            ROOT.SetOwnership(hist1D, False)
            output_list.Add(hist1D)
        output_folders.append(output_list)
    return output_folders

def removeZeros(hist):
    for i in range(hist.GetNbinsX()+2):
        if hist.GetBinContent(i) <= 0:
            if "Up" in hist.GetName():
                hist.SetBinContent(i, 0.0001)
            elif "Down" in hist.GetName():
                hist.SetBinContent(i, 0.00001)
            else: 
                hist.SetBinContent(i, 0.00005)

def getStatHists(hist, name, chan, signal):
    stat_hists = []
    variation_names = []
    for i in range(1, hist.GetNbinsX()+1):
        if "data" in name:
            continue
        if not (signal  == "wzjj_ewk" and "wzjj-vbfnlo" in name) \
                and not (signal  == "wzjj_vbfnlo" and "wzjj-ewk" in name):
            variation_names.append("%s_%s_statBin%i" % (name, chan, i))
        statUp_hist = hist.Clone(hist.GetName().replace(
            chan, "%s_%s_statBin%sUp_%s" % (name, chan, i, chan)))
        statDown_hist = hist.Clone(hist.GetName().replace(
            chan, "%s_%s_statBin%iDown_%s" % (name, chan, i, chan)))
        up = hist.GetBinContent(i)+hist.GetBinErrorUp(i) if \
                hist.GetBinContent(i) > 0 else hist.GetBinErrorUp(i)
        down = hist.GetBinContent(i)-hist.GetBinErrorLow(i)
        statUp_hist.SetBinContent(i, up) 
        statDown_hist.SetBinContent(i, down if down > 0 else 0.0001) 
        stat_hists.extend([statUp_hist, statDown_hist][:])
    for hist in stat_hists:
        removeZeros(hist)
    return (stat_hists, variation_names)

def getWeightHistProjection(init2D_hist, name, entry, rebin): 
    hist_name = init2D_hist.GetName().replace("lheWeights", name+"_weight%i" % entry)
    tmphist = init2D_hist.ProjectionX("temp", entry, entry, "e")
    hist = tmphist.Clone(hist_name) if not rebin else tmphist.Rebin(len(rebin)-1, hist_name, rebin)
    return hist

def getLHEWeightHists(init2D_hist, entries, name, variation_name, rebin=None):
    hists = []
    for i in entries:
        hist = getWeightHistProjection(init2D_hist, name, i, rebin)
        hists.append(hist)
    hist_name = init2D_hist.GetName().replace("lheWeights", "%s_%sUp" % (variation_name, name))
    return hists, hist_name

def getPDFHists(init2D_hist, entries, name, rebin=None):
    hists, hist_name = getLHEWeightHists(init2D_hist, entries, name, "pdf", rebin)
    #return hists
    return getVariationHists(hists, name, hist_name, 
            lambda x: (x[15]-x[83])/2, lambda x: (x[15]-x[83])/2)

def getScaleHists(scale_hist2D, name, rebin=None):
    entries = [i for i in range(1,10) if i not in [7, 9]]
    hists, hist_name = getLHEWeightHists(scale_hist2D, entries, name, "QCDscale", rebin)
    return getVariationHists(hists, name, hist_name, lambda x: x[-1], lambda x: x[0])

def getVariationHists(hists, process_name, histUp_name, up_action, down_action):
    histUp = hists[0].Clone(histUp_name)
    histDown = histUp.Clone(histUp_name.replace("Up", "Down"))
    
    histCentral = hists[0]
    # Include overflow
    for i in range(0, hists[0].GetNbinsX()+2):
        vals = []
        for hist in hists:
            vals.append(hist.GetBinContent(i))
        vals.sort()
        histUp.SetBinContent(i, vals[-1])
        histDown.SetBinContent(i, vals[0])
        # For now, skip this check on aQGC for now, since they're screwed up
        if "aqgc" in process_name: continue
    isValidVariation(process_name, histCentral, histUp, histDown)
    return [histUp, histDown]

def isValidVariation(process_name, histCentral, histUp, histDown):
    for i in range(0, histCentral.GetNbinsX()+2):
        if histDown.GetBinContent(i) >= histCentral.GetBinContent(i) and histCentral.GetBinContent(i) != 0:
            raise RuntimeError("Down scale variation >= central value for %s."
                " This shouldn't be possible.\n"
                "scaleDown_hist: %0.4f\n" 
                "central_hist: %0.4f\n" 
                "bin: %i\n" 
                % (process_name, histUp.GetBinContent(i), histCentral.GetBinContent(i), i)
            )
        if histUp.GetBinContent(i) <= histCentral.GetBinContent(i) and histCentral.GetBinContent(i) != 0:
            raise RuntimeError("Up scale variation <= central value for %s."
                " This shouldn't be possible.\n"
                "scaleUp_hist: %0.2f\n" 
                "central_hist: %0.2f\n" 
                "bin: %i\n" 
                % (process_name, histUp.GetBinContent(i), histCentral.GetBinContent(i), i)
            )

def getTransformed3DScaleHists(scale_hist3D, transformation, transform_args, name):
    scale_hists = []
    for i in range(1,10):
        if i == 7 or i == 9: 
            continue
        scale_hist3D.GetZaxis().SetRange(i,i)
        # Order yx matters to have consistent axes!
        scale_hist2D = scale_hist3D.Project3D("yxe")
        scale_hist_name = scale_hist3D.GetName().replace("lheWeights", name+"_weight%i" % i)
        scale_hist2D.SetName(scale_hist_name)
        scale_hist1D = transformation(scale_hist2D, *transform_args)
        scale_hists.append(scale_hist1D)
    hist_name = scale_hist3D.GetName().replace("2D_lheWeights", "_".join(["unrolled", "QCDscale", name+"Up"]))
    return getVariationHists(scale_hists, name, hist_name, lambda x: x[-1], lambda x: x[0])

def getTransformed3DPDFHists(hist3D, transformation, transform_args, entries, name):
    hists = []
    for i in entries:
        hist3D.GetZaxis().SetRange(i,i)
        # Order yx matters to have consistent axes!
        hist2D = hist3D.Project3D("yxe")
        hist_name = hist3D.GetName().replace("lheWeights", name+"_weight%i" % i)
        hist2D.SetName(hist_name)
        hist1D = transformation(hist2D, *transform_args)
        hists.append(hist1D)
    #return hists
    hist_name = hist3D.GetName().replace("2D_lheWeights", "_".join(["unrolled", "pdf", name+"Up"]))
    return getVariationHists(hists, name, hist_name, 
            lambda x: (x[15]-x[83])/2, lambda x: (x[15]-x[83])/2)

def addControlRegionToFitHist(control_hist, unrolled_hist):
    hist = ROOT.TH1D("tmp", unrolled_hist.GetTitle(), 
            unrolled_hist.GetNbinsX()+1, 0, unrolled_hist.GetNbinsX()+1)
    hist.SetName(unrolled_hist.GetName().replace("unrolled", "unrolled_wCR"))
    control_err = array.array('d', [0])
    control_yield = control_hist.IntegralAndError(0, control_hist.GetNbinsX()+1, control_err)
    hist.SetBinContent(1, control_yield) 
    hist.SetBinError(1, control_err[0])
    for i in range(1, unrolled_hist.GetNbinsX()+1):
        hist.SetBinContent(i+1, unrolled_hist.GetBinContent(i))
        hist.SetBinError(i+1, unrolled_hist.GetBinError(i))
    ROOT.SetOwnership(hist, False)
    return hist

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

def makeCompositeHists(hist_file, name, members, lumi, hists=[], underflow=True, overflow=True, rebin=None):
    composite = ROOT.TList()
    composite.SetName(name)
    for directory in [str(i) for i in members.keys()]:
        # For aQGC, the different plot groups should already be in their own files
        if "aqgc" in directory:
            directory = name
        if not hist_file.Get(directory):
            print "Skipping invalid filename %s" % directory
            continue
        if hists == []:
            hists = [i.GetName() for i in hist_file.Get(directory).GetListOfKeys()]
        sumweights = 0
        if "data" not in directory.lower():
            sumweights_hist = hist_file.Get("/".join([directory.split("__")[0], "sumweights"]))
            if not sumweights_hist:
                raise RuntimeError("Failed to find sumWeights for dataset %s" % directory)
            sumweights = sumweights_hist.Integral()
            sumweights_hist.Delete()
        for histname in hists:
            if histname == "sumweights": continue
            tmphist = hist_file.Get("/".join([directory, histname]))
            if not tmphist: 
                raise RuntimeError("Failed to produce histogram %s" % "/".join([directory, histname]))
            toRebin = rebin and not "TH2" in tmphist.ClassName()
            hist = tmphist.Clone() if not toRebin else tmphist.Rebin(len(rebin)-1, histname, rebin)
            tmphist.Delete()
            if hist:
                sumhist = composite.FindObject(hist.GetName())
                if sumweights:
                    hist.Scale(members[directory.split("__")[0]]*1000*lumi/sumweights)
                addOverflowAndUnderflow(hist, underflow, overflow)
            else:
                raise RuntimeError("hist %s was not produced for "
                    "dataset %s!" % (histname, directory))
            if not sumhist:
                sumhist = hist.Clone()
                composite.Add(sumhist)
            else:
                sumhist.Add(hist)
            hist.Delete()
    return composite

def getTransformedHists(orig_file, folders, input_hists, transformation, transform_inputs):
    output_folders = []
    for folder in folders:
        output_list = ROOT.TList()
        output_list.SetName(folder)
        ROOT.SetOwnership(output_list, False)
        for input_hist_name in input_hists:
            orig_hist = orig_file.Get("/".join([folder, input_hist_name]))
            if not orig_hist:
                if "Fakes" not in input_hist_name and \
                    "Up" not in input_hist_name and "Down" not in input_hist_name:
                    print "WARNING: Histogram %s not found for dataset %s. Skipping." % (input_hist_name, folder)
                continue
            new_hist = transformation(orig_hist, *transform_inputs)
            ROOT.SetOwnership(new_hist, False)
            output_list.Add(new_hist)
        output_folders.append(output_list)
    return output_folders 
