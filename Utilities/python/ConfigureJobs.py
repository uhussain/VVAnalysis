import datetime
import UserInput
import fnmatch

def getJobName(sample_name, analysis, selection):
    date = '{:%Y-%m-%d}'.format(datetime.date.today())
    return '-'.join([date, sample_name, analysis, selection.replace("/", "_")])
def getListOfFiles(filelist, manager_path):
    data_path = "%s/AnalysisDatasetManager/FileInfo" % manager_path
    data_info = UserInput.readJson("/".join([data_path, "%s.json" % "data"]))
    mc_info = UserInput.readJson("/".join([data_path, "%s.json" % "montecarlo"]))
    valid_names = data_info.keys() + mc_info.keys()
    names = []
    for name in filelist:
        if "*" in name:
            names += fnmatch.filter(valid_names, name)
        else:
            if name not in valid_names:
                print "%s is not a valid name" % name
                continue
            names += [name]
    return names
def getInputFilesPath(sample_name, manager_path, selection, analysis):
    data_path = "%s/AnalysisDatasetManager/FileInfo" % manager_path
    selection_map = { "ntuples" : "ntuples",
            "loosepreselection" : "ntuples",
            "preselection" : "ntuples",
            "Mass3l" : "preselection",
            "Zselection" : "Mass3l",
            "Wselection" : "Zselection"
    }
    if selection not in selection_map.keys():
        raise ValueError("Invalid selection '%s'. Valid selections are:"
               "%s" % (selection, selection_map.keys()))
    input_file_name = "/".join([data_path, analysis, "%s.json" %
        selection_map[selection]])
    print "input_file_name %s" % input_file_name
    input_files = UserInput.readJson(input_file_name)
    if sample_name not in input_files.keys():
        raise ValueError("Invalid input file %s. Input file must correspond"
               " to a definition in %s" % (sample_name, input_file_name))
    return input_files[sample_name]['file_path'].rstrip("/*")
def getCutsJsonName(selection, analysis):
    definitions_json = UserInput.readJson("Cuts/definitions.json")
    if selection not in definitions_json.keys():
        raise ValueError("Invalid selection %s. Selection must correspond " 
            "to a definition in Cuts/definitions.json" % selection)
    if analysis not in definitions_json[selection]:
        raise ValueError("Invalid analysis %s. Analysis name must "
            "correspond to a definition in Cuts/definitions.json" % analysis)
    return definitions_json[selection][analysis]
def getTriggerName(input_files_path):
    trigger_names = ["MuonEG", "DoubleMuon", "DoubleEG"]
    if "data" in input_files_path:
        for name in trigger_names:
            if name in input_files_path:
                return "-t " + name
    return ""
