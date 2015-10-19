import json
import argparse
import datetime
from collections import OrderedDict

def readJson(json_file_name):
    json_info = {}
    with open(json_file_name) as json_file:
        try:
            json_info = json.load(json_file)
        except ValueError as err:
            print "Error reading JSON file %s. The error message was:" % json_file_name 
            print(err)
    return json_info
def getJobName(sample_name, selection):
    date = '{:%Y-%m-%d}'.format(datetime.date.today())
    return '-'.join([date, sample_name, selection])
def getInputFilesPath(sample_name, selection):
    input_files = readJson("MetaData/input_files.json")
    if sample_name not in input_files.keys():
        print input_files.keys()
        raise ValueError("Invalid input file %s. Input file must correspond"
               " to a definition in MetaData/input_files.json" % sample_name)
    return input_files[sample_name]['file_paths'][selection].rstrip("/*")
def getCutsJsonName(selection):
    definitions_json = readJson("Cuts/definitions.json")
    if selection not in definitions_json.keys():
        raise ValueError("Cut name must correspond to a definition in " 
            "Cuts/definitions.json")
    return definitions_json[selection]
def getComLineArgs():
    parser = argparse.ArgumentParser()
    parser.add_argument("-a", "--save_all", action='store_true',
                        required=False, help="Save output of step separately")
    parser.add_argument("-s", "--selections", 
                        type=lambda x: OrderedDict((i.strip(),  '')  for i in x.split(",")),
                        required=True, help="List of selections you wish to make "
                        "(as defined in Cuts/definitions.json), separated by commas")
    args = vars(parser.parse_args())
    definitions_json = readJson("Cuts/definitions.json")
    for selection in args['selections']:
        if selection not in definitions_json.keys():
            raise ValueError("Cut name must correspond to a definition in " 
                "Cuts/definitions.json")
        args['selections'][selection] = definitions_json[selection]
    return args
