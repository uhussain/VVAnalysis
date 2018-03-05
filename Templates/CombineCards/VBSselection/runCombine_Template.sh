card="WZjj${sample}.txt"
if [[ "$$1" != "all" && "$$2" != "all" && $$3 != "all" ]]; then
    combineCards.py eee=WZjj${sample}_eee.txt eem=WZjj${sample}_eem.txt emm=WZjj${sample}_emm.txt mmm=WZjj${sample}_mmm.txt > $$card 
else
    card="WZjj${sample}_all.txt"
fi

if [[ $$1 == "mu" ]]; then
    combine -M MaxLikelihoodFit -d $$card -t -1 --expectSignal 1 $$2
elif [[ $$1 == "impacts" ]]; then
    text2workspace.py $$card -m 999 -o $${card/txt/root}
    card=$${card/txt/root}
    rm -rf impact
    mkdir impact
    pushd impact
    combineTool.py -M Impacts -m 125 -d ../$$card -t -1 --expectSignal 1. --doInitialFit
    combineTool.py -M Impacts -m 125 -d ../$$card -t -1 --expectSignal 1. --allPars --doFits --parallel 12
    combineTool.py -M Impacts -m 125 -d ../$$card --allPars -o impacts.json
    plotImpacts.py -i impacts.json -o impacts
    popd
elif [[ "$$1" == "all" ]]; then
    combine -M Significance -t -1 --expectSignal 1 -d $$card 
else
    combine -M Significance -t -1 --expectSignal 1 -d $$card $$1
fi
