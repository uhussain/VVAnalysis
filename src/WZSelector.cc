#include "Analysis/WZAnalysis/interface/WZSelector.h"
#include <TStyle.h>

void WZSelector::Init(TTree *tree)
{
    WZSelectorBase::Init(tree);
    
    fChain->SetBranchAddress("jetPt", &jetPt, &b_jetPt);
    fChain->SetBranchAddress("jetEta", &jetEta, &b_jetEta);
    fChain->SetBranchAddress("Mass", &Mass, &b_Mass);
    fChain->SetBranchAddress("nvtx", &nvtx, &b_nvtx);
    fChain->SetBranchAddress("mjj", &mjj, &b_mjj);
    fChain->SetBranchAddress("type1_pfMETEt", &type1_pfMETEt, &b_type1_pfMETEt);
    fChain->SetBranchAddress("nCBVIDVetoElec", &nCBVIDVetoElec, &b_nCBVIDVetoElec);
    fChain->SetBranchAddress("nWZLooseMuon", &nWZLooseMuon, &b_nWZLooseMuon);

    if (channel_ == eee) {
        fChain->SetBranchAddress("e1_e2_Mass", &Zmass, &b_Zmass);
        fChain->SetBranchAddress("e1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("e1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("e2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("e2Eta", &l2Eta, &b_l2Eta);
        fChain->SetBranchAddress("e3Pt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("e3Eta", &l3Eta, &b_l3Eta);
    }
    else if (channel_ == eem) { 
        fChain->SetBranchAddress("e1_e2_Mass", &Zmass, &b_Zmass);
        fChain->SetBranchAddress("mPt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("mEta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("e1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("e1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("e2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("e2Eta", &l2Eta, &b_l2Eta);
    }
    else if (channel_ == emm) { 
        fChain->SetBranchAddress("m1_m2_Mass", &Zmass, &b_Zmass);
        fChain->SetBranchAddress("ePt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("eEta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("m1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("m1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("m2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("m2Eta", &l2Eta, &b_l2Eta);
    }
    else if (channel_ == mmm) { 
        fChain->SetBranchAddress("m1_m2_Mass", &Zmass, &b_Zmass);
        fChain->SetBranchAddress("m1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("m1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("m2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("m2Eta", &l2Eta, &b_l2Eta);
        fChain->SetBranchAddress("m3Pt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("m3Eta", &l3Eta, &b_l3Eta);
    }
}

void WZSelector::SlaveBegin(TTree * /*tree*/)
{
    pileupSF_ = (ScaleFactor *) GetInputList()->FindObject("pileupSF");
    if (pileupSF_ == nullptr ) 
        Abort("Must pass pileup weights SF");
    eIdSF_ = (ScaleFactor *) GetInputList()->FindObject("electronTightIdSF");
    if (eIdSF_ == nullptr ) 
        Abort("Must pass electron ID SF");
    mIdSF_ = (ScaleFactor *) GetInputList()->FindObject("muonTightIdSF");
    if (mIdSF_ == nullptr ) 
        Abort("Must pass muon ID SF");
    mIsoSF_ = (ScaleFactor *) GetInputList()->FindObject("muonIsoSF");
    if (mIsoSF_ == nullptr ) 
        Abort("Must pass muon Iso SF");
}

Bool_t WZSelector::Process(Long64_t entry)
{
    WZSelectorBase::Process(entry);

    b_Zmass->GetEntry(entry);
    b_Mass->GetEntry(entry);
    b_nvtx->GetEntry(entry);
    b_mjj->GetEntry(entry);
    b_type1_pfMETEt->GetEntry(entry);
    b_nCBVIDVetoElec->GetEntry(entry);
    b_nWZLooseMuon->GetEntry(entry);
    b_l1Eta->GetEntry(entry);
    b_l1Pt->GetEntry(entry);
    b_l2Eta->GetEntry(entry);
    b_l2Pt->GetEntry(entry);
    b_l3Eta->GetEntry(entry);
    b_l3Pt->GetEntry(entry);
    b_jetPt->GetEntry(entry);
    b_jetEta->GetEntry(entry);
    if (isMC_) {
        if (channel_ == eee) {
            genWeight *= eIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
            genWeight *= eIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
            genWeight *= eIdSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
            genWeight *= pileupSF_->Evaluate1D(nTruePU);
        }
        else if (channel_ == eem) {
            genWeight *= eIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
            genWeight *= eIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
            genWeight *= mIdSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
            genWeight *= mIsoSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
            genWeight *= pileupSF_->Evaluate1D(nTruePU);
        }
        else if (channel_ == emm) {
            genWeight *= eIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
            genWeight *= mIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
            genWeight *= mIsoSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
            genWeight *= mIdSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
            genWeight *= mIsoSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
            genWeight *= pileupSF_->Evaluate1D(nTruePU);
        }
        else {
            genWeight *= mIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
            genWeight *= mIsoSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
            genWeight *= mIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
            genWeight *= mIsoSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
            genWeight *= mIdSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
            genWeight *= mIsoSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
            genWeight *= pileupSF_->Evaluate1D(nTruePU);
        }
    }
    //if (type1_pfMETEt < 30)
    //    return true;
    //if (Mass < 100)
    //    return true;
    //if (l1Pt < 25 || l2Pt < 15)
    //    return true;
    //if (Zmass > 116.1876 || Zmass < 76.1876)
    //    return true;
    if (nWZLooseMuon + nCBVIDVetoElec > 3)
        return true;
    if (!(zlep1IsTight() && zlep2IsTight() && lepton3IsTight()))
        return true;
    float dEtajj = 0;
    if (jetEta->size() >= 2)
        dEtajj = std::abs(jetEta->at(0) - jetEta->at(1));
    //if (dEtajj < 2.5 || mjj < 500)
    //if (!(dEtajj > 2.5 || mjj > 500))
    //    return true;
    zmassHist_->Fill(Zmass, genWeight);
    nvtxHist_->Fill(nvtx, genWeight);
    l1PtHist_->Fill(l1Pt, genWeight);
    l2PtHist_->Fill(l2Pt, genWeight);
    l3PtHist_->Fill(l3Pt, genWeight);
    mjjHist_->Fill(mjj, genWeight*(isMC_ || mjj < 500));
    if (dEtajj > 0)
        dEtajjHist_->Fill(dEtajj, genWeight*(isMC_ || dEtajj < 2.5));
    massHist_->Fill(Mass, genWeight*(isMC_ || Mass < 400));
    return true;
}

void WZSelector::SetupNewDirectory()
{
    WZSelectorBase::SetupNewDirectory();

    AddObject<TH1D>(nvtxHist_, ("nvtx_"+channelName_).c_str(), "nvtx; Number of Vertices; Events;", 60, 0, 60);
    AddObject<TH1D>(zmassHist_, ("ZMass_"+channelName_).c_str(), "ZMass; M_{ll} [GeV]; Events;", 15, 0, 150);
    AddObject<TH1D>(massHist_, ("Mass_"+channelName_).c_str(), "Mass; M_{3l} [GeV]; Events / 30 GeV;", 14, 100, 520);
    AddObject<TH1D>(mjjHist_, ("mjj_"+channelName_).c_str(), "mjj; m_{jj} [GeV]; Events / 50 GeV;", 15, 0, 1500);
    AddObject<TH1D>(dEtajjHist_, ("dEtajj_"+channelName_).c_str(), "dEtajj; #Delta#eta(j_{1}, j_{2}); Events;", 12, 0, 6);
    AddObject<TH1D>(l1PtHist_, ("Zlep1_Pt_"+channelName_).c_str(), "l1Pt; p_{T} leading Z lepton [GeV]; Events / 15 GeV;", 10, 25, 175);
    AddObject<TH1D>(l2PtHist_, ("Zlep2_Pt_"+channelName_).c_str(), "l2Pt; p_{T} trailing Z lepton [GeV]; Events / 10 GeV;", 12, 15, 135);
    AddObject<TH1D>(l3PtHist_, ("Wlep_Pt_"+channelName_).c_str(), "l2Pt; p_{T} W lepton [GeV]; Events / 10 GeV;", 10, 20, 220);
}

