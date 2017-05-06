#!/bin/bash

lumi=35.87
lepid="Tight"
DATE=$(date +%d%b%Y)
frfile=data/fakeRate${DATE}-3LooseLeptons-${lepid}Leps.root
histfile=Wselection-${DATE}-${lepid}.root

cd $CMSSW_BASE/src/Analysis/WZAnalysis
./Utilities/scripts/makeFakeRates.py -s 3LooseLeptons -l $lumi -o $frfile
python ScaleFactors/setupScaleFactors.py -t $frfile 
./Utilities/scripts/makeHistFile.py -l $lumi -s WselectionLooseLeps -o $histfile 
