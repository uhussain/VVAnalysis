#ifndef WZSelector_h
#define WZSelector_h

#include <vector>
#include "Analysis/WZAnalysis/interface/ScaleFactor.h"
#include "Analysis/WZAnalysis/interface/WZSelectorBase.h"

class WZSelector : public WZSelectorBase {
public :
    ScaleFactor* pileupSF_;
    ScaleFactor* eIdSF_;
    ScaleFactor* mIdSF_;
    ScaleFactor* mIsoSF_;
    
    std::map<std::string, TH1D*> hists1D_ = {
        { "type1_pfMETEt",  {} }, 
        { "nTruePU",  {} },
        { "nvtx",  {} },
        { "ZMass",  {} },
        { "Zlep1_Eta",  {} },
        { "Zlep1_Pt",  {} },
        { "Zlep2_Eta",  {} },
        { "Zlep2_Pt",  {} },
        { "Wlep_Eta",  {} },
        { "Wlep_Pt",  {} },
        { "Mass", {} },
        { "mjj",  {} },
        { "dEtajj",  {} }
    };
    std::map<std::string, TH2D*> weighthists_ = {
        { "type1_pfMETEt",  {} }, 
        { "nTruePU",  {} },
        { "nvtx",  {} },
        { "ZMass",  {} },
        { "Zlep1_Eta",  {} },
        { "Zlep1_Pt",  {} },
        { "Zlep2_Eta",  {} },
        { "Zlep2_Pt",  {} },
        { "Wlep_Eta",  {} },
        { "Wlep_Pt",  {} },
        { "Mass", {} },
        { "mjj",  {} },
        { "dEtajj",  {} }
    };

    std::vector<float>* scaleWeights = NULL;
    std::vector<float>* pdfWeights = NULL;
    std::vector<float> lheWeights;
    unsigned int weight_info_;
    float dEtajj;
    std::vector<float>* jetPt = NULL;
    std::vector<float>* jetEta = NULL;
    Float_t type1_pfMETEt;
    Float_t nTruePU;
    UInt_t nvtx;
    Float_t Mass;
    Float_t ZMass;
    Float_t mjj;
    Float_t l1Eta;
    Float_t l1Pt;
    Float_t l2Eta;
    Float_t l2Pt;
    Float_t l3Eta;
    Float_t l3Pt;
    
    TBranch* b_pdfWeights;
    TBranch* b_scaleWeights;
    TBranch* b_jetPt;
    TBranch* b_jetEta;
    TBranch* b_nTruePU;
    TBranch* b_nvtx;
    TBranch* b_Mass;
    TBranch* b_type1_pfMETEt;
    TBranch* b_ZMass;
    TBranch* b_mjj;
    TBranch* b_l1Eta;
    TBranch* b_l1Pt;
    TBranch* b_l2Eta;
    TBranch* b_l2Pt;
    TBranch* b_l3Eta;
    TBranch* b_l3Pt;

    // Readers to access the data (delete the ones you do not need).
    WZSelector(TTree * /*tree*/ =0) { }
    virtual ~WZSelector() { }
    virtual void    Init(TTree *tree) override;
    virtual Bool_t  Process(Long64_t entry) override;
    virtual void    SetupNewDirectory() override;
    virtual void    SlaveBegin(TTree *tree) override;

    ClassDefOverride(WZSelector,0);
protected:
    void LoadBranches(Long64_t entry);
    void FillHistograms(float weight, bool noBlind);
    bool PassesSelection(bool tightLeps);
    unsigned int GetLheWeightInfo();
    std::vector<std::string> ReadHistData(std::string histDataString);
};

#endif

