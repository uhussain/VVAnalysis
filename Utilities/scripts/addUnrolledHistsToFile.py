from python import HistTools, OutputTools, ConfigureJobs
import ROOT
import array
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--input_file", "-i", type=str,
    default="test.root", help="Output file name")
args = parser.parse_args()

saveToFile = True
input_file_name = args.input_file
input_file = ROOT.TFile(input_file_name, "update" if saveToFile else "read")
    
transformed_hists = HistTools.getTransformedHists(input_file, 
        ConfigureJobs.getListOfFiles(['WZxsec2016'], 'Wselection'),# + \
            #ConfigureJobs.getListOfFiles(['WZxsec2016data'], 'Wselection'),
        ["mjj_etajj_2D_%s" % c for c in ConfigureJobs.getChannels()] + \
            ["mjj_etajj_2D_Fakes_%s" % c for c in ConfigureJobs.getChannels()] + \
            ["mjj_etajj_2D_jesUp_%s" % c for c in ConfigureJobs.getChannels()] + \
            ["mjj_etajj_2D_jesDown_%s" % c for c in ConfigureJobs.getChannels()] + \
            ["mjj_etajj_2D_jerUp_%s" % c for c in ConfigureJobs.getChannels()] + \
            ["mjj_etajj_2D_jerDown_%s" % c for c in ConfigureJobs.getChannels()], 
        HistTools.makeUnrolledHist, [
            array.array('d', [500, 1000,1500, 2000, 2500]),
            [2.5, 4, 5.5, 20]
        ]
)

if saveToFile:
    for output in transformed_hists:
        OutputTools.writeOutputListItem(output, input_file)
