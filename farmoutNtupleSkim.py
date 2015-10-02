#!/usr/bin/env python
import argparse
import os
import sys
import string
import datetime
import subprocess
import glob
from Utilities.python import UserInput
import math

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
    date = '{:%d-%m-%Y}'.format(datetime.date.today())
    return '-'.join([date, sample_name, selection])
def getInputFilesPath(sample_name, selection):
    input_files = UserInput.readJson("MetaData/input_files.json")
    if sample_name not in input_files.keys():
        raise ValueError("Input file must correspond to a definition in " 
            "MetaData/input_files.json")
    return input_files[sample_name]['file_paths'][selection].rstrip("/*")
def getCutsJsonName(selection):
    definitions_json = UserInput.readJson("Cuts/definitions.json")
    if selection not in definitions_json.keys():
        raise ValueError("Cut name must correspond to a definition in " 
            "Cuts/definitions.json")
    return definitions_json[selection]
# Choose files per job such that each job is ~100MB
def getFilesPerJob(path_to_files):
    file_list = glob.glob(path_to_files.rstrip("/*") + "/*")
    averagesize =sum([os.path.getsize(f) for f in file_list])/len(file_list)
    return int(math.ceil(100000000./averagesize))
def callFarmout(output_dir, script_name):
    with open('/'.join([output_dir, 'log.txt']), 'w') as log:
        log.write('Condor submit info created with the command: %s\n' % ''.join(sys.argv[0]))
        log.write('Using WZAnalysis code by Kenneth Long (U. Wisconsin):\n')
        log.write('https://github.com/kdlong/WZAnalysis\n\n')
        log.write('The git hash of the commit used and the output of git diff is below\n') 
        log.write('-'*80 + '\n')
        log.write('-'*80 + '\n')
    with open('/'.join([output_dir, 'log.txt']), 'a') as log:
        subprocess.call(['git', 'log', '-1', '--format="%H"'], stdout=log)
        subprocess.call(['git', 'diff'], stdout=log)
    with open('/'.join([output_dir, 'log.txt']), 'a') as log:
        log.write('\n' +'-'*80 + '\n')
        log.write('-'*80 + '\n\n')
        log.write('The output of the generated farmout.sh script is below\n')
    with open('/'.join([output_dir, 'log.txt']), 'a') as log:
        subprocess.call(['bash', script_name], stdout=log, stderr=log)
def farmoutNtupleSkim(sample_name, selection):
    job_name = getJobName(sample_name, selection)
    farmout_dict = {}
    farmout_dict['base_dir'] = os.path.dirname(os.path.realpath(sys.argv[0]))# + '/../..' 
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
    callFarmout(farmout_dict['job_dir'], script_name)
def createRunJob(base_dir, job_dir, cuts_json):
    fill_dict = {'cuts_json' : cuts_json,
        'time' : datetime.datetime.now(),
        'command' : ' '.join(sys.argv)
    }
    fillTemplatedFile('/'.join([base_dir, 'Templates/run_job_template.sh']),
        '/'.join([job_dir, 'run_job.sh']), fill_dict)
def main():
    #for selection in selection_map.iteritems():
    farmoutNtupleSkim('wz-powheg', 'preselection')

if __name__ == "__main__":
    main()
