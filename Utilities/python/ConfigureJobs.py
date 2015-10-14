import datetime
import UserInput

def getJobName(sample_name, selection):
    date = '{:%Y-%m-%d}'.format(datetime.date.today())
    return '-'.join([date, sample_name, selection])
def getInputFilesPath(sample_name, selection):
    input_files = UserInput.readJson("MetaData/input_files.json")
    if sample_name not in input_files.keys():
        print input_files.keys()
        raise ValueError("Invalid input file %s. Input file must correspond"
               " to a definition in MetaData/input_files.json" % sample_name)
    return input_files[sample_name]['file_paths'][selection].rstrip("/*")
def getCutsJsonName(selection):
    definitions_json = UserInput.readJson("Cuts/definitions.json")
    if selection not in definitions_json.keys():
        raise ValueError("Cut name must correspond to a definition in " 
            "Cuts/definitions.json")
    return definitions_json[selection]
def getTriggerName(input_files_path):
    trigger_names = ["MuonEG", "DoubleMuon", "DoubleEG"]
    if "data" in input_files_path:
        for name in trigger_names:
            if name in input_files_path:
                return name
    return ""
