#!/bin/bash

lumi=35.87
lepid="Tight"
DATE=$(date +%d%b%Y)
#frfile=/eos/user/k/kelong/WZAnalysisData/FakeRates/fakeRate${DATE}-3LooseLeptons-${lepid}Leps.root
frfile=/eos/user/k/kelong/WZAnalysisData/FakeRates/fakeRate27Jul2017-3LooseLeptons-${lepid}Leps.root
output=VBSselection_Tight
#output=Wselection
input=WselectionLooseLeps
#input=3lDYControl
#output=3lDYControl
#input=3lTTbarControl
#output=3lTTbarControl
histfile=/eos/user/k/kelong/WZAnalysisData/HistFiles/${output}-${DATE}-${lepid}.root

cd $CMSSW_BASE/src/Analysis/WZAnalysis
./Utilities/scripts/makeFakeRates.py -s 3LooseLeptons -l $lumi -o $frfile
python ScaleFactors/setupScaleFactors.py -t $frfile 
./Utilities/scripts/makeHistFile.py -l $lumi -s $input -o $histfile --output_selection $output
echo "File $histfile produced"
