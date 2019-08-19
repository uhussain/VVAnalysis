#!/bin/bash
Date=2019-08-18

echo "Copying leftover ZZ2016Analysis jobs"


for dir in /hdfs/store/user/uhussain/ZZ4l2017AnalysisJobs_${Date}/*/ ; do
  echo "=====================" $dir "========================"
  python copyFromHdfs2.py -s LooseLeptons $dir
done
#done
