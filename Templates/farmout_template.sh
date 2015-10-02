#!/bin/bash
# This script was created at 2015-10-02 18:59:45.807346 with the command: 
#     ./farmoutNtupleSkim.py
pushd /afs/cern.ch/work/k/kelong/WZAnalysis
tar \
    -zcvf /data/kelong/test/analysis_code.tar.gz \
    Cuts/ \
    Utilities/ \
    skimNtuples.py
./Utilities/scripts/makeInputFilesList.py \
    -o /data/kelong/test/input_file_list.txt \
    -p /hdfs/store/user/dntaylor/data/2015-09-13-13TeV-WZ/WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8
popd
chmod +x /data/kelong/test/run_job.sh
farmoutAnalysisJobs \
    --infer-cmssw-path \
    --fwklite \
    --input-file-list=/data/kelong/test/input_file_list.txt \
    --submit-dir=/data/kelong/test/submit \
    --input-files-per-job=1 \
    --input-dir=root://cmsxrootd.hep.wisc.edu/ \
    --assume-input-files-exist \
    --extra-inputs=/data/kelong/test/analysis_code.tar.gz \
    test \
    /data/kelong/test/run_job.sh
