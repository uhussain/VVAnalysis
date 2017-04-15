#include "Analysis/WZAnalysis/interface/WZBackgroundSelector.h"
#include <TStyle.h>

void WZBackgroundSelector::SlaveBegin(TTree * /*tree*/)
{
    fakeRate_allE_ = (ScaleFactor *) GetInputList()->FindObject("fakeRate_allE");
    if (fakeRate_allE_ == nullptr ) Abort("Must pass electron fake rate to input list!");
    fakeRate_allMu_ = (ScaleFactor *) GetInputList()->FindObject("fakeRate_allMu");
    if (fakeRate_allMu_ == nullptr ) Abort("Must pass muon fake rate to input list!");
}

void WZBackgroundSelector::SetupNewDirectory()
{
    WZSelector::SetupNewDirectory();
   
    // Insure that hist ranges are exactly the same as WZSelector, just change name
    for (const auto && obj : *currentHistDir_) {
        std::string name = obj->GetName();
        TNamed* named = dynamic_cast<TNamed*>(obj);
        named->SetName(name.insert(name.length()-3, "Fakes_").c_str());
    }
    
    AddObject<TH1D>(zmassHistPPF_, ("ZMass_PPF"+channelName_).c_str(), "ZMass; M_{ll} [GeV]; Events;", 15, 0, 150);
    AddObject<TH1D>(zmassHistPFP_, ("ZMass_PFP"+channelName_).c_str(), "ZMass; M_{ll} [GeV]; Events;", 15, 0, 150);
    AddObject<TH1D>(zmassHistFPP_, ("ZMass_FPP"+channelName_).c_str(), "ZMass; M_{ll} [GeV]; Events;", 15, 0, 150);
    AddObject<TH1D>(zmassHistFFP_, ("ZMass_FFP"+channelName_).c_str(), "ZMass; M_{ll} [GeV]; Events;", 15, 0, 150);
    AddObject<TH1D>(zmassHistFPF_, ("ZMass_FPF"+channelName_).c_str(), "ZMass; M_{ll} [GeV]; Events;", 15, 0, 150);
    AddObject<TH1D>(zmassHistPFF_, ("ZMass_PFF"+channelName_).c_str(), "ZMass; M_{ll} [GeV]; Events;", 15, 0, 150);
    AddObject<TH1D>(zmassHistFFF_, ("ZMass_FFF"+channelName_).c_str(), "ZMass; M_{ll} [GeV]; Events;", 15, 0, 150);
}

Bool_t WZBackgroundSelector::Process(Long64_t entry)
{
    LoadBranches(entry);
    
    if (!passesLeptonVeto)
        return true;

    float evtwgt = 0;
    if (IsFPPRegion()) {
        zmassHistFPP_->Fill(Zmass, genWeight);
        evtwgt = getl1FakeRate()*genWeight;
    }
    else if (IsPFPRegion()) {
        zmassHistPFP_->Fill(Zmass, genWeight);
        evtwgt = getl2FakeRate()*genWeight;
    }
    else if (IsPPFRegion()) {
        zmassHistPPF_->Fill(Zmass, genWeight);
        evtwgt = getl3FakeRate()*genWeight;
    }
    else if (IsFFFRegion()) {
        zmassHistFFF_->Fill(Zmass, genWeight);
        evtwgt = getl1FakeRate()*getl2FakeRate()*getl3FakeRate()*genWeight;
    }
    else if (IsFPFRegion()) {
        zmassHistFPF_->Fill(Zmass, genWeight);
        evtwgt = -1*getl1FakeRate()*getl3FakeRate()*genWeight;
    }
    else if (IsFFPRegion()) {
        zmassHistFFP_->Fill(Zmass, genWeight);
        evtwgt = -1*getl1FakeRate()*getl2FakeRate()*genWeight;
    }
    else if (IsPFFRegion()) {
        zmassHistPFF_->Fill(Zmass, genWeight);
        evtwgt = -1*getl2FakeRate()*getl3FakeRate()*genWeight;
    }
    else
        return true;
       
    FillHistograms(evtwgt, true);

    return true;
}

float WZBackgroundSelector::getl1FakeRate() {
    float pt_eval = l1Pt < 50 ? l1Pt : 49;
    float fr = 1;
    if (channel_ == eem|| channel_ == eee)
        fr = fakeRate_allE_->Evaluate2D(pt_eval, std::abs(l1Eta));
    else
        fr = fakeRate_allMu_->Evaluate2D(pt_eval, std::abs(l1Eta));
    return fr/(1-fr); 
}

float WZBackgroundSelector::getl2FakeRate() {
    float pt_eval = l2Pt < 50 ? l2Pt : 49;
    float fr = 1;
    if (channel_ == eem || channel_ == eee)
        fr = fakeRate_allE_->Evaluate2D(pt_eval, std::abs(l2Eta));
    else
        fr = fakeRate_allMu_->Evaluate2D(pt_eval, std::abs(l2Eta));
    return fr/(1-fr); 
}

float WZBackgroundSelector::getl3FakeRate() {
    float pt_eval = l3Pt < 50 ? l3Pt : 49;
    float fr = 1;
    if (channel_ == emm || channel_ == eee)
        fr = fakeRate_allE_->Evaluate2D(pt_eval, std::abs(l3Eta));
    else
        fr = fakeRate_allMu_->Evaluate2D(pt_eval, std::abs(l3Eta));
    return fr/(1-fr); 
}

bool WZBackgroundSelector::IsFPPRegion() {
    return !zlep1IsTight() && zlep2IsTight() && lepton3IsTight();
}
bool WZBackgroundSelector::IsPFPRegion() {
    return zlep1IsTight() && !zlep2IsTight() && lepton3IsTight();
}

bool WZBackgroundSelector::IsPPFRegion() {
    return zlep1IsTight() && zlep2IsTight() && !lepton3IsTight();
}

bool WZBackgroundSelector::IsFPFRegion() {
    return !zlep1IsTight() && zlep2IsTight() && !lepton3IsTight();
}

bool WZBackgroundSelector::IsFFPRegion() {
    return !zlep1IsTight() && !zlep2IsTight() && lepton3IsTight();
}

bool WZBackgroundSelector::IsPFFRegion() {
    return zlep1IsTight() && !zlep2IsTight() && !lepton3IsTight();
}

bool WZBackgroundSelector::IsFFFRegion() {
    return !zlep1IsTight() && !zlep2IsTight() && !lepton3IsTight();
}
