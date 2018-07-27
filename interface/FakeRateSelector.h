#ifndef FakeRateSelector_h
#define FakeRateSelector_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <exception>
#include <iostream>

// Headers needed by this particular selector
#include "Analysis/VVAnalysis/interface/WZSelectorBase.h"
#include <vector>


class FakeRateSelector : public WZSelectorBase { 
public :
    TH2D* passingTight2D_;
    TH1D* passingTight1DPt_;
    TH1D* passingTight1DEta_;
    TH2D* passingLoose2D_;
    TH1D* passingLoose1DPt_;
    TH1D* passingLoose1DEta_;
    TH2D* ratio2D_;
    TH1D* ratio1DPt_;
    TH1D* ratio1DEta_;

    Float_t type1_pfMETEt;
    UInt_t nCBVIDVetoElec;
    UInt_t nWZLooseMuon;
    
    TBranch* b_type1_pfMETEt;
    TBranch* b_nCBVIDVetoElec;
    TBranch* b_nWZLooseMuon;

    // Readers to access the data (delete the ones you do not need).
    FakeRateSelector(TTree * /*tree*/ =0) { }
    virtual ~FakeRateSelector() { }
    virtual void    Init(TTree *tree) override;
    virtual Bool_t  Process(Long64_t entry) override;
    virtual void    SetupNewDirectory() override;

    ClassDefOverride(FakeRateSelector,0);
};

#endif
