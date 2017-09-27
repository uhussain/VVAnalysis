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
        "top-ewk" : 0,
        "zg" : 0,
        "vv" : 0,
    },
    "eem" : {
        "wzjj-vbfnlo" : 0,
        "wzjj-ewk" : 0,
        "wz-mgmlm" : 0,
        "top-ewk" : 0,
        "zg" : 0,
        "vv" : 0,
    },
    "emm" : {
        "wzjj-vbfnlo" : 0,
        "wzjj-ewk" : 0,
        "wz-mgmlm" : 0,
        "top-ewk" : 0,
        "zg" : 0,
        "vv" : 0,
    },
    "mmm" : {
        "wzjj-vbfnlo" : 0,
        "wzjj-ewk" : 0,
        "wz-mgmlm" : 0,
        "top-ewk" : 0,
        "zg" : 0,
        "vv" : 0,
    },
}
def getStatHists(hist, name, chan):
    statUp_hist = hist.Clone(hist.GetName().replace(
        chan, "%s_statUp_%s" % (name, chan)))
    statDown_hist = hist.Clone(hist.GetName().replace(
        chan, "%s_statDown_%s" % (name, chan)))
    for i in range(hist.GetNbinsX()):
        up = hist.GetBinContent(i)+hist.GetBinErrorUp(i)
        down = hist.GetBinContent(i)-hist.GetBinErrorLow(i)
        statUp_hist.SetBinContent(i, up if up > 0 else 0) 
        statDown_hist.SetBinContent(i, down if down > 0 else 0) 
    return [statUp_hist, statDown_hist]

def removeZeros(hist):
    for i in range(hist.GetNbinsX()+2):
        if hist.GetBinContent(i) <= 0:
            if "Up" in hist.GetName():
                hist.SetBinContent(i, 0.0001)
            elif "Down" in hist.GetName():
                hist.SetBinContent(i, 0.00001)
            else: 
                hist.SetBinContent(i, 0.00005)

def getStatHists(hist, name, chan):
    stat_hists = []
    for i in range(1, hist.GetNbinsX()+1):
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
    scale_histUp = scale_histCentral.Clone("_".join(["mjj", name, "scaleUp", chan]))
    scale_histDown = scale_histCentral.Clone("_".join(["mjj",name, "scaleDown", chan]))
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

alldata = makeCompositeHists(fIn, "AllData", 
    ConfigureJobs.getListOfFilesWithXSec(["WZxsec2016data"], manager_path), args['lumi'],
    ["mjj_" + c for c in chans])
writeOutputListItem(alldata, fOut)
nonprompt = makeCompositeHists(fIn, "DataEWKCorrected", {"DataEWKCorrected" : 1}, args['lumi'],
    ["mjj_Fakes_" + c for c in chans])
for chan in chans:
    hist = nonprompt.FindObject("mjj_Fakes_"+chan)
    removeZeros(hist)
    card_info[chan]["nonprompt"] = round(hist.Integral() if hist.Integral() > 0 else 0.001, 4)
    stat_hists = getStatHists(hist, "nonprompt", chan)
    nonprompt.extend(stat_hists[:])
writeOutputListItem(nonprompt, fOut)
output_info = PrettyTable(["Filename", "eee", "eem", "emm", "mmm", "All states"])
significance_info = PrettyTable(["Filename", "eee", "eem", "emm", "mmm", "All states"])

for plot_group in ["wz-mgmlm", "wzjj-vbfnlo", "wzjj-ewk", "top-ewk", "zg", "vv"]:
    group = makeCompositeHists(fIn, plot_group, ConfigureJobs.getListOfFilesWithXSec(
        config_factory.getPlotGroupMembers(plot_group), manager_path), args['lumi'],
            ["mjj_" + c for c in chans]+
            ["mjj_jesUp_" + c for c in chans]+
            ["mjj_jesDown_" + c for c in chans]+
            ["mjj_jerUp_" + c for c in chans]+
            ["mjj_jerDown_" + c for c in chans]+
            ["mjj_lheWeights_" + c for c in chans])
    name = plot_group.replace("-", "_")
    for chan in chans:
        hist = group.FindObject("mjj_"+chan)
        card_info[chan]["output_file"] = args['output_file']
        stat_hists = getStatHists(hist, plot_group, chan)
        group.extend(stat_hists)
        if plot_group not in ["zg"]:
            scale_hists = getScaleHists(group.FindObject("mjj_lheWeights_"+chan), plot_group, chan)
            group.extend(scale_hists)
    for hist in group:
        removeZeros(hist)
    for chan in chans:
        hist = group.FindObject("mjj_"+chan)
        card_info[chan][name] = round(hist.Integral(0, 4)) if hist.Integral() > 0 else 0.001
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
with open("/".join([output_dir, "Yields.out"]), "w") as yields:
    yields.write("\n" + " "*30 + "Event Yields")
    yields.write("\n" + str(output_info))
    yields.write("\n" + " "*30 + "S/sqrt(B)")
    yields.write("\n" + str(significance_info))

signal = "wzjj_vbfnlo" if args['vbfnlo'] else "wzjj_ewk"
signal_abv = "vbfnlo" if args['vbfnlo'] else "MG"
for chan, chan_dict in card_info.iteritems():
    chan_dict["signal_name"] = signal.replace("_", "-")
    chan_dict["signal_yield"] = chan_dict[signal]
    ConfigureJobs.fillTemplatedFile(
        'Templates/CombineCards/WZjj_EWK_template_%s.txt' % chan,
        '%s/WZjj_%s_%s.txt' % (output_dir, signal_abv, chan), 
        chan_dict
    )
ConfigureJobs.fillTemplatedFile(
    'Templates/CombineCards/runCombine_Template.sh',
    '%s/runCombine_%s.sh' % (output_dir, signal_abv), 
    {"sample" : signal_abv}
)
