combineCards.py eeee=ZZ_eeee.txt eemm=ZZ_eemm.txt mmee=ZZ_mmee.txt mmmm=ZZ_mmmm.txt > ZZ.txt
#text2workspace.py ZZ.txt -m 999 -o ZZ.root
#
#if [[ $$1 == "impacts" ]]; then
#    rm -rf impact
#    mkdir impact
#    pushd impact
#    card="../ZZ.root"
#    combineTool.py -M Impacts -m 125 -d $$card --doInitialFit
#    combineTool.py -M Impacts -m 125 -d $$card --allPars --doFits --parallel 12
#    combineTool.py -M Impacts -m 125 -d $$card --allPars -o impacts.json
#    plotImpacts.py -i impacts.json -o impacts
#    popd
#else
#    combine -M MaxLikelihoodFit -d ZZ.root $$1
#fi

