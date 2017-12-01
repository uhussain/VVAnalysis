from Utilities.python import HistTools, OutputTools, ConfigureJobs
import ROOT
import array

saveToFile = True
input_file_name = "/eos/user/k/kelong/WZAnalysisData/HistFiles/Wselection-30Nov2017-TightFrom3LooseLeptons.root"
input_file = ROOT.TFile(input_file_name, "update" if saveToFile else "read")
    
transformed_hists = HistTools.getTransformedHists(input_file, 
        ConfigureJobs.getListOfFiles(['WZxsec2016'], 'Wselection'),# + \
            #ConfigureJobs.getListOfFiles(['WZxsec2016data'], 'Wselection'),
        ["mjj_etajj_2D_%s" % c for c in ConfigureJobs.getChannels()] + \
            ["mjj_etajj_2D_Fakes_%s" % c for c in ConfigureJobs.getChannels()], 
        HistTools.makeUnrolledHist, [
            array.array('d', [500, 1000,1500, 2000, 2500]),
            [2.5, 4, 5.5, 20]
        ]
)

if saveToFile:
    for output in transformed_hists:
        OutputTools.writeOutputListItem(output, input_file)
