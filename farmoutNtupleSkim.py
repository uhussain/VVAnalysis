#!/usr/bin/env python
import argparse
import json
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
    parser = UserInput.getDefaultParser()
    parser.add_argument("--noScaleFacs", action='store_true',
                        help="Don't add lepton and pilup scale factors to "
                        "ntuple (by default they are added)")
    parser.add_argument("-e", "--extraArgs", type=str, default='',
                        help="Extra arguments to pass to skimNtuples script")
    parser.add_argument("--noSubmit", action='store_true',
                        help="Create submit scripts but don't call farmout")
    return vars(parser.parse_args())

def getFilesPerJob(path_to_files):
    file_type = "Local" if "store" not in path_to_files else "HDFS"
    num, tot_size = getattr(ConfigureJobs, "getNumberAndSizeOf%sFiles" % file_type)(path_to_files)
    if num == 0:
        raise ValueError("Size of file list is zero for path: %s" % path_to_files)
    average_size = tot_size/num
    return int(math.ceil(200./average_size))
def fillTemplatedFile(template_file_name, out_file_name, template_dict):
    with open(template_file_name, "r") as templateFile:
        source = string.Template(templateFile.read())
        result = source.substitute(template_dict)
    with open(out_file_name, "w") as outFile:
        outFile.write(result)
# The intention here was to make sure the output order isn't jumbled together,
# as it is when subprocess outputs are written together. This is an admittedly
# sloppy solution, however
def callFarmout(output_dir, script_name, noSubmit):
    log_file_name = '/'.join([output_dir, 'log.txt'])
    with open(log_file_name, 'w') as log:
        log.write('Condor submit info created with the command:'
            '\n    %s\n' % ' '.join(sys.argv))
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
    status = 2
    farmout_command = ['bash', script_name]
    if noSubmit:
        farmout_command.append("--no-submit")
    with open(log_file_name, 'a') as log:
        status = subprocess.call(farmout_command, stdout=log, stderr=log)
    if status != 0:
        print "Error in submitting files to condor. Check the log file: %s" % log_file_name
    return status
def farmoutNtupleSkim(sample_name, path, selection, analysis, version, noScaleFacs, noSubmit, extraArgs):
    farmout_dict = {}
    farmout_dict['input_files_path'] = ConfigureJobs.getInputFilesPath(
        sample_name, 
        path, 
        ConfigureJobs.getPreviousStep(selection, analysis), 
        analysis
    )
    job_name = ConfigureJobs.getJobName(sample_name, analysis, selection, version) 
    farmout_dict['base_dir'] = os.path.dirname(os.path.realpath(sys.argv[0]))
    submission_dir = ('/data/kelong/%s' if "kelong" in path else "/nfs_scratch/kdlong/%s") \
        % '{:%Y-%m-%d}_WZAnalysisJobs'.format(datetime.date.today())
    try:
        os.mkdir(submission_dir)
    except:
        pass
    farmout_dict['job_dir'] = submission_dir + "/" + job_name
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
        ConfigureJobs.getTriggerName(sample_name, analysis, selection),
        not noScaleFacs and ("data" not in sample_name),
        extraArgs
    )
    status = callFarmout(farmout_dict['job_dir'], script_name, noSubmit)
    if status == 0:
        print "Submitted jobs for %s file set to condor." % sample_name
    else: 
        print "Jobs not submitted"
def createRunJob(base_dir, job_dir, selection, analysis, trigger_name, addScaleFacs, extraArgs):
    fill_dict = {'selection' : selection,
        'analysis' : analysis,
        'time' : datetime.datetime.now(),
        'trigger' : trigger_name,
        'command' : ' '.join(sys.argv),
        'extraArgs' : extraArgs,
        'addScaleFacs' : addScaleFacs,
    }
    fillTemplatedFile('/'.join([base_dir, 'Templates/skim_template.sh']),
        '/'.join([job_dir, 'skim.sh']), fill_dict)
def main():
    #for selection in selection_map.iteritems():
    args = getComLineArgs()
    path = "/cms/kdlong" if "hep.wisc.edu" in os.environ['HOSTNAME'] else \
            "/afs/cern.ch/user/k/kelong/work"
    for file_name in ConfigureJobs.getListOfFiles(args['filenames'], path):
        try:
            farmoutNtupleSkim(file_name, path, args['selection'], 
                args['analysis'], args['version'], args['noScaleFacs'], 
                args['noSubmit'], args['extraArgs'])
        except (ValueError, OSError) as error:
            logging.warning(error)
            logging.warning("Skipping submission for %s" % file_name)

if __name__ == "__main__":
    main()
