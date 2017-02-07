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
#include <TH2.h>
#include <TParameter.h>

class SelectorBase : public TSelector {
public :
    TH2D* passingTight2D_;
    TH1D* passingTight1DPt_;
    TH1D* passingTight1DEta_;
    TH2D* passingLoose2D_;
    TH1D* passingLoose1DPt_;
    TH1D* passingLoose1DEta_;
    TTreeReaderValue<Float_t> type1_pfMETEt = {fReader, "type1_pfMETEt"};
    TTreeReaderValue<UInt_t> nWWLooseElec = {fReader, "nWWLooseElec"};
    TTreeReaderValue<UInt_t> nWZLooseMuon = {fReader, "nWZLooseMuon"};
    TTreeReaderValue<Float_t> e1PVDXY = {fReader, "e1PVDXY"};
    TTreeReaderValue<Float_t> e1PVDZ = {fReader, "e1PVDZ"};
//    TTreeReaderValue<Bool_t> e1IsCBVIDLoose = {fReader, "e1IsCBVIDLoose"};
//    TTreeReaderValue<Bool_t> e1IsCBVIDMedium = {fReader, "e1IsCBVIDMedium"};
    TTreeReaderValue<Bool_t> e1IsCBVIDTight = {fReader, "e1IsCBVIDTight"};
    TTreeReaderValue<Bool_t> e1IsEB = {fReader, "e1IsEB"};
    TTreeReaderValue<Float_t> e2PVDXY = {fReader, "e2PVDXY"};
    TTreeReaderValue<Float_t> e2PVDZ = {fReader, "e2PVDZ"};
    TTreeReaderValue<Bool_t> e2IsCBVIDTight = {fReader, "e2IsCBVIDTight"};
    TTreeReaderValue<Bool_t> e2IsEB = {fReader, "e2IsEB"};
    TTreeReaderValue<Float_t> e3Eta = {fReader, "e3Eta"};
    TTreeReaderValue<Float_t> e3PVDXY = {fReader, "e3PVDXY"};
    TTreeReaderValue<Float_t> e3PVDZ = {fReader, "e3PVDZ"};
    TTreeReaderValue<Float_t> e3Pt = {fReader, "e3Pt"};
    TTreeReaderValue<Bool_t> e3IsCBVIDTight = {fReader, "e3IsCBVIDTight"};
    TTreeReaderValue<Bool_t> e3IsEB = {fReader, "e3IsEB"};

  // ------------ End of convenience functions

  TTreeReader    fReader;  //!the tree reader
  TTree       *fChain = 0;  //!pointer to the analyzed TTree or TChain
  const char* name_;
  const char* channel_ = "eee";

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
