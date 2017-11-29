combineCards.py eee=WZjj${sample}_eee.txt eem=WZjj${sample}_eem.txt emm=WZjj${sample}_emm.txt mmm=WZjj${sample}_mmm.txt > WZjj_EWK${sample}.txt
text2workspace.py WZjj_EWK${sample}.txt -m 999 -o WZjj_EWK${sample}.root
combine -M ProfileLikelihood --significance -t -1 --expectSignal 1 -d WZjj_EWK${sample}.root $$1

if [[ $$1 == "impacts" ]]; then
    rm -rf impact
    mkdir impact
    pushd impact
    card="../WZjj_EWK${sample}.root"
    #card="../dgh_card.dat.root"
    combineTool.py -M Impacts -m 125 -d $$card -t -1 --expectSignal 1. --doInitialFit
    combineTool.py -M Impacts -m 125 -d $$card -t -1 --expectSignal 1. --allPars --doFits --parallel 12
    combineTool.py -M Impacts -m 125 -d $$card --allPars -o impacts.json
    plotImpacts.py -i impacts.json -o impacts
    popd
fi
