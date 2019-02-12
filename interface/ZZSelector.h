#ifndef ZZSelector_h
#define ZZSelector_h

#include <vector>
#include "Analysis/VVAnalysis/interface/ZZSelectorBase.h"
#include <TH3.h>

class ZZSelector : public ZZSelectorBase {
public :
    bool doSystematics_ = true;
    bool applyFullSelection_ = true;
    bool isaQGC_ = false;
    bool doaQGC_ = false;

    enum Systematic {
        Central,
        muonEfficiencyUp,
        muonEfficiencyDown,
        electronEfficiencyUp,
        electronEfficiencyDown,
        pileupUp,
        pileupDown,
    }; 

    std::vector<std::string> systHists_ = {
        "yield",
        "backgroundControlYield",
        "Mass",
        //"Z1Mass",
        //"Z2Mass",
        "ZMass",
        "ZZPt",
        "Z1Pt",
        "Z2Pt"
        "ZPt",
        "LepPt",
        "Z1lep1_Pt",
        //"nTruePU",
        //"nvtx",
        //"Zlep1_Pt",
        //"Zlep2_Pt",
        //"Wlep_Pt",
        //"Zlep1_Eta",
        //"Zlep2_Eta",
        //"Wlep_Eta",
    };

    std::map<Systematic, std::string> systematics_ = {
        {electronEfficiencyUp, "CMS_eff_eUp"},
        {electronEfficiencyDown, "CMS_eff_eDown"},
        {muonEfficiencyUp, "CMS_eff_mUp"},
        {muonEfficiencyDown, "CMS_eff_mDown"},
        {pileupUp, "CMS_pileupUp"},
        {pileupDown, "CMS_pileupDown"},
    };

    std::map<std::string, TH1D*> hists1D_ = {
        { "yield",  {} },
        { "backgroundControlYield",  {} },
        //{ "nTruePU",  {} },
        { "nvtx",  {} },
        { "ZMass",  {} },
        { "Z1Mass",  {} },
        { "Z2Mass",  {} },
        { "ZZPt",  {} },
        { "Z1Pt",  {} },
        { "Z2Pt",  {} },
        { "ZPt",  {} },
       // { "ZPt",  {} },
       // { "ZEta",  {} },
       // { "ZPhi",  {} },
       // { "dR_lW_Z",  {} },
        {"LepPt",{}},
        {"LepEta",{}},
        {"Lep12Pt",{}},
        {"Lep12Eta",{}},
        {"Lep34Pt",{}},
        {"Lep34Eta",{}},
        { "Z1lep1_Eta",  {} },
        { "Z1lep1_Phi",  {} },
        { "Z1lep1_Pt",  {} },
        { "Z1lep1_PdgId",  {} },
        { "Z1lep2_Eta",  {} },
        { "Z1lep2_Phi",  {} },
        { "Z1lep2_Pt",  {} },
        { "Z1lep2_PdgId",  {} },
        { "Z2lep1_Eta",  {} },
        { "Z2lep1_Phi",  {} },
        { "Z2lep1_Pt",  {} },
        { "Z2lep1_PdgId",  {} },
        { "Z2lep2_Eta",  {} },
        { "Z2lep2_Phi",  {} },
        { "Z2lep2_Pt",  {} },
        { "Z2lep2_PdgId",  {} },
        { "Mass", {} },
       // { "Pt", {} },
       // { "nJets", {} },
       // { "nJetCSVv2T", {} },
    };

    // Weight hists must be subset of 1D hists!
    std::map<std::string, TH2D*> weighthists_ = {
        { "backgroundControlYield",  {} },
        { "Mass",  {} },
        { "yield",  {} },
    };
    
    std::map<std::string, TH2D*> hists2D_ = {
        {"Z1lep1_Z1lep2_Pt", {}},
        {"Z1lep1_Z1lep2_Eta", {}},
        {"Z1lep1_Z1lep2_Phi", {}},
        {"Z2lep1_Z2lep2_Pt", {}},
        {"Z2lep1_Z2lep2_Eta", {}},
        {"Z2lep1_Z2lep2_Phi", {}},
        {"Z1Mass_Z2Mass",{}},
    };

    std::vector<std::string> systHists2D_ = {
        "mjj_etajj_2D", "mjj_dRjj_2D"
    };

    std::map<std::string, TH3D*> weighthists2D_ {
        {"mjj_etajj_2D", {}},
        {"mjj_dRjj_2D", {}}
    };

    std::vector<float>* scaleWeights = NULL;
    std::vector<float>* pdfWeights = NULL;
    std::vector<float> lheWeights;
    unsigned int weight_info_;
    float dEtajj;
    float dEtajj_jesUp;
    float dEtajj_jesDown;
    float dEtajj_jerUp;
    float dEtajj_jerDown;
    float dRjj;
    float dRjj_jesUp;
    float dRjj_jesDown;
    float dRjj_jerUp;
    float dRjj_jerDown;
    float zep3l;
    float zep3l_jesUp;
    float zep3l_jesDown;
    float zep3l_jerUp;
    float zep3l_jerDown;
    std::vector<float>* jetEta = NULL;
    std::vector<float>* jetEta_jesUp = NULL;
    std::vector<float>* jetEta_jesDown = NULL;
    std::vector<float>* jetEta_jerUp = NULL;
    std::vector<float>* jetEta_jerDown = NULL;
    std::vector<float>* jetPhi = NULL;
    std::vector<float>* jetPhi_jesUp = NULL;
    std::vector<float>* jetPhi_jesDown = NULL;
    std::vector<float>* jetPhi_jerUp = NULL;
    std::vector<float>* jetPhi_jerDown = NULL;
    std::vector<float>* jetPt = NULL;
    std::vector<float>* jetPt_jesUp = NULL;
    std::vector<float>* jetPt_jesDown = NULL;
    std::vector<float>* jetPt_jerUp = NULL;
    std::vector<float>* jetPt_jerDown = NULL;
    std::vector<float>* jetCSVv2 = NULL;
    UInt_t nvtx;
    Float_t Mass;
    Float_t Pt;
    //Float_t Zlep1_Wlep_Mass;
    //Float_t Zlep2_Wlep_Mass;
    //Float_t Eta;
    //Float_t Pt;
    //Float_t ZPt;
    //Float_t ZEta;
    //Float_t ZPhi;
    //Float_t mjj;
    //Float_t mjj_jesUp;
    //Float_t mjj_jesDown;
    //Float_t mjj_jerUp;
    //Float_t mjj_jerDown;
    //Float_t l1Phi;
    //Float_t l2Phi;
    //Float_t l3Phi;
    //Float_t l1Mass;
    //Float_t l2Mass;
    //Float_t l3Mass;
    //Float_t MtToMET;
    //Float_t MtWZ;
    //Float_t M3lMET;
    //Float_t type1_pfMETEt_UncTool;
    //Float_t type1_pfMETEt_jesUp;
    //Float_t type1_pfMETEt_jesDown;
    //Float_t type1_pfMETEt_jerUp;
    //Float_t type1_pfMETEt_jerDown;
    //Float_t type1_pfMETEt_unclusteredEnUp;
    //Float_t type1_pfMETEt_unclusteredEnDown;
    //Float_t l1PtScaleCorrErr;
    //Float_t l2PtScaleCorrErr;
    //Float_t l3PtScaleCorrErr;
    
    //TBranch* b_l3MtToMET;
    //TBranch* b_MtToMET;
    //TBranch* b_pdfWeights;
    //TBranch* b_scaleWeights;
    //TBranch* b_jetCSVv2;
    //TBranch* b_jetPt;
    //TBranch* b_jetEta;
    //TBranch* b_jetEta_jesUp;
    //TBranch* b_jetEta_jesDown;
    //TBranch* b_jetEta_jerUp;
    //TBranch* b_jetEta_jerDown;
    //TBranch* b_jetPhi;
    //TBranch* b_jetPhi_jesUp;
    //TBranch* b_jetPhi_jesDown;
    //TBranch* b_jetPhi_jerUp;
    //TBranch* b_jetPhi_jerDown;
    //TBranch* b_jetPt_jesUp;
    //TBranch* b_jetPt_jesDown;
    //TBranch* b_jetPt_jerUp;
    //TBranch* b_jetPt_jerDown;
    TBranch* b_nvtx;
    TBranch* b_Mass;
    TBranch* b_Pt;
    //TBranch* b_Zlep1_Wlep_Mass;
    //TBranch* b_Zlep2_Wlep_Mass;
    //TBranch* b_Eta;
    //TBranch* b_Pt;
    //TBranch* b_Z1Pt;
    //TBranch* b_ZPhi;
    //TBranch* b_Z1Eta;
    //TBranch* b_mjj;
    //TBranch* b_mjj_jesUp;
    //TBranch* b_mjj_jesDown;
    //TBranch* b_mjj_jerUp;
    //TBranch* b_mjj_jerDown;
    //TBranch* b_l1Phi;
    //TBranch* b_l2Phi;
    //TBranch* b_l3Phi;
    //TBranch* b_l1Mass;
    //TBranch* b_l2Mass;
    //TBranch* b_l3Mass;
    //TBranch* b_l1PtScaleCorrErr;
    //TBranch* b_l2PtScaleCorrErr;
    //TBranch* b_l3PtScaleCorrErr;
    //TBranch* b_type1_pfMETEt_jesUp;
    //TBranch* b_type1_pfMETEt_UncTool;
    //TBranch* b_type1_pfMETEt_jesDown;
    //TBranch* b_type1_pfMETEt_jerUp;
    //TBranch* b_type1_pfMETEt_jerDown;
    //TBranch* b_type1_pfMETEt_unclusteredEnUp;
    //TBranch* b_type1_pfMETEt_unclusteredEnDown;
    int cen_count = 0;
    float count = 0;

    // Readers to access the data (delete the ones you do not need).
    ZZSelector(TTree * /*tree*/ =0) { }
    virtual ~ZZSelector() { }
    virtual void    Init(TTree *tree) override;
    virtual Bool_t  Process(Long64_t entry) override;
    virtual void    SetupNewDirectory() override;

    ClassDefOverride(ZZSelector,0);
protected:
    void LoadBranches(Long64_t entry, std::pair<Systematic, std::string> variation);
    void FillHistograms(Long64_t entry, float weight, bool noBlind,
            std::pair<Systematic, std::string> variation);
    //void FillVBSHistograms(float weight, bool noBlind, 
    //        std::pair<Systematic, std::string> variation);
    //bool PassesBaseSelection(Long64_t entry, bool tightLeps, Selection selection);
    //bool PassesVBSSelection(bool noBlind);
    //bool PassesVBSBackgroundControlSelection();
    //bool PassesFullWZSelection(Long64_t entry);
    void InitialzeHistogram(std::string name, std::vector<std::string> histData);
    //unsigned int GetLheWeightInfo();
    std::vector<std::string> ReadHistData(std::string histDataString);
    std::string getHistName(std::string histName, std::string variationName);
    void SetVariables(Long64_t entry); 
    bool TightZZLeptons();
    bool ZZSelection();
    bool Z4lSelection();
    bool ZSelection();
    bool HZZSIPSelection();
    bool TestMuons();
    void ShiftEfficiencies(Systematic variation);
    //float GetMuonScaleUncertainty(float muEta);
    //void SetShiftedMasses();
    template<typename T, typename... Args>
    void SafeHistFill(std::map<std::string, T*> container, 
            std::string histname, Args... args) {
        if (container[histname] != nullptr)
            container[histname]->Fill(args...);
    };
};

#endif

