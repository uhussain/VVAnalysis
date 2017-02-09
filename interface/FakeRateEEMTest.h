//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Feb  8 23:47:12 2017 by ROOT version 6.06/01
// from TTree ntuple/ntuple
// found on file: /data/kelong/DibosonAnalysisData/DYControlFakeRate/2017-02-05-data_MuonEG_Run2016B-23Sep2016-v3-WZxsec2016-DYControlFakeRate-v1/skim-ntuplize_1.root
//////////////////////////////////////////////////////////

#ifndef FakeRateEEMTest_h
#define FakeRateEEMTest_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TH1.h>
#include <TH2.h>

// Headers needed by this particular selector
#include <vector>



class FakeRateEEMTest : public TSelector {
public :
    TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain
    const char* name_ = "Unnamed";
    const char* channel_ = "eee";
    
    TList* histDir_;
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
    UInt_t nWWLooseElec;
    UInt_t nWZLooseMuon;
    Bool_t e1IsCBVIDTight;
    Bool_t e2IsCBVIDTight;
    Bool_t e1IsEB;
    Bool_t e2IsEB;
    Float_t e1PVDXY;
    Float_t e2PVDXY;
    Float_t e1PVDZ;
    Float_t e2PVDZ;
    Bool_t mIsTightMuon;
    Float_t mRelPFIsoDBR04;
    Float_t mEta;
    Float_t mPt;
    
    TBranch* b_type1_pfMETEt;
    TBranch* b_nWWLooseElec;
    TBranch* b_nWZLooseMuon;
    TBranch* b_e1IsCBVIDTight;
    TBranch* b_e2IsCBVIDTight;
    TBranch* b_e1IsEB;
    TBranch* b_e2IsEB;
    TBranch* b_e1PVDXY;
    TBranch* b_e2PVDXY;
    TBranch* b_e1PVDZ;
    TBranch* b_e2PVDZ;
    TBranch* b_mIsTightMuon;
    TBranch* b_mRelPFIsoDBR04;
    TBranch* b_mEta;
    TBranch* b_mPt;

    // Readers to access the data (delete the ones you do not need).
    FakeRateEEMTest(TTree * /*tree*/ =0) { }
    virtual ~FakeRateEEMTest() { }
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

    ClassDef(FakeRateEEMTest,0);

};

#endif

#ifdef FakeRateEEMTest_cxx
void FakeRateEEMTest::Init(TTree *tree)
{
    // The Init() function is called when the selector needs to initialize
    // a new tree or chain. Typically here the reader is initialized.
    // It is normally not necessary to make changes to the generated
    // code, but the routine can be extended by the user if needed.
    // Init() will be called many times when running on PROOF
    // (once per file to be processed).
    if (!tree) return;
    fChain = tree;

    fChain->SetBranchAddress("type1_pfMETEt", &type1_pfMETEt, &b_type1_pfMETEt);
    fChain->SetBranchAddress("nWWLooseElec", &nWWLooseElec, &b_nWWLooseElec);
    fChain->SetBranchAddress("nWZLooseMuon", &nWZLooseMuon, &b_nWZLooseMuon);
    fChain->SetBranchAddress("e1IsCBVIDTight", &e1IsCBVIDTight, &b_e1IsCBVIDTight);
    fChain->SetBranchAddress("e2IsCBVIDTight", &e2IsCBVIDTight, &b_e2IsCBVIDTight);
    fChain->SetBranchAddress("e1IsEB", &e1IsEB, &b_e1IsEB);
    fChain->SetBranchAddress("e2IsEB", &e2IsEB, &b_e2IsEB);
    fChain->SetBranchAddress("e1PVDXY", &e1PVDXY, &b_e1PVDXY);
    fChain->SetBranchAddress("e2PVDXY", &e2PVDXY, &b_e2PVDXY);
    fChain->SetBranchAddress("e1PVDZ", &e1PVDZ, &b_e1PVDZ);
    fChain->SetBranchAddress("e2PVDZ", &e2PVDZ, &b_e2PVDZ);
    fChain->SetBranchAddress("mIsTightMuon", &mIsTightMuon, &b_mIsTightMuon);
    fChain->SetBranchAddress("mRelPFIsoDBR04", &mRelPFIsoDBR04, &b_mRelPFIsoDBR04);
    fChain->SetBranchAddress("mPt", &mPt, &b_mPt);
    fChain->SetBranchAddress("mEta", &mEta, &b_mEta);
}

Bool_t FakeRateEEMTest::Notify()
{
    // The Notify() function is called when a new file is opened. This
    // can be either for a new TTree in a TChain or when when a new TTree
    // is started when using PROOF. It is normally not necessary to make changes
    // to the generated code, but the routine can be extended by the
    // user if needed. The return value is currently not used.

    return kTRUE;
}


#endif // #ifdef FakeRateEEMTest_cxx
