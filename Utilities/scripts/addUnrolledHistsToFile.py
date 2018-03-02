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

jeVariations = ["jesUp", "jesDown", "jerUp", "jerDown"]

transformed_mjj_etajj_hists = HistTools.getTransformedHists(input_file, 
        ConfigureJobs.getListOfFiles(['WZxsec2016'], 'Wselection'),
        ["mjj_etajj_2D_%s" % c for c in ConfigureJobs.getChannels()] + \
        ["mjj_etajj_2D_Fakes_%s" % c for c in ConfigureJobs.getChannels()] + \
        ["mjj_etajj_2D_%s_%s" % (var, c) for c in ConfigureJobs.getChannels()
                for var in jeVariations] + \
        ["mjj_etajj_2D_%s_Fakes_%s" % (var, c) for c in ConfigureJobs.getChannels()
                for var in jeVariations],
        HistTools.makeUnrolledHist, ConfigureJobs.get2DBinning()
)
print ConfigureJobs.get2DBinning()

transformed_mjj_mtwz_hists = HistTools.getTransformedHists(input_file, 
        ConfigureJobs.getListOfFiles(['WZxsec2016'], 'Wselection'),
        ["mjj_mtwz_2D_%s" % c for c in ConfigureJobs.getChannels()] + \
        ["mjj_mtwz_2D_Fakes_%s" % c for c in ConfigureJobs.getChannels()] + \
        ["mjj_mtwz_2D_%s_%s" % (var, c) for c in ConfigureJobs.getChannels()
                for var in jeVariations] + \
        ["mjj_mtwz_2D_%s_Fakes_%s" % (var, c) for c in ConfigureJobs.getChannels()
                for var in jeVariations],
        HistTools.makeUnrolledHist, [
            array.array('d', [500, 1000,1500, 2000, 2500]),
            [0, 150, 300, 450]
        ]
)

#transformed_mjj_zep3l_hists = HistTools.getTransformedHists(input_file, 
#        ConfigureJobs.getListOfFiles(['WZxsec2016'], 'Wselection'),
#        ["mjj_zep3l_2D_%s_%s" % (var, c) for c in ConfigureJobs.getChannels()
#                for var in jeVariations] + \
#        ["mjj_zep3l_2D_%s_Fakes_%s" % (var, c) for c in ConfigureJobs.getChannels()
#                for var in jeVariations],
#        HistTools.makeUnrolledHist, [
#            array.array('d', [500, 1000,1500, 2000, 2500]),
#            [1, 2, 3, 10]
#        ]
#)

transformed_hists = transformed_mjj_etajj_hists+transformed_mjj_mtwz_hists#+transformed_mjj_zep3l_hists

if saveToFile:
    for output in transformed_hists:
        OutputTools.writeOutputListItem(output, input_file)
