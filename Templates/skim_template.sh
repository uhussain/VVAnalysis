#!/bin/bash
# This script was created at ${time} with the command: 
#     ${command}

# To run this script locally (for testing) from the submit directory 
# uncomment the following lines:
# INPUT=input_file_list.txt
# OUTPUT=test.root
tar -xvzf analysis_code.tar.gz
./skimNtuples.py -s ${selection} -a ${analysis} ${trigger} -f $$INPUT -o $$OUTPUT ${extraArgs}

addScaleFacs=${addScaleFacs}
if [ $$addScaleFacs == "true" ]; then
    echo "adding lepton id and pilupe scale factors"
    ./utilities/scripts/addScaleFactors.py -f $$output
fi

addDuplicatedFlag=${addDuplicatedFlag}
if [ $$addDuplicatedFlag == "true" ]; then
    echo "adding a duplicated flag if an event is in more than one final state"
    ./utilities/scripts/addDuplicatedFlag.py -f $$output
fi
