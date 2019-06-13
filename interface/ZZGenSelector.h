#ifndef ZZGenSelector_h
#define ZZGenSelector_h

#include <vector>
#include "Analysis/VVAnalysis/interface/ZZGenSelectorBase.h"
#include <TH3.h>

class ZZGenSelector : public ZZGenSelectorBase {
public :
    
    std::map<std::string, TH1D*> hists1D_ = {
        { "Genyield",  {} },
        { "GenZMass",  {} },
        { "GenZ1Mass",  {} },
        { "GenZ2Mass",  {} },
        { "GenZZPt",  {} },
        { "GenZZEta",  {} },
        { "GenZ1Pt",  {} },
        { "GenZ2Pt",  {} },
        { "GenZPt",  {} },
        //{ "GenZ1Phi",  {} },
        //{ "GenZ2Phi",  {} },
        //{"GendPhiZ1Z2",{}},
       // { "ZPt",  {} },
       // { "ZEta",  {} },
       // { "ZPhi",  {} },
       // { "dR_lW_Z",  {} },
        {"GenLepPt",{}},
        //{"GenLepEta",{}},
        //{"GenLep12Pt",{}},
        //{"GenLep12Eta",{}},
        //{"GenLep34Pt",{}},
        //{"GenLep34Eta",{}},
        //{ "GenZ1lep1_Eta",  {} },
        //{ "GenZ1lep1_Phi",  {} },
        //{ "GenZ1lep1_Pt",  {} },
        //{ "GenZ1lep2_Eta",  {} },
        //{ "GenZ1lep2_Phi",  {} },
        //{ "GenZ1lep2_Pt",  {} },
        //{ "GenZ2lep1_Eta",  {} },
        //{ "GenZ2lep1_Phi",  {} },
        //{ "GenZ2lep1_Pt",  {} },
        //{ "GenZ2lep2_Eta",  {} },
        //{ "GenZ2lep2_Phi",  {} },
        //{ "GenZ2lep2_Pt",  {} },
        { "GenMass", {} },
       // { "Pt", {} },
       // { "nJets", {} },
       // { "nJetCSVv2T", {} },
    };

    std::map<std::string, TH2D*> hists2D_ = {
        {"GenZ1Mass_GenZ2Mass",{}},
    };


    Float_t GenMass;
    Float_t GenPt;
    Float_t GenEta;
    float GendPhiZZ; //DeltaPhi between Z1 and Z2
    TBranch* b_GenMass;
    TBranch* b_GenPt;
    TBranch* b_GenEta;

    // Readers to access the data (delete the ones you do not need).
    ZZGenSelector(TTree * /*tree*/ =0) { }
    virtual ~ZZGenSelector() { }
    virtual void    Init(TTree *tree) override;
    virtual Bool_t  Process(Long64_t entry) override;
    virtual void    SetupNewDirectory() override;

    ClassDefOverride(ZZGenSelector,0);
protected:
    void LoadBranches(Long64_t entry);
    void FillHistograms(Long64_t entry,float Genweight);
    
    void InitialzeHistogram(std::string name, std::vector<std::string> histData);
    std::vector<std::string> ReadHistData(std::string histDataString);
    std::string getHistName(std::string histName);
    void SetVariables(Long64_t entry); 
    bool ZZSelection();
    bool Z4lSelection();
    bool ZSelection();
    template<typename T, typename... Args>
    void SafeHistFill(std::map<std::string, T*> container, 
            std::string histname, Args... args) {
        if (container[histname] != nullptr)
            container[histname]->Fill(args...);
    };
};

#endif

