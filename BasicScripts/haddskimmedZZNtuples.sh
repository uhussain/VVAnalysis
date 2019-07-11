DATE=2017-11-08
analysis=ZZ4l2017
selections=preselectionToQCDVeto-v1
selectionlevel=full

rm -rf /data/uhussain/SkimmedZZNtuples_data_${DATE}_hadd/results_${selectionlevel}
mkdir -p /data/uhussain/SkimmedZZNtuples_data_${DATE}_hadd/results_${selectionlevel}

#plotgroup=( $(jq -r '.[].plot_group' /afs/cern.ch/user/u/uhussain/work/ZZ4lAnalysisDatasetManager/FileInfo/ZZ4l2017/ntuples.json) ) 
datadir=( $(jq -r 'to_entries[] | select(.value.plot_group == "data") | .key' /afs/cern.ch/user/u/uhussain/work/ZZ4lAnalysisDatasetManager/FileInfo/ZZ4l2017/ntuples.json) )

#Combined Data Groups

for dir in "${datadir[@]}" 
do
  echo $dir 
  
  hadd -f /data/uhussain/SkimmedZZNtuples_data_${DATE}_hadd/results_${selectionlevel}/${dir}.root /hdfs/store/user/uhussain/ZZAnalysisJobs_${DATE}/${DATE}-${dir}-${analysis}-${selections}/*.root

done

rm -rf /data/uhussain/SkimmedZZNtuples_mc_${DATE}_hadd/results_${selectionlevel}
mkdir -p /data/uhussain/SkimmedZZNtuples_mc_${DATE}_hadd/results_${selectionlevel}

mcdir=( $(jq -r 'to_entries[] | select(.value.plot_group != "data") | .key' /afs/cern.ch/user/u/uhussain/work/ZZ4lAnalysisDatasetManager/FileInfo/ZZ4l2017/ntuples.json) )

for list in "${mcdir[@]}" 
do
  echo $list

  hadd -f /data/uhussain/SkimmedZZNtuples_mc_${DATE}_hadd/results_${selectionlevel}/${list}.root /hdfs/store/user/uhussain/ZZAnalysisJobs_${DATE}/${DATE}-${list}-${analysis}-${selections}/*.root

done
#for dir in "${arr[@]}" 
#for group in "${plotgroup[@]}"
#do
#  if [[ $group == *"data"* ]]
#  then
#    arr=( $(jq -r 'keys[]' /afs/cern.ch/user/u/uhussain/work/ZZ4lAnalysisDatasetManager/FileInfo/ZZ4l2017/ntuples.json) )
#  #hadd -f /data/uhussain/SkimmedZZNtuples_${DATE}_hadd/${dir}.root /hdfs/store/user/uhussain/ZZAnalysisJobs_${DATE}/${DATE}-${dir}-${analysis}-${selections}/*.root
#  fi
#done

# Combined Data Groups
#hadd -f /data/uhussain/TauEff_${DATE}_hadd/ICHEPRuns.root /data/uhussain/TauEff_${DATE}_hadd/Run2016[B,C,D].root
#hadd -f /data/uhussain/TauEff_${DATE}_hadd/AllRuns.root /data/uhussain/TauEff_${DATE}_hadd/Run*.root

# MC Samples
#hadd -f /data/uhussain/TauEff_${DATE}_hadd/DYJets.root /hdfs/store/user/uhussain/tagAndProbe${DATE}DYJets-ConfFile_MC_reHLT_cfg/*.root 
