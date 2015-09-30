#!/bin/bash

farmoutAnalysisJobs \
    --infer-cmssw-path \
    --fwklite \
    --input-file-list={input_file_list \
    --submit-dir={submit_dir} \
    --output-dag-file={output_dag_file} \
    --output-dir={output_dir} \
    --input-files-per-job={files_per_job} \
    {job_name} \
    {script_name}
