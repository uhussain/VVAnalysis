#!/usr/bin/env python
import ROOT
from python import SelectorTools
from python import UserInput
from python import ConfigureJobs
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
def writeOutputListItem(item, directory):
    if item.ClassName() == "TList":
        d = directory.Get(item.GetName())
        if not d:
            d = directory.mkdir(item.GetName())
            ROOT.SetOwnership(d, False)
        for subItem in item:
            writeOutputListItem(subItem, d)
    elif hasattr(item, 'Write'):
        directory.cd()
        item.Write()
    else:
        print "Couldn't write output item:"
        print repr(item)
    directory.cd()

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
        if not hist_file.Get(directory):
            print "Skipping invalid filename %s" % directory
            continue
        if hists == []:
            hists = [i.GetName() for i in hist_file.Get(directory).GetListOfKeys()]
        for histname in hists:
            if histname == "sumweights": continue
            hist = hist_file.Get("/".join([directory, histname]))
            if hist:
                sumhist = composite.FindObject(hist.GetName())
                if "data" not in directory.lower() and hist.GetEntries() > 0:
                    sumweights_hist = hist_file.Get("/".join([directory, "sumweights"]))
                    sumweights = sumweights_hist.Integral()
                    hist.Scale(members[directory]*1000*lumi/sumweights)
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

ROOT.gROOT.SetBatch(True)
chans = ["eee", "eem", "emm", "mmm"]
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
    for i in range(1, hist.GetNbinsX()+1):
        if "data" in name:
            continue
        if not (signal  == "wzjj_ewk" and "wzjj-vbfnlo" in name) \
                and not (signal  == "wzjj_vbfnlo" and "wzjj-ewk" in name):
            stat_variations[chan].append("%s_statBin%i" % (name, i))
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
    return stat_hists

def getScaleHists(scale_hist2D, name, chan, underflow=True, overflow=True):
    scale_hists = []
    for i in range(1,10):
        if i == 7 or i == 9: continue
        scale_hist = scale_hist2D.ProjectionX(name+"_weight%i"%i, i, i, "e")
        addOverflowAndUnderflow(scale_hist, underflow, overflow)
        scale_hists.append(scale_hist)
    scale_histCentral = scale_hist2D.ProjectionX(name+"_central", 1, 1, "e")
    addOverflowAndUnderflow(scale_histCentral, underflow, overflow)
    hist_name = scale_hist2D.GetName().replace("lheWeights", name+"_scaleUp")
    scale_histUp = scale_histCentral.Clone(hist_name)
    scale_histDown = scale_histCentral.Clone(hist_name.replace("Up", "Down"))
    for i in range(0, scale_hists[0].GetNbinsX()+1):
        for hist in scale_hists:
            if hist.GetBinContent(i) > scale_histUp.GetBinContent(i):
                scale_histUp.SetBinContent(i, hist.GetBinContent(i))
            if hist.GetBinContent(i) < scale_histDown.GetBinContent(i):
                scale_histDown.SetBinContent(i, hist.GetBinContent(i))
        if scale_histDown.GetBinContent(i) >= scale_histCentral.GetBinContent(i) and hist.GetBinContent(i) != 0:
            raise RuntimeError("Down scale variation >= central value for %s."
                " This shouldn't be possible.\n"
                "scaleDown_hist: %0.4f\n" 
                "central_hist: %0.4f\n" 
                % (name, scale_histDown.GetBinContent(i), scale_histCentral.GetBinContent(i))
            )
        if scale_histUp.GetBinContent(i) <= scale_histCentral.GetBinContent(i) and hist.GetBinContent(i) != 0:
            raise RuntimeError("Up scale variation <= central value for %s."
                " This shouldn't be possible.\n"
                "scaleUp_hist: %0.2f\n" 
                "central_hist: %0.2f\n" 
                % (name, scale_histUp.GetBinContent(i), scale_histCentral.GetBinContent(i))
            )
    return [scale_histUp, scale_histDown]

signal = "wzjj_vbfnlo" if args['vbfnlo'] else "wzjj_ewk"
numvars = 18 if "VBS" in args['selection'] else 13
isVBS = "VBS" in args['selection'] 
variable = "mjj" if isVBS else "yield"
alldata = makeCompositeHists(fIn, "AllData", 
    ConfigureJobs.getListOfFilesWithXSec(["WZxsec2016data"], manager_path), args['lumi'],
    [variable +"_"+ c for c in chans])
writeOutputListItem(alldata, fOut)
nonprompt = makeCompositeHists(fIn, "DataEWKCorrected", {"DataEWKCorrected" : 1}, args['lumi'],
    [variable+"_Fakes_" + c for c in chans])
for chan in chans:
    hist = nonprompt.FindObject(variable+"_Fakes_"+chan)
    removeZeros(hist)
    card_info[chan]["nonprompt"] = round(hist.Integral(), 4) if hist.Integral() > 0 else 0.001
    stat_hists = getStatHists(hist, "nonprompt", chan, signal)
    nonprompt.extend(stat_hists[:])
writeOutputListItem(nonprompt, fOut)
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

    group = makeCompositeHists(fIn, plot_group, ConfigureJobs.getListOfFilesWithXSec(
        config_factory.getPlotGroupMembers(plot_group), manager_path), args['lumi'], plots)
    name = plot_group.replace("-", "_")
    for chan in chans:
        hist = group.FindObject(variable+"_"+chan)
        card_info[chan]["output_file"] = args['output_file']
        stat_hists = getStatHists(hist, plot_group, chan, signal)
        group.extend(stat_hists)
        if "data" not in plot_group:
            scale_hists = getScaleHists(group.FindObject(variable+"_lheWeights_"+chan), plot_group, chan)
            group.extend(scale_hists)
    for hist in group:
        removeZeros(hist)
    for chan in chans:
        hist = group.FindObject(variable+"_"+chan)
        card_info[chan][name] = round(hist.Integral(), 4) if hist.Integral() > 0 else 0.001
    writeOutputListItem(group, fOut)
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
