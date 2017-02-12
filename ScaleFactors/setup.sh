#!/usr/bin/env bash

# Modified from N. Smith, U. Wisconsin
pushd ..
mkdir -p ../data
# https://twiki.cern.ch/twiki/bin/view/CMS/EgammaIDRecipesRun2#Efficiencies_and_scale_factors
wget http://fcouderc.web.cern.ch/fcouderc/EGamma/scaleFactors/ichep2016_80X/resultsGsfID/egammaEffi.txt_SF2D.root -O ../data/ichepElectronGsfSF.root
wget http://fcouderc.web.cern.ch/fcouderc/EGamma/scaleFactors/ichep2016_80X//resultsEleID/runBCD/passingMedium/egammaEffi.txt_SF2D.root -O ../data/ichepElectronMediumSF.root
wget http://fcouderc.web.cern.ch/fcouderc/EGamma/scaleFactors/ichep2016_80X/resultsEleID/runBCD/passingTight/egammaEffi.txt_SF2D.root -O ../data/ichepElectronTightSF.root
exit
# https://twiki.cern.ch/twiki/bin/view/CMS/MuonWorkInProgressAndPagResults
cp /afs/cern.ch/cms/Physics/muon/ReferenceEfficiencies/Run2016/25ns/proviSFs_7p65/MuonID_Z_RunBCD_prompt80X_7p65.root ../data/
cp /afs/cern.ch/cms/Physics/muon/ReferenceEfficiencies/Run2016/25ns/proviSFs_7p65/MuonIso_Z_RunBCD_prompt80X_7p65.root ../data/

# Getting things from twiki is hard but possible
# See http://linux.web.cern.ch/linux/docs/cernssocookie.shtml
if which cern-get-sso-cookie &>/dev/null; then
  cern-get-sso-cookie --krb -r -u https://twiki.cern.ch/twiki/bin/view/CMS/WebHome -o ~/private/ssocookie.txt

  # https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation80X
  wget --load-cookies ~/private/ssocookie.txt https://twiki.cern.ch/twiki/pub/CMS/BtagRecommendation80X/CSVv2_ichep.csv -O ../data/CSVv2_ichep.csv
else
  echo -e "\033[33mWARNING: You will have to download some files yourself, sorry\033[0m"
fi
popd

# Pileup from https://gitlab.cern.ch/ncsmith/PileupWeights
git clone https://:@gitlab.cern.ch:8443/ncsmith/PileupWeights.git
pushd PileupWeights
# This needs pileupCalc.py and takes a while
cmsenv
. getDataDistribution.sh
./calculatePileupCorrections.py
popd
