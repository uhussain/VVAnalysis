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
#include "Analysis/VVAnalysis/interface/ZLSelectorBase.h"
#include <vector>


class FakeRateSelector : public ZLSelectorBase { 
public :
    Float_t Zmass;
    Float_t type1_pfMETEt;
    Float_t l1Pt;
    Float_t l2Pt;
    Float_t l3Pt;
    Float_t l3Eta;
    Float_t l3MtToMET;
    
    TBranch* b_Zmass;
    TBranch* b_type1_pfMETEt;
    TBranch* b_l1Pt;
    TBranch* b_l2Pt;
    TBranch* b_l3Pt;
    TBranch* b_l3Eta;
    TBranch* b_l3MtToMET;

    // Readers to access the data (delete the ones you do not need).
    FakeRateSelector(TTree * /*tree*/ =0) { }
    virtual ~FakeRateSelector() { }
    virtual void    Init(TTree *tree) override;
    virtual Bool_t  Process(Long64_t entry) override;
    virtual void    SetupNewDirectory() override;

    ClassDefOverride(FakeRateSelector,0);
};

#endif
