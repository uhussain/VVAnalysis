import datetime
import UserInput
import fnmatch
import glob
import subprocess
import os

def getJobName(sample_name, analysis, selection, version):
    date = '{:%Y-%m-%d}'.format(datetime.date.today())
    selection_name = selection.split(",")[-1 ]
    return '-'.join([date, sample_name, analysis, selection_name, 
        ("v%s" % version) if version.isdigit() else version])
def getNumberAndSizeOfLocalFiles(path_to_files):
    file_list = glob.glob(path_to_files)
    return (len(file_list), sum([os.path.getsize(f)/1000000 for f in file_list]))
def getNumberAndSizeOfHDFSFiles(file_path):
    p = subprocess.Popen(["hdfs", "dfs", "-ls", "-h", file_path.replace("/hdfs", "")],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
    out,err = p.communicate()
    file_info = []
    for line in out.splitlines():
        split = line.split()
        if len(split) != 8:
            continue
        file_info.append(float(split[4].rstrip("mkg")))
    return (0,0) if len(file_info) == 0 else (len(file_info), sum(file_info))
def getListOfHDFSFiles(file_path):
    try:
        out = subprocess.check_output(["hdfs", "dfs", "-ls", file_path.replace("/hdfs", "")])
    except subprocess.CalledProcessError as error:
        logging.warning(error)
        return []
    files = []
    for line in out.splitlines():
        split = line.split(" ", 1)
        if len(split) != 2:
            continue
        elif "root" in split[1]:
            files.append("/"+split[1])
    return files
def getListOfFiles(filelist, manager_path):
    data_path = "%s/AnalysisDatasetManager/FileInfo" % manager_path
    data_info = UserInput.readAllJson("/".join([data_path, "%s.json" % "data/*"]))
    mc_info = UserInput.readAllJson("/".join([data_path, "%s.json" % "montecarlo/*"]))
    valid_names = data_info.keys() + mc_info.keys()
    names = []
    for name in filelist:
        if "*" in name:
            names += fnmatch.filter(valid_names, name)
        else:
            if name.split("__")[0] not in valid_names:
                print "%s is not a valid name" % name
                continue
            names += [name]
    return names
def getPreviousStep(selection, analysis):
    if analysis == "WZxsec2016":
        selection_map = { "ntuples" : "ntuples",
                "loosepreselection" : "ntuples",
                "preselection" : "ntuples",
                "3LooseLeptons" : "ntuples",
                "preselectionLooseVeto" : "ntuples",
                "preselectionNoVeto" : "ntuples",
                "LepVetoAnd3lmass" : "preselection",
                "3lmass" : "preselection",
                "Zselection" : "3lmass",
                "Wselection" : "Zselection",
                "3lDYControl" : "Zselection",
                "3lTTbarControl" : "3lmass",
        }
    elif analysis == "WZDecemberAnalysis":
        selection_map = { "ntuples" : "ntuples",
                "loosepreselection" : "ntuples",
                "preselection" : "ntuples",
                "Mass3l" : "preselection",
                "Zselection" : "preselection",
                "Wselection" : "Zselection"
        }
    first_selection = selection.split(",")[0].strip()
    if first_selection not in selection_map.keys():
        if "preselection" in first_selection:
            first_selection = "preselection"
        else:
            raise ValueError("Invalid selection '%s'. Valid selections are:"
                "%s" % (first_selection, selection_map.keys()))
    return selection_map[first_selection]
def getInputFilesPath(sample_name, manager_path, selection, analysis):
    data_path = "%s/AnalysisDatasetManager/FileInfo" % manager_path
    input_file_name = "/".join([data_path, analysis, "%s.json" %
        selection])
    input_files = UserInput.readJson(input_file_name)
    if sample_name not in input_files.keys():
        raise ValueError("Invalid input file %s. Input file must correspond"
               " to a definition in %s" % (sample_name, input_file_name))
    return input_files[sample_name]['file_path']
def getCutsJsonName(selection, analysis):
    return "/".join(["Cuts", analysis, selection + ".json"]) 
def getTriggerName(sample_name, selection):
    trigger_names = ["MuonEG", "DoubleMuon", "DoubleEG", "SingleMuon", "SingleElectron"]
    if "data" in sample_name and "preselection" in selection:
        for name in trigger_names:
            if name in sample_name:
                return "-t " + name
    return ""
