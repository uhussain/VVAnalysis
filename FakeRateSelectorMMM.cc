#define FakeRateSelectorMMM_cxx

#include "FakeRateSelectorMMM.h"
#include <TStyle.h>

Bool_t FakeRateSelectorMMM::Process(Long64_t entry)
{
    if (! SelectorBase::Process(entry)) return false;
    if (std::abs(static_cast<int>(*nWWLooseElec + *nWZLooseMuon) - 3) > 0.1)
        return false;
    if (*type1_pfMETEt > 30)
        return false;
    if (!(*m1IsTightMuon && *m1RelPFIsoDBR04 < 0.15 &&
            *m2IsTightMuon && *m2RelPFIsoDBR04 < 0.15))
        return false;
    passingLoose2D_->Fill(*m3Pt, *m3Eta);
    passingLoose1DPt_->Fill(*m3Pt);
    passingLoose1DEta_->Fill(*m3Eta);
    if (*m3IsTightMuon && *m3RelPFIsoDBR04 < 0.15) {
        passingTight2D_->Fill(*m3Pt, *m3Eta);
        passingTight1DPt_->Fill(*m3Pt);
        passingTight1DEta_->Fill(*m3Eta);
    }
    return true;
}

void FakeRateSelectorMMM::SetupNewDirectory()
{
    // Must call base class setup
    SelectorBase::SetupNewDirectory();
    const std::string channel = "mmm";
    AddObject<TH2D>(passingTight2D_, (std::string("passingTight2D_") + channel).c_str(), 
        "Tight leptons; p_{T} [GeV]; Eta", 8, 10, 50, 5, -2.5, 2.5);
    AddObject<TH1D>(passingTight1DPt_, (std::string("passingTight1DPt_") + channel).c_str(), 
        "Tight leptons; p_{T} [GeV]", 8, 10, 50);
    AddObject<TH1D>(passingTight1DEta_, (std::string("passingTight1DEta_") + channel).c_str(), 
        "Tight leptons; #eta; Events", 5, -2.5, 2.5);
    AddObject<TH2D>(passingLoose2D_, (std::string("passingLoose2D_") + channel).c_str(), 
        "Leptons passing tight failing loose; p_{T} [GeV]; Eta", 8, 10, 50, 5, -2.5, 2.5);
    AddObject<TH1D>(passingLoose1DPt_, (std::string("passingLoose1DPt_") + channel).c_str(), 
        "Loose leptons; p_{T} [GeV]; Events", 8, 10, 50);
    AddObject<TH1D>(passingLoose1DEta_, (std::string("passingLoose1DEta_") + channel).c_str(), 
        "Loose leptons; #eta; Events", 5, -2.5, 2.5);
}

