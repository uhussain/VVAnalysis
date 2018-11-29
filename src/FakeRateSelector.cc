#include "Analysis/VVAnalysis/interface/FakeRateSelector.h"
#include <TStyle.h>

Bool_t FakeRateSelector::Process(Long64_t entry)
{
    ZZSelectorBase::Process(entry);
    //In HZZ AN it says: |M_inv(l1,l2)- MZ| < 7 GeV, to reduce the contribution from photon (asymmetric) conversions populating low masses.
    if (Z1mass > 98.1876 || Z1mass < 84.1876)
        return true;
    if (type1_pfMETEt > 25)
        return true;
    if (l3MtToMET > 30)
        return false;
    if (!tightZ1Leptons())
        return true;
    
    float pt_fillval = l3Pt;
    float eta_fillval = std::abs(l3Eta);

    float loose_weight = weight;
    //if (channel_ == eee || channel_ == emm) {
    //    loose_weight /= eIdSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
    //}
    //else if (channel_ == eem || channel_ == mmm) {
    //    loose_weight /= mIsoSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
    //}
    passingLoose2D_->Fill(pt_fillval, eta_fillval, loose_weight);
    passingLoose1DPt_->Fill(pt_fillval, loose_weight);
    passingLoose1DEta_->Fill(eta_fillval, loose_weight);
    if (lep3IsTight()) {
        passingTight2D_->Fill(pt_fillval, eta_fillval, weight);
        passingTight1DPt_->Fill(pt_fillval, weight);
        passingTight1DEta_->Fill(eta_fillval, weight);
    }
    return kTRUE;
}

void FakeRateSelector::Init(TTree *tree)
{
    ZZSelectorBase::Init(tree);
}

void FakeRateSelector::SetupNewDirectory()
{
    ZZSelectorBase::SetupNewDirectory();

    const int nvarbins = 8;
    double variable_pt_bins[nvarbins+1] = {5,10,20,30,40,50,60,70,FR_MAX_PT_};
    AddObject<TH2D>(passingTight2D_, ("passingTight2D_"+channelName_).c_str(), "|#eta|; p_{T} [GeV]", nvarbins, variable_pt_bins, 4, 0, 2.5);
    AddObject<TH1D>(passingTight1DPt_, ("passingTight1DPt_"+channelName_).c_str(), "Tight leptons; p_{T} [GeV]", nvarbins, variable_pt_bins);
    AddObject<TH1D>(passingTight1DEta_, ("passingTight1DEta_"+channelName_).c_str(), "Tight leptons; |#eta|", 4, 0, 2.5);
    
    AddObject<TH2D>(passingLoose2D_, ("passingLoose2D_"+channelName_).c_str(), "|#eta|; p_{T} [GeV]", nvarbins, variable_pt_bins, 4, 0, 2.5);
    AddObject<TH1D>(passingLoose1DPt_, ("passingLoose1DPt_"+channelName_).c_str(), "Loose leptons; p_{T} [GeV]", nvarbins, variable_pt_bins);
    AddObject<TH1D>(passingLoose1DEta_, ("passingLoose1DEta_"+channelName_).c_str(), "Loose leptons; |#eta|", 4, 0, 2.5);
}
