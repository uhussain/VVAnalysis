from python import HistTools, OutputTools, ConfigureJobs
import ROOT
import argparse
import json

parser = argparse.ArgumentParser()
parser.add_argument("--input_file", "-i", type=str,
    default="test.root", help="Output file name")
args = parser.parse_args()

saveToFile = True
input_file_name = args.input_file
input_file = ROOT.TFile(input_file_name, "update" if saveToFile else "read")

plot_group_info = {}
for file_name in ["WZxsec2016_aQGC-" + i for i in ["FT", "FM", "FS"]]:
    with open(ConfigureJobs.getManagerPath() + 
            "/AnalysisDatasetManager/PlotGroups/%s.json" % file_name) as plot_file:
        plot_group_info.update(json.load(plot_file))

aqgc_hists = HistTools.make1DaQGCHists(input_file, 
        ["mjj_lheWeights_%s" % c for c in ConfigureJobs.getChannels()] + \
            ["MTWZ_lheWeights_%s" % c for c in ConfigureJobs.getChannels()] + \
            ["Mass_lheWeights_%s" % c for c in ConfigureJobs.getChannels(),
        plot_group_info,
)

if saveToFile:
    for output in aqgc_hists:
        OutputTools.writeOutputListItem(output, input_file)
