#include "Analysis/VVAnalysis/interface/ZLSelectorBase.h"
#include <TStyle.h>

void ZLSelectorBase::Begin(TTree * /*tree*/)
{
    TString option = GetOption();
}

void ZLSelectorBase::SlaveBegin(TTree * /*tree*/)
{
}
void ZLSelectorBase::Init(TTree *tree)
{
    if (!tree) return;
    fChain = tree;
    
    TString option = GetOption();

    if (GetInputList() != nullptr) {
        TNamed* name = (TNamed *) GetInputList()->FindObject("name");
        TNamed* chan = (TNamed *) GetInputList()->FindObject("channel");
        TNamed* selection = (TNamed *) GetInputList()->FindObject("selection");
        if (name != nullptr) {
            name_ = name->GetTitle();
        }
        if (chan != nullptr) {
            channelName_ = chan->GetTitle();
        }
        if (selection != nullptr) {
            selectionName_ = selection->GetTitle();
        }
    }
    std::cout << "Processing " << name_ << std::endl;
    
    isMC_ = false;
    if (name_.find("data") == std::string::npos){
        isMC_ = true;
        fChain->SetBranchAddress("genWeight", &genWeight, &b_genWeight);
    }
    else {
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
            Abort(Form("ZLSelectorBase: Size of allObjects has changed!: %lu to %lu", existingObjectPtrsSize, allObjects_.size()));
        }
    }
    UpdateDirectory();

    if (channelName_ == "eee") {
        channel_ = eee;
        fChain->SetBranchAddress("e1ZZTightIDNoVtx", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("e1ZZIsoPass", &l1IsIso, &b_l1IsIso);
        fChain->SetBranchAddress("e2ZZTightIDNoVtx", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("e2ZZIsoPass", &l2IsIso, &b_l2IsIso);
        fChain->SetBranchAddress("e3ZZTightIDNoVtx", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("e3ZZIsoPass", &l3IsIso, &b_l3IsIso);
    }
    else if (channelName_ == "eem") { 
        channel_ = eem;
        fChain->SetBranchAddress("e1ZZTightIDNoVtx", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("e1ZZIsoPass", &l1IsIso, &b_l1IsIso);
        fChain->SetBranchAddress("e2ZZTightIDNoVtx", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("e2ZZIsoPass", &l2IsIso, &b_l2IsIso);
        fChain->SetBranchAddress("mZZTightIDNoVtx", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("mZZIsoPass", &l3IsIso, &b_l3IsIso);
    }
    else if (channelName_ == "emm") { 
        channel_ = emm;
        fChain->SetBranchAddress("eZZTightIDNoVtx", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("eZZIsoPass", &l3IsIso, &b_l3IsIso);
        fChain->SetBranchAddress("m1ZZTightIDNoVtx", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("m1ZZIsoPass", &l1IsIso, &b_l1IsIso);
        fChain->SetBranchAddress("m2ZZTightIDNoVtx", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("m2ZZIsoPass", &l2IsIso, &b_l2IsIso);
    }
    else if (channelName_ == "mmm") { 
        channel_ = mmm;
        fChain->SetBranchAddress("m1ZZTightIDNoVtx", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("m1ZZIsoPass", &l1IsIso, &b_l1IsIso);
        fChain->SetBranchAddress("m2ZZTightIDNoVtx", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("m2ZZIsoPass", &l2IsIso, &b_l2IsIso);
        fChain->SetBranchAddress("m3ZZTightIDNoVtx", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("m3ZZIsoPass", &l3IsIso, &b_l3IsIso);
    }
    else
        throw std::invalid_argument("Invalid channel choice!");

    if (selectionName_ == "tightleptons" || selectionName_ == "Wselection")
        selection_ = tightleptons;
    else if (selectionName_ == "VBSselection")
        selection_ = VBSselection;
    else if (selectionName_ == "VBSselection_Tight") {
        selection_ = VBSselection_Tight;
    }
    else {
        std::cerr << "INFO: Selection set to default value: TightLeptons" << std::endl;
        selection_ = tightleptons;
    }
    isVBS_ = (selection_ == VBSselection || 
        selection_ == VBSselection_Tight
    );

}

Bool_t ZLSelectorBase::Notify()
{
    return kTRUE;
}

Bool_t ZLSelectorBase::Process(Long64_t entry)
{
    genWeight = 1;
    if (isMC_)
        b_genWeight->GetEntry(entry);
    else {
    }
    b_l1IsTight->GetEntry(entry);
    b_l1IsIso->GetEntry(entry);
    b_l2IsTight->GetEntry(entry);
    b_l2IsIso->GetEntry(entry);
    b_l3IsTight->GetEntry(entry);
    b_l3IsIso->GetEntry(entry);
 
    return kTRUE;
}

// Meant to be a wrapper for the tight ID just in case it changes
// To be a function of multiple variables
bool ZLSelectorBase::zlep1IsTight() {
    return (l1IsTight && l1IsIso); 
}

bool ZLSelectorBase::zlep2IsTight() {
    return (l2IsTight && l2IsIso); 
}

bool ZLSelectorBase::tightZLeptons() {
    return zlep1IsTight() && zlep2IsTight(); 
}

bool ZLSelectorBase::lepton3IsTight() {
    return (l3IsTight && l3IsIso);
}

void ZLSelectorBase::Terminate()
{
}
    
void ZLSelectorBase::SlaveTerminate()
{
}
void ZLSelectorBase::UpdateDirectory()
{
  for(TNamed** objPtrPtr : allObjects_) {
    if ( *objPtrPtr == nullptr ) Abort("ZLSelectorBase: Call to UpdateObject but existing pointer is null");
    *objPtrPtr = (TNamed *) currentHistDir_->FindObject((*objPtrPtr)->GetName());
    if ( *objPtrPtr == nullptr ) Abort("ZLSelectorBase: Call to UpdateObject but current directory has no instance");
  }
}

void ZLSelectorBase::SetupNewDirectory()
{
}
