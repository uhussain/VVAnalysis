#!/bin/bash
# This script was created at ${time} with the command: 
#     ${command}

# To test this script locally (for testing) from the submit directory 
# uncomment the following lines:
# INPUT=input_file_list.txt
# OUTPUT=test.root
tar -xvzf analysis_code.tar.gz
./skimNtuples.py -s ${selection} -a ${analysis} ${trigger} -f $$INPUT -o $$OUTPUT
