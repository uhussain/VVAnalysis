filepath=`dirname "$0"`
while read dataset
do
    dasgoclient -query="file dataset=$dataset run=$1 lumi=$2"
done < $filepath/alldata.txt
