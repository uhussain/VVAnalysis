#!/bin/bash
Date=2018-05-02

echo "Resubmitting failed ZZAnalysis jobs"

export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch

listOfSamples="DoubleEG DoubleMuon MuonEG SingleElectron SingleMuon"
eras="B C D E F"

for name in $listOfSamples; do
  for era in $eras; do
    echo "=====================" $name "========================"
    bash /data/uhussain/${Date}_ZZ2018AnalysisJobs/${Date}-${name}_Run2017${era}-17Nov2017-v1-ZZ4l2018-preselectionTo4lmass-v1/farmout.sh --resubmit-failed-jobs
  done
done
