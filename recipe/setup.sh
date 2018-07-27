#!/bin/bash

dataset_manager=$(./Utilities/scripts/getConfigValue.py dataset_manager_path)/AnalysisDatasetManager
pushd $CMSSW_BASE/src/Analysis/VVAnalysis/Cuts

for folder in $(ls -d */); do
    analysis_name=${folder/\//}
    alias_file=${dataset_manager}/Aliases/${analysis_name}.json 
    if [ -f $alias_file ]; then
        pushd $analysis_name
        ln -s $alias_file aliases.json
        echo "INFO: Linked analysis $analysis_name to alias file"
        popd
    fi
done

popd
