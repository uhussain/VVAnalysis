import json
import glob
import argparse
import datetime
from collections import OrderedDict

def readAllJson(json_file_path):
    json_info = {}
    for json_file in glob.glob(json_file_path):
        json_info.update(readJson(json_file))
    return json_info
def readJson(json_file_name):
    json_info = {}
    with open(json_file_name) as json_file:
        try:
            json_info = json.load(json_file)
        except ValueError as err:
            print "Error reading JSON file %s. The error message was:" % json_file_name 
            print(err)
    return json_info
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
