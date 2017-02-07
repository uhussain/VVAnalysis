// Modified from Nick Smith, U. Wisconsin
#include "Analysis/WZAnalysis/interface/SelectorBase.h"
#include <iostream>

ClassImp(SelectorBase)

void SelectorBase::Init(TTree *tree)
{
    fReader.SetTree(tree);
    const char* dataset = "Unnamed";
    if (GetInputList() != nullptr) {
        TNamed* name = (TNamed *) GetInputList()->FindObject("name");
        if (name != nullptr)
            dataset = name->GetTitle();
    }
    currentHistDir_ = dynamic_cast<TList*>(fOutput->FindObject(dataset));
    if ( currentHistDir_ == nullptr ) {
        currentHistDir_ = new TList();
        currentHistDir_->SetName(dataset);
        fOutput->Add(currentHistDir_);
        // Watch for something that I hope never happens,
        size_t existingObjectPtrsSize = allObjects_.size();
        SetupNewDirectory();
        if ( existingObjectPtrsSize > 0 && allObjects_.size() != existingObjectPtrsSize ) {
            Abort(Form("SelectorBase: Size of allObjects has changed!: %lu to %lu", existingObjectPtrsSize, allObjects_.size()));
        }
    }
    UpdateDirectory();
}

Bool_t SelectorBase::Notify()
{
    return kTRUE;
}

void SelectorBase::Begin(TTree * /*tree*/)
{
}

void SelectorBase::SlaveBegin(TTree * /*tree*/)
{
}

Bool_t SelectorBase::Process(Long64_t entry)
{
    fReader.SetEntry(entry);        
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
    passingLoose2D_->Fill(*e3Pt, *e3Eta);
    passingLoose1DPt_->Fill(*e3Pt);
    passingLoose1DEta_->Fill(*e3Eta);
    if (*e3IsCBVIDTight &&
        ( *e3IsEB ? *e3PVDXY < 0.05 : *e3PVDXY < 0.1) &&
        ( *e3IsEB ? *e3PVDZ < 0.1 : *e3PVDZ < 0.2)) {
        passingTight2D_->Fill(*e3Pt, *e3Eta);
        passingTight1DPt_->Fill(*e3Pt);
        passingTight1DEta_->Fill(*e3Eta);
    }
    return true;
}

void SelectorBase::SlaveTerminate()
{
}

void SelectorBase::Terminate()
{
}

void SelectorBase::SetupNewDirectory()
{
    AddObject<TH2D>(passingTight2D_, (std::string("passingTight2D_") + channel_).c_str(), 
        "Tight leptons; p_{T} [GeV]; Eta", 8, 10, 50, 5, -2.5, 2.5);
    AddObject<TH1D>(passingTight1DPt_, (std::string("passingTight1DPt_") + channel_).c_str(), 
        "Tight leptons; p_{T} [GeV]", 8, 10, 50);
    AddObject<TH1D>(passingTight1DEta_, (std::string("passingTight1DEta_") + channel_).c_str(), 
        "Tight leptons; #eta; Events", 5, -2.5, 2.5);
    AddObject<TH2D>(passingLoose2D_, (std::string("passingLoose2D_") + channel_).c_str(), 
        "Leptons passing tight failing loose; p_{T} [GeV]; Eta", 8, 10, 50, 5, -2.5, 2.5);
    AddObject<TH1D>(passingLoose1DPt_, (std::string("passingLoose1DPt_") + channel_).c_str(), 
        "Loose leptons; p_{T} [GeV]; Events", 8, 10, 50);
    AddObject<TH1D>(passingLoose1DEta_, (std::string("passingLoose1DEta_") + channel_).c_str(), 
        "Loose leptons; #eta; Events", 5, -2.5, 2.5);
}

void SelectorBase::UpdateDirectory()
{
    for(TNamed** objPtrPtr : allObjects_) {
        if ( *objPtrPtr == nullptr ) Abort("SelectorBase: Call to UpdateObject but existing pointer is null");
        *objPtrPtr = (TNamed *) currentHistDir_->FindObject((*objPtrPtr)->GetName());
        if ( *objPtrPtr == nullptr ) Abort("SelectorBase: Call to UpdateObject but current directory has no instance");
    }
}
