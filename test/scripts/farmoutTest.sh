#!/bin/bash
pushd /afs/cern.ch/user/k/kelong/work/WZAnalysis
mkdir /data/kelong/test
tar \
    -zcvf /data/kelong/test/user_code.tar.gz \
    Cuts/ \
    Utilities/ \
    skimNtuples.py
./Utilities/scripts/makeInputFilesList.py \
    -o /data/kelong/test/test_filelist.txt \
    -p /hdfs/store/user/dntaylor/data/2015-09-13-13TeV-WZ/WZTo3LNu_TuneCUETP8M1_13TeV-powheg-pythia8
cp test/farmout/test.sh /data/kelong/test/test.sh
popd
farmoutAnalysisJobs \
    --infer-cmssw-path \
    --fwklite \
    --input-file-list=/data/kelong/test/test_filelist.txt \
    --submit-dir=/data/kelong/test/submit \
    --input-files-per-job=10 \
    --input-dir=root://cmsxrootd.hep.wisc.edu/ \
    --assume-input-files-exist \
    --extra-inputs=/data/kelong/test/user_code.tar.gz \
    test_job \
    /data/kelong/test/test.sh
