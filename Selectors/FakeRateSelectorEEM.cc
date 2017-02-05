#define FakeRateSelectorEEM_cxx

#include "FakeRateSelectorEEM.h"
#include <TStyle.h>

void FakeRateSelectorEEM::SlaveBegin(TTree * /*tree*/)
{
}

Bool_t FakeRateSelectorEEM::Process(Long64_t entry)
{
    if (! SelectorBase::Process(entry)) return false;
    if (std::abs(static_cast<int>(*nWWLooseElec + *nWZLooseMuon) - 3) > 0.1)
        return false;
    if (*type1_pfMETEt > 30)
        return false;
    if (!(*e1IsCBVIDTight &&
            ( *e1IsEB ? *e1PVDXY < 0.05 : *e1PVDXY < 0.1) &&
            ( *e1IsEB ? *e1PVDZ < 0.1 : *e1PVDZ < 0.2)) &&
            (*e2IsCBVIDTight &&
            ( *e2IsEB ? *e2PVDXY < 0.05 : *e2PVDXY < 0.1) &&
            ( *e2IsEB ? *e2PVDZ < 0.1 : *e2PVDZ < 0.2)))
        return false;
    passingLoose2D_->Fill(*mPt, *mEta);
    passingLoose1DPt_->Fill(*mPt);
    passingLoose1DEta_->Fill(*mEta);
    if (*mIsTightMuon && *mRelPFIsoDBR04 < 0.15) {
        passingTight2D_->Fill(*mPt, *mEta);
        passingTight1DPt_->Fill(*mPt);
        passingTight1DEta_->Fill(*mEta);
    }

    return true;
}

void FakeRateSelectorEEM::SetupNewDirectory()
{
    // Must call base class setup
    SelectorBase::SetupNewDirectory();
    const std::string channel = "eem";
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

