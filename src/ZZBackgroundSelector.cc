#include "Analysis/VVAnalysis/interface/ZZBackgroundSelector.h"
#include <TStyle.h>

void ZZBackgroundSelector::SlaveBegin(TTree * /*tree*/)
{
    //systematics_ = {
    //    {jetEnergyScaleUp, "CMS_scale_jUp"}, 
    //    {jetEnergyScaleDown, "CMS_scale_jDown"}, 
    //    {jetEnergyResolutionUp, "CMS_res_jUp"},
    //    {jetEnergyResolutionDown, "CMS_res_jDown"},
    //};
    //isNonpromptEstimate_ = true;
    //doaQGC_ = false;
    ZZSelector::SlaveBegin(0);
    fakeRate_allE_ = (ScaleFactor *) GetInputList()->FindObject("fakeRate_allE");
    if (fakeRate_allE_ == nullptr ) Abort("Must pass electron fake rate to input list!");
    fakeRate_allMu_ = (ScaleFactor *) GetInputList()->FindObject("fakeRate_allMu");
    if (fakeRate_allMu_ == nullptr ) Abort("Must pass muon fake rate to input list!");
}

void ZZBackgroundSelector::SetupNewDirectory()
{
    ZZSelector::SetupNewDirectory();
   
    // Insure that hist ranges are exactly the same as ZZSelector, just change name
    for (const auto && obj : *currentHistDir_) {
        std::string name = obj->GetName();
        TNamed* named = dynamic_cast<TNamed*>(obj);
        named->SetName(name.insert(name.length()-3, "Fakes_").c_str());
    }
    
    AddObject<TH1D>(Z1MassHistPPPF_, ("Z1Mass_PPPF_"+channelName_).c_str(), "Z1Mass; m_{Z_{1}} [GeV]; Events;", 60, 0, 120);
    AddObject<TH1D>(Z1MassHistPPFF_, ("Z1Mass_PPFF_"+channelName_).c_str(), "Z1Mass; m_{Z_{1}} [GeV]; Events;", 60, 0, 120);

    AddObject<TH1D>(MassHistPPPF_, ("Mass_PPPF_"+channelName_).c_str(), "Mass; m_{4l} [GeV]; Events;", 32, 0, 800);
    AddObject<TH1D>(MassHistPPFF_, ("Mass_PPFF_"+channelName_).c_str(), "Mass; m_{4l} [GeV]; Events;", 32, 0, 800);
}
float ZZBackgroundSelector::getEventWeight(Long64_t entry) {
    float evtwgt = 0;

    if (IsPPPFRegion()) {
        if (true){
            Z1MassHistPPPF_->Fill(Z1mass, weight);
            MassHistPPPF_->Fill(Mass,weight);
        }
        evtwgt = getl4FakeRate(entry)*weight;
    }
    else if (IsPPFFRegion()) {
        if (true) {
            Z1MassHistPPFF_->Fill(Z1mass, weight);
            MassHistPPFF_->Fill(Mass,weight);
        }
        evtwgt = -1*getl3FakeRate(entry)*getl4FakeRate(entry)*weight;
    }
    return evtwgt;
}

Bool_t ZZBackgroundSelector::Process(Long64_t entry)
{
    std::pair<Systematic, std::string> central_var = std::make_pair(Central, "");

    LoadBranches(entry, central_var);
    //if (!PassesBaseSelection(entry, false, selection_))
    //    return true;
    FillHistograms(entry, getEventWeight(entry), true, central_var);

   // if (doSystematics_) {
   //     for (const auto& systematic : systematics_) {
   //         LoadBranches(entry, systematic);
   //         if (!PassesBaseSelection(entry, false, selection_))
   //             return true;
   //         FillHistograms(entry, getEventWeight(), true, systematic);
   //     }
   // }

    return true;
}

float ZZBackgroundSelector::getl3FakeRate(Long64_t entry) {
    float pt_fillval = l3Pt < FR_MAX_PT_ ? l3Pt : FR_MAX_PT_ - 0.01;
    float fr = 1;
    if (channel_ == eeee || (channel_ == eemm && !(e1e2IsZ1(entry))))
        fr = fakeRate_allE_->Evaluate2D(pt_fillval, std::abs(l3Eta));
    else
        fr = fakeRate_allMu_->Evaluate2D(pt_fillval, std::abs(l3Eta));
    return fr/(1-fr); 
}

float ZZBackgroundSelector::getl4FakeRate(Long64_t entry) {
    float pt_fillval = l4Pt < FR_MAX_PT_ ? l4Pt : FR_MAX_PT_ - 0.01;
    float fr = 1;
    if (channel_ == eeee || (channel_ == eemm && !(e1e2IsZ1(entry))))
        fr = fakeRate_allE_->Evaluate2D(pt_fillval, std::abs(l4Eta));
    else
        fr = fakeRate_allMu_->Evaluate2D(pt_fillval, std::abs(l4Eta));
    return fr/(1-fr); 
}
bool ZZBackgroundSelector::IsPPPFRegion() {
    return tightZ1Leptons() && lep3IsTight() && lep4IsLoose() ;
}

bool ZZBackgroundSelector::IsPPFFRegion() {
    return tightZ1Leptons() && lep3IsLoose() && lep4IsLoose();
}

