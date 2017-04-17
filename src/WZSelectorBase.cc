#include "Analysis/WZAnalysis/interface/WZSelectorBase.h"
#include <TStyle.h>

void WZSelectorBase::Begin(TTree * /*tree*/)
{
    TString option = GetOption();
}

void WZSelectorBase::SlaveBegin(TTree * /*tree*/)
{
}
void WZSelectorBase::Init(TTree *tree)
{
    if (!tree) return;
    fChain = tree;
    
    TString option = GetOption();

    if (GetInputList() != nullptr) {
        TNamed* name = (TNamed *) GetInputList()->FindObject("name");
        TNamed* chan = (TNamed *) GetInputList()->FindObject("channel");
        if (name != nullptr) {
            name_ = name->GetTitle();
        }
        if (chan != nullptr) {
            channelName_ = chan->GetTitle();
        }
    }
    std::cout << "Processing " << name_ << std::endl;
    
    isMC_ = false;
    if (name_.find("data") == std::string::npos){
        isMC_ = true;
        fChain->SetBranchAddress("genWeight", &genWeight, &b_genWeight);
    }

    currentHistDir_ = dynamic_cast<TList*>(fOutput->FindObject(name_.c_str()));
    if ( currentHistDir_ == nullptr ) {
        currentHistDir_ = new TList();
        currentHistDir_->SetName(name_.c_str());
        fOutput->Add(currentHistDir_);
        // Watch for something that I hope never happens,
        size_t existingObjectPtrsSize = allObjects_.size();
        SetupNewDirectory();
        if ( existingObjectPtrsSize > 0 && allObjects_.size() != existingObjectPtrsSize ) {
            Abort(Form("WZSelectorBase: Size of allObjects has changed!: %lu to %lu", existingObjectPtrsSize, allObjects_.size()));
        }
    }
    UpdateDirectory();

    if (channelName_ == "eee") {
        channel_ = eee;
        fChain->SetBranchAddress("e1IsCBVIDTight", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("e2IsCBVIDTight", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("e3IsCBVIDTight", &l3IsTight, &b_l3IsTight);
    }
    else if (channelName_ == "eem") { 
        channel_ = eem;
        fChain->SetBranchAddress("e1IsCBVIDTight", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("e2IsCBVIDTight", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("mIsMedium", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("mRelPFIsoDBR04", &m3RelPFIsoDBR04, &b_m3RelPFIsoDBR04);
    }
    else if (channelName_ == "emm") { 
        channel_ = emm;
        fChain->SetBranchAddress("eIsCBVIDTight", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("m1IsMedium", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("m1RelPFIsoDBR04", &m1RelPFIsoDBR04, &b_m1RelPFIsoDBR04);
        fChain->SetBranchAddress("m2IsMedium", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("m2RelPFIsoDBR04", &m2RelPFIsoDBR04, &b_m2RelPFIsoDBR04);
    }
    else if (channelName_ == "mmm") { 
        channel_ = mmm;
        fChain->SetBranchAddress("m1IsMedium", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("m1RelPFIsoDBR04", &m1RelPFIsoDBR04, &b_m1RelPFIsoDBR04);
        fChain->SetBranchAddress("m2IsMedium", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("m2RelPFIsoDBR04", &m2RelPFIsoDBR04, &b_m2RelPFIsoDBR04);
        fChain->SetBranchAddress("m3IsMedium", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("m3RelPFIsoDBR04", &m3RelPFIsoDBR04, &b_m3RelPFIsoDBR04);
    }
    else
        throw std::invalid_argument("Invalid channel choice!");
    fChain->SetBranchAddress("nCBVIDTightElec", &nCBVIDTightElec, &b_nCBVIDTightElec);
    fChain->SetBranchAddress("nWZTightMuon", &nWZTightMuon, &b_nWZTightMuon);
}

Bool_t WZSelectorBase::Notify()
{
    return kTRUE;
}

Bool_t WZSelectorBase::Process(Long64_t entry)
{
    genWeight = 1;
    if (isMC_)
        b_genWeight->GetEntry(entry);
    b_l1IsTight->GetEntry(entry);
    b_l2IsTight->GetEntry(entry);
    b_l3IsTight->GetEntry(entry);
    b_nCBVIDTightElec->GetEntry(entry);
    b_nWZTightMuon->GetEntry(entry);
    
    passesLeptonVeto = nWZTightMuon + nCBVIDTightElec <= 3;
    
    if (channel_ == eem) {
        b_m3RelPFIsoDBR04->GetEntry(entry);
    }
    else if (channel_ == emm) {
        b_m1RelPFIsoDBR04->GetEntry(entry);
        b_m2RelPFIsoDBR04->GetEntry(entry);
    }
    else if (channel_ == mmm) {
        b_m1RelPFIsoDBR04->GetEntry(entry);
        b_m2RelPFIsoDBR04->GetEntry(entry);
        b_m3RelPFIsoDBR04->GetEntry(entry);
    }

    return kTRUE;
}

// Meant to be a wrapper for the tight ID just in case it changes
// To be a function of multiple variables
bool WZSelectorBase::zlep1IsTight() {
    if (channel_ == eem || channel_ == eee) {
        return l1IsTight; 
    }
    else 
        return m1RelPFIsoDBR04 < 0.15;
}

bool WZSelectorBase::zlep2IsTight() {
    if (channel_ == eem || channel_ == eee) {
        return l2IsTight; 
    }
    else 
        return m2RelPFIsoDBR04 < 0.15;
}

bool WZSelectorBase::tightZLeptons() {
    return zlep1IsTight() && zlep2IsTight(); 
}

bool WZSelectorBase::lepton3IsTight() {
    if (channel_ == eee || channel_ == emm) {
        return l3IsTight;
    }
    else
        return m3RelPFIsoDBR04 < 0.15;
}

void WZSelectorBase::Terminate()
{
}
    
void WZSelectorBase::SlaveTerminate()
{
}
void WZSelectorBase::UpdateDirectory()
{
  for(TNamed** objPtrPtr : allObjects_) {
    if ( *objPtrPtr == nullptr ) Abort("WZSelectorBase: Call to UpdateObject but existing pointer is null");
    *objPtrPtr = (TNamed *) currentHistDir_->FindObject((*objPtrPtr)->GetName());
    if ( *objPtrPtr == nullptr ) Abort("WZSelectorBase: Call to UpdateObject but current directory has no instance");
  }
}

void WZSelectorBase::SetupNewDirectory()
{
}
