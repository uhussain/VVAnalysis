#ifndef ZZGenSelectorBase_h
#define ZZGenSelectorBase_h

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

class ZZGenSelectorBase : public TSelector {
public :
    
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

    Float_t Genweight;
    Float_t GenZ1mass;
    Float_t GenZ2mass;
    Float_t GenZ1pt;
    Float_t GenZ2pt;
    Float_t GenZ1phi;
    Float_t GenZ2phi; 
    Float_t GenZ1eta;
    Float_t GenZ1energy;
    Float_t GenZ2eta;
    Float_t GenZ2energy;

    Float_t Genl1Pt;
    Float_t Genl2Pt;
    Float_t Genl3Pt;
    Float_t Genl4Pt;
    Float_t Genl1Eta;
    Float_t Genl2Eta;
    Float_t Genl3Eta;
    Float_t Genl4Eta;
    Float_t Genl1Phi;
    Float_t Genl2Phi;
    Float_t Genl3Phi;
    Float_t Genl4Phi;

    Float_t Genl1Energy; 
    Float_t Genl2Energy;
    Float_t Genl3Energy;
    Float_t Genl4Energy;

    Float_t Genl1Mass; 
    Float_t Genl2Mass;
    Float_t Genl3Mass;
    Float_t Genl4Mass;

    TBranch* b_GenZ1mass;
    TBranch* b_GenZ2mass;
    TBranch* b_GenZ1pt;
    TBranch* b_GenZ2pt;
    TBranch* b_GenZ1phi;
    TBranch* b_GenZ2phi;

    TBranch* b_GenZ1eta;
    TBranch* b_GenZ2eta;
    TBranch* b_GenZ1energy;
    TBranch* b_GenZ2energy;

    TBranch* b_Genl1Pt;
    TBranch* b_Genl2Pt;
    TBranch* b_Genl3Pt;
    TBranch* b_Genl4Pt;
    TBranch* b_Genl1Eta;
    TBranch* b_Genl2Eta;
    TBranch* b_Genl3Eta;
    TBranch* b_Genl4Eta;
    TBranch* b_Genl1Phi;
    TBranch* b_Genl2Phi;
    TBranch* b_Genl3Phi;
    TBranch* b_Genl4Phi; 
    TBranch* b_Genl1Energy;
    TBranch* b_Genl2Energy;
    TBranch* b_Genl3Energy;
    TBranch* b_Genl4Energy;

    TBranch* b_Genl1Mass;
    TBranch* b_Genl2Mass;
    TBranch* b_Genl3Mass;
    TBranch* b_Genl4Mass;
    // Readers to access the data (delete the ones you do not need).
    ZZGenSelectorBase(TTree * /*tree*/ =0) { }
    virtual ~ZZGenSelectorBase() { }
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
    ClassDef(ZZGenSelectorBase,0);

protected:
    std::string name_ = "Unnamed";
    std::string channelName_ = "Unnamed";
    Channel channel_ = Unknown;
    std::string selectionName_ = "tightleptons";
    Selection selection_ = tightleptons;
    bool isMC_;
    const float FR_MAX_PT_ = 80;
    const float FR_MAX_ETA_ = 2.5;
    const float MuSF_MAX_PT_ = 200;
    const float EleSF_MAX_PT_ = 500;
    bool e1e2IsZ1(Long64_t entry);
    std::string GetNameFromFile();
};

#endif

