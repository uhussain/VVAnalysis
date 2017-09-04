#!/bin/bash

lumi=35.87
lepid="Tight"
DATE=$(date +%d%b%Y)
#looselepsfile=3MediumLeptons
looselepsfile=3LooseLeptons
frfile=/eos/user/k/kelong/WZAnalysisData/FakeRates/fakeRate${DATE}-${lepid}LepsFrom${looselepsfile}.root

#input=3lDYControl${looselepsfile/3/}
#output=3lDYControl
#input=3lTTbarControl
#output=3lTTbarControl
input=Wselection${looselepsfile/3/}
#output=Wselection
output=VBSselection
#output=VBSselectionTight
#input=Zselection${looselepsfile/3/}
#output=Zselection
#input=${looselepsfile}
#output=3LooseLeptons
histfile=/eos/user/k/kelong/WZAnalysisData/HistFiles/${output}-${DATE}-${lepid}From${looselepsfile}.root

cd $CMSSW_BASE/src/Analysis/WZAnalysis
./Utilities/scripts/makeFakeRates.py -s ${looselepsfile} -l $lumi -o $frfile
python ScaleFactors/setupScaleFactors.py -t $frfile 
./Utilities/scripts/makeHistFile.py -l $lumi -s $input -o $histfile --output_selection $output
if [ -f $histfile ]; then
    echo "Histogram file $histfile produced"
else
    echo "ERROR: $histfile was not produced!"
fi
