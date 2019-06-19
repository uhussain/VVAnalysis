#include "Analysis/VVAnalysis/interface/WGenSelector.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include <TStyle.h>
#include <regex>

void WGenSelector::Init(TTree *tree)
{
    allChannels_ = {"e", "m", "Unknown"};
    hists1D_ = {"CutFlow", "mW", "yW", "ptW", "ptl", "etal", "phil", "MET", "MET_phi",};

    SelectorBase::Init(tree);
    
}

void WGenSelector::SetBranchesNanoAOD() {
    fChain->SetBranchAddress("genWeight", &genWeight, &b_genWeight);
    fChain->SetBranchAddress("nGenPart", &nGenPart, &b_nGenPart);
    fChain->SetBranchAddress("GenPart_pt", &GenPart_pt, &b_GenPart_pt);
    fChain->SetBranchAddress("GenPart_phi", &GenPart_phi, &b_GenPart_phi);
    fChain->SetBranchAddress("GenPart_eta", &GenPart_eta, &b_GenPart_eta);
    fChain->SetBranchAddress("GenPart_phi", &GenPart_phi, &b_GenPart_phi);
    fChain->SetBranchAddress("GenPart_mass", &GenPart_mass, &b_GenPart_mass);
    fChain->SetBranchAddress("GenPart_status", &GenPart_status, &b_GenPart_status);
    fChain->SetBranchAddress("GenPart_pdgId", &GenPart_pdgId, &b_GenPart_pdgId);
}

void WGenSelector::LoadBranchesNanoAOD(Long64_t entry, std::pair<Systematic, std::string> variation) { 
    weight = 1;
    b_nGenPart->GetEntry(entry);
    b_GenMET_pt->GetEntry(entry);
    b_GenMET_phi->GetEntry(entry);
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
    std::vector<size_t> goodIndices = {};

    for (size_t i = 0; i < nGenPart; i++) {
        if (GenPart_status[i] == 1 && (std::abs(GenPart_pdgId[i]) == 11 || std::abs(GenPart_pdgId[i] == 13))) {
            LorentzVector vec;
            vec.SetPt(GenPart_pt[i]);
            vec.SetEta(GenPart_eta[i]);
            vec.SetPhi(GenPart_phi[i]);
            vec.SetM(GenPart_mass[i]);
            int charge = (GenPart_pdgId[i] < 0) ? -1: 1;
            leptons.push_back(reco::GenParticle(charge, vec, reco::Particle::Point(), GenPart_status[i], GenPart_pdgId[i], charge));
        }
    }
    std::sort(leptons.begin(), leptons.end(), 
        [](const reco::GenParticle& a, const reco::GenParticle& b) { return a.pt() > b.pt(); });

    if (leptons.size() > 0 && std::abs(leptons.at(0).pdgId()) == 11) {
        channel_ = e;
        channelName_ = "e";
    }
    else if (leptons.size() > 0 && std::abs(leptons.at(0).pdgId()) == 13) {
        channel_ = m;
        channelName_ = "m";
    }
    else {
        channel_ = Unknown;
        channelName_ = "Unknown";
        return;
    }
    genMet.SetPt(GenMET_pt);
    genMet.SetPhi(GenMET_phi);
    genMet.SetM(0.);
    genMet.SetEta(0.);

    wCand = leptons.at(0).polarP4() + genMet;

    b_genWeight->GetEntry(entry);
    weight = genWeight;
}

void WGenSelector::FillHistograms(Long64_t entry, std::pair<Systematic, std::string> variation) { 
    int step = 0;
    SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);

    if (channel_ != mm && channel_ != ee) 
        return;
    SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);

    if (leptons.size() < 1)
        return;
    SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);

    auto lep = leptons.at(0);
    if (std::abs(lep.eta() < 2.5))
        return;
    SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);

    if (genMet.pt() < 25)
        return;
    SafeHistFill(histMap1D_, getHistName("CutFlow", variation.second), step++, weight);


    SafeHistFill(histMap1D_, getHistName("mW", variation.second), wCand.pt(), weight);
    SafeHistFill(histMap1D_, getHistName("yW", variation.second), wCand.Rapidity(), weight);
    SafeHistFill(histMap1D_, getHistName("ptW", variation.second), wCand.pt(), weight);
    SafeHistFill(histMap1D_, getHistName("MET", variation.second), genMet.pt(), weight);
    SafeHistFill(histMap1D_, getHistName("MET_phi", variation.second), genMet.phi(), weight);
    SafeHistFill(histMap1D_, getHistName("ptl", variation.second), lep.pt(), weight);
    SafeHistFill(histMap1D_, getHistName("etal", variation.second), lep.eta(), weight);
    SafeHistFill(histMap1D_, getHistName("phil", variation.second), lep.eta(), weight);
}

void WGenSelector::SetupNewDirectory() {
    SelectorBase::SetupNewDirectory();

    InitializeHistogramsFromConfig();
}

