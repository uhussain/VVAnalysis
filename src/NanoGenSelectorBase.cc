#include "Analysis/VVAnalysis/interface/NanoGenSelectorBase.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include <TStyle.h>
#include <regex>

void NanoGenSelectorBase::Init(TTree *tree)
{
    SelectorBase::Init(tree);
}

void NanoGenSelectorBase::SetBranchesNanoAOD() {
    fChain->SetBranchAddress("genWeight", &genWeight, &b_genWeight);
    fChain->SetBranchAddress("nGenPart", &nGenPart, &b_nGenPart);
    fChain->SetBranchAddress("GenMET_pt", &GenMET_pt, &b_GenMET_pt);
    fChain->SetBranchAddress("GenMET_phi", &GenMET_phi, &b_GenMET_phi);
    fChain->SetBranchAddress("GenPart_pt", &GenPart_pt, &b_GenPart_pt);
    fChain->SetBranchAddress("GenPart_phi", &GenPart_phi, &b_GenPart_phi);
    fChain->SetBranchAddress("GenPart_eta", &GenPart_eta, &b_GenPart_eta);
    fChain->SetBranchAddress("GenPart_phi", &GenPart_phi, &b_GenPart_phi);
    fChain->SetBranchAddress("GenPart_mass", &GenPart_mass, &b_GenPart_mass);
    fChain->SetBranchAddress("GenPart_status", &GenPart_status, &b_GenPart_status);
    fChain->SetBranchAddress("GenPart_pdgId", &GenPart_pdgId, &b_GenPart_pdgId);
}

void NanoGenSelectorBase::LoadBranchesNanoAOD(Long64_t entry, std::pair<Systematic, std::string> variation) { 
    weight = 1;
    b_nGenPart->GetEntry(entry);
    //b_GenMET_pt->GetEntry(entry);
    //b_GenMET_phi->GetEntry(entry);
    b_GenPart_mass->GetEntry(entry);
    b_GenPart_pt->GetEntry(entry);
    b_GenPart_eta->GetEntry(entry);
    b_GenPart_phi->GetEntry(entry);
    b_GenPart_pdgId->GetEntry(entry);
    b_GenPart_status->GetEntry(entry);
    if (nGenPart > N_KEEP_GEN_) {
        std::string message = "Found more Gen particles than max read number.\n    Found ";
        message += std::to_string(nGenPart);
        message += " particles\n  --> Max read number was ";
        message += std::to_string(N_KEEP_GEN_);
        message += "\nExiting because this can cause problems. Increase N_KEEP_GEN_ to avoid this error.\n";
        throw std::domain_error(message);
    }

    channel_ = channelMap_[channelName_];
    return;

    leptons.clear();
    neutrinos.clear();
    std::vector<unsigned int> idsToKeep = {11, 12, 13, 14};
    for (size_t i = 0; i < nGenPart; i++) {
        if (GenPart_status[i] != 1)
            continue;
        LorentzVector vec;
        if (std::find(idsToKeep.begin(), idsToKeep.end(), std::abs(GenPart_pdgId[i])) != idsToKeep.end()) {
            vec.SetPt(GenPart_pt[i]);
            vec.SetEta(GenPart_eta[i]);
            vec.SetPhi(GenPart_phi[i]);
            vec.SetM(GenPart_mass[i]);
        }
        if (std::abs(GenPart_pdgId[i]) == 11 || std::abs(GenPart_pdgId[i] == 13)) {
            int charge = (GenPart_pdgId[i] < 0) ? -1: 1;
            leptons.push_back(reco::GenParticle(charge, vec, reco::Particle::Point(), GenPart_pdgId[i], GenPart_status[i], true));
        }
        else if (std::abs(GenPart_pdgId[i]) == 12 || std::abs(GenPart_pdgId[i] == 14)) {
            neutrinos.push_back(reco::GenParticle(0, vec, reco::Particle::Point(), GenPart_pdgId[i], GenPart_status[i], true));
        }
    }
    std::sort(leptons.begin(), leptons.end(), 
        [](const reco::GenParticle& a, const reco::GenParticle& b) { return a.pt() > b.pt(); });

    //genMet.SetPt(GenMET_pt);
    //genMet.SetPhi(GenMET_phi);
    //genMet.SetM(0.);
    //genMet.SetEta(0.);

    b_genWeight->GetEntry(entry);
    weight = genWeight;
}

void NanoGenSelectorBase::SetupNewDirectory() {
    SelectorBase::SetupNewDirectory();

    InitializeHistogramsFromConfig();
}
