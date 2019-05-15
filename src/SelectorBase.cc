#include "Analysis/VVAnalysis/interface/SelectorBase.h"
#include <boost/algorithm/string.hpp>
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

void SelectorBase::InitializeHistogramsFromConfig() {
    TList* histInfo = (TList *) GetInputList()->FindObject("histinfo");
    if (histInfo == nullptr ) 
        Abort("Can't initialize histograms without passing histogram information to TSelector");
    
    for (auto && entry : *histInfo) {  
        TNamed* currentHistInfo = dynamic_cast<TNamed*>(entry);
        std::string name = currentHistInfo->GetName();
        std::vector<std::string> histData = ReadHistDataFromConfig(currentHistInfo->GetTitle());
        if (hists2D_.find(name) != hists2D_.end() || hists1D_.find(name) != hists1D_.end()) { 
            if (channelName_ == "Inclusive")
                //for (const auto& chan : allChannels_)
                //TODO This needs to be fixed to add all channels
                InitializeHistogramFromConfig(name, "emm", histData);
            else
                InitializeHistogramFromConfig(name, channelName_, histData);
        }
        else
            std::cerr << "Skipping invalid histogram " << name << std::endl;
    }
}

void SelectorBase::InitializeHistogramFromConfig(std::string name, std::string channel, std::vector<std::string> histData) {
    if (histData.size() != 4 && histData.size() != 7) {
        std::cerr << "Malformed data string for histogram '" << name
                    << ".' Must have form: 'Title; (optional info) $ nbins, xmin, xmax'"
                    << "\n   OR form: 'Title; (optional info) $ nbins, xmin, xmax nbinsy ymin ymax'"
                    << std::endl;
        exit(1);
    }
    std::string hist_name = name+"_"+channel;
    int nbins = std::stoi(histData[1]);
    float xmin = std::stof(histData[2]);
    float xmax = std::stof(histData[3]);

    if (histData.size() == 4) {
        AddObject<TH1D>(hists1D_[name], hist_name.c_str(), histData[0].c_str(),nbins, xmin, xmax);
        if (doSystematics_ && std::find(systHists_.begin(), systHists_.end(), name) != systHists_.end()) {
            for (auto& syst : systematics_) {
                std::string syst_hist_name = name+"_"+syst.second;
                hists1D_[syst_hist_name] = {};
                AddObject<TH1D>(hists1D_[syst_hist_name], (syst_hist_name+"_"+channel).c_str(), 
                    histData[0].c_str(),nbins, xmin, xmax);
                // TODO: Cleaner way to determine if you want to store systematics for weighted entries
                //if (isaQGC_ && doaQGC_ && (weighthists_.find(name) != weighthists_.end())) { 
                //    std::string weightsyst_hist_name = name+"_lheWeights_"+syst.second;
                //    AddObject<TH2D>(weighthists_[syst_hist_name], 
                //        (weightsyst_hist_name+"_"+channel).c_str(), histData[0].c_str(),
                //        nbins, xmin, xmax, 1000, 0, 1000);
                //}
            }
        }
        // Weight hists must be subset of 1D hists!
        if (isMC_ && (weighthists_.find(name) != weighthists_.end())) { 
            AddObject<TH2D>(weighthists_[name], 
                (name+"_lheWeights_"+channel).c_str(), histData[0].c_str(),
                nbins, xmin, xmax, 1000, 0, 1000);
        }
    }
    else {
        int nbinsy = std::stoi(histData[4]);
        float ymin = std::stof(histData[5]);
        float ymax = std::stof(histData[6]);
        AddObject<TH2D>(hists2D_[name], hist_name.c_str(), histData[0].c_str(),nbins, xmin, xmax,
                nbinsy, ymin, ymax);
        if (doSystematics_ && std::find(systHists2D_.begin(), systHists2D_.end(), name) != systHists2D_.end()) {
            for (auto& syst : systematics_) {
                std::string syst_hist_name = name+"_"+syst.second;
                hists2D_[syst_hist_name] = {};
                AddObject<TH2D>(hists2D_[syst_hist_name], (syst_hist_name+"_"+channel).c_str(), 
                    histData[0].c_str(),nbins, xmin, xmax, nbinsy, ymin, ymax);
            }
        }
        // 3D weight hists must be subset of 2D hists!
        if (isMC_ && (weighthists2D_.find(name) != weighthists2D_.end())) { 
            AddObject<TH3D>(weighthists2D_[name], 
                (name+"_lheWeights_"+channel).c_str(), histData[0].c_str(),
                nbins, xmin, xmax, nbinsy, ymin, ymax, 1000, 0, 1000);
        }
    }
}

std::vector<std::string> SelectorBase::ReadHistDataFromConfig(std::string histDataString) {
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

void SelectorBase::SetupNewDirectory()
{
    if (addSumweights_)
        AddObject<TH1D>(sumWeightsHist_, "sumweights", "sumweights", 1, 0, 10);
}
