#include "Analysis/VVAnalysis/interface/ZZSelector.h"
#include "TLorentzVector.h"
#include <boost/algorithm/string.hpp>

void ZZSelector::Init(TTree *tree)
{
    ZZSelectorBase::Init(tree);

    //weight_info_ = 0;
    if (isMC_) {
        fChain->SetBranchAddress("nTruePU", &nTruePU, &b_nTruePU);
      //weight_info_ = GetLheWeightInfo();
    }
    fChain->SetBranchAddress("Mass", &Mass, &b_Mass);
    fChain->SetBranchAddress("Pt", &Pt, &b_Pt);
    //std::cout<<"Is it able to initialize"<<std::endl; 
}
void ZZSelector::LoadBranches(Long64_t entry, std::pair<Systematic, std::string> variation) { 
    ZZSelectorBase::Process(entry);

    //b_MtToMET->GetEntry(entry);
    //b_l1Pt->GetEntry(entry);
    //b_l2Pt->GetEntry(entry);
    //b_l3Pt->GetEntry(entry);
    b_Mass->GetEntry(entry);
    b_Pt->GetEntry(entry);
    //std::cout<<"channel in LoadBranches function: "<<channel_<<std::endl;
    if(channel_ == eemm || channel_ == mmee){
      //if(TightZZLeptons()){//i don't think this condition is needed even though it might save time but it messes up sf application for CRs in eemm,mmee states
      if(TightZZLeptons()){
        SetVariables(entry);}
    } 
    if (isMC_) {
        //In order to get around the Overflow issue, set the Pt, not ideal.
        //std::cout<<"weight before SF: "<<weight<<std::endl;
        if (channel_ == eeee) {
            //In order to get around the Overflow issue, set the Pt, not ideal.
            float pt_e1 = l1Pt < EleSF_MAX_PT_ ? l1Pt : EleSF_MAX_PT_ - 0.01;
            float pt_e2 = l2Pt < EleSF_MAX_PT_ ? l2Pt : EleSF_MAX_PT_ - 0.01;
            float pt_e3 = l3Pt < EleSF_MAX_PT_ ? l3Pt : EleSF_MAX_PT_ - 0.01;
            float pt_e4 = l4Pt < EleSF_MAX_PT_ ? l4Pt : EleSF_MAX_PT_ - 0.01;
            if(l1IsGap){
              weight *= eGapIdSF_->Evaluate2D(std::abs(l1Eta), pt_e1);
              //std::cout<<"weight for GapE: "<<weight<<std::endl;
            }
            else{
              weight *= eIdSF_->Evaluate2D(std::abs(l1Eta), pt_e1);
            }
            if(l2IsGap){
              weight *= eGapIdSF_->Evaluate2D(std::abs(l2Eta), pt_e2);
            }
            else{
              weight *= eIdSF_->Evaluate2D(std::abs(l2Eta), pt_e2);
            }
            if(l3IsGap){
              weight *= eGapIdSF_->Evaluate2D(std::abs(l3Eta), pt_e3);
            }
            else{
              weight *= eIdSF_->Evaluate2D(std::abs(l3Eta), pt_e3);
            }
            if(l4IsGap){
              weight *= eGapIdSF_->Evaluate2D(std::abs(l4Eta), pt_e4);
            }
            else{
              weight *= eIdSF_->Evaluate2D(std::abs(l4Eta), pt_e4);
            }
            //Applying Electron Reco SFs
            if(l1Pt < 20){ 
              weight *= eLowRecoSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
            }
            else{ 
              weight *= eRecoSF_->Evaluate2D(std::abs(l1Eta), pt_e1);
            }
            if(l2Pt < 20){ 
              weight *= eLowRecoSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
            }
            else{ 
              weight *= eRecoSF_->Evaluate2D(std::abs(l2Eta), pt_e2);
            }
            if(l3Pt < 20){ 
              weight *= eLowRecoSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
            }
            else{ 
              weight *= eRecoSF_->Evaluate2D(std::abs(l3Eta), pt_e3);
            }
            if(l4Pt < 20){ 
              weight *= eLowRecoSF_->Evaluate2D(std::abs(l4Eta), l4Pt);
            }
            else{ 
              weight *= eRecoSF_->Evaluate2D(std::abs(l4Eta), pt_e4);
            }
        }
        else if (channel_ == eemm) {
            //In order to get around the Overflow issue, set the Pt, not ideal.
            float pt_e1 = l1Pt < EleSF_MAX_PT_ ? l1Pt : EleSF_MAX_PT_ - 0.01;
            float pt_e2 = l2Pt < EleSF_MAX_PT_ ? l2Pt : EleSF_MAX_PT_ - 0.01;
            float pt_m3 = l3Pt < MuSF_MAX_PT_ ? l3Pt : MuSF_MAX_PT_ - 0.01;
            float pt_m4 = l4Pt < MuSF_MAX_PT_ ? l4Pt : MuSF_MAX_PT_ - 0.01;
            if(l1IsGap){
              weight *= eGapIdSF_->Evaluate2D(std::abs(l1Eta), pt_e1);
            }
            else{
              weight *= eIdSF_->Evaluate2D(std::abs(l1Eta), pt_e1);
            }
            if(l2IsGap){
              weight *= eGapIdSF_->Evaluate2D(std::abs(l2Eta), pt_e2);
            }
            else{
              weight *= eIdSF_->Evaluate2D(std::abs(l2Eta), pt_e2);
            }
            //Applying Electron Reco SFs
            if(l1Pt < 20){ 
              weight *= eLowRecoSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
            }
            else{ 
              weight *= eRecoSF_->Evaluate2D(std::abs(l1Eta), pt_e1);
            }
            if(l2Pt < 20){ 
              weight *= eLowRecoSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
            }
            else{ 
              weight *= eRecoSF_->Evaluate2D(std::abs(l2Eta), pt_e2);
            }
            weight *= mIdSF_->Evaluate2D(std::abs(l3Eta), pt_m3);
            weight *= mIdSF_->Evaluate2D(std::abs(l4Eta), pt_m4);
        }
        else if (channel_ == mmee) {
            float pt_m1 = l1Pt < MuSF_MAX_PT_ ? l1Pt : MuSF_MAX_PT_ - 0.01;
            float pt_m2 = l2Pt < MuSF_MAX_PT_ ? l2Pt : MuSF_MAX_PT_ - 0.01;
            float pt_e3 = l3Pt < EleSF_MAX_PT_ ? l3Pt : EleSF_MAX_PT_ - 0.01;
            float pt_e4 = l4Pt < EleSF_MAX_PT_ ? l4Pt : EleSF_MAX_PT_ - 0.01;

            weight *= mIdSF_->Evaluate2D(std::abs(l1Eta), pt_m1);
            weight *= mIdSF_->Evaluate2D(std::abs(l2Eta), pt_m2);
            if(l3IsGap){
              weight *= eGapIdSF_->Evaluate2D(std::abs(l3Eta), pt_e3);
            }
            else{
              weight *= eIdSF_->Evaluate2D(std::abs(l3Eta), pt_e3);
            }
            if(l4IsGap){
              weight *= eGapIdSF_->Evaluate2D(std::abs(l4Eta), pt_e4);
            }
            else{
              weight *= eIdSF_->Evaluate2D(std::abs(l4Eta), pt_e4);
            }
            //Applying Electron Reco SFs
            if(l3Pt < 20){ 
              weight *= eLowRecoSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
            }
            else{ 
              weight *= eRecoSF_->Evaluate2D(std::abs(l3Eta), pt_e3);
            }
            if(l4Pt < 20){ 
              weight *= eLowRecoSF_->Evaluate2D(std::abs(l4Eta), l4Pt);
            }
            else{ 
              weight *= eRecoSF_->Evaluate2D(std::abs(l4Eta), pt_e4);
            }
        }
        else {
            float pt_m1 = l1Pt < MuSF_MAX_PT_ ? l1Pt : MuSF_MAX_PT_ - 0.01;
            float pt_m2 = l2Pt < MuSF_MAX_PT_ ? l2Pt : MuSF_MAX_PT_ - 0.01;
            float pt_m3 = l3Pt < MuSF_MAX_PT_ ? l3Pt : MuSF_MAX_PT_ - 0.01;
            float pt_m4 = l4Pt < MuSF_MAX_PT_ ? l4Pt : MuSF_MAX_PT_ - 0.01;
            weight *= mIdSF_->Evaluate2D(std::abs(l1Eta), pt_m1);
            weight *= mIdSF_->Evaluate2D(std::abs(l2Eta), pt_m2);
            weight *= mIdSF_->Evaluate2D(std::abs(l3Eta), pt_m3);
            weight *= mIdSF_->Evaluate2D(std::abs(l4Eta), pt_m4);
        }
        weight *= pileupSF_->Evaluate1D(nTruePU);
        //std::cout<<"weight after muon sf applied: "<<weight<<std::endl;
      //Systematic uncertainties and creating shiftUp and shiftDown histograms
      //Starting with lepton Efficiencies  
        if (variation.first == electronEfficiencyUp || variation.first == electronEfficiencyDown || variation.first == electronRecoEffUp || variation.first == electronRecoEffDown || 
          variation.first == muonEfficiencyUp || variation.first == muonEfficiencyDown) {
          //std::cout<<"weight before mu sys is applied: "<<weight<<std::endl;
          ShiftEfficiencies(variation.first);
          //std::cout<<"l1Pt: "<<l1Pt<<" l2Pt: "<<l2Pt<<" l3Pt: "<<l3Pt<<" l4Pt: "<<l4Pt<<std::endl;
          //std::cout<<"weight after 4mu sys is applied: "<<weight<<std::endl;
        }
        else if (variation.first == pileupUp) {
             weight *= pileupSF_->Evaluate1D(nTruePU, ScaleFactor::ShiftUp)/pileupSF_->Evaluate1D(nTruePU);
        }
        else if (variation.first == pileupDown) {
            weight *= pileupSF_->Evaluate1D(nTruePU, ScaleFactor::ShiftDown)/pileupSF_->Evaluate1D(nTruePU);
      }
    }
    auto deltaPhiZZ = [](float phi1, float phi2) {
      float pi = TMath::Pi();
      float dphi = fabs(phi1-phi2);
      if(dphi>pi)
          dphi = 2.0*pi - dphi;
      return dphi;
    };

    dPhiZZ = deltaPhiZZ(Z1Phi,Z2Phi);
}
//Similar to Kenneth's SetShiftedMasses function which i will need later as well
void ZZSelector::SetVariables(Long64_t entry) {
    if(!(e1e2IsZ1(entry))){
      //std::cout<<"e1e2IsZ1 is working"<<std::endl;
      float tempMass = Z1mass;
      Z1mass = Z2mass;
      Z2mass = tempMass;
      float tempPt = Z1pt;
      Z1pt = Z2pt;
      Z2pt = tempPt;
      bool templ1IsTight = l1IsTight;
      l1IsTight = l3IsTight;
      l3IsTight = templ1IsTight;
      bool templ2IsTight = l2IsTight;
      l2IsTight = l4IsTight;
      l4IsTight = templ2IsTight;
      bool templ1IsIso = l1IsIso;
      l1IsIso = l3IsIso;
      l3IsIso = templ1IsIso;
      bool templ2IsIso = l2IsIso;
      l2IsIso = l4IsIso;
      l4IsIso = templ2IsIso;
      bool templ1IsGap = l1IsGap;
      l1IsGap = l3IsGap;
      l3IsGap = templ1IsGap;
      bool templ2IsGap = l2IsGap;
      l2IsGap = l4IsGap;
      l4IsGap = templ2IsGap;
      float templ1Pt = l1Pt;
      l1Pt = l3Pt;
      l3Pt = templ1Pt;
      float templ2Pt = l2Pt;
      l2Pt = l4Pt;
      l4Pt = templ2Pt;
      float templ1Eta = l1Eta;
      l1Eta = l3Eta;
      l3Eta = templ1Eta;
      float templ2Eta = l2Eta;
      l2Eta = l4Eta;
      l4Eta = templ2Eta;
      float templ1Phi = l1Phi;
      l1Phi = l3Phi;
      l3Phi = templ1Phi;
      float templ2Phi = l2Phi;
      l2Phi = l4Phi;
      l4Phi = templ2Phi;
      float templ1SIP3D = l1SIP3D;
      l1SIP3D = l3SIP3D;
      l3SIP3D = templ1SIP3D;
      float templ2SIP3D = l2SIP3D;
      l2SIP3D = l4SIP3D;
      l4SIP3D = templ2SIP3D;
      int templ1PdgId = l1PdgId;
      l1PdgId = l3PdgId;
      l3PdgId = templ1PdgId;
      int templ2PdgId = l2PdgId;
      l2PdgId = l4PdgId;
      l4PdgId = templ2PdgId;
      float templ1Mass = l1Mass;
      l1Mass = l3Mass;
      l3Mass = templ1Mass;
      float templ2Mass = l2Mass;
      l2Mass = l4Mass;
      l4Mass = templ2Mass;
    }
}
void ZZSelector::ShiftEfficiencies(Systematic variation) {
    ScaleFactor::Variation shift = ScaleFactor::Variation::ShiftUp;
    if (variation == electronEfficiencyDown || variation == electronRecoEffDown || variation == muonEfficiencyDown)
        shift = ScaleFactor::Variation::ShiftDown;

    if (channel_ == eeee){ 
      float pt_e1 = l1Pt < EleSF_MAX_PT_ ? l1Pt : EleSF_MAX_PT_ - 0.01;
      float pt_e2 = l2Pt < EleSF_MAX_PT_ ? l2Pt : EleSF_MAX_PT_ - 0.01;
      float pt_e3 = l3Pt < EleSF_MAX_PT_ ? l3Pt : EleSF_MAX_PT_ - 0.01;
      float pt_e4 = l4Pt < EleSF_MAX_PT_ ? l4Pt : EleSF_MAX_PT_ - 0.01;
      if (variation == electronRecoEffUp || variation == electronRecoEffDown) {
        //Applying Electron Reco SFs Up/Down for ElectronRecoSyst
        if(l1Pt < 20){ 
          weight *= eLowRecoSF_->Evaluate2D(std::abs(l1Eta), l1Pt,shift)/eLowRecoSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
        }
        else{ 
          weight *= eRecoSF_->Evaluate2D(std::abs(l1Eta), pt_e1,shift)/eRecoSF_->Evaluate2D(std::abs(l1Eta), pt_e1);
        }
        if(l2Pt < 20){ 
          weight *= eLowRecoSF_->Evaluate2D(std::abs(l2Eta), l2Pt,shift)/eLowRecoSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
        }
        else{ 
          weight *= eRecoSF_->Evaluate2D(std::abs(l2Eta), pt_e2,shift)/eRecoSF_->Evaluate2D(std::abs(l2Eta), pt_e2);
        }
        if(l3Pt < 20){ 
          weight *= eLowRecoSF_->Evaluate2D(std::abs(l3Eta), l3Pt,shift)/eLowRecoSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
        }
        else{ 
          weight *= eRecoSF_->Evaluate2D(std::abs(l3Eta), pt_e3,shift)/eRecoSF_->Evaluate2D(std::abs(l3Eta), pt_e3) ;
        }
        if(l4Pt < 20){ 
          weight *= eLowRecoSF_->Evaluate2D(std::abs(l4Eta), l4Pt,shift)/eLowRecoSF_->Evaluate2D(std::abs(l4Eta), l4Pt);
        }
        else{ 
          weight *= eRecoSF_->Evaluate2D(std::abs(l4Eta), pt_e4,shift)/eRecoSF_->Evaluate2D(std::abs(l4Eta), pt_e4);
        }
      }
      //Applying Electron ID SFs Up/Down for ElectronIDEffSyst
      else if (variation == electronEfficiencyUp || variation == electronEfficiencyDown) {
        if(l1IsGap){
          weight *= eGapIdSF_->Evaluate2D(std::abs(l1Eta), pt_e1, shift)/eGapIdSF_->Evaluate2D(std::abs(l1Eta), pt_e1);
        }
        else{
          weight *= eIdSF_->Evaluate2D(std::abs(l1Eta), pt_e1, shift)/eIdSF_->Evaluate2D(std::abs(l1Eta), pt_e1);
        }
        if(l2IsGap){
          weight *= eGapIdSF_->Evaluate2D(std::abs(l2Eta), pt_e2, shift)/eGapIdSF_->Evaluate2D(std::abs(l2Eta), pt_e2);
        }
        else{
          weight *= eIdSF_->Evaluate2D(std::abs(l2Eta), pt_e2, shift)/eIdSF_->Evaluate2D(std::abs(l2Eta), pt_e2);
        }
        if(l3IsGap){
          weight *= eGapIdSF_->Evaluate2D(std::abs(l3Eta), pt_e3, shift)/eGapIdSF_->Evaluate2D(std::abs(l3Eta), pt_e3);
        }
        else{
          weight *= eIdSF_->Evaluate2D(std::abs(l3Eta), pt_e3, shift)/eIdSF_->Evaluate2D(std::abs(l3Eta), pt_e3);
        }
        if(l4IsGap){
          weight *= eGapIdSF_->Evaluate2D(std::abs(l4Eta), pt_e4, shift)/eGapIdSF_->Evaluate2D(std::abs(l4Eta), pt_e4);
        }
        else{
          weight *= eIdSF_->Evaluate2D(std::abs(l4Eta), pt_e4, shift)/eIdSF_->Evaluate2D(std::abs(l4Eta), pt_e4);
        }
    }
  }//channel eeee
    else if (channel_ == eemm) {
      //In order to get around the Overflow issue, set the Pt, not ideal.
      float pt_e1 = l1Pt < EleSF_MAX_PT_ ? l1Pt : EleSF_MAX_PT_ - 0.01;
      float pt_e2 = l2Pt < EleSF_MAX_PT_ ? l2Pt : EleSF_MAX_PT_ - 0.01;
      float pt_m3 = l3Pt < MuSF_MAX_PT_ ? l3Pt : MuSF_MAX_PT_ - 0.01;
      float pt_m4 = l4Pt < MuSF_MAX_PT_ ? l4Pt : MuSF_MAX_PT_ - 0.01;
      if (variation == electronRecoEffUp || variation == electronRecoEffDown) {
        //Applying Electron Reco SFs Up/Down for ElectronRecoEffSyst
        if(l1Pt < 20){ 
          weight *= eLowRecoSF_->Evaluate2D(std::abs(l1Eta), l1Pt,shift)/eLowRecoSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
        }
        else{ 
          weight *= eRecoSF_->Evaluate2D(std::abs(l1Eta), pt_e1,shift)/eRecoSF_->Evaluate2D(std::abs(l1Eta), pt_e1);
        }
        if(l2Pt < 20){ 
          weight *= eLowRecoSF_->Evaluate2D(std::abs(l2Eta), l2Pt,shift)/eLowRecoSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
        }
        else{ 
          weight *= eRecoSF_->Evaluate2D(std::abs(l2Eta), pt_e2,shift)/eRecoSF_->Evaluate2D(std::abs(l2Eta), pt_e2);
        }
      }
      //Applying Electron ID SFs Up/Down for ElectronIDEffSyst
      else if (variation == electronEfficiencyUp || variation == electronEfficiencyDown) {
        if(l1IsGap){
          weight *= eGapIdSF_->Evaluate2D(std::abs(l1Eta), pt_e1, shift)/eGapIdSF_->Evaluate2D(std::abs(l1Eta), pt_e1);
        }
        else{
          weight *= eIdSF_->Evaluate2D(std::abs(l1Eta), pt_e1, shift)/eIdSF_->Evaluate2D(std::abs(l1Eta), pt_e1);
        }
        if(l2IsGap){
          weight *= eGapIdSF_->Evaluate2D(std::abs(l2Eta), pt_e2, shift)/eGapIdSF_->Evaluate2D(std::abs(l2Eta), pt_e2);
        }
        else{
          weight *= eIdSF_->Evaluate2D(std::abs(l2Eta), pt_e2, shift)/eIdSF_->Evaluate2D(std::abs(l2Eta), pt_e2);
        }
      }
      else if (variation == muonEfficiencyUp || variation == muonEfficiencyDown) {
          weight *= mIdSF_->Evaluate2D(std::abs(l3Eta), pt_m3, shift)/mIdSF_->Evaluate2D(std::abs(l3Eta), pt_m3);
          weight *= mIdSF_->Evaluate2D(std::abs(l4Eta), pt_m4, shift)/mIdSF_->Evaluate2D(std::abs(l4Eta), pt_m4);
      }
    }
    else if (channel_ == mmee) {
      float pt_m1 = l1Pt < MuSF_MAX_PT_ ? l1Pt : MuSF_MAX_PT_ - 0.01;
      float pt_m2 = l2Pt < MuSF_MAX_PT_ ? l2Pt : MuSF_MAX_PT_ - 0.01;
      float pt_e3 = l3Pt < EleSF_MAX_PT_ ? l3Pt : EleSF_MAX_PT_ - 0.01;
      float pt_e4 = l4Pt < EleSF_MAX_PT_ ? l4Pt : EleSF_MAX_PT_ - 0.01;
      if (variation == muonEfficiencyUp || variation == muonEfficiencyDown) {
          weight *= mIdSF_->Evaluate2D(std::abs(l1Eta), pt_m1, shift)/mIdSF_->Evaluate2D(std::abs(l1Eta), pt_m1);
          weight *= mIdSF_->Evaluate2D(std::abs(l2Eta), pt_m2, shift)/mIdSF_->Evaluate2D(std::abs(l2Eta), pt_m2);
      }
      else if (variation == electronRecoEffUp || variation == electronRecoEffDown) {
        //Applying Electron Reco SFs Up/Down for ElectronRecoEffSyst 
        if(l3Pt < 20){ 
          weight *= eLowRecoSF_->Evaluate2D(std::abs(l3Eta), l3Pt,shift)/eLowRecoSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
        }
        else{ 
          weight *= eRecoSF_->Evaluate2D(std::abs(l3Eta), pt_e3,shift)/eRecoSF_->Evaluate2D(std::abs(l3Eta), pt_e3) ;
        }
        if(l4Pt < 20){ 
          weight *= eLowRecoSF_->Evaluate2D(std::abs(l4Eta), l4Pt,shift)/eLowRecoSF_->Evaluate2D(std::abs(l4Eta), l4Pt);
        }
        else{ 
          weight *= eRecoSF_->Evaluate2D(std::abs(l4Eta), pt_e4,shift)/eRecoSF_->Evaluate2D(std::abs(l4Eta), pt_e4);
        }
      }
      //Applying Electron ID SFs Up/Down for ElectronIDEffSyst
      else if (variation == electronEfficiencyUp || variation == electronEfficiencyDown) {
        if(l3IsGap){
          weight *= eGapIdSF_->Evaluate2D(std::abs(l3Eta), pt_e3, shift)/eGapIdSF_->Evaluate2D(std::abs(l3Eta), pt_e3);
        }
        else{
          weight *= eIdSF_->Evaluate2D(std::abs(l3Eta), pt_e3, shift)/eIdSF_->Evaluate2D(std::abs(l3Eta), pt_e3);
        }
        if(l4IsGap){
          weight *= eGapIdSF_->Evaluate2D(std::abs(l4Eta), pt_e4, shift)/eGapIdSF_->Evaluate2D(std::abs(l4Eta), pt_e4);
        }
        else{
          weight *= eIdSF_->Evaluate2D(std::abs(l4Eta), pt_e4, shift)/eIdSF_->Evaluate2D(std::abs(l4Eta), pt_e4);
        }
      }
    }
    else if (channel_ == mmmm && (variation == muonEfficiencyUp || variation == muonEfficiencyDown)) {
      float pt_m1 = l1Pt < MuSF_MAX_PT_ ? l1Pt : MuSF_MAX_PT_ - 0.01;
      float pt_m2 = l2Pt < MuSF_MAX_PT_ ? l2Pt : MuSF_MAX_PT_ - 0.01;
      float pt_m3 = l3Pt < MuSF_MAX_PT_ ? l3Pt : MuSF_MAX_PT_ - 0.01;
      float pt_m4 = l4Pt < MuSF_MAX_PT_ ? l4Pt : MuSF_MAX_PT_ - 0.01;
      weight *= mIdSF_->Evaluate2D(std::abs(l1Eta), pt_m1, shift)/mIdSF_->Evaluate2D(std::abs(l1Eta), pt_m1);
      //std::cout<<"l1 Shift: "<< mIdSF_->Evaluate2D(std::abs(l1Eta), pt_m1, shift)<<std::endl;
      //std::cout<<"l1 sf: "<< mIdSF_->Evaluate2D(std::abs(l1Eta), pt_m1)<<std::endl;
      weight *= mIdSF_->Evaluate2D(std::abs(l2Eta), pt_m2, shift)/mIdSF_->Evaluate2D(std::abs(l2Eta), pt_m2);
      weight *= mIdSF_->Evaluate2D(std::abs(l3Eta), pt_m3, shift)/mIdSF_->Evaluate2D(std::abs(l3Eta), pt_m3);
      //std::cout<<"l3 Shift: "<< mIdSF_->Evaluate2D(std::abs(l3Eta), pt_m3, shift)<<std::endl;
      //std::cout<<"l3 sf: "<< mIdSF_->Evaluate2D(std::abs(l3Eta), pt_m3)<<std::endl;
      weight *= mIdSF_->Evaluate2D(std::abs(l4Eta), pt_m4, shift)/mIdSF_->Evaluate2D(std::abs(l4Eta), pt_m4);
    }
}
bool ZZSelector::TightZZLeptons() {
    if (tightZ1Leptons() && tightZ2Leptons())
        return true;
    else
        return false;
}
bool ZZSelector::ZZSelection() {
    if ((Z1mass > 60.0 && Z1mass < 120.0) && (Z2mass > 60.0 && Z2mass < 120.0))
        return true;
    else
        return false;
}
//We already require 4 < Z1,Z2 < 120  in the "Loose Skim"
bool ZZSelector::ZSelection() {
    if (Z1mass > 40.0 && Z2mass > 12.0)
        return true;
    else
        return false;
}
bool ZZSelector::Z4lSelection() {
    if (Mass > 80.0 && Mass < 100.0)
        return true;
    else
        return false;
}
bool ZZSelector::HZZSIPSelection(){
    if ((l1SIP3D < 4.0 && l2SIP3D < 4.0 && l3SIP3D < 4.0 && l4SIP3D < 4.0))
        return true;
    else
        return false;
}
bool ZZSelector::TestMuons(){
    if ((Z1mass > 82.0 && Z1mass < 102.0) && (Z2mass < 40.0))
        return true;
    else
        return false;
}

std::string ZZSelector::getHistName(std::string histName, std::string variationName) {
    return variationName == "" ? histName : histName + "_" + variationName;
}

void ZZSelector::FillHistograms(Long64_t entry, float weight, bool noBlind, 
        std::pair<Systematic, std::string> variation) { 
    //bool passesVBS = PassesVBSSelection(noBlind);
    if (hists1D_[getHistName("backgroundControlYield", variation.second)] != nullptr)
        if (true)
            hists1D_[getHistName("backgroundControlYield", variation.second)]->Fill(1, weight);

    //if ((variation.first == Central || (doaQGC_ && isaQGC_)) && isMC_) 
    if(isMC_){
        //SafeHistFill(hists1D_, getHistName("duplicated", variation.second), duplicated, 1);
        for (size_t i = 0; i < lheWeights.size(); i++) {
              SafeHistFill(weighthists_, "backgroundControlYield", 1, i, lheWeights[i]/lheWeights[0]*weight);
              SafeHistFill(weighthists_, getHistName("yield", variation.second), 1, i, lheWeights[i]/lheWeights[0]*weight);
              SafeHistFill(weighthists_, getHistName("Mass", variation.second), Mass, i, lheWeights[i]/lheWeights[0]*weight);

        }
    }
    SafeHistFill(hists1D_, getHistName("nTruePU", variation.second), nTruePU, weight);
    SafeHistFill(hists1D_, getHistName("yield", variation.second), 1, weight);
    SafeHistFill(hists1D_, getHistName("Mass", variation.second), Mass,weight);
    SafeHistFill(hists1D_, getHistName("ZMass", variation.second), Z1mass, weight);
    SafeHistFill(hists1D_, getHistName("ZMass", variation.second), Z2mass, weight);
    //Making LeptonPt and Eta plots
    SafeHistFill(hists1D_, getHistName("LepPt", variation.second), l1Pt, weight);
    SafeHistFill(hists1D_, getHistName("LepPt", variation.second), l2Pt, weight);
    SafeHistFill(hists1D_, getHistName("LepPt", variation.second), l3Pt, weight);
    SafeHistFill(hists1D_, getHistName("LepPt", variation.second), l4Pt, weight);
    SafeHistFill(hists1D_, getHistName("LepEta", variation.second), l1Eta, weight);
    SafeHistFill(hists1D_, getHistName("LepEta", variation.second), l2Eta, weight);
    SafeHistFill(hists1D_, getHistName("LepEta", variation.second), l3Eta, weight);
    SafeHistFill(hists1D_, getHistName("LepEta", variation.second), l4Eta, weight);
    // Summing 12,34 leptons
    SafeHistFill(hists1D_, getHistName("Lep12Pt", variation.second), l1Pt, weight);
    SafeHistFill(hists1D_, getHistName("Lep12Pt", variation.second), l2Pt, weight);
    SafeHistFill(hists1D_, getHistName("Lep34Pt", variation.second), l3Pt, weight);
    SafeHistFill(hists1D_, getHistName("Lep34Pt", variation.second), l4Pt, weight);
    SafeHistFill(hists1D_, getHistName("Lep12Eta", variation.second), l1Eta, weight);
    SafeHistFill(hists1D_, getHistName("Lep12Eta", variation.second), l2Eta, weight);
    SafeHistFill(hists1D_, getHistName("Lep34Eta", variation.second), l3Eta, weight);
    SafeHistFill(hists1D_, getHistName("Lep34Eta", variation.second), l4Eta, weight);
    SafeHistFill(hists1D_, getHistName("Z1Mass", variation.second), Z1mass, weight);
    SafeHistFill(hists1D_, getHistName("Z2Mass", variation.second), Z2mass, weight);
    SafeHistFill(hists1D_, getHistName("ZPt", variation.second), Z1pt, weight);
    SafeHistFill(hists1D_, getHistName("ZPt", variation.second), Z2pt, weight);
    SafeHistFill(hists1D_, getHistName("Z1Pt", variation.second), Z1pt, weight);
    SafeHistFill(hists1D_, getHistName("Z2Pt", variation.second), Z2pt, weight);
    SafeHistFill(hists1D_, getHistName("ZZPt", variation.second), Pt, weight);
    SafeHistFill(hists1D_, getHistName("Z1Phi", variation.second), Z1Phi, weight);
    SafeHistFill(hists1D_, getHistName("Z2Phi", variation.second), Z2Phi, weight);
    SafeHistFill(hists1D_, getHistName("dPhiZ1Z2", variation.second), dPhiZZ, weight);
    SafeHistFill(hists1D_, getHistName("Z1lep1_Pt", variation.second), l1Pt, weight);
    SafeHistFill(hists1D_, getHistName("Z1lep1_Eta", variation.second), l1Eta, weight);
    SafeHistFill(hists1D_, getHistName("Z1lep1_Phi", variation.second), l1Phi, weight);
    SafeHistFill(hists1D_, getHistName("Z1lep1_PdgId", variation.second), l1PdgId, weight);
    SafeHistFill(hists1D_, getHistName("Z1lep2_Pt", variation.second), l2Pt, weight);
    SafeHistFill(hists1D_, getHistName("Z1lep2_Eta", variation.second), l2Eta, weight);
    SafeHistFill(hists1D_, getHistName("Z1lep2_Phi", variation.second), l2Phi, weight);
    SafeHistFill(hists1D_, getHistName("Z1lep2_PdgId", variation.second), l2PdgId, weight);
    SafeHistFill(hists1D_, getHistName("Z2lep1_Pt", variation.second), l3Pt, weight);
    SafeHistFill(hists1D_, getHistName("Z2lep1_Eta", variation.second), l3Eta, weight);
    SafeHistFill(hists1D_, getHistName("Z2lep1_Phi", variation.second), l3Phi, weight);
    SafeHistFill(hists1D_, getHistName("Z2lep1_PdgId", variation.second), l3PdgId, weight);
    SafeHistFill(hists1D_, getHistName("Z2lep2_Pt", variation.second), l4Pt, weight);
    SafeHistFill(hists1D_, getHistName("Z2lep2_Eta", variation.second), l4Eta, weight);
    SafeHistFill(hists1D_, getHistName("Z2lep2_Phi", variation.second), l4Phi, weight);
    SafeHistFill(hists1D_, getHistName("Z2lep2_PdgId", variation.second), l4PdgId, weight);
    SafeHistFill(hists2D_, getHistName("Z1lep1_Z1lep2_Pt",variation.second),l1Pt,l2Pt,weight);
    SafeHistFill(hists2D_, getHistName("Z1lep1_Z1lep2_Eta",variation.second),l1Eta,l2Eta,weight);
    SafeHistFill(hists2D_, getHistName("Z1lep1_Z1lep2_Phi",variation.second),l1Phi,l2Phi,weight);
    SafeHistFill(hists2D_, getHistName("Z2lep1_Z2lep2_Pt",variation.second),l3Pt,l4Pt,weight);
    SafeHistFill(hists2D_, getHistName("Z2lep1_Z2lep2_Eta",variation.second),l3Eta,l4Eta,weight);
    SafeHistFill(hists2D_, getHistName("Z2lep1_Z2lep2_Phi",variation.second),l3Phi,l4Phi,weight);
    //2D Z1 vs Z2
    SafeHistFill(hists2D_, getHistName("Z1Mass_Z2Mass",variation.second),Z1mass,Z2mass,weight);

    if (hists1D_[getHistName("nvtx", variation.second)] != nullptr) {
        b_nvtx->GetEntry(entry);
        hists1D_[getHistName("nvtx", variation.second)]->Fill(nvtx, weight);
    }
}

Bool_t ZZSelector::Process(Long64_t entry)
{
    bool blindVBS = false;

    std::pair<Systematic, std::string> central_var = std::make_pair(Central, "");
    LoadBranches(entry, central_var);
    //Define weight of event based on channel in case of eemm or mmee
    if (HZZSIPSelection() && ZZSelection() && TightZZLeptons()) {
      if (true) {
        //std::cout<<run<<":"<<lumi<<":"<<evt<<std::endl;
        //std::cout<<"Weight in ZZSelector inside HZZ: "<<weight<<std::endl;
        FillHistograms(entry, weight, !blindVBS, central_var);
    }
  }
    if (doSystematics_ && (isMC_ || isNonpromptEstimate_)) {
        for (const auto& systematic : systematics_) {
          //if(systematic.first==pileupUp || systematic.first==pileupDown){ 
          LoadBranches(entry, systematic);
          if (ZZSelection() && HZZSIPSelection() && TightZZLeptons()) {
              //std::cout<<run<<":"<<lumi<<":"<<evt<<std::endl;
              FillHistograms(entry, weight, !blindVBS, systematic);
              }
        }
    //}
    } 
    return true;
}

std::vector<std::string> ZZSelector::ReadHistData(std::string histDataString) {
    std::vector<std::string> histData;
    boost::split(histData, histDataString, boost::is_any_of("$"));
    std::vector<std::string> binInfo;
    if (histData.size() != 2)
        return {};
    
    boost::split(binInfo, histData[1], boost::is_any_of(","));
   
    histData.pop_back();
    for (const auto& x : binInfo) {
        histData.push_back(x);
    }
    
    return histData;
}

void ZZSelector::InitialzeHistogram(std::string name, std::vector<std::string> histData) {
    if (histData.size() != 4 && histData.size() != 7) {
        std::cerr << "Malformed data string for histogram '" << name
                    << ".' Must have form: 'Title; (optional info) $ nbins, xmin, xmax'"
                    << "\n   OR form: 'Title; (optional info) $ nbins, xmin, xmax nbinsy ymin ymax'"
                    << std::endl;
        exit(1);
    } 
    std::string hist_name = name+"_"+channelName_;
    int nbins = std::stoi(histData[1]);
    float xmin = std::stof(histData[2]);
    float xmax = std::stof(histData[3]);

    if (histData.size() == 4) {
          AddObject<TH1D>(hists1D_[name], hist_name.c_str(), histData[0].c_str(),nbins, xmin, xmax);
        if (doSystematics_ && std::find(systHists_.begin(), systHists_.end(), name) != systHists_.end()) {
            for (auto& syst : systematics_) {
                std::string syst_hist_name = name+"_"+syst.second;
                hists1D_[syst_hist_name] = {};
                AddObject<TH1D>(hists1D_[syst_hist_name], (syst_hist_name+"_"+channelName_).c_str(), 
                    histData[0].c_str(),nbins, xmin, xmax);
                //if (isaQGC_ && doaQGC_ && (weighthists_.find(name) != weighthists_.end())) { 
                //    std::string weightsyst_hist_name = name+"_lheWeights_"+syst.second;
                //    AddObject<TH2D>(weighthists_[syst_hist_name], 
                //        (weightsyst_hist_name+"_"+channelName_).c_str(), histData[0].c_str(),
                //        nbins, xmin, xmax, 1000, 0, 1000);
                //}
            }
        }
        // Weight hists must be subset of 1D hists!
        if (isMC_ && (weighthists_.find(name) != weighthists_.end())) { 
              AddObject<TH2D>(weighthists_[name], 
                  (name+"_lheWeights_"+channelName_).c_str(), histData[0].c_str(),
                  nbins, xmin, xmax, 1000, 0, 1000);
        }
    }
    else {
        int nbinsy = std::stoi(histData[4]);
        float ymin = std::stof(histData[5]);
        float ymax = std::stof(histData[6]);
          AddObject<TH2D>(hists2D_[name], hist_name.c_str(), histData[0].c_str(),nbins, xmin, xmax,
                nbinsy, ymin, ymax);
       // if (doSystematics_ && std::find(systHists2D_.begin(), systHists2D_.end(), name) != systHists2D_.end()) {
       //     for (auto& syst : systematics_) {
       //         std::string syst_hist_name = name+"_"+syst.second;
       //         hists2D_[syst_hist_name] = {};
       //         AddObject<TH2D>(hists2D_[syst_hist_name], (syst_hist_name+"_"+channelName_).c_str(), 
       //             histData[0].c_str(),nbins, xmin, xmax, nbinsy, ymin, ymax);
       //     }
       // }
        // 3D weight hists must be subset of 2D hists!
        if (isMC_ && (weighthists2D_.find(name) != weighthists2D_.end())) { 
            AddObject<TH3D>(weighthists2D_[name], 
                (name+"_lheWeights_"+channelName_).c_str(), histData[0].c_str(),
                nbins, xmin, xmax, nbinsy, ymin, ymax, 1000, 0, 1000);
        }
    }
}

void ZZSelector::SetupNewDirectory()
{
    ZZSelectorBase::SetupNewDirectory();
    //isaQGC_ = name_.find("aqgc") != std::string::npos;
    //applyFullSelection_ = (selection_ == VBSselection_Loose_Full ||
    //                  selection_ == VBSselection_Tight_Full || 
    //                  selection_ == VBSselection_NoZeppenfeld_Full || 
    //                  selection_ == Inclusive2Jet_Full ||
    //                  selection_ == Wselection_Full ||
    //                  selection_ == VBSBackgroundControl_Full ||
    //                  selection_ == VBSBackgroundControlLoose_Full);
    //doSystematics_ = applyFullSelection_;
    //doSystematics_ = false;
   
    TList* histInfo = (TList *) GetInputList()->FindObject("histinfo");
    if (histInfo == nullptr ) 
        Abort("Must pass histogram information");
    
    for (auto && entry : *histInfo) {  
        TNamed* currentHistInfo = dynamic_cast<TNamed*>(entry);
        std::string name = currentHistInfo->GetName();
        std::vector<std::string> histData = ReadHistData(currentHistInfo->GetTitle());
        if (hists2D_.find(name) != hists2D_.end() || hists1D_.find(name) != hists1D_.end()) { 
            InitialzeHistogram(name, histData);
        }
        else
            std::cerr << "Skipping invalid histogram " << name << std::endl;
    }
}
