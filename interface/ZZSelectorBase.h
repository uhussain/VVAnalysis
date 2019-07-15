#ifndef ZZSelectorBase_h
#define ZZSelectorBase_h

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
#include "Analysis/VVAnalysis/interface/ScaleFactor.h"

class ZZSelectorBase : public TSelector {
public :
    ScaleFactor* pileupSF_;
    ScaleFactor* eLowRecoSF_;
    ScaleFactor* eRecoSF_;
    ScaleFactor* eIdSF_;
    ScaleFactor* eGapIdSF_;
    ScaleFactor* mIdSF_;
    //ScaleFactor* mIsoSF_;
    
    TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain
    enum Channel {
        eeee,
        eemm,
        mmee,
        mmmm,
        eee,
        eem,
        emm,
        mmm,
        Unknown,
    }; 
    enum Selection {
        tightleptons,
        FakeRateSelectionLoose,
        FakeRateSelectionTight,
    };

    TList *currentHistDir_{nullptr};
    TH1D* sumWeightsHist_;

    bool addSumweights_;
    //bool isVBS_;
    //MC variable to check for duplication(this is a flag to differentiate between channels) 
    UInt_t run;
    UInt_t lumi;
    ULong64_t evt;
    //Int_t duplicated;
    Float_t weight;
    Float_t genWeight;
    Float_t nTruePU;
    Float_t Z1mass;
    Float_t Z2mass; 
    //Float_t Zamass;
    //Float_t Zbmass;
    Float_t Z1pt;
    Float_t Z2pt;
    Float_t Z1Phi;
    Float_t Z2Phi;
    Float_t type1_pfMETEt;
    Float_t type1_pfMETPhi;
    Float_t l1GenPt;
    Float_t l2GenPt;
    Float_t l3GenPt;
    Float_t l4GenPt;

    Bool_t l1IsTight;
    Bool_t l2IsTight;
    Bool_t l3IsTight;
    Bool_t l4IsTight;
    Bool_t l1IsIso;
    Bool_t l2IsIso;
    Bool_t l3IsIso;
    Bool_t l4IsIso;

    Bool_t l1IsGap;
    Bool_t l2IsGap;
    Bool_t l3IsGap;
    Bool_t l4IsGap;
    
    Float_t l1Pt;
    Float_t l2Pt;
    Float_t l3Pt;
    Float_t l4Pt;
    Float_t l1Eta;
    Float_t l2Eta;
    Float_t l3Eta;
    Float_t l4Eta;
    Float_t l1Phi;
    Float_t l2Phi;
    Float_t l3Phi;
    Float_t l4Phi;
    Float_t l1SIP3D;
    Float_t l2SIP3D;
    Float_t l3SIP3D;
    Float_t l4SIP3D;
    Int_t l1PdgId;
    Int_t l2PdgId;
    Int_t l3PdgId;
    Int_t l4PdgId;
    Float_t l1Mass;
    Float_t l2Mass;
    Float_t l3Mass;
    Float_t l4Mass;

    Float_t l1PVDXY;
    Float_t l2PVDXY;
    Float_t l3PVDXY;
    Float_t l4PVDXY;
    Float_t l1PVDZ;
    Float_t l2PVDZ;
    Float_t l3PVDZ;
    Float_t l4PVDZ;
    
    Float_t l3MtToMET;
    
    //TBranch* b_duplicated;
    TBranch* b_genWeight;
    TBranch* b_Z1mass;
    TBranch* b_Z2mass;
    //Smart cut on 4e/4mu 
    //TBranch* b_Zamass;
    //TBranch* b_Zbmass;
    TBranch* b_Z1pt;
    TBranch* b_Z2pt;
    TBranch* b_Z1Phi;
    TBranch* b_Z2Phi;
    TBranch* b_nTruePU;
    TBranch* b_type1_pfMETEt;
    TBranch* b_type1_pfMETPhi;
    TBranch* b_l1GenPt;
    TBranch* b_l2GenPt;
    TBranch* b_l3GenPt;
    TBranch* b_l4GenPt;
    
    TBranch* b_run;
    TBranch* b_lumi;
    TBranch* b_evt;

    TBranch* b_l1IsTight;
    TBranch* b_l2IsTight;
    TBranch* b_l3IsTight;
    TBranch* b_l4IsTight; 
    TBranch* b_l1IsIso;
    TBranch* b_l2IsIso;
    TBranch* b_l3IsIso;
    TBranch* b_l4IsIso;

    TBranch* b_l1IsGap;
    TBranch* b_l2IsGap;
    TBranch* b_l3IsGap;
    TBranch* b_l4IsGap;

    TBranch* b_l1Pt;
    TBranch* b_l2Pt;
    TBranch* b_l3Pt;
    TBranch* b_l4Pt;
    TBranch* b_l1Eta;
    TBranch* b_l2Eta;
    TBranch* b_l3Eta;
    TBranch* b_l4Eta;
    TBranch* b_l1Phi;
    TBranch* b_l2Phi;
    TBranch* b_l3Phi;
    TBranch* b_l4Phi; 
    TBranch* b_l1SIP3D;
    TBranch* b_l2SIP3D;
    TBranch* b_l3SIP3D;
    TBranch* b_l4SIP3D;
    TBranch* b_l1Mass;
    TBranch* b_l2Mass;
    TBranch* b_l3Mass;
    TBranch* b_l4Mass;
    TBranch* b_l1PdgId;
    TBranch* b_l2PdgId;
    TBranch* b_l3PdgId;
    TBranch* b_l4PdgId;
    TBranch* b_l3MtToMET;

    // Readers to access the data (delete the ones you do not need).
    ZZSelectorBase(TTree * /*tree*/ =0) { }
    virtual ~ZZSelectorBase() { }
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

    // We'll collect pointers to objects from derived classes
    // as they are registered with AddObject, and update them to
    // the new object when a dataset is switched
    std::set<TNamed**> allObjects_;
    // Derived classes override (and call) this to register new objects
    // With AddObject<Type>(localPtr, ...);
    virtual void SetupNewDirectory();

    //void AddObject(T* &ptr, const char* name, Args... args) {
        //ptr = (T*) gROOT->FindObject(name);
        //SafeDelete(ptr);
        //ptr = new T(name, args...);
    template<typename T, typename... Args>
    void AddObject(T* &ptr, Args... args) {
        static_assert(std::is_base_of<TNamed, T>::value, "Objects must inheirit from ROOT TNamed to be streamable from PROOF sessions");
        ptr = new T(args...);
        ptr->SetDirectory(gROOT);
        currentHistDir_->Add(ptr);
        allObjects_.insert((TNamed**) &ptr);
    };
    void UpdateDirectory();
    ClassDef(ZZSelectorBase,0);

protected:
    std::vector<std::string> nonprompt3l_ = {
        "tt-lep", "st-schan", "st-tchan-t", "st-tchan-tbar",
        "st-tw", "st-tbarw", "DYm50", "DYm50-1j",
        "DYm50-2j","DYm50-3j","DYm50-4j", "DYm50__LO",
    };

    std::string name_ = "Unnamed";
    std::string channelName_ = "Unnamed";
    Channel channel_ = Unknown;
    std::string selectionName_ = "tightleptons";
    Selection selection_ = tightleptons;
    bool isMC_;
    bool isNonpromptEstimate_;
    bool isNonpromptMC_;
    bool isZgamma_;
    const float FR_MAX_PT_ = 80;
    const float FR_MAX_ETA_ = 2.5;
    const float MuSF_MAX_PT_ = 200;
    const float EleSF_MAX_PT_ = 500;
    bool e1e2IsZ1(Long64_t entry);
    bool lep1IsTight();
    bool lep2IsTight();
    bool lep3IsTight();
    bool lep4IsTight();
    bool Z1PF();
    bool Z1FP();
    bool Z1FF();
    bool Z2PF();
    bool Z2FP();
    bool Z2FF();
    bool tightSIP();
    bool tightZ1Leptons();
    bool tightZ2Leptons();
    //bool IsGenMatched3l();
    std::string GetNameFromFile();
};

#endif

