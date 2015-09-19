#ifndef disambiguateFinalStates_h
#define disambiguateFinalStates_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TEntryList.h>
#include <TTreeFormula.h>
#include <iostream>

class disambiguateFinalStates : public TSelector {
public :
  TTree          *fChain;

  Float_t         Mass;
  Int_t           evt;
  Int_t           run;

  TBranch        *b_Mass;
  TBranch        *b_evt;
  TBranch        *b_run;

  TTreeFormula   *fCutFormula;

  TEntryList     *fBestCandidateEntryList;

  disambiguateFinalStates(TTree * /*tree*/ =0) : fChain(0), fBestCandidateEntryList(0), fCutFormula(0) {
    zCand_name = const_cast<char*>("Mass"); }
  virtual ~disambiguateFinalStates() { SafeDelete(fBestCandidateEntryList); SafeDelete(fCutFormula); }
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
  void setZCandidateBranchName(const char* name) { zCand_name = const_cast<char*>(name); }
  ClassDef(disambiguateFinalStates,0);

private :
  bool  fPostInit;
  Int_t fCurrentRun;
  Int_t fCurrentEvt;
  std::vector<Long64_t> fEntriesToCompare;
  std::vector<Float_t> fEntryDiscriminants;
  char* zCand_name;
};

#endif

#ifdef disambiguateFinalStates_cxx
void disambiguateFinalStates::Init(TTree *tree)
{
  if (!tree) return;
  fChain = tree;
  std::cout << "ZCand_name is: " << zCand_name;
  fChain->SetBranchAddress(zCand_name, &Mass, &b_Mass);
  fChain->SetBranchAddress("evt", &evt, &b_evt);
  fChain->SetBranchAddress("run", &run, &b_run);

  SafeDelete(fCutFormula);
  fCutFormula = new TTreeFormula("CutFormula", fOption, fChain);
  fCutFormula->SetQuickLoad(kTRUE);
  if (!fCutFormula->GetNdim()) { delete fCutFormula; fCutFormula = 0; }

  fPostInit = true;
}

Bool_t disambiguateFinalStates::Notify()
{
  return kTRUE;
}

#endif // #ifdef disambiguateFinalStates_cxx
