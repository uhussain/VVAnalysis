#!/bin/bash
pushd /afs/cern.ch/user/k/kelong/work/WZAnalysis
tar \
    -zcvf user_code.tar.gz \
    Cuts/ \
    Utilities/ \
    skimNtuples.py
popd
mkdir /data/kelong/test
cp test.sh /data/kelong/test
mv /afs/cern.ch/user/k/kelong/work/WZAnalysis/user_code.tar.gz /data/kelong/test
cp /afs/cern.ch/user/k/kelong/work/WZAnalysis/MetaData/InputFileLists/test.txt /data/kelong/test

farmoutAnalysisJobs \
    --infer-cmssw-path \
    --fwklite \
    --input-file-list=/data/kelong/test/test.txt \
    --submit-dir=/data/kelong/test/submit \
    --input-files-per-job=2 \
    --input-dir=root://cmsxrootd.hep.wisc.edu/ \
    --assume-input-files-exist \
    --extra-inputs=/data/kelong/test/user_code.tar.gz \
    test_job \
    /data/kelong/test/test.sh
