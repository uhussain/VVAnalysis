//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Feb  8 23:47:12 2017 by ROOT version 6.06/01
// from TTree ntuple/ntuple
// found on file: /data/kelong/DibosonAnalysisData/DYControlFakeRate/2017-02-05-data_MuonEG_Run2016B-23Sep2016-v3-WZxsec2016-DYControlFakeRate-v1/skim-ntuplize_1.root
//////////////////////////////////////////////////////////

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
#include "Analysis/WZAnalysis/interface/WZSelectorBase.h"
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

    Float_t Zmass;
    Float_t type1_pfMETEt;
    UInt_t nCBVIDVetoElec;
    UInt_t nWZLooseMuon;
    Float_t l1Pt;
    Float_t l2Pt;
    Float_t l3Pt;
    Float_t l3Eta;
    Float_t l3MtToMET;
    
    TBranch* b_Zmass;
    TBranch* b_type1_pfMETEt;
    TBranch* b_nCBVIDVetoElec;
    TBranch* b_nWZLooseMuon;
    TBranch* b_l1Pt;
    TBranch* b_l2Pt;
    TBranch* b_l3Pt;
    TBranch* b_l3Eta;
    TBranch* b_l3MtToMET;

    // Readers to access the data (delete the ones you do not need).
    FakeRateSelector(TTree * /*tree*/ =0) { }
    virtual ~FakeRateSelector() { }
    virtual Int_t   Version() const { return 2; }
    virtual void    Begin(TTree *tree);
    virtual void    SlaveBegin(TTree *tree);
    virtual void    Init(TTree *tree);
    virtual Bool_t  Notify();
    virtual Bool_t  Process(Long64_t entry);
    virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
    virtual void    SetOption(const char *option) { fOption = option; }
    virtual void    SetObject(TObject *obj) { fObject = obj; }
    virtual void    SetInputList(TList *input) { fInput = input; }
    virtual TList  *GetOutputList() const { return fOutput; }
    virtual void    SlaveTerminate();
    virtual void    Terminate();
    virtual void    SetupNewDirectory() override;

    ClassDef(FakeRateSelector,0);
};

#endif
