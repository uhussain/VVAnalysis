#!/usr/bin/env python
import ROOT
from python import SelectorTools, HistTools
from python import UserInput,OutputTools,ConfigureJobs
from python.prettytable import PrettyTable
import datetime
import os
import sys
import math

stat_variations = { "eee" : [],
    "eem" : [],
    "emm" : [],
    "mmm" : [],
}

def getComLineArgs():
    parser = UserInput.getDefaultParser()
    parser.add_argument("--vbfnlo",
        action='store_true', help="Use VBFNLO for signal")
    parser.add_argument("--lumi", "-l", type=float,
        default=35.87, help="luminosity value (in fb-1)")
    parser.add_argument("--output_file", "-o", type=str,
        default="test.root", help="Output file name")
    parser.add_argument("--input_file", "-i", type=str,
        default="test.root", help="Output file name")
    parser.add_argument("--folder_name", type=str,
        default="", help="Name for combine folder (date by default)")
    parser.add_argument("-b", "--hist_names", 
                        type=lambda x : [i.strip() for i in x.split(',')],
                        default=["all"], help="List of histograms, "
                        "as defined in AnalysisDatasetManager, separated "
                        "by commas")
    return vars(parser.parse_args())

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
        "wzjj-vbfnlo" : 0,
        "wzjj-ewk" : 0,
        "wz-mgmlm" : 0,
        "wz-powheg" : 0,
        "vv-powheg" : 0,
        "top-ewk" : 0,
        "zg" : 0,
        "vv" : 0,
        "AllData" : 0,
    },
    "eem" : {
        "wzjj-vbfnlo" : 0,
        "wzjj-ewk" : 0,
        "wz-powheg" : 0,
        "vv-powheg" : 0,
        "top-ewk" : 0,
        "zg" : 0,
        "vv" : 0,
        "AllData" : 0,
        "AllData" : 0,
    },
    "emm" : {
        "wzjj-vbfnlo" : 0,
        "wzjj-ewk" : 0,
        "wz-powheg" : 0,
        "vv-powheg" : 0,
        "top-ewk" : 0,
        "zg" : 0,
        "vv" : 0,
        "AllData" : 0,
    },
    "mmm" : {
        "wzjj-vbfnlo" : 0,
        "wzjj-ewk" : 0,
        "wz-powheg" : 0,
        "vv-powheg" : 0,
        "top-ewk" : 0,
        "zg" : 0,
        "vv" : 0,
        "AllData" : 0,
    },
}

signal = "wzjj_vbfnlo" if args['vbfnlo'] else "wzjj_ewk"
numvars = 18 if "VBS" in args['selection'] else 13
isVBS = "VBS" in args['selection'] 
variable = "mjj" if isVBS else "yield"
alldata = HistTools.makeCompositeHists(fIn, "AllData", 
    ConfigureJobs.getListOfFilesWithXSec(["WZxsec2016data"], manager_path), args['lumi'],
    [variable +"_"+ c for c in chans])
OutputTools.writeOutputListItem(alldata, fOut)
nonprompt = HistTools.makeCompositeHists(fIn, "DataEWKCorrected", {"DataEWKCorrected" : 1}, args['lumi'],
    [variable+"_Fakes_" + c for c in chans])
for chan in chans:
    hist = nonprompt.FindObject(variable+"_Fakes_"+chan)
    HistTools.removeZeros(hist)
    card_info[chan]["nonprompt"] = round(hist.Integral(), 4) if hist.Integral() > 0 else 0.001
    stat_hists,variation_names = HistTools.getStatHists(hist, "nonprompt", chan, signal)
    stat_variations[chan].extend(variation_names)
    nonprompt.extend(stat_hists[:])
OutputTools.writeOutputListItem(nonprompt, fOut)
output_info = PrettyTable(["Filename", "eee", "eem", "emm", "mmm", "All states"])
significance_info = PrettyTable(["Filename", "eee", "eem", "emm", "mmm", "All states"])

plot_groups = ["wz-powheg", "wzjj-vbfnlo", "wzjj-ewk", "top-ewk", "zg", "vv-powheg", "data_2016"]
if isVBS:
    plot_groups = ["wz-mgmlm", "wzjj-vbfnlo", "wzjj-ewk", "top-ewk", "zg", "vv"]

for plot_group in plot_groups:
    plots = [variable+"_" + c for c in chans]
    variations = ["lheWeights"] if "data" not in plot_group else []
    if variable == "mjj":
        variations += ["jerUp", "jerDown", "jesUp", "jesDown"] 
    plots += ["_".join([variable, var, c]) for var in variations for c in chans]

    group = HistTools.makeCompositeHists(fIn, plot_group, ConfigureJobs.getListOfFilesWithXSec(
        config_factory.getPlotGroupMembers(plot_group), manager_path), args['lumi'], plots)
    name = plot_group.replace("-", "_")
    for chan in chans:
        hist = group.FindObject(variable+"_"+chan)
        card_info[chan]["output_file"] = args['output_file']
        stat_hists,variation_names = HistTools.getStatHists(hist, plot_group, chan, signal)
        stat_variations[chan].extend(variation_names)
        group.extend(stat_hists)
        if "data" not in plot_group:
            scale_hists = HistTools.getScaleHists(group.FindObject(variable+"_lheWeights_"+chan), plot_group, chan)
            group.extend(scale_hists)
    for hist in group:
        HistTools.removeZeros(hist)
    for chan in chans:
        hist = group.FindObject(variable+"_"+chan)
        card_info[chan][name] = round(hist.Integral(), 4) if hist.Integral() > 0 else 0.001
    OutputTools.writeOutputListItem(group, fOut)
    output_info.add_row([plot_group, card_info["eee"][name], 
        card_info["eem"][name], 
        card_info["emm"][name], 
        card_info["mmm"][name], 
        sum([card_info[c][name] for c in chans])]
    )
output_info.add_row(["nonprompt", card_info["eee"]["nonprompt"], 
    card_info["eem"]["nonprompt"], 
    card_info["emm"]["nonprompt"], 
    card_info["mmm"]["nonprompt"], 
    sum([card_info[c]["nonprompt"] for c in chans])]
)
background = {c : 0 for c in chans}
for chan,yields in card_info.iteritems():
    for name,value in yields.iteritems():
        if name not in ["wzjj_ewk", "wzjj_vbfnlo", "output_file"]:
            background[chan] += float(value)
output_info.add_row(["Total background", 
    round(background["eee"], 4), 
    round(background["eem"], 4), 
    round(background["emm"], 4), 
    round(background["mmm"], 4),
    round(sum([background[c] for c in chans]), 4), 
])
for name in ["wzjj_ewk", "wzjj_vbfnlo"]:
    significance_info.add_row([name, 
        round(card_info["eee"][name]/math.sqrt(background["eee"]), 4), 
        round(card_info["eem"][name]/math.sqrt(background["eem"]), 4), 
        round(card_info["emm"][name]/math.sqrt(background["emm"]), 4), 
        round(card_info["mmm"][name]/math.sqrt(background["mmm"]), 4), 
        round(sum([card_info[c][name] for c in chans])
            /math.sqrt(sum([background[c] for c in chans])), 4), 
    ])

combine_dir = "/afs/cern.ch/user/k/kelong/work/HiggsCombine/CMSSW_7_4_7/src/HiggsAnalysis/CombinedLimit"
folder_name = args['folder_name'] if args['folder_name'] != "" else \
                datetime.date.today().strftime("%d%b%Y")
output_dir = '/'.join([combine_dir,args['selection'], folder_name])
try:
    os.makedirs(output_dir)
except OSError as e:
    print e
    pass

signal_abv = "_vbfnlo" if args['vbfnlo'] else ""
with open("/".join([output_dir, "Yields%s.out" % signal_abv]), "w") as yields:
    yields.write("\n" + " "*30 + "Event Yields")
    yields.write("\n" + str(output_info))
    yields.write("\n" + " "*30 + "S/sqrt(B)")
    yields.write("\n" + str(significance_info))

for chan, chan_dict in card_info.iteritems():
    chan_dict["signal_name"] = signal.replace("_", "-")
    chan_dict["signal_yield"] = chan_dict[signal]
    chan_dict["nuisances"] = numvars+len(stat_variations[chan]) -1*("Wselection" in args['selection'])
    file_name = '%s/WZjj%s_%s.txt' % (output_dir, signal_abv, chan) if isVBS \
            else '%s/WZ_%s.txt' % (output_dir, chan)
    template_name = 'Templates/CombineCards/%s/%s_template_%s.txt' % \
        (args['selection'].split("/")[-1], ("WZjj_EWK" if isVBS else "WZ"), chan)
    ConfigureJobs.fillTemplatedFile(template_name,
        file_name,
        chan_dict
    )
    with open(file_name, "a") as chan_file:
        for hist_name in stat_variations[chan]:
            if 'Wselection' in args['selection'] and "wzjj-ewk" in hist_name:
                continue
            if "VBS" in args['selection']:
                chan_file.write(
                    "%s     shape   %i               %i               %i           %i               %i           %i\n" \
                        % (hist_name, 
                            chan_dict["signal_name"] in hist_name,
                            "wz-mgmlm" in hist_name,
                            "vv" in hist_name,
                            "top-ewk" in hist_name,
                            "zg" in hist_name,
                            "nonprompt" in hist_name,
                        )
            )
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
        chan_file.write("nonprompt_all group = nonprompt_norm %s\n" % " ".join([h for h in stat_variations[chan] if "nonprompt" in h]))
        if "VBS" in args['selection']:
            chan_file.write("nonprompt_stat group = %s\n" % " ".join([h for h in stat_variations[chan] if "nonprompt" in h]))
            chan_file.write("wz_qcd_all group = wz-mgmlm_scale WZjj_qcd_modeling %s\n" % " ".join([h for h in stat_variations[chan] if "wz-mgmlm" in h]))
ConfigureJobs.fillTemplatedFile(
    'Templates/CombineCards/%s/runCombine_Template.sh' % args['selection'].split("/")[-1],
    '%s/runCombine%s.sh' % (output_dir, signal_abv), 
    {"sample" : signal_abv}
)
