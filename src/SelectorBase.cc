#include "Analysis/VVAnalysis/interface/SelectorBase.h"
#include <TStyle.h>
#include <regex>
#include "TParameter.h"

void SelectorBase::Begin(TTree * /*tree*/)
{
    TString option = GetOption();
}

void SelectorBase::SlaveBegin(TTree * /*tree*/)
{
    TParameter<bool>* addSum = (TParameter<bool>*) GetInputList()->FindObject("addSumweights");
    if (addSum != nullptr) 
        addSumweights_ = addSum->GetVal();
    else
        addSumweights_ = false;

    //if (applyScaleFactors_)
    //    SetScaleFactors();
}

void SelectorBase::Init(TTree *tree)
{
    if (!tree) return;
    fChain = tree;
    
    TString option = GetOption();

    if (doSystematics_ && isMC_) // isNonpromptEstimate?
        variations_.insert(systematics_.begin(), systematics_.end());

    if (GetInputList() != nullptr) {
        TNamed* ntupleType = (TNamed *) GetInputList()->FindObject("ntupleType");
        TNamed* name = (TNamed *) GetInputList()->FindObject("name");
        TNamed* chan = (TNamed *) GetInputList()->FindObject("channel");
        TNamed* selection = (TNamed *) GetInputList()->FindObject("selection");

        if (ntupleType != nullptr) {
            std::string ntupleName = ntupleType->GetTitle();
            if (ntupleName == "NanoAOD")
                ntupleType_ = NanoAOD;
            else if (ntupleName  == "UWVV")
                ntupleType_ = UWVV;
            else
                throw std::invalid_argument("Unsupported ntuple type!");
        }
        else {
            std::cerr << "INFO: Assuming NanoAOD ntuples" << std::endl;
            ntupleType_ = NanoAOD;
        }

        if (name != nullptr) {
            name_ = name->GetTitle();
        }
        else {
            name_ = GetNameFromFile();
        }
        if (name_ == ""){
            std::cerr << "INFO: Using default name \"Unknown\" for file" << std::endl;
            name_ = "Unknown";
        }

        if (chan != nullptr) {
            channelName_ = chan->GetTitle();
        }
        else if (ntupleType_ == UWVV)
            channelName_ = fChain->GetTree()->GetDirectory()->GetName();
        if (selection != nullptr) {
            selectionName_ = selection->GetTitle();
        }
    }

    if (selectionMap_.find(selectionName_) != selectionMap_.end()) {
        selection_ = selectionMap_[selectionName_];
    }
    else
        throw std::invalid_argument("Invalid selection!");
    
    isMC_ = false;
    if (name_.find("data") == std::string::npos){
        isMC_ = true;
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
            Abort(Form("SelectorBase: Size of allObjects has changed!: %lu to %lu", existingObjectPtrsSize, allObjects_.size()));
        }
    }
    UpdateDirectory();

    if (channelMap_.find(channelName_) != channelMap_.end())
        channel_ = channelMap_[channelName_];
    else
        throw std::invalid_argument("Invalid channel choice!");
    
    SetBranches();
}

void SelectorBase::SetBranches() {
    if (ntupleType_ == UWVV)
        SetBranchesUWVV();
    else if (ntupleType_ == NanoAOD)
        SetBranchesNanoAOD();
}

void SelectorBase::LoadBranches(Long64_t entry, std::pair<Systematic, std::string> variation) {
    if (ntupleType_ == UWVV) {
        LoadBranchesUWVV(entry, variation);
    }
    else if (ntupleType_ == NanoAOD)
        LoadBranchesNanoAOD(entry, variation);
}

Bool_t SelectorBase::Process(Long64_t entry)
{
    for (const auto& variation : variations_) {
        LoadBranches(entry, variation);
        FillHistograms(entry, variation);
    }
    return kTRUE;
}

Bool_t SelectorBase::Notify()
{
    return kTRUE;
}

float SelectorBase::GetPrefiringEfficiencyWeight(
        std::vector<float>* jetPt, std::vector<float>* jetEta) {
    float prefire_weight = 1;
    for (size_t i = 0; i < jetPt->size(); i++) {
        float jPt = jetPt->at(i);
        float jEta = std::abs(jetEta->at(i));
        prefire_weight *= (1-prefireEff_->GetEfficiency(prefireEff_->FindFixBin(jEta, jPt)));
    }
    return prefire_weight;
}

void SelectorBase::Terminate()
{
}
    
void SelectorBase::SlaveTerminate()
{
}
void SelectorBase::UpdateDirectory()
{
  for(TNamed** objPtrPtr : allObjects_) {
    if ( *objPtrPtr == nullptr ) Abort("SelectorBase: Call to UpdateObject but existing pointer is null");
    *objPtrPtr = (TNamed *) currentHistDir_->FindObject((*objPtrPtr)->GetName());
    if ( *objPtrPtr == nullptr ) Abort("SelectorBase: Call to UpdateObject but current directory has no instance");
  }
}

void SelectorBase::SetupNewDirectory()
{
    if (addSumweights_)
        AddObject<TH1D>(sumWeightsHist_, "sumweights", "sumweights", 1, 0, 10);
}
