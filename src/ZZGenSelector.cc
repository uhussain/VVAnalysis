#include "Analysis/VVAnalysis/interface/ZZGenSelector.h"
#include "TLorentzVector.h"
#include <boost/algorithm/string.hpp>

void ZZGenSelector::Init(TTree *tree)
{
    ZZGenSelectorBase::Init(tree);

    fChain->SetBranchAddress("Mass", &GenMass, &b_GenMass);
    fChain->SetBranchAddress("Pt", &GenPt, &b_GenPt);
    //std::cout<<"Is it able to initialize"<<std::endl; 
}
void ZZGenSelector::LoadBranches(Long64_t entry) { 
    ZZGenSelectorBase::Process(entry);

    b_GenMass->GetEntry(entry);
    b_GenPt->GetEntry(entry);
    //std::cout<<"channel in LoadBranches function: "<<channel_<<std::endl;
    if(channel_ == eemm || channel_ == mmee){
      SetVariables(entry);
    } 
    auto deltaPhiZZ = [](float phi1, float phi2) {
      float pi = TMath::Pi();
      float dphi = fabs(phi1-phi2);
      if(dphi>pi)
          dphi = 2.0*pi - dphi;
      return dphi;
    };

    GendPhiZZ = deltaPhiZZ(GenZ1Phi,GenZ2Phi);
}
//Similar to Kenneth's SetShiftedMasses function which i will need later as well
void ZZGenSelector::SetVariables(Long64_t entry) {
    if(!(e1e2IsZ1(entry))){
      //std::cout<<"e1e2IsZ1 is working"<<std::endl;
      float tempMass = GenZ1mass;
      GenZ1mass = GenZ2mass;
      GenZ2mass = tempMass;
      float tempPt = GenZ1pt;
      GenZ1pt = GenZ2pt;
      GenZ2pt = tempPt;
      float templ1Pt = Genl1Pt;
      Genl1Pt = Genl3Pt;
      Genl3Pt = templ1Pt;
      float templ2Pt = Genl2Pt;
      Genl2Pt = Genl4Pt;
      Genl4Pt = templ2Pt;
      float templ1Eta = Genl1Eta;
      Genl1Eta = Genl3Eta;
      Genl3Eta = templ1Eta;
      float templ2Eta = Genl2Eta;
      Genl2Eta = Genl4Eta;
      Genl4Eta = templ2Eta;
      float templ1Phi = Genl1Phi;
      Genl1Phi = Genl3Phi;
      Genl3Phi = templ1Phi;
      float templ2Phi = Genl2Phi;
      Genl2Phi = Genl4Phi;
      Genl4Phi = templ2Phi;
    }
}
bool ZZGenSelector::ZZSelection() {
    if ((GenZ1mass > 60.0 && GenZ1mass < 120.0) && (GenZ2mass > 60.0 && GenZ2mass < 120.0))
        return true;
    else
        return false;
}
//We already require 4 < Z1,Z2 < 120  in the "Loose Skim"
bool ZZGenSelector::ZSelection() {
    if (GenZ1mass > 40.0 && GenZ2mass > 12.0)
        return true;
    else
        return false;
}
bool ZZGenSelector::Z4lSelection() {
    if (GenMass > 80.0 && GenMass < 100.0)
        return true;
    else
        return false;
}

std::string ZZGenSelector::getHistName(std::string histName) {
  //return variationName == "" ? histName : histName + "_" + variationName;
  return histName;
}

void ZZGenSelector::FillHistograms(Long64_t entry, float Genweight) { 
    
    SafeHistFill(hists1D_, getHistName("Genyield"), 1, Genweight);
    SafeHistFill(hists1D_, getHistName("GenMass"), GenMass,Genweight);
    SafeHistFill(hists1D_, getHistName("GenZMass"), GenZ1mass, Genweight);
    SafeHistFill(hists1D_, getHistName("GenZMass"), GenZ2mass, Genweight);
    //Making LeptonPt and Eta plots
    SafeHistFill(hists1D_, getHistName("GenLepPt"), Genl1Pt, Genweight);
    SafeHistFill(hists1D_, getHistName("GenLepPt"), Genl2Pt, Genweight);
    SafeHistFill(hists1D_, getHistName("GenLepPt"), Genl3Pt, Genweight);
    SafeHistFill(hists1D_, getHistName("GenLepPt"), Genl4Pt, Genweight);
    SafeHistFill(hists1D_, getHistName("GenLepEta"), Genl1Eta, Genweight);
    SafeHistFill(hists1D_, getHistName("GenLepEta"), Genl2Eta, Genweight);
    SafeHistFill(hists1D_, getHistName("GenLepEta"), Genl3Eta, Genweight);
    SafeHistFill(hists1D_, getHistName("GenLepEta"), Genl4Eta, Genweight);
    // Summing 12,34 leptons
    SafeHistFill(hists1D_, getHistName("GenLep12Pt"), Genl1Pt, Genweight);
    SafeHistFill(hists1D_, getHistName("GenLep12Pt"), Genl2Pt, Genweight);
    SafeHistFill(hists1D_, getHistName("GenLep34Pt"), Genl3Pt, Genweight);
    SafeHistFill(hists1D_, getHistName("GenLep34Pt"), Genl4Pt, Genweight);
    SafeHistFill(hists1D_, getHistName("GenLep12Eta"), Genl1Eta, Genweight);
    SafeHistFill(hists1D_, getHistName("GenLep12Eta"), Genl2Eta, Genweight);
    SafeHistFill(hists1D_, getHistName("GenLep34Eta"), Genl3Eta, Genweight);
    SafeHistFill(hists1D_, getHistName("GenLep34Eta"), Genl4Eta, Genweight);
    SafeHistFill(hists1D_, getHistName("GenZ1Mass"), GenZ1mass, Genweight);
    SafeHistFill(hists1D_, getHistName("GenZ2Mass"), GenZ2mass, Genweight);
    SafeHistFill(hists1D_, getHistName("GenZPt"), GenZ1pt, Genweight);
    SafeHistFill(hists1D_, getHistName("GenZPt"), GenZ2pt, Genweight);
    SafeHistFill(hists1D_, getHistName("GenZ1Pt"), GenZ1pt, Genweight);
    SafeHistFill(hists1D_, getHistName("GenZ2Pt"), GenZ2pt, Genweight);
    SafeHistFill(hists1D_, getHistName("GenZZPt"), GenPt, Genweight);
    SafeHistFill(hists1D_, getHistName("GenZ1Phi"), GenZ1Phi, Genweight);
    SafeHistFill(hists1D_, getHistName("GenZ2Phi"), GenZ2Phi, Genweight);
    SafeHistFill(hists1D_, getHistName("GendPhiZ1Z2"), GendPhiZZ, Genweight);
    SafeHistFill(hists1D_, getHistName("GenZ1lep1_Pt"), Genl1Pt, Genweight);
    SafeHistFill(hists1D_, getHistName("GenZ1lep1_Eta"), Genl1Eta, Genweight);
    SafeHistFill(hists1D_, getHistName("GenZ1lep1_Phi"), Genl1Phi, Genweight);
    SafeHistFill(hists1D_, getHistName("GenZ1lep2_Pt"), Genl2Pt, Genweight);
    SafeHistFill(hists1D_, getHistName("GenZ1lep2_Eta"), Genl2Eta, Genweight);
    SafeHistFill(hists1D_, getHistName("GenZ1lep2_Phi"), Genl2Phi, Genweight);
    SafeHistFill(hists1D_, getHistName("GenZ2lep1_Pt"), Genl3Pt, Genweight);
    SafeHistFill(hists1D_, getHistName("GenZ2lep1_Eta"), Genl3Eta, Genweight);
    SafeHistFill(hists1D_, getHistName("GenZ2lep1_Phi"), Genl3Phi, Genweight);
    SafeHistFill(hists1D_, getHistName("GenZ2lep2_Pt"), Genl4Pt, Genweight);
    SafeHistFill(hists1D_, getHistName("GenZ2lep2_Eta"), Genl4Eta, Genweight);
    SafeHistFill(hists1D_, getHistName("GenZ2lep2_Phi"), Genl4Phi, Genweight);
    //2D Z1 vs Z2
    SafeHistFill(hists2D_, getHistName("GenZ1Mass_GenZ2Mass"),GenZ1mass,GenZ2mass,Genweight);

}

Bool_t ZZGenSelector::Process(Long64_t entry)
{
    LoadBranches(entry);
    //Define Genweight of event based on channel in case of eemm or mmee
    if (ZZSelection()) {
      if (true) {
        //std::cout<<run<<":"<<lumi<<":"<<evt<<std::endl;
        //std::cout<<"Genweight in ZZGenSelector inside HZZ: "<<Genweight<<std::endl;
        FillHistograms(entry, Genweight);
    }
  }
    return true;
}

std::vector<std::string> ZZGenSelector::ReadHistData(std::string histDataString) {
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

void ZZGenSelector::InitialzeHistogram(std::string name, std::vector<std::string> histData) {
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
    }
    else {
        int nbinsy = std::stoi(histData[4]);
        float ymin = std::stof(histData[5]);
        float ymax = std::stof(histData[6]);
          AddObject<TH2D>(hists2D_[name], hist_name.c_str(), histData[0].c_str(),nbins, xmin, xmax,
                nbinsy, ymin, ymax);
    }
}

void ZZGenSelector::SetupNewDirectory()
{
    ZZGenSelectorBase::SetupNewDirectory();
   
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
