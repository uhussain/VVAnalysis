//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Feb  8 23:47:12 2017 by ROOT version 6.06/01
// from TTree ntuple/ntuple
// found on file: /data/kelong/DibosonAnalysisData/DYControlFakeRate/2017-02-05-data_MuonEG_Run2016B-23Sep2016-v3-WZxsec2016-DYControlFakeRate-v1/skim-ntuplize_1.root
//////////////////////////////////////////////////////////

#ifndef MakeBackgroundEstimate_h
#define MakeBackgroundEstimate_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TH1.h>
#include <TH2.h>
#include <exception>
#include <iostream>

// Headers needed by this particular selector
#include <vector>
#include "Analysis/WZAnalysis/interface/ScaleFactor.h"

class MakeBackgroundEstimate : public TSelector {
public :
    TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain
    
    ScaleFactor * fakeRate_allE_;
    ScaleFactor * fakeRate_allMu_;
    
    TList* histDir_;
    TH1D* zmassHist_;
    TH1D* nvtxHist_;
    TH1D* massHist_;
    TH1D* mjjHist_;
    TH1D* l1PtHist_;
    TH1D* l2PtHist_;
    TH1D* l3PtHist_;
    TH1D* dEtajjHist_;
    TH1D* zmassHistPPF_;
    TH1D* l1PtHistPPF_;
    TH1D* zmassHistPFP_;
    TH1D* zmassHistFPP_;
    TH1D* zmassHistFFP_;
    TH1D* zmassHistFPF_;
    TH1D* zmassHistPFF_;
    TH1D* zmassHistFFF_;
    
    std::vector<float>* jetPt = nullptr;
    std::vector<float>* jetEta = nullptr;
    UInt_t nvtx;
    Float_t genWeight;
    Float_t type1_pfMETEt;
    Float_t Mass;
    Float_t mjj;
    Float_t Zmass;
    UInt_t nCBVIDVetoElec;
    UInt_t nWZLooseMuon;
    Bool_t l1IsTight;
    Bool_t l2IsTight;
    Bool_t l3IsTight;
    Bool_t e1IsEB;
    Bool_t e2IsEB;
    Bool_t e3IsEB;
    Float_t e1PVDXY;
    Float_t e2PVDXY;
    Float_t e3PVDXY;
    Float_t e1PVDZ;
    Float_t e2PVDZ;
    Float_t e3PVDZ;
    Float_t m1RelPFIsoDBR04;
    Float_t m2RelPFIsoDBR04;
    Float_t m3RelPFIsoDBR04;
    Float_t l1Eta;
    Float_t l1Pt;
    Float_t l2Eta;
    Float_t l2Pt;
    Float_t l3Eta;
    Float_t l3Pt;
    
    TBranch* b_jetPt;
    TBranch* b_jetEta;
    TBranch* b_nvtx;
    TBranch* b_genWeight;
    TBranch* b_Zmass;
    TBranch* b_type1_pfMETEt;
    TBranch* b_Mass;
    TBranch* b_mjj;
    TBranch* b_nCBVIDVetoElec;
    TBranch* b_nWZLooseMuon;
    TBranch* b_l1IsTight;
    TBranch* b_l2IsTight;
    TBranch* b_l3IsTight;
    TBranch* b_e1IsEB;
    TBranch* b_e2IsEB;
    TBranch* b_e3IsEB;
    TBranch* b_e1PVDXY;
    TBranch* b_e2PVDXY;
    TBranch* b_e3PVDXY;
    TBranch* b_e1PVDZ;
    TBranch* b_e2PVDZ;
    TBranch* b_e3PVDZ;
    TBranch* b_m1RelPFIsoDBR04;
    TBranch* b_m2RelPFIsoDBR04;
    TBranch* b_m3RelPFIsoDBR04;
    TBranch* b_l1Eta;
    TBranch* b_l1Pt;
    TBranch* b_l2Eta;
    TBranch* b_l2Pt;
    TBranch* b_l3Eta;
    TBranch* b_l3Pt;

    // Readers to access the data (delete the ones you do not need).
    MakeBackgroundEstimate(TTree * /*tree*/ =0) { }
    virtual ~MakeBackgroundEstimate() { }
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

    ClassDef(MakeBackgroundEstimate,0);
private:
    std::string name_ = "Unnamed";
    std::string channel_ = "undefined";
    bool zlep1IsTight(); 
    bool zlep2IsTight(); 
    bool wlepIsTight(); 
    float getl1FakeRate();
    float getl2FakeRate();
    float getl3FakeRate();
    bool IsFPPRegion();
    bool IsPFPRegion();
    bool IsPPFRegion();
    bool IsFPFRegion();
    bool IsFFPRegion();
    bool IsPFFRegion();
    bool IsFFFRegion();
};

#endif

#ifdef MakeBackgroundEstimate_cxx
void MakeBackgroundEstimate::Init(TTree *tree)
{
    // The Init() function is called when the selector needs to initialize
    // a new tree or chain. Typically here the reader is initialized.
    // It is normally not necessary to make changes to the generated
    // code, but the routine can be extended by the user if needed.
    // Init() will be called many times when running on PROOF
    // (once per file to be processed).
    if (!tree) return;
    fChain = tree;
    if (name_.find("data") == std::string::npos){
        fChain->SetBranchAddress("genWeight", &genWeight, &b_genWeight);
    }
    fChain->SetBranchAddress("nvtx", &nvtx, &b_nvtx);
    fChain->SetBranchAddress("Mass", &Mass, &b_Mass);
    fChain->SetBranchAddress("mjj", &mjj, &b_mjj);
    fChain->SetBranchAddress("type1_pfMETEt", &type1_pfMETEt, &b_type1_pfMETEt);
    fChain->SetBranchAddress("nCBVIDVetoElec", &nCBVIDVetoElec, &b_nCBVIDVetoElec);
    fChain->SetBranchAddress("nWZLooseMuon", &nWZLooseMuon, &b_nWZLooseMuon);
    fChain->SetBranchAddress("jetPt", &jetPt, &b_jetPt);
    fChain->SetBranchAddress("jetEta", &jetEta, &b_jetEta);

    if (channel_ == "eee") {
        fChain->SetBranchAddress("e1_e2_Mass", &Zmass, &b_Zmass);
        fChain->SetBranchAddress("e1IsCBVIDTight", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("e2IsCBVIDTight", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("e3IsCBVIDTight", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("e1IsEB", &e1IsEB, &b_e1IsEB);
        fChain->SetBranchAddress("e2IsEB", &e2IsEB, &b_e2IsEB);
        fChain->SetBranchAddress("e3IsEB", &e3IsEB, &b_e3IsEB);
        fChain->SetBranchAddress("e1PVDXY", &e1PVDXY, &b_e1PVDXY);
        fChain->SetBranchAddress("e2PVDXY", &e2PVDXY, &b_e2PVDXY);
        fChain->SetBranchAddress("e3PVDXY", &e3PVDXY, &b_e3PVDXY);
        fChain->SetBranchAddress("e1PVDZ", &e1PVDZ, &b_e1PVDZ);
        fChain->SetBranchAddress("e2PVDZ", &e2PVDZ, &b_e2PVDZ);
        fChain->SetBranchAddress("e3PVDZ", &e3PVDZ, &b_e3PVDZ);
        
        fChain->SetBranchAddress("e1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("e1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("e2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("e2Eta", &l2Eta, &b_l2Eta);
        fChain->SetBranchAddress("e3Pt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("e3Eta", &l3Eta, &b_l3Eta);
    }
    else if (channel_ == "eem") { 
        fChain->SetBranchAddress("e1_e2_Mass", &Zmass, &b_Zmass);
        fChain->SetBranchAddress("e1IsCBVIDTight", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("e2IsCBVIDTight", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("e1IsEB", &e1IsEB, &b_e1IsEB);
        fChain->SetBranchAddress("e2IsEB", &e2IsEB, &b_e2IsEB);
        fChain->SetBranchAddress("e1PVDXY", &e1PVDXY, &b_e1PVDXY);
        fChain->SetBranchAddress("e2PVDXY", &e2PVDXY, &b_e2PVDXY);
        fChain->SetBranchAddress("e1PVDZ", &e1PVDZ, &b_e1PVDZ);
        fChain->SetBranchAddress("e2PVDZ", &e2PVDZ, &b_e2PVDZ);
        
        fChain->SetBranchAddress("mIsMedium", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("mRelPFIsoDBR04", &m3RelPFIsoDBR04, &b_m3RelPFIsoDBR04);
        fChain->SetBranchAddress("mPt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("mEta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("e1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("e1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("e2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("e2Eta", &l2Eta, &b_l2Eta);
    }
    else if (channel_ == "emm") { 
        fChain->SetBranchAddress("m1_m2_Mass", &Zmass, &b_Zmass);
        fChain->SetBranchAddress("eIsCBVIDTight", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("eIsEB", &e3IsEB, &b_e3IsEB);
        fChain->SetBranchAddress("ePVDXY", &e3PVDXY, &b_e3PVDXY);
        fChain->SetBranchAddress("ePVDZ", &e3PVDZ, &b_e3PVDZ);
        fChain->SetBranchAddress("m1IsMedium", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("m1RelPFIsoDBR04", &m1RelPFIsoDBR04, &b_m1RelPFIsoDBR04);
        fChain->SetBranchAddress("m2IsMedium", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("m2RelPFIsoDBR04", &m2RelPFIsoDBR04, &b_m2RelPFIsoDBR04);
        fChain->SetBranchAddress("ePt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("eEta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("m1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("m1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("m2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("m2Eta", &l2Eta, &b_l2Eta);
    }
    else if (channel_ == "mmm") { 
        fChain->SetBranchAddress("m1_m2_Mass", &Zmass, &b_Zmass);
        fChain->SetBranchAddress("m1IsMedium", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("m1RelPFIsoDBR04", &m1RelPFIsoDBR04, &b_m1RelPFIsoDBR04);
        fChain->SetBranchAddress("m2IsMedium", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("m2RelPFIsoDBR04", &m2RelPFIsoDBR04, &b_m2RelPFIsoDBR04);
        fChain->SetBranchAddress("m3IsMedium", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("m3RelPFIsoDBR04", &m3RelPFIsoDBR04, &b_m3RelPFIsoDBR04);
        fChain->SetBranchAddress("m1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("m1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("m2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("m2Eta", &l2Eta, &b_l2Eta);
        fChain->SetBranchAddress("m3Pt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("m3Eta", &l3Eta, &b_l3Eta);
    }
    else
        throw std::invalid_argument("Invalid channel choice!");
}

Bool_t MakeBackgroundEstimate::Notify()
{
    // The Notify() function is called when a new file is opened. This
    // can be either for a new TTree in a TChain or when when a new TTree
    // is started when using PROOF. It is normally not necessary to make changes
    // to the generated code, but the routine can be extended by the
    // user if needed. The return value is currently not used.

    return kTRUE;
}


#endif // #ifdef MakeBackgroundEstimate_cxx

