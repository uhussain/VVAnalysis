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
        print "Name is", name
        print "Entry is", entry

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
            variation_names.append("%s_statBin%i" % (name, i))
        statUp_hist = hist.Clone(hist.GetName().replace(
            chan, "%s_statBin%sUp_%s" % (name, i, chan)))
        statDown_hist = hist.Clone(hist.GetName().replace(
            chan, "%s_statBin%iDown_%s" % (name, i, chan)))
        up = hist.GetBinContent(i)+hist.GetBinErrorUp(i) if \
                hist.GetBinContent(i) > 0 else hist.GetBinErrorUp(i)
        down = hist.GetBinContent(i)-hist.GetBinErrorLow(i)
        statUp_hist.SetBinContent(i, up) 
        statDown_hist.SetBinContent(i, down if down > 0 else 0.0001) 
        stat_hists.extend([statUp_hist, statDown_hist][:])
    for hist in stat_hists:
        removeZeros(hist)
    return (stat_hists, variation_names)
def getScaleHists(scale_hist2D, name):
    scale_hists = []
    for i in range(1,10):
        if i == 7 or i == 9: continue
        scale_hist_name = scale_hist2D.GetName().replace("lheWeights", name+"_weight%i" % i)
        scale_hist = scale_hist2D.ProjectionX(scale_hist_name, i, i, "e")
        scale_hists.append(scale_hist)
    hist_name = scale_hist2D.GetName().replace("lheWeights", name+"_scaleUp")
    return getScaleVariationHists(scale_hists, hist_name, name)

def getScaleVariationHists(scale_hists, scaleUp_name, process_name):
    scale_histUp = scale_hists[0].Clone(scaleUp_name)
    scale_histDown = scale_histUp.Clone(scaleUp_name.replace("Up", "Down"))
    
    scale_histCentral = scale_hists[0]
    # Include overflow
    for i in range(0, scale_hists[0].GetNbinsX()+2):
        for hist in scale_hists:
            if hist.GetBinContent(i) > scale_histUp.GetBinContent(i):
                scale_histUp.SetBinContent(i, hist.GetBinContent(i))
            if hist.GetBinContent(i) < scale_histDown.GetBinContent(i):
                scale_histDown.SetBinContent(i, hist.GetBinContent(i))
        # For now, skip this check on aQGC for now, since they're screwed up
        if "aqgc" in process_name: continue
        if scale_histDown.GetBinContent(i) >= scale_histCentral.GetBinContent(i) and hist.GetBinContent(i) != 0:
            raise RuntimeError("Down scale variation >= central value for %s."
                " This shouldn't be possible.\n"
                "scaleDown_hist: %0.4f\n" 
                "central_hist: %0.4f\n" 
                % (process_name, scale_histUp.GetBinContent(i), scale_histCentral.GetBinContent(i))
            )
        if scale_histUp.GetBinContent(i) <= scale_histCentral.GetBinContent(i) and hist.GetBinContent(i) != 0:
            raise RuntimeError("Up scale variation <= central value for %s."
                " This shouldn't be possible.\n"
                "scaleUp_hist: %0.2f\n" 
                "central_hist: %0.2f\n" 
                % (process_name, scale_histUp.GetBinContent(i), scale_histCentral.GetBinContent(i))
            )
    return [scale_histUp, scale_histDown]

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
    hist_name = scale_hist3D.GetName().replace("2D_lheWeights", "_".join(["unrolled",name,"scaleUp"]))
    return getScaleVariationHists(scale_hists, hist_name, name)

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

def makeCompositeHists(hist_file, name, members, lumi, hists=[], underflow=True, overflow=True):
    composite = ROOT.TList()
    composite.SetName(name)
    for directory in [str(i) for i in members.keys()]:
        # For aQGC, the different plot groups should already be in their own files
        if "aqgc" in directory:
            directory = name
            print "NAME IS", name
        print "Members are", members
        if not hist_file.Get(directory):
            print "Skipping invalid filename %s" % directory
            print hist_file.Get(directory)
            print "File is", hist_file
            continue
        if hists == []:
            hists = [i.GetName() for i in hist_file.Get(directory).GetListOfKeys()]
            print hists
        for histname in hists:
            if histname == "sumweights": continue
            print histname
            tmphist = hist_file.Get("/".join([directory, histname]))
            hist = tmphist.Clone()
            if hist:
                sumhist = composite.FindObject(hist.GetName())
                if "data" not in directory.lower() and hist.GetEntries() > 0:
                    sumweights_hist = hist_file.Get("/".join([directory.split("__")[0], "sumweights"]))
                    sumweights = sumweights_hist.Integral()
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
                print "WARNING: Histogram %s not found for dataset %s. Skipping." % (input_hist_name, folder)
                continue
            new_hist = transformation(orig_hist, *transform_inputs)
            ROOT.SetOwnership(new_hist, False)
            output_list.Add(new_hist)
        output_folders.append(output_list)
    return output_folders 
