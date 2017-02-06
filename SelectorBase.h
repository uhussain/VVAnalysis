// Modified from Nick Smith, U. Wisconsin
#ifndef SelectorBase_h
#define SelectorBase_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TKey.h>
#include <TDirectory.h>
#include <TSelector.h>
#include <TTreeReader.h>
#include <TTreeReaderArray.h>
#include <TH1D.h>
#include <TParameter.h>

class SelectorBase : public TSelector {
public :
  // Flags are read from tuple file and kept up-to-date
  bool isMC_;
  inline bool isRealData() const { return !isMC_; };

  // ------------ End of convenience functions

  TTreeReader    fReader;  //!the tree reader
  TTree       *fChain = 0;  //!pointer to the analyzed TTree or TChain
  const char* name_;

  TTreeReaderValue<Float_t> Mass = {fReader, "Mass"};

  SelectorBase(TTree * /*tree*/ =0) : name_("Unnamed") { }
  SelectorBase(const char* name, TTree * /*tree*/ =0) : name_(name) { }
  virtual ~SelectorBase() { }
  virtual Int_t  Version() const { return 2; }
  virtual void   Begin(TTree *tree);
  virtual void   SlaveBegin(TTree *tree);
  virtual void   Init(TTree *tree);
  virtual Bool_t Notify();
  virtual Bool_t Process(Long64_t entry);
  virtual Int_t  GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
  virtual void   SetOption(const char *option) { fOption = option; }
  virtual void   SetObject(TObject *obj) { fObject = obj; }
  virtual void   SetInputList(TList *input) { fInput = input; }
  virtual TList  *GetOutputList() const { return fOutput; }
  virtual void   SlaveTerminate();
  virtual void   Terminate();

  // Code to handle switching datasets

  // Output directory for current dataset
  TList *currentHistDir_{nullptr};
  // Counter
  TH1D *selectorCounter_{nullptr};
  // We'll collect pointers to objects from derived classes
  // as they are registered with AddObject, and update them to
  // the new object when a dataset is switched
  std::set<TNamed**> allObjects_;

  // Derived classes override (and call) this to register new objects
  // With AddObject<Type>(localPtr, ...);
  virtual void SetupNewDirectory();

  template<typename T, typename... Args>
  void AddObject(T* &ptr, Args... args) {
    static_assert(std::is_base_of<TNamed, T>::value, "Objects must inheirit from ROOT TNamed to be streamable from PROOF sessions");
    ptr = new T(args...);
    ptr->SetDirectory(gROOT);
    currentHistDir_->Add(ptr);
    allObjects_.insert((TNamed**) &ptr);
  };

  void   UpdateDirectory();

  ClassDef(SelectorBase,0);
};

#endif
