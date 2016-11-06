#!/bin/bash
tar -xvzf user_code.tar.gz
echo $INPUT
echo $OUTPUT
./skimNtuples.py -s Cuts/preselection.json -f $INPUT -o $OUTPUT
