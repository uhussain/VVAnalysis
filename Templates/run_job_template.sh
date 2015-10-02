#!/bin/bash
# This script was created at ${time} with the command: 
#     ${command}
tar -xvzf analysis_code.tar.gz
./skimNtuples.py -s ${cuts_json} -f $$INPUT -o $$OUTPUT
