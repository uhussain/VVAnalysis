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
        ConfigureJobs.getListOfFiles(ConfigureJobs.getListOfEWKFilenames() + \
            ['wzjj-vbfnlo-sf', 'wzjj-vbfnlo-of', ] + \
                ['wz3lnu-mg5amcnlo','wz3lnu-powheg', 'zz4l-mg5amcnlo'] + \
                ['AllData', 'WZxsec2016data', 'DataEWKCorrected'], 
            'Wselection'),
        ["mjj_etajj_2D_%s" % c for c in ConfigureJobs.getChannels()] + \
        ["mjj_etajj_2D_Fakes_%s" % c for c in ConfigureJobs.getChannels()] + \
        ["mjj_etajj_2D_%s_%s" % (var, c) for c in ConfigureJobs.getChannels()
                for var in jeVariations] + \
        ["mjj_etajj_2D_%s_Fakes_%s" % (var, c) for c in ConfigureJobs.getChannels()
                for var in jeVariations],
        HistTools.makeUnrolledHist, ConfigureJobs.get2DBinning()
)
addControlRegion=True
mjj_etajj_hists_wcontrol = []
if addControlRegion:
    for folder in transformed_mjj_etajj_hists:
        new_folder = folder.Clone()
        mjj_etajj_hists_wcontrol.append(new_folder)
        for h in folder:
            append = h.GetName().split("_")
            append = append[-1:] if "Fakes" not in h.GetName() else append[-2:]
            hist_name = "_".join(["backgroundControlYield"] + append)
            control_hist = input_file.Get(folder.GetName() + "/" +hist_name)
            if not control_hist:
                raise RuntimeError("failed to find hist %s for folder %s" % (hist_name, folder))
            hist = HistTools.addControlRegionToFitHist(control_hist, h)
            new_folder.Add(hist)
    transformed_mjj_etajj_hists += mjj_etajj_hists_wcontrol

#transformed_mjj_mtwz_hists = HistTools.getTransformedHists(input_file, 
#        ConfigureJobs.getListOfFiles(ConfigureJobs.getListOfEWKFilenames() + \
#            ['wzjj-vbfnlo-sf', 'wzjj-vbfnlo-of', ] + \
#                ['wz3lnu-mg5amcnlo','wz3lnu-powheg', 'zz4l-mg5amcnlo'] + \
#                ['AllData', 'WZxsec2016data', 'DataEWKCorrected'], 
#             'Wselection'),
#        ["mjj_mtwz_2D_%s" % c for c in ConfigureJobs.getChannels()] + \
#            ["mjj_mtwz_2D_Fakes_%s" % c for c in ConfigureJobs.getChannels()],
#        HistTools.makeUnrolledHist, [
#            array.array('d', [500, 1000,1500, 2000, 2500]),
#            [0, 150, 300, 450]
#        ]
#)

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

transformed_hists = transformed_mjj_etajj_hists#+transformed_mjj_mtwz_hists+transformed_mjj_zep3l_hists

if saveToFile:
    for output in transformed_hists:
        OutputTools.writeOutputListItem(output, input_file)
