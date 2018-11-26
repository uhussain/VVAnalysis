#include "Analysis/VVAnalysis/interface/ZZSelector.h"
#include "TLorentzVector.h"
#include <boost/algorithm/string.hpp>

void ZZSelector::Init(TTree *tree)
{
    ZZSelectorBase::Init(tree);

    //weight_info_ = 0;
    //if (isMC_) {
    //    fChain->SetBranchAddress("nTruePU", &nTruePU, &b_nTruePU);
        //weight_info_ = GetLheWeightInfo();
    //}
    fChain->SetBranchAddress("Mass", &Mass, &b_Mass);
    //std::cout<<"Is it able to initialize"<<std::endl; 
}
void ZZSelector::LoadBranches(Long64_t entry, std::pair<Systematic, std::string> variation) { 
    ZZSelectorBase::Process(entry);

    //b_MtToMET->GetEntry(entry);
    //b_l1Pt->GetEntry(entry);
    //b_l2Pt->GetEntry(entry);
    //b_l3Pt->GetEntry(entry);
    b_Mass->GetEntry(entry);
    //std::cout<<"channel in LoadBranches function: "<<channel_<<std::endl;
    if(channel_ == eemm || channel_ == mmee){
      SetVariables(entry);}
}
//Similar to Kenneth's SetShiftedMasses function which i will need later as well
void ZZSelector::SetVariables(Long64_t entry) {
    if(!(e1e2IsZ1(entry))){
      std::cout<<"e1e2IsZ1 is working"<<std::endl;
      float tempMass = Z1mass;
      Z1mass = Z2mass;
      Z2mass = tempMass;
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
      float templ1PdgId = l1PdgId;
      l1PdgId = l3PdgId;
      l3PdgId = templ1PdgId;
      float templ2PdgId = l2PdgId;
      l2PdgId = l4PdgId;
      l4PdgId = templ2PdgId;
    }
}
bool ZZSelector::TightZZLeptons() {
    if (tightZ1Leptons() && tightZ2Leptons()){
        return true;}
    else{
        return false;}
}
bool ZZSelector::ZZSelection() {
    if ((Z1mass > 60.0 && Z1mass < 120.0) && (Z2mass > 60.0 && Z2mass < 120.0)){
        return true;}
    else{
        return false;}
}
bool ZZSelector::Z4lSelection() {
    if (Mass > 80.0 && Mass < 100.0){
        return true;}
    else{
        return false;}
}
bool ZZSelector::HZZSelection(){
    if ((l1SIP3D < 4.0 && l2SIP3D < 4.0 && l3SIP3D < 4.0 && l4SIP3D < 4.0) && (Z2mass > 12.0)){
        return true;}
    else{
        return false;}
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
        for (size_t i = 0; i < lheWeights.size(); i++) {
              SafeHistFill(weighthists_, "backgroundControlYield", 1, i, lheWeights[i]/lheWeights[0]*weight);
              SafeHistFill(weighthists_, getHistName("yield", variation.second), 1, i, lheWeights[i]/lheWeights[0]*weight);
              SafeHistFill(weighthists_, getHistName("Mass", variation.second), Mass, i, lheWeights[i]/lheWeights[0]*weight);

        }
    }
    SafeHistFill(hists1D_, getHistName("yield", variation.second), 1, weight);
    SafeHistFill(hists1D_, getHistName("Mass", variation.second), Mass,weight); 
    SafeHistFill(hists1D_, getHistName("Z1Mass", variation.second), Z1mass, weight);
    SafeHistFill(hists1D_, getHistName("Z2Mass", variation.second), Z2mass, weight);
    SafeHistFill(hists1D_, getHistName("Z1lep1_Pt", variation.second), l1Pt, weight);
    SafeHistFill(hists1D_, getHistName("Z1lep1_Eta", variation.second), l1Eta, weight);
    SafeHistFill(hists1D_, getHistName("Z1lep1_Phi", variation.second), l1Phi, weight);
    SafeHistFill(hists1D_, getHistName("Z1lep2_Pt", variation.second), l2Pt, weight);
    SafeHistFill(hists1D_, getHistName("Z1lep2_Eta", variation.second), l2Eta, weight);
    SafeHistFill(hists1D_, getHistName("Z1lep2_Phi", variation.second), l2Phi, weight);
    SafeHistFill(hists1D_, getHistName("Z2lep1_Pt", variation.second), l3Pt, weight);
    SafeHistFill(hists1D_, getHistName("Z2lep1_Eta", variation.second), l3Eta, weight);
    SafeHistFill(hists1D_, getHistName("Z2lep1_Phi", variation.second), l3Phi, weight);
    SafeHistFill(hists1D_, getHistName("Z2lep2_Pt", variation.second), l4Pt, weight);
    SafeHistFill(hists1D_, getHistName("Z2lep2_Eta", variation.second), l4Eta, weight);
    SafeHistFill(hists1D_, getHistName("Z2lep2_Phi", variation.second), l4Phi, weight);

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
    if (true) {
      if (HZZSelection()) {
        std::cout<<"Weight in ZZSelector inside HZZ: "<<weight<<std::endl;
        FillHistograms(entry, weight, !blindVBS, central_var);
    }
  }
   // if (doSystematics_ && (isMC_ || isNonpromptEstimate_)) {
   //     for (const auto& systematic : systematics_) {
   //         LoadBranches(entry, systematic);
   //         if (PassesBaseSelection(entry, true, selection_)) {
   //             FillHistograms(entry, weight, !blindVBS, systematic);
   //         }
   //     }
   // }
    
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
       // if (doSystematics_ && std::find(systHists_.begin(), systHists_.end(), name) != systHists_.end()) {
       //     for (auto& syst : systematics_) {
       //         std::string syst_hist_name = name+"_"+syst.second;
       //         hists1D_[syst_hist_name] = {};
       //         AddObject<TH1D>(hists1D_[syst_hist_name], (syst_hist_name+"_"+channelName_).c_str(), 
       //             histData[0].c_str(),nbins, xmin, xmax);
       //         if (isaQGC_ && doaQGC_ && (weighthists_.find(name) != weighthists_.end())) { 
       //             std::string weightsyst_hist_name = name+"_lheWeights_"+syst.second;
       //             AddObject<TH2D>(weighthists_[syst_hist_name], 
       //                 (weightsyst_hist_name+"_"+channelName_).c_str(), histData[0].c_str(),
       //                 nbins, xmin, xmax, 1000, 0, 1000);
       //         }
       //     }
       // }
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
