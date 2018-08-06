#!/usr/bin/env python
import ROOT
from python import SelectorTools, HistTools
from python import UserInput,OutputTools,ConfigureJobs
from python.prettytable import PrettyTable
import datetime
import os
import sys
import math
import array
import logging

stat_variations = { "eee" : [],
    "eem" : [],
    "emm" : [],
    "mmm" : [],
    "all" : [],
}

def getComLineArgs():
    parser = UserInput.getDefaultParser()
    parser.add_argument("--vbfnlo",
        action='store_true', help="Use VBFNLO for signal")
    parser.add_argument("--noTheory",
        action='store_true', help="Don't add theory hists")
    parser.add_argument("--noCards",
        action='store_true', help="Don't create cards for combine")
    parser.add_argument("--aqgc",
        action='store_true', help="Add aqgc files")
    parser.add_argument("--manualStats",
        action='store_true', help="Stat uncertainties inserted by bin")
    parser.add_argument("--lumi", "-l", type=float,
        default=35.87, help="luminosity value (in fb-1)")
    parser.add_argument("--output_file", "-o", type=str,
        default="test.root", help="Output file name")
    parser.add_argument("--input_file", "-i", type=str,
        default="test.root", help="Output file name")
    parser.add_argument("--folder_name", type=str,
        default="", help="Name for combine folder (date by default)")
    parser.add_argument("--fit_variable", type=str,
        default="", help="Variable to use in the shape based fit")
    parser.add_argument("--combineChannels", action='store_true',
        help="Don't fit channels independently")
    parser.add_argument("--addControlRegion", action='store_true',
        help="Add control region to fit distribution")
    parser.add_argument("-b", "--hist_names", 
                        type=lambda x : [i.strip() for i in x.split(',')],
                        default=["all"], help="List of histograms, "
                        "as defined in AnalysisDatasetManager, separated "
                        "by commas")
    return vars(parser.parse_args())

def combineChannels(group, chans, variations=[], central=True):
    if central:
        variations.append("")
    for var in variations:
        name = variable if var is "" else "_".join([variable, var])
        hist_name = name + "_" + chans[0]
        hist = group.FindObject(hist_name)
        if not hist:
            logging.warning("Failed to find hist %s in group %s. Skipping" % (hist_name, group.GetName()))
            continue
        hist = hist.Clone(name)
        ROOT.SetOwnership(hist, False)
        group.Add(hist) 
        for chan in chans[1:]:
            chan_hist = group.FindObject(name + "_" + chan)
            hist.Add(chan_hist)

ROOT.gROOT.SetBatch(True)
chans = ConfigureJobs.getChannels()
args = getComLineArgs()

manager_path = ConfigureJobs.getManagerPath() 
sys.path.append("/".join([manager_path, "AnalysisDatasetManager",
    "Utilities/python"]))

from ConfigHistFactory import ConfigHistFactory
config_factory = ConfigHistFactory(
    "%s/AnalysisDatasetManager" % manager_path,
    args['selection'],
)

fOut = ROOT.TFile(args['output_file'], "recreate")
fIn = ROOT.TFile(args['input_file'])

card_info = {
    "eee" : { 
        "wzjj_vbfnlo" : 0,
        "EW_WZjj" : 0,
        "QCD_WZjj" : 0,
        "wz_powheg" : 0,
        "vv_powheg" : 0,
        "top_ewk" : 0,
        "zg" : 0,
        "vv" : 0,
        "AllData" : 0,
    },
    "eem" : {
        "wzjj_vbfnlo" : 0,
        "EW_WZjj" : 0,
        "wz_powheg" : 0,
        "vv_powheg" : 0,
        "top_ewk" : 0,
        "zg" : 0,
        "vv" : 0,
        "AllData" : 0,
    },
    "emm" : {
        "wzjj_vbfnlo" : 0,
        "EW_WZjj" : 0,
        "QCD_WZjj" : 0,
        "wz_powheg" : 0,
        "vv_powheg" : 0,
        "top_ewk" : 0,
        "zg" : 0,
        "vv" : 0,
        "AllData" : 0,
    },
    "mmm" : {
        "wzjj_vbfnlo" : 0,
        "EW_WZjj" : 0,
        "QCD_WZjj" : 0,
        "wz_powheg" : 0,
        "vv_powheg" : 0,
        "top_ewk" : 0,
        "zg" : 0,
        "vv" : 0,
        "AllData" : 0,
    },
    "all" : {
        "wzjj_vbfnlo" : 0,
        "EW_WZjj" : 0,
        "wz" : 0,
        "QCD_WZjj" : 0,
        "wz_powheg" : 0,
        "vv_powheg" : 0,
        "top_ewk" : 0,
        "zg" : 0,
        "vv" : 0,
        "nonprompt" : 0,
        "AllData" : 0,
    },
}

pdf_entries = {
    "wzjj-vbfnlo" : 0,
    "EW-WZjj" : [1]+range(11,112),
    "wzjj-aqgcfm" : [1]+range(11,112),
    "wzjj-aqgcfs" : [1]+range(11,112),
    "wzjj-aqgcft" : [1]+range(11,112),
    "QCD-WZjj" : [1]+range(11,112),
    "wz-powheg" : [1]+range(11,112),
    "wz" : [1]+range(11,112),
    "vv-powheg" : [1]+range(11,112),
    "top-ewk" : [1]+range(11,112),
    "zg" : 0,
    "vv" : [1]+range(11,112),
    "AllData" : 0,
}

wz_scalefacs = {
    "QCD-WZjj" : 0.813,
    "wz" : 0.643,
    "wz-powheg" : 0.845,
}
# All to the same integral in the signal region
#wz_scalefacs = {
#    "QCD-WZjj" : 0.813,
#    "wz" : 0.58342,
#    "wz-powheg" : 0.77856,
#}


scaleWZ = False
manualStatUnc = args['manualStats']
variations = [i for x in ["CMS_scale_j", "CMS_res_j", \
    "CMS_eff_m", "CMS_scale_m", "CMS_eff_e", "CMS_scale_e", "CMS_pileup", "CMS_scale_unclEnergy"] for i in [x+"Up", x+"Down"]]
jeVariations = [i for x in ["CMS_scale_j", "CMS_res_j"] for i in [x+"Up", x+"Down"]]
#variations = jeVariations 

output_info = PrettyTable(["Filename", "eee", "eem", "emm", "mmm", "All states"])

signal = "wzjj_vbfnlo" if args['vbfnlo'] else "EW_WZjj"
initNumvars = 22 if "VBS" in args['selection'] else 17
isVBS = "VBS" in args['selection'] 
#variable = "mjj" if isVBS else "yield"
#variable = "yield"
#variable = "mjj_etajj_unrolled" if isVBS else "yield"
if args['fit_variable'] is "":
    variable = "mjj_etajj_unrolled" if isVBS else "yield"
    if isVBS and args['aqgc']:
        variable = "MTWZ"
else:
    variable = args['fit_variable']

base_variable = variable
if args['addControlRegion']:
    base_variable = variable
    variable = base_variable + "_wCR"

#mjj_binning = ConfigureJobs.get2DBinning()[0]
mjj_binning = array.array('d', [i*100 for i in range(0,25)]) 
rebin = mjj_binning if base_variable == "mjj" else None
if variable == "MTWZ":
    rebin = array.array('d', [0,50,100,200,300,400,500,700,1000,1500,2000]) 
alldata = HistTools.makeCompositeHists(fIn, "AllData", 
    ConfigureJobs.getListOfFilesWithXSec(["WZxsec2016data"], manager_path), args['lumi'],
    [variable +"_"+ c for c in chans], rebin=rebin)
for chan in chans:
    hist = alldata.FindObject(variable+"_"+chan)
    card_info[chan]["AllData"] = round(hist.Integral(), 4) if hist.Integral() > 0 else 0.001
combineChannels(alldata, chans)

OutputTools.writeOutputListItem(alldata, fOut)
nonprompt = HistTools.makeCompositeHists(fIn, "DataEWKCorrected", {"DataEWKCorrected" : 1}, args['lumi'],
    [variable+"_Fakes_" + c for c in chans], rebin=rebin)
for var in jeVariations:
    hists = HistTools.makeCompositeHists(fIn, "DataEWKCorrected", {"DataEWKCorrected" : 1}, args['lumi'],
            ["_".join([variable, var, "Fakes", c]) for c in chans], rebin=rebin)
    for h in hists:
        h.SetName(h.GetName().replace(var+"_Fakes", "Fakes_"+var))
    nonprompt.extend(hists[:])

combineChannels(nonprompt, chans, ["Fakes"]+["Fakes_"+i for i in jeVariations], False)
for h in nonprompt:
    HistTools.removeZeros(h)
for chan in chans + ["all"]:
    hist = nonprompt.FindObject(variable+"_Fakes_"+chan)
    if chan == "all":
        hist = nonprompt.FindObject(variable+"_Fakes")
    card_info[chan]["nonprompt"] = round(hist.Integral(), 4) if hist.Integral() > 0 else 0.001
    if manualStatUnc:
        stat_hists,variation_names = HistTools.getStatHists(hist, "nonprompt", chan, signal)
        stat_variations[chan].extend(variation_names)
        nonprompt.extend(stat_hists[:])

OutputTools.writeOutputListItem(nonprompt, fOut)
significance_info = PrettyTable(["Filename", "eee", "eem", "emm", "mmm", "All states"])

plot_groups = ["QCD-WZjj", "wz", "wz-powheg", "EW-WZjj", "top-ewk", "zg", "vv-powheg", "vv"]

aqgc_groups =  []
if args['aqgc']:
    import json
    base_name = manager_path +"/PlotGroups/"
    for filename in ["WZxsec2016_aQGC-FM.json", "WZxsec2016_aQGC-FS.json", "WZxsec2016_aQGC-FT.json",]:
        aqgc_names = json.load(open(base_name+filename))
        aqgc_groups.extend([str(n) for n in aqgc_names.keys()])
    plot_groups.extend(aqgc_groups)

wz_qcd_theory_hists = ROOT.TList()
for plot_group in plot_groups:
    # Theory hists won't work right for non-central aQGC points
    isaQGCpoint = "aqgc" in plot_group and "__" in plot_group
    plots = [variable+"_" + c for c in chans]
    if "data" not in plot_group and not isaQGCpoint and not args['noTheory']:
        plots += ["_".join([base_variable.replace("unrolled", "2D"), "lheWeights", c]) for c in chans]
    plots += ["_".join([variable, var, c]) for var in variations for c in chans]
    if args['addControlRegion'] and not isaQGCpoint:
        plots += ["backgroundControlYield_lheWeights_" + c for c in chans]

    group = HistTools.makeCompositeHists(fIn, plot_group, ConfigureJobs.getListOfFilesWithXSec(
        config_factory.getPlotGroupMembers(plot_group), manager_path), args['lumi'], plots, rebin=rebin)
    if scaleWZ and plot_group in wz_scalefacs.keys():
        for h in group:
            if h.InheritsFrom("TH1"):
                h.Scale(wz_scalefacs[plot_group])
    name = plot_group.replace("-", "_")
    for chan in chans:
        hist = group.FindObject(variable+"_"+chan)
        card_info[chan]["output_file"] = args['output_file']
        if plot_group not in ["wz", "wz-powheg"]:
            if manualStatUnc:
                stat_hists,variation_names = HistTools.getStatHists(hist, plot_group, chan, signal)
                stat_variations[chan].extend(variation_names)
                group.extend(stat_hists)
        if "data" not in plot_group and not isaQGCpoint and not args['noTheory']:
            weight_hist_name = base_variable.replace("unrolled", "2D")+"_lheWeights_"+chan
            weight_hist = group.FindObject(weight_hist_name)
            if not weight_hist:
                logging.warning("Failed to find %s. Skipping" % weight_hist_name)
                continue
            pdf_hists = []
            if "TH2" in weight_hist.ClassName():
                if "MTWZ" in variable and isVBS:
                    rebin = array.array('d', [0,50,100,200,300,400,500,700,1000,1500,2000]) 
                elif "MTWZ" in variable:
                    rebin = array.array('d', [0,50,100,200,300,400,500,700,1000,1200]) 
                scale_hists = HistTools.getScaleHists(weight_hist, plot_group, rebin)
                if pdf_entries[plot_group]:
                    pdf_hists = HistTools.getPDFHists(weight_hist, pdf_entries[plot_group], plot_group, rebin)
            elif "TH3" in weight_hist.ClassName(): 
                scale_hists = HistTools.getTransformed3DScaleHists(weight_hist, 
                    HistTools.makeUnrolledHist,
                    ConfigureJobs.get2DBinning(yvar="etajj" if "dRjj" not in variable else "dRjj"),
                    plot_group
                )
                if pdf_entries[plot_group]:
                    pdf_hists = HistTools.getTransformed3DPDFHists(weight_hist, 
                        HistTools.makeUnrolledHist, 
                        ConfigureJobs.get2DBinning(yvar="etajj" if "dRjj" not in variable else "dRjj"),
                        pdf_entries[plot_group],
                        plot_group
                    )
            else:
                raise RuntimeError("Invalid weight hist %s" % weight_hist_name +
                        " for %s. Can't make scale variations" % plot_group)
                
            # Account for gg component which doesn't have weights
            if "vv" in plot_group:
                print "INFO: Scaling VV theory hists by 1.1!"
            for hist in scale_hists+pdf_hists:
                HistTools.addOverflowAndUnderflow(hist,underflow=False)
                if "vv" in plot_group:
                    hist.Scale(1.1)

            if plot_group in ["wz", "QCD-WZjj", "wz-powheg"]:
                wz_qcd_theory_hists.append(hist.Clone(hist.GetName().replace(chan, "_".join([plot_group, chan]))))
                wz_qcd_theory_hists.extend(scale_hists+pdf_hists)
            
            theory_hists = []
            if args['addControlRegion']:
                control_hist2D = group.FindObject("backgroundControlYield_lheWeights_" + chan)
                control_hists = ROOT.TList()
                unrolled_theory = HistTools.getScaleHists(control_hist2D, plot_group) 
                if pdf_entries[plot_group]:
                    unrolled_theory += HistTools.getPDFHists(control_hist2D, pdf_entries[plot_group], plot_group)
                for h in unrolled_theory:
                    control_hists.Add(h)
                for h in scale_hists+pdf_hists:
                    control_hist_name = "_".join(["backgroundControlYield"] + h.GetName().split("_")[-3:])
                    control_hist = control_hists.FindObject(control_hist_name)
                    hist = HistTools.addControlRegionToFitHist(control_hist, h, base_variable)
                    theory_hists.append(hist)
            elif not isaQGCpoint: 
                theory_hists = scale_hists + pdf_hists
            group.extend(theory_hists)

    theory_vars = []
    if "aqgc" not in plot_group and "__" not in plot_group: 
        theory_vars = ["_".join([var, plot_group + shift]) for var in ["QCDscale", "pdf"] for shift in ["Up", "Down"]]
    combineChannels(group, chans, variations + theory_vars, True)
    for hist in group:
        HistTools.removeZeros(hist)
    for chan in chans:
        hist = group.FindObject(variable+"_"+chan)
        card_info[chan][name] = round(hist.Integral(), 4) if hist.Integral() > 0 else 0.001
    OutputTools.writeOutputListItem(group, fOut)
    yields = [card_info[c][name] for c in chans]
    yields.append(sum([card_info[c][name] for c in chans]))
    output_info.add_row([plot_group] + yields)

output_info.add_row(["nonprompt", card_info["eee"]["nonprompt"], 
    card_info["eem"]["nonprompt"], 
    card_info["emm"]["nonprompt"], 
    card_info["mmm"]["nonprompt"], 
    sum([card_info[c]["nonprompt"] for c in chans])]
)
background = {c : 0 for c in chans}
for chan,yields in card_info.iteritems():
    if chan == "all":
        continue
    for name,value in yields.iteritems():
        if "data" in name:
            continue
        if name not in ["EW_WZjj", "wz", 
                "AllData", "wz_powheg", "wzjj_vbfnlo", "output_file"]:
            background[chan] += float(value)
output_info.add_row(["Total background", 
    round(background["eee"], 4), 
    round(background["eem"], 4), 
    round(background["emm"], 4), 
    round(background["mmm"], 4),
    round(sum([background[c] for c in chans]), 4), 
])

yields = [card_info[c]["AllData"] for c in chans]
yields.append(sum([card_info[c]["AllData"] for c in chans]))
output_info.add_row(["Data"] + yields)

for name in ["EW_WZjj", "wzjj_vbfnlo"]:
    significance_info.add_row([name, 
        round(card_info["eee"][name]/math.sqrt(background["eee"]), 4), 
        round(card_info["eem"][name]/math.sqrt(background["eem"]), 4), 
        round(card_info["emm"][name]/math.sqrt(background["emm"]), 4), 
        round(card_info["mmm"][name]/math.sqrt(background["mmm"]), 4), 
        round(sum([card_info[c][name] for c in chans])
            /math.sqrt(sum([background[c] for c in chans])), 4), 
    ])

combine_dir = ConfigureJobs.getCombinePath() 
folder_name = args['folder_name'] if args['folder_name'] != "" else \
                datetime.date.today().strftime("%d%b%Y") 

output_dir = '/'.join([combine_dir,args['selection'], folder_name])
try:
    os.makedirs(output_dir)
except OSError as e:
    logging.warning(e)
    pass

signal_abv = "_vbfnlo" if args['vbfnlo'] else ""
with open("/".join([output_dir, "Yields%s.out" % signal_abv]), "w") as yields:
    yields.write("\n" + " "*30 + "Event Yields")
    yields.write("\n" + str(output_info))
    yields.write("\n" + " "*30 + "S/sqrt(B)")
    yields.write("\n" + str(significance_info))

if not args['noCards']:
    if args['combineChannels']:
        card_info["all"]["output_file"] = args['output_file']
        for chan in chans:
            for process, rate in card_info[chan].iteritems():
                if process == "nonprompt" or "aqgc" in process: continue
                if type(rate) is float:
                    card_info["all"][process] += rate
    for chan, chan_dict in card_info.iteritems():
        chan_dict["signal_name"] = signal.replace("_", "-")
        chan_dict["fit_variable"] = variable
        chan_dict["signal_yield"] = chan_dict[signal]
        numvars = initNumvars+len(chans)*(chan != "all")*len(stat_variations[chan]) 
        if chan != "all":
            numvars += 3
        chan_dict["nuisances"] = numvars
        file_name = '%s/WZjj%s_%s.txt' % (output_dir, signal_abv, chan) if isVBS \
                else '%s/WZ_%s.txt' % (output_dir, chan)
        template_name = 'Templates/CombineCards/%s/%s_template_%s.txt' % \
            (args['selection'].split("/")[-1], ("WZjj_EWK" if isVBS else "WZ"), chan)
        ConfigureJobs.fillTemplatedFile(template_name,
            file_name,
            chan_dict
        )
        with open(file_name, "a") as chan_file:
            if not manualStatUnc:
                chan_file.write("* autoMCStats 1\n")
            for c in chans:
                for hist_name in stat_variations[c]:
                    if 'Wselection' in args['selection'] and "EW-WZjj" in hist_name:
                        continue
                    if isVBS and chan == c:
                        chan_file.write(
                            "%s     shape   %i               %i               %i           %i               %i           %i\n" \
                                % (hist_name, 
                                    chan_dict["signal_name"] in hist_name,
                                    "QCD-WZjj" in hist_name,
                                    "vv" in hist_name,
                                    "top-ewk" in hist_name,
                                    "zg" in hist_name,
                                    "nonprompt" in hist_name,
                                )
                    )
                    elif isVBS:
                        chan_file.write(
                            "%s     shape   0               0               0           0               0           0\n" % hist_name)
                    else:
                        chan_file.write(
                            "%s     shape   %i               %i               %i           %i               %i\n" \
                                % (hist_name, 
                                    "wz-powheg" in hist_name,
                                    "vv" in hist_name,
                                    "top-ewk" in hist_name,
                                    "zg" in hist_name,
                                    "nonprompt" in hist_name,
                                )
                    )
            chan_file.write("mc_stat group = %s\n" % " ".join([h for h in stat_variations[chan] if "nonprompt" not in h]))
            chan_file.write("lepton_unc group = CMS_eff_e CMS_eff_m CMS_scale_e CMS_scale_m\n")
            if isVBS:
                chan_file.write("nonprompt_stat group = %s\n" % " ".join([h for h in stat_variations[chan] if "nonprompt" not in h]))
                chan_file.write("wz_qcd_all group = QCDscale_QCD-WZjj pdf_QCD-WZjj CMS_norm_QCD-WZjj %s\n" % " ".join([h for h in stat_variations[chan] if "QCD-WZjj" in h]))
    ConfigureJobs.fillTemplatedFile(
        'Templates/CombineCards/%s/runCombine_Template.sh' % args['selection'].split("/")[-1],
        '%s/runCombine%s.sh' % (output_dir, signal_abv), 
        {"sample" : signal_abv}
    )

if args['aqgc']:
    fOut.Close()
    HistTools.addaQGCTheoryHists(args['output_file'], aqgc_groups, variable)

