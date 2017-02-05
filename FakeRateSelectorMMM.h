//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Feb  3 11:12:05 2017 by ROOT version 6.06/01
// from TTree ntuple/ntuple
// found on file: /data/kelong/DibosonAnalysisData/3LooseLeptons/2017-02-03-wz3lnu-powheg-WZxsec2016-3LooseLeptons-v1/skim-ntuplize_1.root
//////////////////////////////////////////////////////////

#ifndef FakeRateSelectorMMM_h
#define FakeRateSelectorMMM_h

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
#include "SelectorBase.h"

class FakeRateSelectorMMM : public SelectorBase {
public :
    TH2D* passingTight2D_;
    TH1D* passingTight1DPt_;
    TH1D* passingTight1DEta_;
    TH2D* passingLoose2D_;
    TH1D* passingLoose1DPt_;
    TH1D* passingLoose1DEta_;
    TTreeReaderValue<Float_t> type1_pfMETEt = {fReader, "type1_pfMETEt"};
    TTreeReaderValue<UInt_t> nWZLooseMuon = {fReader, "nWZLooseMuon"};
    TTreeReaderValue<UInt_t> nWWLooseElec = {fReader, "nWWLooseElec"};
    TTreeReaderValue<Float_t> m1Pt = {fReader, "m1Pt"};
    TTreeReaderValue<Float_t> m1Eta = {fReader, "m1Eta"};
    TTreeReaderValue<Bool_t> m1IsTightMuon = {fReader, "m1IsTightMuon"};
    TTreeReaderValue<Float_t> m1RelPFIsoDBR04 = {fReader, "m1RelPFIsoDBR04"};
    TTreeReaderValue<Float_t> m2Pt = {fReader, "m2Pt"};
    TTreeReaderValue<Float_t> m2Eta = {fReader, "m2Eta"};
    TTreeReaderValue<Bool_t> m2IsTightMuon = {fReader, "m2IsTightMuon"};
    TTreeReaderValue<Float_t> m2RelPFIsoDBR04 = {fReader, "m2RelPFIsoDBR04"};
    TTreeReaderValue<Float_t> m3Pt = {fReader, "m3Pt"};
    TTreeReaderValue<Float_t> m3Eta = {fReader, "m3Eta"};
    TTreeReaderValue<Bool_t> m3IsTightMuon = {fReader, "m3IsTightMuon"};
    TTreeReaderValue<Float_t> m3RelPFIsoDBR04 = {fReader, "m3RelPFIsoDBR04"};

    FakeRateSelectorMMM(TTree * /*tree*/ =0) { }
    FakeRateSelectorMMM(const char* name, TTree * /*tree*/ =0) : SelectorBase(name, 0) { }
    virtual ~FakeRateSelectorMMM() { }
    virtual void   SetupNewDirectory() override;
    virtual Bool_t Process(Long64_t entry) override;

    ClassDefOverride(FakeRateSelectorMMM,0);

};

#endif



