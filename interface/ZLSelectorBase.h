#ifndef ZLSelectorBase_h
#define ZLSelectorBase_h

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
//#include "Analysis/VVAnalysis/interface/ScaleFactor.h"

class ZLSelectorBase : public TSelector {
public :
    //ScaleFactor* pileupSF_;
    //ScaleFactor* eIdSF_;
    //ScaleFactor* eGsfSF_;
    //ScaleFactor* mIdSF_;
    //ScaleFactor* mIsoSF_;
    
    TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain
    enum Channel {
        eee,
        eem,
        emm,
        mmm,
        Unknown,
    }; 
    enum Selection {
        tightleptons,
        Wselection,
        Wselection_Full,
        FakeRateSelectionLoose,
        FakeRateSelectionTight,
        VBSselection_Loose,
        VBSselection_NoZeppenfeld,
        VBSselection_Tight,
        VBSselection_Loose_Full,
        VBSselection_NoZeppenfeld_Full,
        VBSselection_Tight_Full,
        VBSBackgroundControl,
        VBSBackgroundControlATLAS,
        VBSBackgroundControl_Full,
        VBSBackgroundControlLoose,
        VBSBackgroundControlLoose_Full,
        Inclusive2Jet,
        Inclusive2Jet_Full,
    };

    TList *currentHistDir_{nullptr};
    TH1D* sumWeightsHist_;

    bool addSumweights_;
    bool isVBS_;
    Float_t weight;
    Float_t genWeight; 
    Float_t Zmass;
    Float_t type1_pfMETEt;
    Bool_t l1IsTight;
    Bool_t l2IsTight;
    Bool_t l3IsTight; 
    Bool_t l1IsIso;
    Bool_t l2IsIso;
    Bool_t l3IsIso;

    Float_t l3Eta;
    Float_t l1Pt;
    Float_t l2Pt;
    Float_t l3Pt;
    Float_t l3MtToMET;

    TBranch* b_genWeight;
    TBranch* b_Zmass;
    TBranch* b_type1_pfMETEt;
    TBranch* b_l1IsTight;
    TBranch* b_l2IsTight;
    TBranch* b_l3IsTight; 
    TBranch* b_l1IsIso;
    TBranch* b_l2IsIso;
    TBranch* b_l3IsIso;

    TBranch* b_l3Eta;
    TBranch* b_l1Pt;
    TBranch* b_l2Pt;
    TBranch* b_l3Pt;
    TBranch* b_l3MtToMET;

    // Readers to access the data (delete the ones you do not need).
    ZLSelectorBase(TTree * /*tree*/ =0) { }
    virtual ~ZLSelectorBase() { }
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
    ClassDef(ZLSelectorBase,0);

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
    const float FR_MAX_PT_ = 50;
    const float FR_MAX_ETA_ = 2.5;
    bool zlep1IsTight();
    bool zlep2IsTight();
    bool lepton3IsTight();
    bool tightZLeptons();
    //bool IsGenMatched3l();
    std::string GetNameFromFile();
};

#endif

