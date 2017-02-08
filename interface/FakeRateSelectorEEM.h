//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Feb  7 18:52:14 2017 by ROOT version 6.06/01
// from TTree ntuple/ntuple
// found on file: /data/kelong/DibosonAnalysisData/DYControlFakeRate/2017-02-05-data_SingleMuon_Run2016B-23Sep2016-v3-WZxsec2016-DYControlFakeRate-v1/skim-ntuplize_1.root
//////////////////////////////////////////////////////////

#ifndef FakeRateSelectorEEM_h
#define FakeRateSelectorEEM_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TH1.h>
#include <TH2.h>

// Headers needed by this particular selector
#include <vector>

class FakeRateSelectorEEM : public TSelector {
public :
    TTreeReader     fReader;  //!the tree reader
    TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain
    TList* histDir_;
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
    TTreeReaderValue<Bool_t> e1IsCBVIDTight = {fReader, "e1IsCBVIDTight"};
    TTreeReaderValue<Bool_t> e1IsEB = {fReader, "e1IsEB"};
    TTreeReaderValue<Float_t> e2PVDXY = {fReader, "e2PVDXY"};
    TTreeReaderValue<Float_t> e2PVDZ = {fReader, "e2PVDZ"};
    TTreeReaderValue<Bool_t> e2IsCBVIDTight = {fReader, "e2IsCBVIDTight"};
    TTreeReaderValue<Bool_t> e2IsEB = {fReader, "e2IsEB"};
    TTreeReaderValue<Float_t> mPt = {fReader, "mPt"};
    TTreeReaderValue<Float_t> mEta = {fReader, "mEta"};
    TTreeReaderValue<Bool_t> mIsTightMuon = {fReader, "mIsTightMuon"};
    TTreeReaderValue<Float_t> mRelPFIsoDBR04 = {fReader, "mRelPFIsoDBR04"};

   // Readers to access the data (delete the ones you do not need).
   FakeRateSelectorEEM(TTree * /*tree*/ =0) { }
   virtual ~FakeRateSelectorEEM();
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

   ClassDef(FakeRateSelectorEEM,0);

};

#endif
