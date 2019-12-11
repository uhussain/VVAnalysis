#!/bin/bash
Date=2019-09-12

echo "Copying leftover ZZ2016Analysis jobs"


for dir in /hdfs/store/user/uhussain/ZplusL2016AnalysisJobs_${Date}/*/ ; do
  echo "=====================" $dir "========================"
  python copyFromHdfs2.py -s ZplusLSkim $dir
done
#done
