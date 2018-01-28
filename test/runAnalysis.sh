#!/bin/bash

lumi=35.87
lepid="Tight"
DATE=$(date +%d%b%Y)
DATE_MONTHONLY=$(date +%b%Y)
looselepsfile=3MediumLeptons
#looselepsfile=3LooseLeptons

input=Wselection${looselepsfile/3/}
output=Wselection
if [[ $1 == VBS* ]]; then
    output=$1
elif [[ $1 == "Inclusive2Jet" ]]; then
    output=$1
elif [[ $1 == "3lDYControl" ]]; then
    if [[ $looselepsfile == "3MediumLeptons" ]]; then
        input=3lDYControl${looselepsfile/3/}
    else
        input=3lDYControl
    fi
    output=3lDYControl
elif [[ $1 == "3lTTbarControl" ]]; then
    if [[ $looselepsfile == "3MediumLeptons" ]]; then
        input=3lTTbarControl${looselepsfile/3/}
    else
        input=3lTTbarControl
    fi
    output=3lTTbarControl
elif [[ $1 == "Zselection" ]]; then
    input=Zselection${looselepsfile/3/}
    output=Zselection
elif [[ $1 == "FakeRateSelectionLoose" ]]; then
    input=${looselepsfile}
    output=FakeRateSelectionLoose
elif [[ $1 == "FakeRateSelectionTight" ]]; then
    input=${looselepsfile}
    output=FakeRateSelectionTight
else
    echo "INFO: Using default selection 'Wselection'"
fi

frfile=/eos/user/k/kelong/WZAnalysisData/FakeRates/fakeRate${DATE_MONTHONLY}-${lepid}LepsFrom${looselepsfile}.root
#histfile=/eos/user/k/kelong/WZAnalysisData/HistFiles/${output}-${DATE}-${lepid}From${looselepsfile}_SvenjasFakeRates.root
#histfile=/eos/user/k/kelong/WZAnalysisData/HistFiles/j1Pt50-j2Pt50/${output}-${DATE}-${lepid}From${looselepsfile}.root
histfile=/eos/user/k/kelong/WZAnalysisData/HistFiles/${output}-${DATE}-${lepid}From${looselepsfile}.root

cd $CMSSW_BASE/src/Analysis/WZAnalysis
if [ -f $frfile ] && [ "$2" != "RedoFakeRate" ]; then
    echo "INFO: Fake rate file $frfile exists! Using exisiting file."
else
    echo "INFO: Fake rate file $frfile not found. It will be created."
    ./Utilities/scripts/makeFakeRates.py -s ${looselepsfile} -l $lumi -o $frfile
    python ScaleFactors/setupScaleFactors.py -t $frfile 
fi

if [ "$2" != "combineOnly" ]; then
    ./Utilities/scripts/makeHistFile.py -l $lumi -s $input -o $histfile --output_selection $output
    if [ -f $histfile ]; then
        echo "Histogram file $histfile produced"
    else
        echo "ERROR: $histfile was not produced!"
    fi
fi

if [ "$2" != "noCombine" ]; then
    combine_selection=WZxsec2016/${output}
    combine_folder=${DATE}
    if [[ $output == *_* ]]; then
        combine_selection=WZxsec2016/${output/_*/}
        combine_folder=${output/*_/}/${DATE}
    fi

    if [[ "$2" != "combineOnly" && "$1" == "VBSselection"* ]]; then
        python ./Utilities/scripts/addaQGCHistsToFile.py -i $histfile
        python ./Utilities/scripts/addUnrolledHistsToFile.py -i $histfile
    fi
    if [ "$1" != "FakeRate"* ]; then
        ./Utilities/scripts/prepareCombine.py \
            --input_file $histfile \
            -s $combine_selection \
            --folder_name $combine_folder \
            -l 35.9 \
            --output_file /eos/user/k/kelong/WZAnalysisData/CombineData/$(basename $histfile) $3
        echo "Info: File /eos/user/k/kelong/WZAnalysisData/CombineData/$(basename  $histfile) created"
    fi
fi
