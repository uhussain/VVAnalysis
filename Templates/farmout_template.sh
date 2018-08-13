#!/bin/bash
# This script was created at ${time} with the command: 
#     ${command}
#
# To resubmit failed jobs: ./farmout.sh --resubmit-failed-jobs
DATE=`date +%Y-%m-%d`
#DATE=2018-03-06
pushd ${base_dir}
if [ ! -f ${job_dir}/analysis_code.tar.gz ]; then
    tar \
        -zcvhf ${job_dir}/analysis_code.tar.gz \
        Cuts/ \
        Utilities/ \
        skimNtuples.py
fi
if [ ! -f ${job_dir}/input_file_list.txt ]; then
./Utilities/scripts/makeInputFilesList.py \
    -o ${job_dir}/input_file_list.txt \
    -p "${input_files_path}" \
    --only_root_files
fi
popd
chmod +x ${job_dir}/skim.sh
farmoutAnalysisJobs \
    --fwklite \
    --output-dir=gsiftp://cms-lvs-gridftp.hep.wisc.edu:2811//hdfs/store/user/$$USER/ZZAnalysisJobs2018_$$DATE/${job_name} \
    --input-file-list=${job_dir}/input_file_list.txt \
    --submit-dir=${job_dir}/submit \
    --input-files-per-job=${files_per_job} \
    --input-dir=root://cmsxrootd.hep.wisc.edu/ \
    --assume-input-files-exist \
    --extra-inputs=${job_dir}/analysis_code.tar.gz \
    --input-basenames-not-unique \
    $$1 \
    ${job_name} \
    $$CMSSW_BASE \
    ${job_dir}/skim.sh
