#ifndef WZSelectorBase_h
#define WZSelectorBase_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TH1.h>
#include <TH2.h>
#include <TEfficiency.h>
#include <exception>
#include <iostream>

// Headers needed by this particular selector
#include <vector>
#include "Analysis/VVAnalysis/interface/ScaleFactor.h"
#include "Analysis/VVAnalysis/interface/SelectorBase.h"

class WZSelectorBase : public SelectorBase {
public :
    ScaleFactor* pileupSF_;
    ScaleFactor* muonSF_;
    ScaleFactor* eIdSF_ ;
    ScaleFactor* eGsfSF_;
    ScaleFactor* mIdSF_;
    ScaleFactor* mIsoSF_;

    bool isVBS_;
    bool passesLeptonVeto;
    Float_t weight;
    Float_t genWeight;
    Float_t nTruePU;
    Float_t ZMass;
    Float_t MET;
    Float_t type1_pfMETPhi;
    UInt_t nCBVIDTightElec;
    UInt_t nCBVIDHLTSafeElec;
    UInt_t nWZTightMuon;
    UInt_t nWZMediumMuon;
    Bool_t l1IsTight;
    Bool_t l2IsTight;
    Bool_t l3IsTight;
    Float_t l1GenPt;
    Float_t l2GenPt;
    Float_t l3GenPt;
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
    Float_t l2Eta;
    Float_t l3Eta;
    Float_t l1Pt;
    Float_t l2Pt;
    Float_t l3Pt;
    Float_t l3MtToMET;
    Float_t Flag_BadChargedCandidateFilterPass;
    Float_t Flag_BadPFMuonFilterPass;
    Bool_t Flag_duplicateMuonsPass;
    Bool_t Flag_badMuonsPass;
    Bool_t Flag_HBHENoiseFilterPass;
    Bool_t Flag_HBHENoiseIsoFilterPass;
    Bool_t Flag_EcalDeadCellTriggerPrimitiveFilterPass;
    Bool_t Flag_goodVerticesPass;
    Bool_t Flag_eeBadScFilterPass;
    Bool_t Flag_globalTightHalo2016FilterPass;
    
    TBranch* b_genWeight;
    TBranch* b_nTruePU;
    TBranch* b_Zmass;
    TBranch* b_MET;
    TBranch* b_type1_pfMETPhi;
    TBranch* b_nCBVIDTightElec;
    TBranch* b_nCBVIDHLTSafeElec;
    TBranch* b_nWZTightMuon;
    TBranch* b_nWZMediumMuon;
    TBranch* b_l1IsTight;
    TBranch* b_l2IsTight;
    TBranch* b_l3IsTight;
    TBranch* b_l1GenPt;
    TBranch* b_l2GenPt;
    TBranch* b_l3GenPt;
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
    TBranch* b_l2Eta;
    TBranch* b_l3Eta;
    TBranch* b_l1Pt;
    TBranch* b_l2Pt;
    TBranch* b_l3Pt;
    TBranch* b_ZMass;
    TBranch* b_l3MtToMET;
    TBranch* b_Flag_BadChargedCandidateFilterPass;
    TBranch* b_Flag_BadPFMuonFilterPass;
    TBranch* b_Flag_badMuonsPass;
    TBranch* b_Flag_duplicateMuonsPass;
    TBranch* b_Flag_HBHENoiseFilterPass;
    TBranch* b_Flag_HBHENoiseIsoFilterPass;
    TBranch* b_Flag_EcalDeadCellTriggerPrimitiveFilterPass;
    TBranch* b_Flag_goodVerticesPass;
    TBranch* b_Flag_eeBadScFilterPass;
    TBranch* b_Flag_globalTightHalo2016FilterPass;

    // Readers to access the data (delete the ones you do not need).
    //virtual void    SetScaleFactors() override;
    virtual void    SlaveBegin(TTree *tree) override;
    virtual void    Init(TTree *tree) override;

    ClassDefOverride(WZSelectorBase,0);

protected:
    std::vector<std::string> nonprompt3l_ = {
        "tt-lep", "st-schan", "st-tchan-t", "st-tchan-tbar",
        "st-tw", "st-tbarw", "DYm50", "DYm50-1j",
        "DYm50-2j","DYm50-3j","DYm50-4j", "DYm50__LO",
    };

    bool isNonpromptEstimate_;
    bool isNonpromptMC_;
    bool isZgamma_;
    const float FR_MAX_PT_ = 50;
    const float FR_MAX_ETA_ = 2.5;
    bool zlep1IsTight();
    bool zlep2IsTight();
    bool lepton3IsTight();
    bool tightZLeptons();
    bool IsGenMatched3l();
    virtual std::string GetNameFromFile() override;
    virtual void    SetBranchesNanoAOD() override;
    virtual void    SetBranchesUWVV() override;
    void LoadBranchesUWVV(Long64_t entry, std::pair<Systematic, std::string> variation) override;
    void LoadBranchesNanoAOD(Long64_t entry, std::pair<Systematic, std::string> variation) override;
};

#endif

