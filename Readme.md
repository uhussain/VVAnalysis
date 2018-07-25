Analysis code for WZ/ZZ analyses. Some scripts using selections to skim Ntuples and C++ code to make histogram files for WZ.

Setup
-----------
CMSSW version: CMSSW_8_0_30 # or CMSSW_9_4_8
```bash
X_Y_Z="8_0_30" # or 9_4_8, match with the CMSSW of UWVV ideally
username="kdlong" # or your username
```

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

This repository includes scripts to run all steps (starting with [UWVV ntuples](https://github.com/uwcms/UWVV]) to produce the results of [SMP-18-001](http://cms.cern.ch/iCMS/analysisadmin/cadilines?line=SMP-18-001). Many steps should be trivially modifiable for other analyses. 

The analysis processeeds in several steps.

1. Produce ntuples using [UWVV](https://github.com/uwcms/UWVV]). See this package for further documentation. In principle another package could be used without much trouble, but variable names etc would need to be modified. Output is generally stored on a distributed storage system, e.g. /hdfs at UW or /eos at CERN.
2. Skim ntuples to create smaller files that can be copied to a "local" storage disk, such as /data at uwlogin or /nfs_scratch at Wisconsin.
3. Run analysis code to estimate backgrounds and produce histograms with final selections.
4. Plotting code combines histograms by scaling with cross section and luminosity information. Colors and CMS style are implemented.
