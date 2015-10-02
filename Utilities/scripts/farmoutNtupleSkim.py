#!/usr/bin/env python
import argparse
import os
import sys
import string
import datetime
import subprocess

def getComLineArgs():
    parser = argparse.ArgumentParser()
    parser.add_argument("-s", "--selection_json", type=str,
                        required=True, help="Name of json file containing" 
                        " cuts to make, e.g. Cuts/preselection.json")
    parser.add_argument("-f", "--filelist", type=str,
                        required=True, help="List of input file names "
                        "to be processed (separated by commas)")
    parser.add_argument("-o", "--output_file_name", type=str,
                        required=True, help="Name of output file")
    return vars(parser.parse_args())

def fillTemplatedFile(template_file_name, out_file_name, template_dict):
    with open(template_file_name, "r") as templateFile:
        source = string.Template(templateFile.read())
        result = source.substitute(template_dict)
    with open(out_file_name, "w") as outFile:
        outFile.write(result)
def getJobName(sample_name, selection):
    return 'test'
def getInputFilesPath(sample_name, selection):
    return '/hdfs/store/user/dntaylor/data/2015-09-13-13TeV-WZ/WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8'
def getCutsJsonName(selection):
    return 'Cuts/preselection.json'
def getFilesPerJob(path_to_files):
    return 1
def farmoutNtupleSkim(sample_name, selection):
    job_name = getJobName(sample_name, selection)
    farmout_dict = {}
    farmout_dict['base_dir'] = os.path.dirname(os.path.realpath(sys.argv[0])) + '/../..' 
    farmout_dict['job_dir'] = '/data/kelong/%s' % job_name
    farmout_dict['input_files_path'] = getInputFilesPath(sample_name, selection)
    farmout_dict['files_per_job'] = getFilesPerJob(farmout_dict['input_files_path'])
    farmout_dict['job_name'] = job_name
    farmout_dict['time'] = datetime.datetime.now()
    farmout_dict['command'] = ' '.join(sys.argv)
    script_name = '/'.join([farmout_dict['job_dir'], 'farmout.sh'])
    os.mkdir(farmout_dict['job_dir'])
    fillTemplatedFile('/'.join([farmout_dict['base_dir'], 'Templates/farmout_template.sh']),
        script_name, farmout_dict)
    createRunJob(farmout_dict['base_dir'], farmout_dict['job_dir'], getCutsJsonName(selection))
    subprocess.call(['bash', script_name])
def createRunJob(base_dir, job_dir, cuts_json):
    fill_dict = {'cuts_json' : cuts_json,
        'time' : datetime.datetime.now(),
        'command' : ' '.join(sys.argv)
    }
    fillTemplatedFile('/'.join([base_dir, 'Templates/run_job_template.sh']),
        '/'.join([job_dir, 'run_job.sh']), fill_dict)
def main():
    farmoutNtupleSkim('test', 'test')

if __name__ == "__main__":
    main()
