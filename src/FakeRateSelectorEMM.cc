#include "Analysis/WZAnalysis/interface/FakeRateSelectorEMM.h"
#include <TStyle.h>

Bool_t FakeRateSelectorEMM::Process(Long64_t entry)
{
    if (! SelectorBase::Process(entry)) return false;
    if (std::abs(static_cast<int>(*nWWLooseElec + *nWZLooseMuon) - 3) > 0.1)
        return false;
    if (*type1_pfMETEt > 30)
        return false;
    if (!(*m1IsTightMuon && *m1RelPFIsoDBR04 < 0.15 &&
            *m2IsTightMuon && *m2RelPFIsoDBR04 < 0.15))
        return false;
    passingLoose2D_->Fill(*ePt, *eEta);
    passingLoose1DPt_->Fill(*ePt);
    passingLoose1DEta_->Fill(*eEta);
    if (*eIsCBVIDTight &&
            ( *eIsEB ? *ePVDXY < 0.05 : *ePVDXY < 0.1) &&
            ( *eIsEB ? *ePVDZ < 0.1 : *ePVDZ < 0.2)) {
        passingTight2D_->Fill(*ePt, *eEta);
        passingTight1DPt_->Fill(*ePt);
        passingTight1DEta_->Fill(*eEta);
    }
    return true;
}

void FakeRateSelectorEMM::SetupNewDirectory()
{
    // Must call base class setup
    SelectorBase::SetupNewDirectory();
    const std::string channel = "emm";
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
