#ifndef SelectorBase_h
#define SelectorBase_h

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

class SelectorBase : public TSelector {
public :
    std::map<std::string, ScaleFactor*> scaleFactors;
    TEfficiency* prefireEff_;
    
    TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain
    enum NtupleType {
        UWVV,
        NanoAOD,
    };

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

    std::map<std::string, Selection> selectionMap_ = {
        {"tightleptons", tightleptons},
        {"Wselection", Wselection},
        {"Wselection_Full", Wselection_Full},
        {"FakeRateSelectionLoose", FakeRateSelectionLoose},
        {"FakeRateSelectionTight", FakeRateSelectionTight},
        {"VBSselection_Loose", VBSselection_Loose},
        {"VBSselection_NoZeppenfeld", VBSselection_NoZeppenfeld},
        {"VBSselection_Tight", VBSselection_Tight},
        {"VBSselection_Loose_Full", VBSselection_Loose_Full},
        {"VBSselection_NoZeppenfeld_Full", VBSselection_NoZeppenfeld_Full},
        {"VBSselection_Tight_Full", VBSselection_Tight_Full},
        {"VBSBackgroundControl", VBSBackgroundControl},
        {"VBSBackgroundControlATLAS", VBSBackgroundControlATLAS},
        {"VBSBackgroundControl_Full", VBSBackgroundControl_Full},
        {"VBSBackgroundControlLoose", VBSBackgroundControlLoose},
        {"VBSBackgroundControlLoose_Full", VBSBackgroundControlLoose_Full},
        {"Inclusive2Jet", Inclusive2Jet},
        {"Inclusive2Jet_Full", Inclusive2Jet_Full},
    };

    std::map<std::string, Channel> channelMap_ = {
        {"eee", eee},
        {"eem", eem},
        {"emm", emm},
        {"mmm", mmm},
    };

    TList *currentHistDir_{nullptr};
    TH1D* sumWeightsHist_;

    bool addSumweights_;
    bool applyScaleFactors_;
    bool applyPrefiringCorr_;
    
    // Readers to access the data (delete the ones you do not need).
    SelectorBase(TTree * /*tree*/ =0) { }
    //virtual void    SetScaleFactors() { };
    //virtual void    SetBranches() { };
    virtual ~SelectorBase() { }
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
    ClassDef(SelectorBase,0);

protected:
    std::string name_ = "Unnamed";
    std::string channelName_ = "Unnamed";
    Channel channel_ = Unknown;
    NtupleType ntupleType_ = NanoAOD;
    std::string selectionName_ = "tightleptons";
    Selection selection_ = tightleptons;
    bool isMC_;
    float GetPrefiringEfficiencyWeight(std::vector<float>* jetPt, std::vector<float>* jetEta);
    std::string GetNameFromFile();
};

#endif

