#!/usr/bin/env python
import argparse
import os
import sys
import string
import datetime
import subprocess
import glob
from Utilities.python import UserInput
from Utilities.python import ConfigureJobs
import math
import logging

def getComLineArgs():
    parser = argparse.ArgumentParser()
    parser.add_argument("-s", "--selection", type=str,
                        required=True, help="Name of selection to make, "
                        " as defined in Cuts/preselection.json")
    parser.add_argument("-a", "--analysis", type=str,
                        required=True, help="Analysis name, used"
                        " in selection the cut json")
    parser.add_argument("-f", "--filelist", 
                        type=lambda x : [i.strip() for i in x.split(',')],
                        required=True, help="List of input file names "
                        "to be processed (separated by commas)")
    return vars(parser.parse_args())
# Choose files per job such that each job is ~300MB
def getFilesPerJob(path_to_files):
    file_list = glob.glob(path_to_files.rstrip("/*") + "/*")
    averagesize =sum([os.path.getsize(f) for f in file_list[:50]])/min(len(file_list), 50)
    return int(math.ceil(1000000000./averagesize))
def fillTemplatedFile(template_file_name, out_file_name, template_dict):
    with open(template_file_name, "r") as templateFile:
        source = string.Template(templateFile.read())
        result = source.substitute(template_dict)
    with open(out_file_name, "w") as outFile:
        outFile.write(result)
# The intention here was to make sure the output order isn't jumbled together,
# as it is when subprocess outputs are written together. This is an admittedly
# sloppy solution, however
def callFarmout(output_dir, script_name):
    log_file_name = '/'.join([output_dir, 'log.txt'])
    with open(log_file_name, 'w') as log:
        log.write('Condor submit info created with the command: %s\n' % ' '.join(sys.argv))
        log.write('Using WZAnalysis code by Kenneth Long (U. Wisconsin):\n')
        log.write('https://github.com/kdlong/WZAnalysis\n\n')
        log.write('The git hash of the commit used and the output of git diff is below\n') 
        log.write('-'*80 + '\n')
        log.write('-'*80 + '\n')
    with open(log_file_name, 'a') as log:
        subprocess.call(['git', 'log', '-1', '--format="%H"'], stdout=log)
        subprocess.call(['git', 'diff'], stdout=log)
    with open(log_file_name, 'a') as log:
        log.write('\n' +'-'*80 + '\n')
        log.write('-'*80 + '\n\n')
        log.write('The output of the generated farmout.sh script is below\n')
    with open(log_file_name, 'a') as log:
        status = subprocess.call(['bash', script_name], stdout=log, stderr=log)
    if status != 0:
        print "Error in submitting files to condor. Check the log file: %s" % log_file_name
    return status
def farmoutNtupleSkim(sample_name, path, selection, analysis):
    farmout_dict = {}
    farmout_dict['input_files_path'] = ConfigureJobs.getInputFilesPath(sample_name, path, selection, analysis)
    job_name = ConfigureJobs.getJobName(sample_name, analysis, selection) 
    farmout_dict['base_dir'] = os.path.dirname(os.path.realpath(sys.argv[0]))# + '/../..' 
    farmout_dict['job_dir'] = ('/data/kelong/%s' if "kelong" in path else "/nfs_scratch/kdlong/%s") \
        % job_name
    farmout_dict['files_per_job'] = getFilesPerJob(farmout_dict['input_files_path'])
    farmout_dict['job_name'] = job_name
    farmout_dict['time'] = datetime.datetime.now()
    farmout_dict['command'] = ' '.join(sys.argv)
    script_name = '/'.join([farmout_dict['job_dir'], 'farmout.sh'])
    os.mkdir(farmout_dict['job_dir'])
    fillTemplatedFile('/'.join([farmout_dict['base_dir'], 
        'Templates/farmout_template.sh']),
        script_name, 
        farmout_dict
    )
    createRunJob(farmout_dict['base_dir'], 
        farmout_dict['job_dir'], 
        selection,
        analysis,
        ConfigureJobs.getTriggerName(sample_name, selection)
    )
    status = callFarmout(farmout_dict['job_dir'], script_name)
    if status == 0:
        print "Submitted jobs for %s file set to condor." % sample_name
def createRunJob(base_dir, job_dir, selection, analysis, trigger_name):
    fill_dict = {'selection' : selection,
        'analysis' : analysis,
        'time' : datetime.datetime.now(),
        'trigger' : trigger_name,
        'command' : ' '.join(sys.argv)
    }
    fillTemplatedFile('/'.join([base_dir, 'Templates/skim_template.sh']),
        '/'.join([job_dir, 'skim.sh']), fill_dict)
def main():
    #for selection in selection_map.iteritems():
    args = getComLineArgs()
    path = "/cms/kdlong" if "hep.wisc.edu" in os.environ['HOSTNAME'] else \
            "/afs/cern.ch/user/k/kelong/work"
    for file_name in ConfigureJobs.getListOfFiles(args['filelist'], path):
        try:
            farmoutNtupleSkim(file_name, path, args['selection'], args['analysis'])
        except (ValueError, OSError) as error:
            logging.warning(error)
            logging.warning("Skipping submission for %s" % file_name)

if __name__ == "__main__":
    main()
