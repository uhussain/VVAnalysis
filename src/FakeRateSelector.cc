#include "Analysis/WZAnalysis/interface/FakeRateSelector.h"
#include <TStyle.h>

Bool_t FakeRateSelector::Process(Long64_t entry)
{
    WZSelectorBase::Process(entry);

    if (!passesLeptonVeto)
        return true;
    if (l1Pt < 25 || l2Pt < 15)
        return true;
    if (ZMass > 101.1876 || ZMass < 81.1876)
        return true;
    if (type1_pfMETEt > 25)
        return true;
    if (l3MtToMET > 30)
        return false;
    if (!tightZLeptons())
        return true;
    float pt_fillval = l3Pt < FR_MAX_PT_ ? l3Pt : FR_MAX_PT_ - 0.01;
    float eta_fillval = std::abs(l3Eta) < FR_MAX_ETA_ ? std::abs(l3Eta) : FR_MAX_ETA_ - 0.01;
    passingLoose2D_->Fill(pt_fillval, eta_fillval, genWeight);
    passingLoose1DPt_->Fill(pt_fillval, genWeight);
    passingLoose1DEta_->Fill(eta_fillval, genWeight);
    if (lepton3IsTight()) {
        passingTight2D_->Fill(pt_fillval, eta_fillval, genWeight);
        passingTight1DPt_->Fill(pt_fillval, genWeight);
        passingTight1DEta_->Fill(eta_fillval, genWeight);
    }
    return kTRUE;
}

void FakeRateSelector::Init(TTree *tree)
{
    WZSelectorBase::Init(tree);
}

void FakeRateSelector::SetupNewDirectory()
{
    WZSelectorBase::SetupNewDirectory();

    AddObject<TH2D>(passingTight2D_, ("passingTight2D_"+channelName_).c_str(), "#eta; p_{T} [GeV]", 4, 10, 50, 3, 0, 2.5);
    AddObject<TH1D>(passingTight1DPt_, ("passingTight1DPt_"+channelName_).c_str(), "Tight leptons; p_{T} [GeV]", 4, 10, 50);
    AddObject<TH1D>(passingTight1DEta_, ("passingTight1DEta_"+channelName_).c_str(), "Tight leptons; #eta", 3, 0, 2.5);
    
    AddObject<TH2D>(passingLoose2D_, ("passingLoose2D_"+channelName_).c_str(), "#eta; p_{T} [GeV]", 4, 10, 50, 3, 0, 2.5);
    AddObject<TH1D>(passingLoose1DPt_, ("passingLoose1DPt_"+channelName_).c_str(), "Loose leptons; p_{T} [GeV]", 4, 10, 50);
    AddObject<TH1D>(passingLoose1DEta_, ("passingLoose1DEta_"+channelName_).c_str(), "Loose leptons; #eta", 3, 0, 2.5);
}
