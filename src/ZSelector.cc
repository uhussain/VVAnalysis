#include "Analysis/VVAnalysis/interface/ZSelector.h"
#include "TLorentzVector.h"
#include <TStyle.h>
#include <regex>
#include "TParameter.h"

void ZSelector::Init(TTree *tree)
{
    SelectorBase::Init(tree);
    if (addSumweights_) {
        TFile* file = fChain->GetTree()->GetDirectory()->GetFile(); 
        TTree* metaInfo = dynamic_cast<TTree*>(file->Get("metaInfo/metaInfo"));
        if (metaInfo == nullptr)
            std::cerr << "WARNING: Failed to add sumWeights histogram" << std::endl;
        else {
            metaInfo->Draw("1>>sumweights", "summedWeights");
        }
    }
}

void ZSelector::SetBranchesUWVV() {
    throw std::domain_error("UWVV ntuples not defined for Z selector!");
}

void ZSelector::SetBranchesNanoAOD() {
    fChain->SetBranchAddress("nElectron", &nElectron, &b_nElectron);
    fChain->SetBranchAddress("nMuon", &nMuon, &b_nMuon);
    fChain->SetBranchAddress("Electron_pt", &Electron_pt, &b_Electron_pt);
    fChain->SetBranchAddress("Electron_eta", &Electron_eta, &b_Electron_eta);
    fChain->SetBranchAddress("Electron_phi", &Electron_phi, &b_Electron_phi);
    fChain->SetBranchAddress("Muon_pt", &Muon_pt, &b_Muon_pt);
    fChain->SetBranchAddress("Muon_eta", &Muon_eta, &b_Muon_eta);
    fChain->SetBranchAddress("Muon_phi", &Muon_phi, &b_Muon_phi);
    fChain->SetBranchAddress("Electron_cutBased", &Electron_cutBased, &b_Electron_cutBased);
    fChain->SetBranchAddress("Muon_tightId", &Muon_tightId, &b_Muon_tightId);
    fChain->SetBranchAddress("Muon_tkIsoId", &Muon_tkIsoId, &b_Muon_tkIsoId);
    fChain->SetBranchAddress("MET_pt", &MET, &b_MET);
    fChain->SetBranchAddress("MET_phi", &type1_pfMETPhi, &b_type1_pfMETPhi);
    fChain->SetBranchAddress("Electron_charge", &Electron_charge, &b_Electron_charge);
    fChain->SetBranchAddress("Muon_charge", &Muon_charge, &b_Muon_charge);
    fChain->SetBranchAddress("Electron_mass", &Electron_mass, &b_Electron_mass);
    fChain->SetBranchAddress("Muon_mass", &Muon_mass, &b_Muon_mass);
    if (isMC_) {
        fChain->SetBranchAddress("genWeight", &genWeight, &b_genWeight);
        fChain->SetBranchAddress("Pileup_nPU", &numPU, &b_numPU);
    }
}

void ZSelector::LoadBranchesNanoAOD(Long64_t entry, std::pair<Systematic, std::string> variation) { 
    weight = 1;
    b_nElectron->GetEntry(entry);
    b_nMuon->GetEntry(entry);
    b_Electron_pt->GetEntry(entry);
    b_Electron_eta->GetEntry(entry);
    b_Electron_phi->GetEntry(entry);
    b_Muon_pt->GetEntry(entry);
    b_Muon_eta->GetEntry(entry);
    b_Muon_phi->GetEntry(entry);
    b_Electron_cutBased->GetEntry(entry);
    b_Muon_tightId->GetEntry(entry);
    b_Muon_tkIsoId->GetEntry(entry);
    b_Electron_charge->GetEntry(entry);
    b_Muon_charge->GetEntry(entry);
    b_Electron_mass->GetEntry(entry);
    b_Muon_mass->GetEntry(entry);
    //b_MET->GetEntry(entry);

    if (nElectron > N_KEEP_MU_E_ || nMuon > N_KEEP_MU_E_) {
        std::string message = "Found more electrons or muons than max read number.\n    Found ";
        message += std::to_string(nElectron);
        message += " electrons.\n    Found ";
        message += std::to_string(nMuon);
        message += " Muons\n  --> Max read number was ";
        message += std::to_string(N_KEEP_MU_E_);
        message += "\nExiting because this can cause problems. Increase N_KEEP_MU_E_ to avoid this error.\n";
        throw std::domain_error(message);
    }

    ZMass = 0;
    l1Pt = 0;
    l2Pt = 0;
    l1Eta = 0;
    l2Eta = 0;
    l1Phi = 0;
    l2Phi = 0;
    l1Mass = 0;
    l2Mass = 0;
    if (nElectron == 2) {
        channel_ = ee;
        if (Electron_charge[0] != Electron_charge[1]) {
            l1Pt = Electron_pt[0];
            l2Pt = Electron_pt[1];
            l1Eta = Electron_eta[0];
            l2Eta = Electron_eta[1];
            l1Phi = Electron_phi[0];
            l2Phi = Electron_phi[1];
            l1Mass = Electron_mass[0];
            l2Mass = Electron_mass[1];
            // cut-based ID Fall17 V2 (0:fail, 1:veto, 2:loose, 3:medium, 4:tight)
            l1IsTight = (Electron_cutBased[0] == 4);
            l2IsTight = (Electron_cutBased[1] == 4);
        }
    }
    else if (nMuon == 2) {
        channel_ = mm;
        if (Muon_charge[0] != Muon_charge[1]) {
            l1Pt = Muon_pt[0];
            l2Pt = Muon_pt[1];
            l1Eta = Muon_eta[0];
            l2Eta = Muon_eta[1];
            l1Phi = Muon_phi[0];
            l2Phi = Muon_phi[1];
            l1Mass = Muon_mass[0];
            l2Mass = Muon_mass[1];
            // cut-based ID, tight WP; TkIso ID (1=TkIsoLoose, 2=TkIsoTight)
            l1IsTight = (Muon_tightId[0] && (Muon_tkIsoId[0] == 2));
            l2IsTight = (Muon_tightId[1] && (Muon_tkIsoId[1] == 2));
        }
    }
    SetMass();

    if (isMC_) {
        b_genWeight->GetEntry(entry);
        //TODO: add scale factors
        //b_numPU->GetEntry(entry);
        //b_l1GenPt->GetEntry(entry);
        //b_l2GenPt->GetEntry(entry);
        //b_l3GenPt->GetEntry(entry);
        //ApplyScaleFactors();
    }
    else {
        b_Flag_duplicateMuonsPass->GetEntry(entry);          
        b_Flag_badMuonsPass->GetEntry(entry);          
    }

    passesLeptonVeto = (nMuon + nElectron) == 3;
}

void ZSelector::LoadBranchesUWVV(Long64_t entry, std::pair<Systematic, std::string> variation){ 
    throw std::domain_error("UWVV ntuples not defined for Z selector!");
}

void ZSelector::ApplyScaleFactors() {
    weight = genWeight;
    if (channel_ == ee) {
        weight *= eIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
        weight *= eGsfSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
        weight *= eIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
        weight *= eGsfSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
    }
    else if (channel_ = mm) {
        weight *= mIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
        weight *= mIsoSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
        weight *= mIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
        weight *= mIsoSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
    }
    weight *= pileupSF_->Evaluate1D(nTruePU);
}

void ZSelector::SetMass() {
    TLorentzVector lepton1;
    lepton1.SetPtEtaPhiM(l1Pt, l1Eta, l1Phi, l1Mass);
    TLorentzVector lepton2;
    lepton2.SetPtEtaPhiM(l2Pt, l2Eta, l2Phi, l2Mass);
    ZMass = (lepton1+lepton2).M();
}

// Meant to be a wrapper for the tight ID just in case it changes
// To be a function of multiple variables
bool ZSelector::zlep1IsTight() {
    return l1IsTight; 
}

bool ZSelector::zlep2IsTight() {
    return l2IsTight; 
}

bool ZSelector::tightZLeptons() {
    return zlep1IsTight() && zlep2IsTight(); 
}
