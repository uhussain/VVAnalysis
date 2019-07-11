#!/bin/bash
Date=2019-03-13

echo "Resubmitting failed ZZ2019Analysis jobs"

export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch

#listOfSamples="DoubleEG DoubleMuon MuonEG SingleElectron SingleMuon"
#eras="B C D E F"

#for name in $listOfSamples; do
for dir in /data/uhussain/${Date}_ZZ2019AnalysisJobs/*/ ; do
  echo "=====================" $dir "========================"
  bash ${dir}/farmout.sh --resubmit-failed-jobs
done
#done
