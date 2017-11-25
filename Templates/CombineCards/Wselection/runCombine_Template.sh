combineCards.py eee=WZ_eee.txt eem=WZ_eem.txt emm=WZ_emm.txt mmm=WZ_mmm.txt > WZ.txt
text2workspace.py WZ.txt -m 999 -o WZ.root
combine -M MaxLikelihoodFit -d WZ.root $$1

if [[ $$1 == "impacts" ]]; then
    rm -rf impact
    mkdir impact
    pushd impact
    card="../WZ.root"
    combineTool.py -M Impacts -m 125 -d $$card -t -1 --doInitialFit
    combineTool.py -M Impacts -m 125 -d $$card -t -1 --allPars --doFits --parallel 12
    combineTool.py -M Impacts -m 125 -d $$card --allPars -o impacts.json
    plotImpacts.py -i impacts.json -o impacts
    popd
fi
