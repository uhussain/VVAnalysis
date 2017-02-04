//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Feb  3 11:12:05 2017 by ROOT version 6.06/01
// from TTree ntuple/ntuple
// found on file: /data/kelong/DibosonAnalysisData/3LooseLeptons/2017-02-03-wz3lnu-powheg-WZxsec2016-3LooseLeptons-v1/skim-ntuplize_1.root
//////////////////////////////////////////////////////////

#ifndef FakeRateSelector_h
#define FakeRateSelector_h

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

class FakeRateSelector : public SelectorBase {
public :
    TH2D* passingTight2D_;
    TH1D* passingTight1DPt_;
    TH1D* passingTight1DEta_;
    TH2D* passingLoose2D_;
    TH1D* passingLoose1DPt_;
    TH1D* passingLoose1DEta_;
    // Readers to access the data (delete the ones you do not need).
//    TTreeReaderValue<Float_t> Energy = {fReader, "Energy"};
//    TTreeReaderValue<Float_t> Eta = {fReader, "Eta"};
    TTreeReaderValue<Float_t> Mass = {fReader, "Mass"};
//    TTreeReaderValue<Float_t> Phi = {fReader, "Phi"};
//    TTreeReaderValue<Float_t> Pt = {fReader, "Pt"};
//    TTreeReaderValue<Float_t> e1_e3_DR = {fReader, "e1_e3_DR"};
//    TTreeReaderValue<Float_t> e1_e3_Mass = {fReader, "e1_e3_Mass"};
//    TTreeReaderValue<Float_t> e1_e3_MassNoFSR = {fReader, "e1_e3_MassNoFSR"};
//    TTreeReaderValue<Float_t> e2_e3_DR = {fReader, "e2_e3_DR"};
//    TTreeReaderValue<Float_t> e2_e3_Mass = {fReader, "e2_e3_Mass"};
//    TTreeReaderValue<Float_t> e2_e3_MassNoFSR = {fReader, "e2_e3_MassNoFSR"};
//    TTreeReaderValue<Float_t> etajj = {fReader, "etajj"};
//    TTreeReaderValue<Float_t> etajj_jerDown = {fReader, "etajj_jerDown"};
//    TTreeReaderValue<Float_t> etajj_jerUp = {fReader, "etajj_jerUp"};
//    TTreeReaderValue<Float_t> etajj_jesDown = {fReader, "etajj_jesDown"};
//    TTreeReaderValue<Float_t> etajj_jesUp = {fReader, "etajj_jesUp"};
//    TTreeReaderValue<Float_t> genWeight = {fReader, "genWeight"};
//    TTreeReaderValue<Float_t> maxScaleWeight = {fReader, "maxScaleWeight"};
//    TTreeReaderValue<Float_t> minScaleWeight = {fReader, "minScaleWeight"};
//    TTreeReaderValue<Float_t> mjj = {fReader, "mjj"};
//    TTreeReaderValue<Float_t> mjj_jerDown = {fReader, "mjj_jerDown"};
//    TTreeReaderValue<Float_t> mjj_jerUp = {fReader, "mjj_jerUp"};
//    TTreeReaderValue<Float_t> mjj_jesDown = {fReader, "mjj_jesDown"};
//    TTreeReaderValue<Float_t> mjj_jesUp = {fReader, "mjj_jesUp"};
//    TTreeReaderValue<Float_t> nTruePU = {fReader, "nTruePU"};
//    TTreeReaderValue<Float_t> phijj = {fReader, "phijj"};
//    TTreeReaderValue<Float_t> phijj_jerDown = {fReader, "phijj_jerDown"};
//    TTreeReaderValue<Float_t> phijj_jerUp = {fReader, "phijj_jerUp"};
//    TTreeReaderValue<Float_t> phijj_jesDown = {fReader, "phijj_jesDown"};
//    TTreeReaderValue<Float_t> phijj_jesUp = {fReader, "phijj_jesUp"};
//    TTreeReaderValue<Float_t> ptjj = {fReader, "ptjj"};
//    TTreeReaderValue<Float_t> ptjj_jerDown = {fReader, "ptjj_jerDown"};
//    TTreeReaderValue<Float_t> ptjj_jerUp = {fReader, "ptjj_jerUp"};
//    TTreeReaderValue<Float_t> ptjj_jesDown = {fReader, "ptjj_jesDown"};
//    TTreeReaderValue<Float_t> ptjj_jesUp = {fReader, "ptjj_jesUp"};
//    TTreeReaderValue<Float_t> pvRho = {fReader, "pvRho"};
//    TTreeReaderValue<Float_t> pvZ = {fReader, "pvZ"};
//    TTreeReaderValue<Float_t> pvndof = {fReader, "pvndof"};
//    TTreeReaderValue<Float_t> rapidity = {fReader, "rapidity"};
//    TTreeReaderValue<Float_t> type1_pfMETEt = {fReader, "type1_pfMETEt"};
//    TTreeReaderValue<Float_t> type1_pfMETPhi = {fReader, "type1_pfMETPhi"};
//    TTreeReaderValue<Bool_t> e1_e3_SS = {fReader, "e1_e3_SS"};
//    TTreeReaderValue<Bool_t> e2_e3_SS = {fReader, "e2_e3_SS"};
//    TTreeReaderValue<Bool_t> pvIdFake = {fReader, "pvIdFake"};
//    TTreeReaderValue<Bool_t> pvIsValid = {fReader, "pvIsValid"};
//    TTreeReaderValue<Int_t> Charge = {fReader, "Charge"};
//    TTreeReaderValue<Int_t> PdgId = {fReader, "PdgId"};
//    TTreeReaderValue<UInt_t> lumi = {fReader, "lumi"};
//    TTreeReaderValue<UInt_t> nCBVIDLooseElec = {fReader, "nCBVIDLooseElec"};
    TTreeReaderValue<UInt_t> nCBVIDMediumElec = {fReader, "nCBVIDMediumElec"};
//    TTreeReaderValue<UInt_t> nCBVIDTightElec = {fReader, "nCBVIDTightElec"};
//    TTreeReaderValue<UInt_t> nJetCMVAv2L = {fReader, "nJetCMVAv2L"};
//    TTreeReaderValue<UInt_t> nJetCMVAv2M = {fReader, "nJetCMVAv2M"};
//    TTreeReaderValue<UInt_t> nJetCMVAv2T = {fReader, "nJetCMVAv2T"};
//    TTreeReaderValue<UInt_t> nJetCSVv2L = {fReader, "nJetCSVv2L"};
//    TTreeReaderValue<UInt_t> nJetCSVv2M = {fReader, "nJetCSVv2M"};
//    TTreeReaderValue<UInt_t> nJetCSVv2T = {fReader, "nJetCSVv2T"};
//    TTreeReaderValue<UInt_t> nJetJPL = {fReader, "nJetJPL"};
//    TTreeReaderValue<UInt_t> nJetJPM = {fReader, "nJetJPM"};
//    TTreeReaderValue<UInt_t> nJetJPT = {fReader, "nJetJPT"};
//    TTreeReaderValue<UInt_t> nJets = {fReader, "nJets"};
//    TTreeReaderValue<UInt_t> nJets_jerDown = {fReader, "nJets_jerDown"};
//    TTreeReaderValue<UInt_t> nJets_jerUp = {fReader, "nJets_jerUp"};
//    TTreeReaderValue<UInt_t> nJets_jesDown = {fReader, "nJets_jesDown"};
//    TTreeReaderValue<UInt_t> nJets_jesUp = {fReader, "nJets_jesUp"};
//    TTreeReaderValue<UInt_t> nMediumMuonICHEP = {fReader, "nMediumMuonICHEP"};
//    TTreeReaderValue<UInt_t> nTightMuon = {fReader, "nTightMuon"};
//    TTreeReaderValue<UInt_t> nWWLooseCBVIDMedElec = {fReader, "nWWLooseCBVIDMedElec"};
    TTreeReaderValue<UInt_t> nWWLooseElec = {fReader, "nWWLooseElec"};
    TTreeReaderValue<UInt_t> nWZLooseMuon = {fReader, "nWZLooseMuon"};
//    TTreeReaderValue<UInt_t> nWZMediumMuon = {fReader, "nWZMediumMuon"};
//    TTreeReaderValue<UInt_t> nvtx = {fReader, "nvtx"};
//    TTreeReaderValue<UInt_t> run = {fReader, "run"};
//    TTreeReaderValue<ULong64_t> evt = {fReader, "evt"};
//    TTreeReaderArray<float> jetEta = {fReader, "jetEta"};
//    TTreeReaderArray<float> jetEta_jerDown = {fReader, "jetEta_jerDown"};
//    TTreeReaderArray<float> jetEta_jerUp = {fReader, "jetEta_jerUp"};
//    TTreeReaderArray<float> jetEta_jesDown = {fReader, "jetEta_jesDown"};
//    TTreeReaderArray<float> jetEta_jesUp = {fReader, "jetEta_jesUp"};
//    TTreeReaderArray<float> jetPhi = {fReader, "jetPhi"};
//    TTreeReaderArray<float> jetPt = {fReader, "jetPt"};
//    TTreeReaderArray<float> jetPt_jerDown = {fReader, "jetPt_jerDown"};
//    TTreeReaderArray<float> jetPt_jerUp = {fReader, "jetPt_jerUp"};
//    TTreeReaderArray<float> jetPt_jesDown = {fReader, "jetPt_jesDown"};
//    TTreeReaderArray<float> jetPt_jesUp = {fReader, "jetPt_jesUp"};
//    TTreeReaderArray<float> jetRapidity = {fReader, "jetRapidity"};
//    TTreeReaderArray<float> jetRapidity_jerDown = {fReader, "jetRapidity_jerDown"};
//    TTreeReaderArray<float> jetRapidity_jerUp = {fReader, "jetRapidity_jerUp"};
//    TTreeReaderArray<float> jetRapidity_jesDown = {fReader, "jetRapidity_jesDown"};
//    TTreeReaderArray<float> jetRapidity_jesUp = {fReader, "jetRapidity_jesUp"};
//    TTreeReaderArray<float> pdfWeights = {fReader, "pdfWeights"};
//    TTreeReaderArray<float> scaleWeights = {fReader, "scaleWeights"};
//    TTreeReaderValue<Float_t> e1_e2_DR = {fReader, "e1_e2_DR"};
//    TTreeReaderValue<Float_t> e1_e2_Energy = {fReader, "e1_e2_Energy"};
//    TTreeReaderValue<Float_t> e1_e2_Eta = {fReader, "e1_e2_Eta"};
//    TTreeReaderValue<Float_t> e1_e2_Mass = {fReader, "e1_e2_Mass"};
//    TTreeReaderValue<Float_t> e1_e2_Mt = {fReader, "e1_e2_Mt"};
//    TTreeReaderValue<Float_t> e1_e2_Phi = {fReader, "e1_e2_Phi"};
//    TTreeReaderValue<Float_t> e1_e2_Pt = {fReader, "e1_e2_Pt"};
//    TTreeReaderValue<Bool_t> e1_e2_SS = {fReader, "e1_e2_SS"};
//    TTreeReaderValue<Int_t> e1_e2_Charge = {fReader, "e1_e2_Charge"};
//    TTreeReaderValue<Int_t> e1_e2_PdgId = {fReader, "e1_e2_PdgId"};
//    TTreeReaderValue<Float_t> e1EffScaleFactor = {fReader, "e1EffScaleFactor"};
//    TTreeReaderValue<Float_t> e1EffScaleFactorError = {fReader, "e1EffScaleFactorError"};
//    TTreeReaderValue<Float_t> e1EffectiveArea = {fReader, "e1EffectiveArea"};
//    TTreeReaderValue<Float_t> e1Energy = {fReader, "e1Energy"};
//    TTreeReaderValue<Float_t> e1Eta = {fReader, "e1Eta"};
//    TTreeReaderValue<Float_t> e1GenEta = {fReader, "e1GenEta"};
//    TTreeReaderValue<Float_t> e1GenPhi = {fReader, "e1GenPhi"};
//    TTreeReaderValue<Float_t> e1GenPt = {fReader, "e1GenPt"};
//    TTreeReaderValue<Float_t> e1IDIsoEffScaleFactor = {fReader, "e1IDIsoEffScaleFactor"};
//    TTreeReaderValue<Float_t> e1IDIsoEffScaleFactorError = {fReader, "e1IDIsoEffScaleFactorError"};
//    TTreeReaderValue<Float_t> e1MVANonTrigID = {fReader, "e1MVANonTrigID"};
//    TTreeReaderValue<Float_t> e1Mass = {fReader, "e1Mass"};
//    TTreeReaderValue<Float_t> e1MtToMET = {fReader, "e1MtToMET"};
//    TTreeReaderValue<Float_t> e1PFChargedIso = {fReader, "e1PFChargedIso"};
//    TTreeReaderValue<Float_t> e1PFNeutralIso = {fReader, "e1PFNeutralIso"};
//    TTreeReaderValue<Float_t> e1PFPUIso = {fReader, "e1PFPUIso"};
//    TTreeReaderValue<Float_t> e1PFPhotonIso = {fReader, "e1PFPhotonIso"};
    TTreeReaderValue<Float_t> e1PVDXY = {fReader, "e1PVDXY"};
    TTreeReaderValue<Float_t> e1PVDZ = {fReader, "e1PVDZ"};
//    TTreeReaderValue<Float_t> e1Phi = {fReader, "e1Phi"};
//    TTreeReaderValue<Float_t> e1Pt = {fReader, "e1Pt"};
//    TTreeReaderValue<Float_t> e1RelPFIsoRho = {fReader, "e1RelPFIsoRho"};
//    TTreeReaderValue<Float_t> e1Rho = {fReader, "e1Rho"};
//    TTreeReaderValue<Float_t> e1SCEnergy = {fReader, "e1SCEnergy"};
//    TTreeReaderValue<Float_t> e1SCEta = {fReader, "e1SCEta"};
//    TTreeReaderValue<Float_t> e1SCPhi = {fReader, "e1SCPhi"};
//    TTreeReaderValue<Float_t> e1SCRawEnergy = {fReader, "e1SCRawEnergy"};
//    TTreeReaderValue<Float_t> e1SIP3D = {fReader, "e1SIP3D"};
//    TTreeReaderValue<Float_t> e1TrkRecoEffScaleFactor = {fReader, "e1TrkRecoEffScaleFactor"};
//    TTreeReaderValue<Float_t> e1TrkRecoEffScaleFactorError = {fReader, "e1TrkRecoEffScaleFactorError"};
//    TTreeReaderValue<Float_t> e1ZZIso = {fReader, "e1ZZIso"};
//    TTreeReaderValue<Bool_t> e1IsCBVIDLoose = {fReader, "e1IsCBVIDLoose"};
//    TTreeReaderValue<Bool_t> e1IsCBVIDMedium = {fReader, "e1IsCBVIDMedium"};
    TTreeReaderValue<Bool_t> e1IsCBVIDTight = {fReader, "e1IsCBVIDTight"};
    TTreeReaderValue<Bool_t> e1IsEB = {fReader, "e1IsEB"};
//    TTreeReaderValue<Bool_t> e1IsGap = {fReader, "e1IsGap"};
//    TTreeReaderValue<Bool_t> e1IsWWLoose = {fReader, "e1IsWWLoose"};
//    TTreeReaderValue<Bool_t> e1ZZIsoPass = {fReader, "e1ZZIsoPass"};
//    TTreeReaderValue<Bool_t> e1ZZLooseID = {fReader, "e1ZZLooseID"};
//    TTreeReaderValue<Bool_t> e1ZZLooseIDNoVtx = {fReader, "e1ZZLooseIDNoVtx"};
//    TTreeReaderValue<Bool_t> e1ZZTightID = {fReader, "e1ZZTightID"};
//    TTreeReaderValue<Bool_t> e1ZZTightIDNoVtx = {fReader, "e1ZZTightIDNoVtx"};
//    TTreeReaderValue<Int_t> e1Charge = {fReader, "e1Charge"};
//    TTreeReaderValue<Int_t> e1GenCharge = {fReader, "e1GenCharge"};
//    TTreeReaderValue<Int_t> e1GenPdgId = {fReader, "e1GenPdgId"};
//    TTreeReaderValue<Int_t> e1PdgId = {fReader, "e1PdgId"};
//    TTreeReaderValue<UInt_t> e1MissingHits = {fReader, "e1MissingHits"};
//    TTreeReaderValue<Float_t> e2EffScaleFactor = {fReader, "e2EffScaleFactor"};
//    TTreeReaderValue<Float_t> e2EffScaleFactorError = {fReader, "e2EffScaleFactorError"};
//    TTreeReaderValue<Float_t> e2EffectiveArea = {fReader, "e2EffectiveArea"};
//    TTreeReaderValue<Float_t> e2Energy = {fReader, "e2Energy"};
//    TTreeReaderValue<Float_t> e2Eta = {fReader, "e2Eta"};
//    TTreeReaderValue<Float_t> e2GenEta = {fReader, "e2GenEta"};
//    TTreeReaderValue<Float_t> e2GenPhi = {fReader, "e2GenPhi"};
//    TTreeReaderValue<Float_t> e2GenPt = {fReader, "e2GenPt"};
//    TTreeReaderValue<Float_t> e2IDIsoEffScaleFactor = {fReader, "e2IDIsoEffScaleFactor"};
//    TTreeReaderValue<Float_t> e2IDIsoEffScaleFactorError = {fReader, "e2IDIsoEffScaleFactorError"};
//    TTreeReaderValue<Float_t> e2MVANonTrigID = {fReader, "e2MVANonTrigID"};
//    TTreeReaderValue<Float_t> e2Mass = {fReader, "e2Mass"};
//    TTreeReaderValue<Float_t> e2MtToMET = {fReader, "e2MtToMET"};
//    TTreeReaderValue<Float_t> e2PFChargedIso = {fReader, "e2PFChargedIso"};
//    TTreeReaderValue<Float_t> e2PFNeutralIso = {fReader, "e2PFNeutralIso"};
//    TTreeReaderValue<Float_t> e2PFPUIso = {fReader, "e2PFPUIso"};
//    TTreeReaderValue<Float_t> e2PFPhotonIso = {fReader, "e2PFPhotonIso"};
    TTreeReaderValue<Float_t> e2PVDXY = {fReader, "e2PVDXY"};
    TTreeReaderValue<Float_t> e2PVDZ = {fReader, "e2PVDZ"};
//    TTreeReaderValue<Float_t> e2Phi = {fReader, "e2Phi"};
//    TTreeReaderValue<Float_t> e2Pt = {fReader, "e2Pt"};
//    TTreeReaderValue<Float_t> e2RelPFIsoRho = {fReader, "e2RelPFIsoRho"};
//    TTreeReaderValue<Float_t> e2Rho = {fReader, "e2Rho"};
//    TTreeReaderValue<Float_t> e2SCEnergy = {fReader, "e2SCEnergy"};
//    TTreeReaderValue<Float_t> e2SCEta = {fReader, "e2SCEta"};
//    TTreeReaderValue<Float_t> e2SCPhi = {fReader, "e2SCPhi"};
//    TTreeReaderValue<Float_t> e2SCRawEnergy = {fReader, "e2SCRawEnergy"};
//    TTreeReaderValue<Float_t> e2SIP3D = {fReader, "e2SIP3D"};
//    TTreeReaderValue<Float_t> e2TrkRecoEffScaleFactor = {fReader, "e2TrkRecoEffScaleFactor"};
//    TTreeReaderValue<Float_t> e2TrkRecoEffScaleFactorError = {fReader, "e2TrkRecoEffScaleFactorError"};
//    TTreeReaderValue<Float_t> e2ZZIso = {fReader, "e2ZZIso"};
//    TTreeReaderValue<Bool_t> e2IsCBVIDLoose = {fReader, "e2IsCBVIDLoose"};
//    TTreeReaderValue<Bool_t> e2IsCBVIDMedium = {fReader, "e2IsCBVIDMedium"};
    TTreeReaderValue<Bool_t> e2IsCBVIDTight = {fReader, "e2IsCBVIDTight"};
    TTreeReaderValue<Bool_t> e2IsEB = {fReader, "e2IsEB"};
//    TTreeReaderValue<Bool_t> e2IsGap = {fReader, "e2IsGap"};
//    TTreeReaderValue<Bool_t> e2IsWWLoose = {fReader, "e2IsWWLoose"};
//    TTreeReaderValue<Bool_t> e2ZZIsoPass = {fReader, "e2ZZIsoPass"};
//    TTreeReaderValue<Bool_t> e2ZZLooseID = {fReader, "e2ZZLooseID"};
//    TTreeReaderValue<Bool_t> e2ZZLooseIDNoVtx = {fReader, "e2ZZLooseIDNoVtx"};
//    TTreeReaderValue<Bool_t> e2ZZTightID = {fReader, "e2ZZTightID"};
//    TTreeReaderValue<Bool_t> e2ZZTightIDNoVtx = {fReader, "e2ZZTightIDNoVtx"};
//    TTreeReaderValue<Int_t> e2Charge = {fReader, "e2Charge"};
//    TTreeReaderValue<Int_t> e2GenCharge = {fReader, "e2GenCharge"};
//    TTreeReaderValue<Int_t> e2GenPdgId = {fReader, "e2GenPdgId"};
//    TTreeReaderValue<Int_t> e2PdgId = {fReader, "e2PdgId"};
//    TTreeReaderValue<UInt_t> e2MissingHits = {fReader, "e2MissingHits"};
//    TTreeReaderValue<Float_t> e3EffScaleFactor = {fReader, "e3EffScaleFactor"};
//    TTreeReaderValue<Float_t> e3EffScaleFactorError = {fReader, "e3EffScaleFactorError"};
//    TTreeReaderValue<Float_t> e3EffectiveArea = {fReader, "e3EffectiveArea"};
//    TTreeReaderValue<Float_t> e3Energy = {fReader, "e3Energy"};
    TTreeReaderValue<Float_t> e3Eta = {fReader, "e3Eta"};
//    TTreeReaderValue<Float_t> e3GenEta = {fReader, "e3GenEta"};
//    TTreeReaderValue<Float_t> e3GenPhi = {fReader, "e3GenPhi"};
//    TTreeReaderValue<Float_t> e3GenPt = {fReader, "e3GenPt"};
//    TTreeReaderValue<Float_t> e3IDIsoEffScaleFactor = {fReader, "e3IDIsoEffScaleFactor"};
//    TTreeReaderValue<Float_t> e3IDIsoEffScaleFactorError = {fReader, "e3IDIsoEffScaleFactorError"};
//    TTreeReaderValue<Float_t> e3MVANonTrigID = {fReader, "e3MVANonTrigID"};
//    TTreeReaderValue<Float_t> e3Mass = {fReader, "e3Mass"};
//    TTreeReaderValue<Float_t> e3MtToMET = {fReader, "e3MtToMET"};
//    TTreeReaderValue<Float_t> e3PFChargedIso = {fReader, "e3PFChargedIso"};
//    TTreeReaderValue<Float_t> e3PFNeutralIso = {fReader, "e3PFNeutralIso"};
//    TTreeReaderValue<Float_t> e3PFPUIso = {fReader, "e3PFPUIso"};
//    TTreeReaderValue<Float_t> e3PFPhotonIso = {fReader, "e3PFPhotonIso"};
    TTreeReaderValue<Float_t> e3PVDXY = {fReader, "e3PVDXY"};
    TTreeReaderValue<Float_t> e3PVDZ = {fReader, "e3PVDZ"};
//    TTreeReaderValue<Float_t> e3Phi = {fReader, "e3Phi"};
    TTreeReaderValue<Float_t> e3Pt = {fReader, "e3Pt"};
//    TTreeReaderValue<Float_t> e3RelPFIsoRho = {fReader, "e3RelPFIsoRho"};
//    TTreeReaderValue<Float_t> e3Rho = {fReader, "e3Rho"};
//    TTreeReaderValue<Float_t> e3SCEnergy = {fReader, "e3SCEnergy"};
//    TTreeReaderValue<Float_t> e3SCEta = {fReader, "e3SCEta"};
//    TTreeReaderValue<Float_t> e3SCPhi = {fReader, "e3SCPhi"};
//    TTreeReaderValue<Float_t> e3SCRawEnergy = {fReader, "e3SCRawEnergy"};
//    TTreeReaderValue<Float_t> e3SIP3D = {fReader, "e3SIP3D"};
//    TTreeReaderValue<Float_t> e3TrkRecoEffScaleFactor = {fReader, "e3TrkRecoEffScaleFactor"};
//    TTreeReaderValue<Float_t> e3TrkRecoEffScaleFactorError = {fReader, "e3TrkRecoEffScaleFactorError"};
//    TTreeReaderValue<Float_t> e3ZZIso = {fReader, "e3ZZIso"};
//    TTreeReaderValue<Bool_t> e3IsCBVIDLoose = {fReader, "e3IsCBVIDLoose"};
//    TTreeReaderValue<Bool_t> e3IsCBVIDMedium = {fReader, "e3IsCBVIDMedium"};
    TTreeReaderValue<Bool_t> e3IsCBVIDTight = {fReader, "e3IsCBVIDTight"};
    TTreeReaderValue<Bool_t> e3IsEB = {fReader, "e3IsEB"};
//    TTreeReaderValue<Bool_t> e3IsGap = {fReader, "e3IsGap"};
//    TTreeReaderValue<Bool_t> e3IsWWLoose = {fReader, "e3IsWWLoose"};
//    TTreeReaderValue<Bool_t> e3ZZIsoPass = {fReader, "e3ZZIsoPass"};
//    TTreeReaderValue<Bool_t> e3ZZLooseID = {fReader, "e3ZZLooseID"};
//    TTreeReaderValue<Bool_t> e3ZZLooseIDNoVtx = {fReader, "e3ZZLooseIDNoVtx"};
//    TTreeReaderValue<Bool_t> e3ZZTightID = {fReader, "e3ZZTightID"};
//    TTreeReaderValue<Bool_t> e3ZZTightIDNoVtx = {fReader, "e3ZZTightIDNoVtx"};
//    TTreeReaderValue<Int_t> e3Charge = {fReader, "e3Charge"};
//    TTreeReaderValue<Int_t> e3GenCharge = {fReader, "e3GenCharge"};
//    TTreeReaderValue<Int_t> e3GenPdgId = {fReader, "e3GenPdgId"};
//    TTreeReaderValue<Int_t> e3PdgId = {fReader, "e3PdgId"};
//    TTreeReaderValue<UInt_t> e3MissingHits = {fReader, "e3MissingHits"};
//    TTreeReaderValue<Bool_t> doubleMuPass = {fReader, "doubleMuPass"};
//    TTreeReaderValue<Bool_t> doubleMuDZPass = {fReader, "doubleMuDZPass"};
//    TTreeReaderValue<Bool_t> doubleEPass = {fReader, "doubleEPass"};
//    TTreeReaderValue<Bool_t> singleESingleMuPass = {fReader, "singleESingleMuPass"};
//    TTreeReaderValue<Bool_t> singleMuSingleEPass = {fReader, "singleMuSingleEPass"};
//    TTreeReaderValue<Bool_t> tripleEPass = {fReader, "tripleEPass"};
//    TTreeReaderValue<Bool_t> doubleESingleMuPass = {fReader, "doubleESingleMuPass"};
//    TTreeReaderValue<Bool_t> doubleMuSingleEPass = {fReader, "doubleMuSingleEPass"};
//    TTreeReaderValue<Bool_t> tripleMuPass = {fReader, "tripleMuPass"};
//    TTreeReaderValue<Bool_t> singleEPass = {fReader, "singleEPass"};
//    TTreeReaderValue<Bool_t> singleIsoMuPass = {fReader, "singleIsoMuPass"};
//    TTreeReaderValue<Bool_t> singleIsoMu20Pass = {fReader, "singleIsoMu20Pass"};
//    TTreeReaderValue<Bool_t> singleMuPass = {fReader, "singleMuPass"};
//    TTreeReaderValue<Float_t> pileupSF = {fReader, "pileupSF"};
//    TTreeReaderValue<Float_t> e1TightIDSF = {fReader, "e1TightIDSF"};
//    TTreeReaderValue<Float_t> e1MediumIDSF = {fReader, "e1MediumIDSF"};
//    TTreeReaderValue<Float_t> e2TightIDSF = {fReader, "e2TightIDSF"};
//    TTreeReaderValue<Float_t> e2MediumIDSF = {fReader, "e2MediumIDSF"};
//    TTreeReaderValue<Float_t> e3TightIDSF = {fReader, "e3TightIDSF"};
//    TTreeReaderValue<Float_t> e3MediumIDSF = {fReader, "e3MediumIDSF"};
    const char* channel_;

    FakeRateSelector(TTree * /*tree*/ =0) { }
    FakeRateSelector(const char* name, TTree * /*tree*/ =0) : SelectorBase(name, 0) { }
    FakeRateSelector(const char* name, const char* channel, TTree * /*tree*/ =0) : 
        SelectorBase(name, 0), channel_(channel) { }
    virtual ~FakeRateSelector() { }
    virtual void   SlaveBegin(TTree *) override;
    virtual void   SetupNewDirectory() override;
    virtual Bool_t Process(Long64_t entry) override;

    ClassDefOverride(FakeRateSelector,0);

};

#endif
