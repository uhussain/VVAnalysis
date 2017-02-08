//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Feb  8 23:47:12 2017 by ROOT version 6.06/01
// from TTree ntuple/ntuple
// found on file: /data/kelong/DibosonAnalysisData/DYControlFakeRate/2017-02-05-data_MuonEG_Run2016B-23Sep2016-v3-WZxsec2016-DYControlFakeRate-v1/skim-ntuplize_1.root
//////////////////////////////////////////////////////////

#ifndef FakeRateEEMTest_h
#define FakeRateEEMTest_h

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



class FakeRateEEMTest : public TSelector {
public :
   TTreeReader     fReader;  //!the tree reader
   TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain
   // TList* histDir_;
   // TH2D* passingTight2D_;
   // TH1D* passingTight1DPt_;
   // TH1D* passingTight1DEta_;
   // TH2D* passingLoose2D_;
   // TH1D* passingLoose1DPt_;
   // TH1D* passingLoose1DEta_;

   // Readers to access the data (delete the ones you do not need).
   TTreeReaderValue<Float_t> Energy = {fReader, "Energy"};
   TTreeReaderValue<Float_t> Eta = {fReader, "Eta"};
   TTreeReaderValue<Float_t> Mass = {fReader, "Mass"};
   TTreeReaderValue<Float_t> Phi = {fReader, "Phi"};
   TTreeReaderValue<Float_t> Pt = {fReader, "Pt"};
   TTreeReaderValue<Float_t> e1_m_DR = {fReader, "e1_m_DR"};
   TTreeReaderValue<Float_t> e1_m_Mass = {fReader, "e1_m_Mass"};
   TTreeReaderValue<Float_t> e1_m_MassNoFSR = {fReader, "e1_m_MassNoFSR"};
   TTreeReaderValue<Float_t> e2_m_DR = {fReader, "e2_m_DR"};
   TTreeReaderValue<Float_t> e2_m_Mass = {fReader, "e2_m_Mass"};
   TTreeReaderValue<Float_t> e2_m_MassNoFSR = {fReader, "e2_m_MassNoFSR"};
   TTreeReaderValue<Float_t> etajj = {fReader, "etajj"};
   TTreeReaderValue<Float_t> mjj = {fReader, "mjj"};
   TTreeReaderValue<Float_t> phijj = {fReader, "phijj"};
   TTreeReaderValue<Float_t> ptjj = {fReader, "ptjj"};
   TTreeReaderValue<Float_t> pvRho = {fReader, "pvRho"};
   TTreeReaderValue<Float_t> pvZ = {fReader, "pvZ"};
   TTreeReaderValue<Float_t> pvndof = {fReader, "pvndof"};
   TTreeReaderValue<Float_t> rapidity = {fReader, "rapidity"};
   TTreeReaderValue<Float_t> type1_pfMETEt = {fReader, "type1_pfMETEt"};
   TTreeReaderValue<Float_t> type1_pfMETPhi = {fReader, "type1_pfMETPhi"};
   TTreeReaderValue<Bool_t> e1_m_SS = {fReader, "e1_m_SS"};
   TTreeReaderValue<Bool_t> e2_m_SS = {fReader, "e2_m_SS"};
   TTreeReaderValue<Bool_t> pvIdFake = {fReader, "pvIdFake"};
   TTreeReaderValue<Bool_t> pvIsValid = {fReader, "pvIsValid"};
   TTreeReaderValue<Int_t> Charge = {fReader, "Charge"};
   TTreeReaderValue<Int_t> PdgId = {fReader, "PdgId"};
   TTreeReaderValue<UInt_t> lumi = {fReader, "lumi"};
   TTreeReaderValue<UInt_t> nCBVIDLooseElec = {fReader, "nCBVIDLooseElec"};
   TTreeReaderValue<UInt_t> nCBVIDMediumElec = {fReader, "nCBVIDMediumElec"};
   TTreeReaderValue<UInt_t> nCBVIDTightElec = {fReader, "nCBVIDTightElec"};
   TTreeReaderValue<UInt_t> nJetCMVAv2L = {fReader, "nJetCMVAv2L"};
   TTreeReaderValue<UInt_t> nJetCMVAv2M = {fReader, "nJetCMVAv2M"};
   TTreeReaderValue<UInt_t> nJetCMVAv2T = {fReader, "nJetCMVAv2T"};
   TTreeReaderValue<UInt_t> nJetCSVv2L = {fReader, "nJetCSVv2L"};
   TTreeReaderValue<UInt_t> nJetCSVv2M = {fReader, "nJetCSVv2M"};
   TTreeReaderValue<UInt_t> nJetCSVv2T = {fReader, "nJetCSVv2T"};
   TTreeReaderValue<UInt_t> nJetJPL = {fReader, "nJetJPL"};
   TTreeReaderValue<UInt_t> nJetJPM = {fReader, "nJetJPM"};
   TTreeReaderValue<UInt_t> nJetJPT = {fReader, "nJetJPT"};
   TTreeReaderValue<UInt_t> nJets = {fReader, "nJets"};
   TTreeReaderValue<UInt_t> nMediumMuonICHEP = {fReader, "nMediumMuonICHEP"};
   TTreeReaderValue<UInt_t> nTightMuon = {fReader, "nTightMuon"};
   TTreeReaderValue<UInt_t> nWWLooseCBVIDMedElec = {fReader, "nWWLooseCBVIDMedElec"};
   TTreeReaderValue<UInt_t> nWWLooseElec = {fReader, "nWWLooseElec"};
   TTreeReaderValue<UInt_t> nWZLooseMuon = {fReader, "nWZLooseMuon"};
   TTreeReaderValue<UInt_t> nWZMediumMuon = {fReader, "nWZMediumMuon"};
   TTreeReaderValue<UInt_t> nvtx = {fReader, "nvtx"};
   TTreeReaderValue<UInt_t> run = {fReader, "run"};
   TTreeReaderValue<ULong64_t> evt = {fReader, "evt"};
   TTreeReaderArray<float> jetEta = {fReader, "jetEta"};
   TTreeReaderArray<float> jetPhi = {fReader, "jetPhi"};
   TTreeReaderArray<float> jetPt = {fReader, "jetPt"};
   TTreeReaderArray<float> jetRapidity = {fReader, "jetRapidity"};
   TTreeReaderValue<Float_t> e1_e2_DR = {fReader, "e1_e2_DR"};
   TTreeReaderValue<Float_t> e1_e2_Energy = {fReader, "e1_e2_Energy"};
   TTreeReaderValue<Float_t> e1_e2_Eta = {fReader, "e1_e2_Eta"};
   TTreeReaderValue<Float_t> e1_e2_Mass = {fReader, "e1_e2_Mass"};
   TTreeReaderValue<Float_t> e1_e2_Mt = {fReader, "e1_e2_Mt"};
   TTreeReaderValue<Float_t> e1_e2_Phi = {fReader, "e1_e2_Phi"};
   TTreeReaderValue<Float_t> e1_e2_Pt = {fReader, "e1_e2_Pt"};
   TTreeReaderValue<Bool_t> e1_e2_SS = {fReader, "e1_e2_SS"};
   TTreeReaderValue<Int_t> e1_e2_Charge = {fReader, "e1_e2_Charge"};
   TTreeReaderValue<Int_t> e1_e2_PdgId = {fReader, "e1_e2_PdgId"};
   TTreeReaderValue<Float_t> e1EffScaleFactor = {fReader, "e1EffScaleFactor"};
   TTreeReaderValue<Float_t> e1EffScaleFactorError = {fReader, "e1EffScaleFactorError"};
   TTreeReaderValue<Float_t> e1EffectiveArea = {fReader, "e1EffectiveArea"};
   TTreeReaderValue<Float_t> e1Energy = {fReader, "e1Energy"};
   TTreeReaderValue<Float_t> e1Eta = {fReader, "e1Eta"};
   TTreeReaderValue<Float_t> e1IDIsoEffScaleFactor = {fReader, "e1IDIsoEffScaleFactor"};
   TTreeReaderValue<Float_t> e1IDIsoEffScaleFactorError = {fReader, "e1IDIsoEffScaleFactorError"};
   TTreeReaderValue<Float_t> e1MVANonTrigID = {fReader, "e1MVANonTrigID"};
   TTreeReaderValue<Float_t> e1Mass = {fReader, "e1Mass"};
   TTreeReaderValue<Float_t> e1MtToMET = {fReader, "e1MtToMET"};
   TTreeReaderValue<Float_t> e1PFChargedIso = {fReader, "e1PFChargedIso"};
   TTreeReaderValue<Float_t> e1PFNeutralIso = {fReader, "e1PFNeutralIso"};
   TTreeReaderValue<Float_t> e1PFPUIso = {fReader, "e1PFPUIso"};
   TTreeReaderValue<Float_t> e1PFPhotonIso = {fReader, "e1PFPhotonIso"};
   TTreeReaderValue<Float_t> e1PVDXY = {fReader, "e1PVDXY"};
   TTreeReaderValue<Float_t> e1PVDZ = {fReader, "e1PVDZ"};
   TTreeReaderValue<Float_t> e1Phi = {fReader, "e1Phi"};
   TTreeReaderValue<Float_t> e1Pt = {fReader, "e1Pt"};
   TTreeReaderValue<Float_t> e1RelPFIsoRho = {fReader, "e1RelPFIsoRho"};
   TTreeReaderValue<Float_t> e1Rho = {fReader, "e1Rho"};
   TTreeReaderValue<Float_t> e1SCEnergy = {fReader, "e1SCEnergy"};
   TTreeReaderValue<Float_t> e1SCEta = {fReader, "e1SCEta"};
   TTreeReaderValue<Float_t> e1SCPhi = {fReader, "e1SCPhi"};
   TTreeReaderValue<Float_t> e1SCRawEnergy = {fReader, "e1SCRawEnergy"};
   TTreeReaderValue<Float_t> e1SIP3D = {fReader, "e1SIP3D"};
   TTreeReaderValue<Float_t> e1TrkRecoEffScaleFactor = {fReader, "e1TrkRecoEffScaleFactor"};
   TTreeReaderValue<Float_t> e1TrkRecoEffScaleFactorError = {fReader, "e1TrkRecoEffScaleFactorError"};
   TTreeReaderValue<Float_t> e1ZZIso = {fReader, "e1ZZIso"};
   TTreeReaderValue<Bool_t> e1IsCBVIDLoose = {fReader, "e1IsCBVIDLoose"};
   TTreeReaderValue<Bool_t> e1IsCBVIDMedium = {fReader, "e1IsCBVIDMedium"};
   TTreeReaderValue<Bool_t> e1IsCBVIDTight = {fReader, "e1IsCBVIDTight"};
   TTreeReaderValue<Bool_t> e1IsEB = {fReader, "e1IsEB"};
   TTreeReaderValue<Bool_t> e1IsGap = {fReader, "e1IsGap"};
   TTreeReaderValue<Bool_t> e1IsWWLoose = {fReader, "e1IsWWLoose"};
   TTreeReaderValue<Bool_t> e1ZZIsoPass = {fReader, "e1ZZIsoPass"};
   TTreeReaderValue<Bool_t> e1ZZLooseID = {fReader, "e1ZZLooseID"};
   TTreeReaderValue<Bool_t> e1ZZLooseIDNoVtx = {fReader, "e1ZZLooseIDNoVtx"};
   TTreeReaderValue<Bool_t> e1ZZTightID = {fReader, "e1ZZTightID"};
   TTreeReaderValue<Bool_t> e1ZZTightIDNoVtx = {fReader, "e1ZZTightIDNoVtx"};
   TTreeReaderValue<Int_t> e1Charge = {fReader, "e1Charge"};
   TTreeReaderValue<Int_t> e1PdgId = {fReader, "e1PdgId"};
   TTreeReaderValue<UInt_t> e1MissingHits = {fReader, "e1MissingHits"};
   TTreeReaderValue<Float_t> e2EffScaleFactor = {fReader, "e2EffScaleFactor"};
   TTreeReaderValue<Float_t> e2EffScaleFactorError = {fReader, "e2EffScaleFactorError"};
   TTreeReaderValue<Float_t> e2EffectiveArea = {fReader, "e2EffectiveArea"};
   TTreeReaderValue<Float_t> e2Energy = {fReader, "e2Energy"};
   TTreeReaderValue<Float_t> e2Eta = {fReader, "e2Eta"};
   TTreeReaderValue<Float_t> e2IDIsoEffScaleFactor = {fReader, "e2IDIsoEffScaleFactor"};
   TTreeReaderValue<Float_t> e2IDIsoEffScaleFactorError = {fReader, "e2IDIsoEffScaleFactorError"};
   TTreeReaderValue<Float_t> e2MVANonTrigID = {fReader, "e2MVANonTrigID"};
   TTreeReaderValue<Float_t> e2Mass = {fReader, "e2Mass"};
   TTreeReaderValue<Float_t> e2MtToMET = {fReader, "e2MtToMET"};
   TTreeReaderValue<Float_t> e2PFChargedIso = {fReader, "e2PFChargedIso"};
   TTreeReaderValue<Float_t> e2PFNeutralIso = {fReader, "e2PFNeutralIso"};
   TTreeReaderValue<Float_t> e2PFPUIso = {fReader, "e2PFPUIso"};
   TTreeReaderValue<Float_t> e2PFPhotonIso = {fReader, "e2PFPhotonIso"};
   TTreeReaderValue<Float_t> e2PVDXY = {fReader, "e2PVDXY"};
   TTreeReaderValue<Float_t> e2PVDZ = {fReader, "e2PVDZ"};
   TTreeReaderValue<Float_t> e2Phi = {fReader, "e2Phi"};
   TTreeReaderValue<Float_t> e2Pt = {fReader, "e2Pt"};
   TTreeReaderValue<Float_t> e2RelPFIsoRho = {fReader, "e2RelPFIsoRho"};
   TTreeReaderValue<Float_t> e2Rho = {fReader, "e2Rho"};
   TTreeReaderValue<Float_t> e2SCEnergy = {fReader, "e2SCEnergy"};
   TTreeReaderValue<Float_t> e2SCEta = {fReader, "e2SCEta"};
   TTreeReaderValue<Float_t> e2SCPhi = {fReader, "e2SCPhi"};
   TTreeReaderValue<Float_t> e2SCRawEnergy = {fReader, "e2SCRawEnergy"};
   TTreeReaderValue<Float_t> e2SIP3D = {fReader, "e2SIP3D"};
   TTreeReaderValue<Float_t> e2TrkRecoEffScaleFactor = {fReader, "e2TrkRecoEffScaleFactor"};
   TTreeReaderValue<Float_t> e2TrkRecoEffScaleFactorError = {fReader, "e2TrkRecoEffScaleFactorError"};
   TTreeReaderValue<Float_t> e2ZZIso = {fReader, "e2ZZIso"};
   TTreeReaderValue<Bool_t> e2IsCBVIDLoose = {fReader, "e2IsCBVIDLoose"};
   TTreeReaderValue<Bool_t> e2IsCBVIDMedium = {fReader, "e2IsCBVIDMedium"};
   TTreeReaderValue<Bool_t> e2IsCBVIDTight = {fReader, "e2IsCBVIDTight"};
   TTreeReaderValue<Bool_t> e2IsEB = {fReader, "e2IsEB"};
   TTreeReaderValue<Bool_t> e2IsGap = {fReader, "e2IsGap"};
   TTreeReaderValue<Bool_t> e2IsWWLoose = {fReader, "e2IsWWLoose"};
   TTreeReaderValue<Bool_t> e2ZZIsoPass = {fReader, "e2ZZIsoPass"};
   TTreeReaderValue<Bool_t> e2ZZLooseID = {fReader, "e2ZZLooseID"};
   TTreeReaderValue<Bool_t> e2ZZLooseIDNoVtx = {fReader, "e2ZZLooseIDNoVtx"};
   TTreeReaderValue<Bool_t> e2ZZTightID = {fReader, "e2ZZTightID"};
   TTreeReaderValue<Bool_t> e2ZZTightIDNoVtx = {fReader, "e2ZZTightIDNoVtx"};
   TTreeReaderValue<Int_t> e2Charge = {fReader, "e2Charge"};
   TTreeReaderValue<Int_t> e2PdgId = {fReader, "e2PdgId"};
   TTreeReaderValue<UInt_t> e2MissingHits = {fReader, "e2MissingHits"};
   TTreeReaderValue<Float_t> mEffScaleFactor = {fReader, "mEffScaleFactor"};
   TTreeReaderValue<Float_t> mEffScaleFactorError = {fReader, "mEffScaleFactorError"};
   TTreeReaderValue<Float_t> mEnergy = {fReader, "mEnergy"};
   TTreeReaderValue<Float_t> mEta = {fReader, "mEta"};
   TTreeReaderValue<Float_t> mMass = {fReader, "mMass"};
   TTreeReaderValue<Float_t> mMtToMET = {fReader, "mMtToMET"};
   TTreeReaderValue<Float_t> mPFChargedIso = {fReader, "mPFChargedIso"};
   TTreeReaderValue<Float_t> mPFNeutralIso = {fReader, "mPFNeutralIso"};
   TTreeReaderValue<Float_t> mPFPUIso = {fReader, "mPFPUIso"};
   TTreeReaderValue<Float_t> mPFPhotonIso = {fReader, "mPFPhotonIso"};
   TTreeReaderValue<Float_t> mPVDXY = {fReader, "mPVDXY"};
   TTreeReaderValue<Float_t> mPVDZ = {fReader, "mPVDZ"};
   TTreeReaderValue<Float_t> mPhi = {fReader, "mPhi"};
   TTreeReaderValue<Float_t> mPt = {fReader, "mPt"};
   TTreeReaderValue<Float_t> mPtErr = {fReader, "mPtErr"};
   TTreeReaderValue<Float_t> mPtErrUncorrected = {fReader, "mPtErrUncorrected"};
   TTreeReaderValue<Float_t> mPtUncorrected = {fReader, "mPtUncorrected"};
   TTreeReaderValue<Float_t> mRelPFIsoDBR03 = {fReader, "mRelPFIsoDBR03"};
   TTreeReaderValue<Float_t> mRelPFIsoDBR04 = {fReader, "mRelPFIsoDBR04"};
   TTreeReaderValue<Float_t> mSIP3D = {fReader, "mSIP3D"};
   TTreeReaderValue<Float_t> mTrackIso = {fReader, "mTrackIso"};
   TTreeReaderValue<Float_t> mZZIso = {fReader, "mZZIso"};
   TTreeReaderValue<Bool_t> mHighPtID = {fReader, "mHighPtID"};
   TTreeReaderValue<Bool_t> mHighPtIDNoVtx = {fReader, "mHighPtIDNoVtx"};
   TTreeReaderValue<Bool_t> mIsGlobal = {fReader, "mIsGlobal"};
   TTreeReaderValue<Bool_t> mIsLooseMuon = {fReader, "mIsLooseMuon"};
   TTreeReaderValue<Bool_t> mIsMediumMuon = {fReader, "mIsMediumMuon"};
   TTreeReaderValue<Bool_t> mIsMediumMuonICHEP = {fReader, "mIsMediumMuonICHEP"};
   TTreeReaderValue<Bool_t> mIsPFMuon = {fReader, "mIsPFMuon"};
   TTreeReaderValue<Bool_t> mIsTightMuon = {fReader, "mIsTightMuon"};
   TTreeReaderValue<Bool_t> mIsTracker = {fReader, "mIsTracker"};
   TTreeReaderValue<Bool_t> mIsWZLooseMuon = {fReader, "mIsWZLooseMuon"};
   TTreeReaderValue<Bool_t> mIsWZLooseMuonNoIso = {fReader, "mIsWZLooseMuonNoIso"};
   TTreeReaderValue<Bool_t> mIsWZMediumMuon = {fReader, "mIsWZMediumMuon"};
   TTreeReaderValue<Bool_t> mIsWZMediumMuonNoIso = {fReader, "mIsWZMediumMuonNoIso"};
   TTreeReaderValue<Bool_t> mPFID = {fReader, "mPFID"};
   TTreeReaderValue<Bool_t> mPFIDNoVtx = {fReader, "mPFIDNoVtx"};
   TTreeReaderValue<Bool_t> mZZIsoPass = {fReader, "mZZIsoPass"};
   TTreeReaderValue<Bool_t> mZZLooseID = {fReader, "mZZLooseID"};
   TTreeReaderValue<Bool_t> mZZLooseIDNoVtx = {fReader, "mZZLooseIDNoVtx"};
   TTreeReaderValue<Bool_t> mZZTightID = {fReader, "mZZTightID"};
   TTreeReaderValue<Bool_t> mZZTightIDNoVtx = {fReader, "mZZTightIDNoVtx"};
   TTreeReaderValue<Int_t> mCharge = {fReader, "mCharge"};
   TTreeReaderValue<Int_t> mPdgId = {fReader, "mPdgId"};
   TTreeReaderValue<UInt_t> mBestTrackType = {fReader, "mBestTrackType"};
   TTreeReaderValue<UInt_t> mMatchedStations = {fReader, "mMatchedStations"};
   TTreeReaderValue<Bool_t> doubleMuPass = {fReader, "doubleMuPass"};
   TTreeReaderValue<Bool_t> doubleMuDZPass = {fReader, "doubleMuDZPass"};
   TTreeReaderValue<Bool_t> doubleEPass = {fReader, "doubleEPass"};
   TTreeReaderValue<Bool_t> singleESingleMuPass = {fReader, "singleESingleMuPass"};
   TTreeReaderValue<Bool_t> singleMuSingleEPass = {fReader, "singleMuSingleEPass"};
   TTreeReaderValue<Bool_t> tripleEPass = {fReader, "tripleEPass"};
   TTreeReaderValue<Bool_t> doubleESingleMuPass = {fReader, "doubleESingleMuPass"};
   TTreeReaderValue<Bool_t> doubleMuSingleEPass = {fReader, "doubleMuSingleEPass"};
   TTreeReaderValue<Bool_t> tripleMuPass = {fReader, "tripleMuPass"};
   TTreeReaderValue<Bool_t> singleEPass = {fReader, "singleEPass"};
   TTreeReaderValue<Bool_t> singleIsoMuPass = {fReader, "singleIsoMuPass"};
   TTreeReaderValue<Bool_t> singleIsoMu20Pass = {fReader, "singleIsoMu20Pass"};
   TTreeReaderValue<Bool_t> singleMuPass = {fReader, "singleMuPass"};


   FakeRateEEMTest(TTree * /*tree*/ =0) { }
   virtual ~FakeRateEEMTest() { }
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

   ClassDef(FakeRateEEMTest,0);

};

#endif

#ifdef FakeRateEEMTest_cxx
void FakeRateEEMTest::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the reader is initialized.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   fReader.SetTree(tree);
}

Bool_t FakeRateEEMTest::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}


#endif // #ifdef FakeRateEEMTest_cxx
