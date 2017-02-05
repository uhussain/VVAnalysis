// Modified from Nick Smith, U. Wisconsin
#include "SelectorBase.h"

ClassImp(SelectorBase)

void SelectorBase::Init(TTree *tree)
{
    fReader.SetTree(tree);

    const char* dataset = name_;
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
    selectorCounter_->Fill(0);
    return kTRUE;
}

void SelectorBase::SlaveTerminate()
{
}

void SelectorBase::Terminate()
{
}

void SelectorBase::SetupNewDirectory()
{
    AddObject<TH1D>(selectorCounter_, "selectorCounter", "Counter of SelectorBase::Process() calls", 1, 0, 1);
}

void SelectorBase::UpdateDirectory()
{
    for(TNamed** objPtrPtr : allObjects_) {
        if ( *objPtrPtr == nullptr ) Abort("SelectorBase: Call to UpdateObject but existing pointer is null");
        *objPtrPtr = (TNamed *) currentHistDir_->FindObject((*objPtrPtr)->GetName());
        if ( *objPtrPtr == nullptr ) Abort("SelectorBase: Call to UpdateObject but current directory has no instance");
    }
}
