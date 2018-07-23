Analysis code for WZ/ZZ analyses. Some scripts using selections to skim Ntuples and C++ code to make histogram files for WZ.

Setup
-----------
CMSSW version: CMSSW_8_0_30 or CMSSW_9_4_8

X_Y_Z = 8_0_30 (or CMSSW_9_4_8)

username=kdlong (or your username)

To checkout and compile:

```console
cmsrel CMSSW_version
cd CMSSW_X_Y_Z/src
mkdir Analysis
cd Analysis
git clone git@github.com:<username>/VVAnalysis.git
scram b -j 8
```

You will also need to install a separate package that contains information on the datasets used and histograms and binning. It's recommended that you fork this into your github because some files/settings will be user specific.

install_path = ~username/work/ (or whatever you prefer)

```console
cd install_path
git clone git@github.com:<username>/AnalysisDatasetManager.git
```

You should edit the configuration file found [here](../master/Templates/config) to have settings specific to you. 
