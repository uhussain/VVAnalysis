#!/usr/bin/env python

# This script takes a directory as input and creates a new file (name 
# specifed by the user) with a list of filenames in the format required by 
# farmoutAnalysisJobs. 

#That is, containing a list of all the files in the directoy, one file 
# name per line. For filenames begining with /hdfs, /hdfs is removed. 
#
# Author: Kenneth Long, U. Wisconsin 01-10-2015

import glob
import argparse
import subprocess

def getComLineArgs():
    parser = argparse.ArgumentParser()
    parser.add_argument("-o", "--output_file", type=str,
                        required=True, help="Name of file containing file"
                        " to be created (containing list of files)")
    parser.add_argument("-p", "--file_path", type=str,
                        required=True, help="directory containing files")
    return vars(parser.parse_args())
def makeFileList(output_file, file_path):
    p = subprocess.Popen(["hdfs", "dfs", "-ls", file_path.replace("/hdfs", "")],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
    out,err = p.communicate()
    files = []
    for line in out.splitlines():
        split = line.split("/", 1)
        if len(split) != 2:
            continue
        else:
            files.append(split[1])
    with open(output_file, "w") as file_list:
        for file_name in files:
            file_list.write("/"+file_name+"\n")
def main():
    args = getComLineArgs()
    makeFileList(args['output_file'], args['file_path'])
if __name__ == "__main__":
    main()
