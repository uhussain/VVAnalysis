combineCards.py eee=WZjj${sample}_eee.txt eem=WZjj${sample}_eem.txt emm=WZjj${sample}_emm.txt mmm=WZjj${sample}_mmm.txt > WZjj_EWK${sample}.txt
text2workspace.py WZjj_EWK${sample}.txt -m 999 -o WZjj_EWK${sample}.root

card="WZjj_EWK.root"
if [[ $$1 == "mu" ]]; then
    combine -M MaxLikelihoodFit -d $$card -t -1 --expectSignal 1 $$2
elif [[ $$1 == "impacts" ]]; then
    rm -rf impact
    mkdir impact
    pushd impact
    combineTool.py -M Impacts -m 125 -d ../$$card -t -1 --expectSignal 1. --doInitialFit
    combineTool.py -M Impacts -m 125 -d ../$$card -t -1 --expectSignal 1. --allPars --doFits --parallel 12
    combineTool.py -M Impacts -m 125 -d ../$$card --allPars -o impacts.json
    plotImpacts.py -i impacts.json -o impacts
    popd
else
    combine -M Significance -t -1 --expectSignal 1 -d $$card $$1
fi
