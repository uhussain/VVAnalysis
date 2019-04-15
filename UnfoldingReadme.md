#I am trying to call a bunch of compiled C++ classes with pyROOT
cmsrel CMSSW_8_0_26
#I'm using 8_0_26 for this branch because i cannot make RooUnfold compile in 9_4_2 where I am doing the rest of the analysis - to be resolved
cd CMSSW_8_0_26/src
cmsenv
mkdir Analysis
cd Analysis
#Unfolding branch is built on top of the Super Analysis branch which makes all the Hist files in our analysis
git clone -b Unfolding git@github.com:uhussain/VVAnalysis.git
cd ../..
scram b -j 8
cd Analysis/VVAnalysis/Utilities/scripts
echo .L ResponseMatrixMaker.cxx+ | root.exe -b
cd ../..
source setup.sh
cd RooUnfold
#make clean and then make
make
