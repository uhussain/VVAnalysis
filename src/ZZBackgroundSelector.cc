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
    fakeRate_allE_ = (TH2D *) GetInputList()->FindObject("fakeRate_allE");
    if (fakeRate_allE_ == nullptr ) Abort("Must pass electron fake rate to input list!");
    fakeRate_allMu_ = (TH2D *) GetInputList()->FindObject("fakeRate_allMu");
    if (fakeRate_allMu_ == nullptr ) Abort("Must pass muon fake rate to input list!");
}

void ZZBackgroundSelector::SetupNewDirectory()
{
    ZZSelector::SetupNewDirectory();
   
    // Insure that hist ranges are exactly the same as ZZSelector, just change name
    for (const auto && obj : *currentHistDir_) {
        std::string name = obj->GetName();
        //std::cout<<"histname: " <<name<<std::endl;
        TNamed* named = dynamic_cast<TNamed*>(obj);
        named->SetName(name.insert(name.length()-4, "Fakes_").c_str());
        //std::cout<<"histname in BkgSelector: " <<named<<std::endl;
    } 
      AddObject<TH1D>(Z1MassHistPPPF_, ("Z1Mass_PPPF_"+channelName_).c_str(), "Z1Mass; m_{Z_{1}} [GeV]; Events;", 60, 0, 120);
      AddObject<TH1D>(Z1MassHistPPFF_, ("Z1Mass_PPFF_"+channelName_).c_str(), "Z1Mass; m_{Z_{1}} [GeV]; Events;", 60, 0, 120);
      
      AddObject<TH1D>(Z2MassHistPPPF_, ("Z2Mass_PPPF_"+channelName_).c_str(), "Z2Mass; m_{Z_{2}} [GeV]; Events;", 60, 0, 120);
      AddObject<TH1D>(Z2MassHistPPFF_, ("Z2Mass_PPFF_"+channelName_).c_str(), "Z2Mass; m_{Z_{2}} [GeV]; Events;", 60, 0, 120);

      AddObject<TH1D>(MassHistPPPF_, ("Mass_PPPF_"+channelName_).c_str(), "Mass; m_{4l} [GeV]; Events;", 32, 0, 800);
      AddObject<TH1D>(MassHistPPFF_, ("Mass_PPFF_"+channelName_).c_str(), "Mass; m_{4l} [GeV]; Events;", 32, 0, 800);
}
float ZZBackgroundSelector::getEventWeight(Long64_t entry) {
    float evtwgt = 0;

   // if(channel_ == mmee){
   //   if(e1e2IsZ1(entry))
   //     weight=0;
   //     //Makes weight 0 if Z1 is ee hence should not go in _mmee histos
   // }
   // else if(channel_ == eemm){
   //   if(!(e1e2IsZ1(entry)))
   //     weight=0;
   //     //Makes weight 0 if Z1 is mm hence should not go in _eemm 
   // }
    //std::cout<<"Weight in Bkg Seletor getEventWeight function: "<<weight<<std::endl;
    if (IsPPPFRegion()) {
        if (true){
            Z1MassHistPPPF_->Fill(Z1mass, weight);
            Z2MassHistPPPF_->Fill(Z2mass, weight);
            MassHistPPPF_->Fill(Mass,weight);
        }
        evtwgt = (getl4FakeRate(entry)*weight);
    }
    else if (IsPPFFRegion()) {
        if (true) {
            Z1MassHistPPFF_->Fill(Z1mass, weight);
            Z2MassHistPPFF_->Fill(Z2mass, weight);
            MassHistPPFF_->Fill(Mass,weight);
        }
        evtwgt = ((-1*getl3FakeRate(entry)*getl4FakeRate(entry))*weight);
    }
    //std::cout<<"eventWeight after Fake Rate: "<<evtwgt<<std::endl;
    return evtwgt;
}

Bool_t ZZBackgroundSelector::Process(Long64_t entry)
{
    std::pair<Systematic, std::string> central_var = std::make_pair(Central, "");

    LoadBranches(entry, central_var);
    //if (!PassesBaseSelection(entry, false, selection_))
    //    return true;
    //float event_weight=getEventWeight(entry);
    if (HZZSelection()) {
      if (TestMuons()) {
      //if(getEventWeight(entry)>0){
      //std::cout<<"weight before filling: "<<getEventWeight(entry)<<std::endl;}
        FillHistograms(entry, getEventWeight(entry), true, central_var);
    }
    }
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
    if ((channel_ == eeee) || (channel_ == eemm && !(e1e2IsZ1(entry))) || (channel_ == mmee && !(e1e2IsZ1(entry)))){
        fr = fakeRate_allE_->GetBinContent(fakeRate_allE_->FindBin(pt_fillval, std::abs(l3Eta)));
        //std::cout<<"channel: "<<channel_<<std::endl;
        //std::cout<<"l3 E Fake Rate: "<<fr<<std::endl;
    }
    else{
        fr = fakeRate_allMu_->GetBinContent(fakeRate_allMu_->FindBin(pt_fillval, std::abs(l3Eta)));
        //std::cout<<"channel: "<<channel_<<std::endl;
        //std::cout<<"l3 Mu Fake Rate: "<<fr<<std::endl;
    }
    return fr/(1-fr); 
}

float ZZBackgroundSelector::getl4FakeRate(Long64_t entry) {
    float pt_fillval = l4Pt < FR_MAX_PT_ ? l4Pt : FR_MAX_PT_ - 0.01;
    float fr = 1;
    if ((channel_ == eeee) || (channel_ == eemm && !(e1e2IsZ1(entry))) || (channel_ == mmee && !(e1e2IsZ1(entry)))){
        fr = fakeRate_allE_->GetBinContent(fakeRate_allE_->FindBin(pt_fillval, std::abs(l4Eta)));
        //std::cout<<"channel: "<<channel_<<std::endl;
        //std::cout<<"l4 E Fake Rate: "<<fr<<std::endl;
    }
    else{
        fr = fakeRate_allMu_->GetBinContent(fakeRate_allMu_->FindBin(pt_fillval, std::abs(l4Eta)));
        //std::cout<<"channel: "<<channel_<<std::endl;
        //std::cout<<"l4 Mu Fake Rate: "<<fr<<std::endl;
    }
    return fr/(1-fr); 
}
bool ZZBackgroundSelector::IsPPPFRegion() {
    return tightZ1Leptons() && lep3IsTight() && !lep4IsTight();
}

bool ZZBackgroundSelector::IsPPFFRegion() {
    return tightZ1Leptons() && !lep3IsTight() && !lep4IsTight();
}

