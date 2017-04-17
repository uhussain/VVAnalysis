#include "Analysis/WZAnalysis/interface/WZSelector.h"
#include <boost/algorithm/string.hpp>

void WZSelector::Init(TTree *tree)
{
    WZSelectorBase::Init(tree);
    
    weight_info_ = 0;
    if (isMC_) {
        weight_info_ = GetLheWeightInfo();
        if (weight_info_ > 0)
            fChain->SetBranchAddress("scaleWeights", &scaleWeights, &b_scaleWeights);
        if (weight_info_ == 2)
            fChain->SetBranchAddress("pdfWeights", &pdfWeights, &b_pdfWeights);
    }
    
    fChain->SetBranchAddress("jetPt", &jetPt, &b_jetPt);
    fChain->SetBranchAddress("jetEta", &jetEta, &b_jetEta);
    fChain->SetBranchAddress("Mass", &Mass, &b_Mass);
    fChain->SetBranchAddress("nvtx", &nvtx, &b_nvtx);
    fChain->SetBranchAddress("mjj", &mjj, &b_mjj);
    fChain->SetBranchAddress("type1_pfMETEt", &type1_pfMETEt, &b_type1_pfMETEt);

    if (channel_ == eee) {
        fChain->SetBranchAddress("e1_e2_Mass", &ZMass, &b_ZMass);
        fChain->SetBranchAddress("e1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("e1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("e2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("e2Eta", &l2Eta, &b_l2Eta);
        fChain->SetBranchAddress("e3Pt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("e3Eta", &l3Eta, &b_l3Eta);
    }
    else if (channel_ == eem) { 
        fChain->SetBranchAddress("e1_e2_Mass", &ZMass, &b_ZMass);
        fChain->SetBranchAddress("mPt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("mEta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("e1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("e1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("e2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("e2Eta", &l2Eta, &b_l2Eta);
    }
    else if (channel_ == emm) { 
        fChain->SetBranchAddress("m1_m2_Mass", &ZMass, &b_ZMass);
        fChain->SetBranchAddress("ePt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("eEta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("m1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("m1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("m2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("m2Eta", &l2Eta, &b_l2Eta);
    }
    else if (channel_ == mmm) { 
        fChain->SetBranchAddress("m1_m2_Mass", &ZMass, &b_ZMass);
        fChain->SetBranchAddress("m1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("m1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("m2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("m2Eta", &l2Eta, &b_l2Eta);
        fChain->SetBranchAddress("m3Pt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("m3Eta", &l3Eta, &b_l3Eta);
    }
}

unsigned int WZSelector::GetLheWeightInfo() {
    std::vector<std::string> noLheWeights = {
        "ggZZ2e2mu", "ggZZ4e", "ggZZ4m", "wzjj-vbfnlo",
        "st-schan", "st-tchan"
    };
    std::vector<std::string> allLheWeights = {
        // PDF weights are saved in the ntuples but not really used at this point
        //"wz3lnu-powheg", "wz3lnu-mg5amcnlo",
        //"wz3lnu-mgmlm-0j", "wz3lnu-mgmlm-1j",
        //"wz3lnu-mgmlm-2j", "wz3lnu-mgmlm-3j", "wlljj", 
        "wzjj-aqgcft", "wzjj-aqgcfm", "wzjj-aqgcfs",
        "wz-atgc_pt0-200", "wz-atgc_pt200-300",
        "wz-atgc_pt300"
    };
    if (std::find(noLheWeights.begin(), noLheWeights.end(), name_) != noLheWeights.end())
        return 0;
    if (std::find(allLheWeights.begin(), allLheWeights.end(), name_) != allLheWeights.end())
        return 2;
    return 1;
}

void WZSelector::SlaveBegin(TTree * /*tree*/)
{
    pileupSF_ = (ScaleFactor *) GetInputList()->FindObject("pileupSF");
    if (pileupSF_ == nullptr ) 
        Abort("Must pass pileup weights SF");
    eIdSF_ = (ScaleFactor *) GetInputList()->FindObject("electronTightIdSF");
    if (eIdSF_ == nullptr ) 
        Abort("Must pass electron ID SF");
    mIdSF_ = (ScaleFactor *) GetInputList()->FindObject("muonTightIdSF");
    if (mIdSF_ == nullptr ) 
        Abort("Must pass muon ID SF");
    mIsoSF_ = (ScaleFactor *) GetInputList()->FindObject("muonIsoSF");
    if (mIsoSF_ == nullptr ) 
        Abort("Must pass muon Iso SF");
}
void WZSelector::LoadBranches(Long64_t entry) { 
    WZSelectorBase::Process(entry);
    
    b_ZMass->GetEntry(entry);
    b_Mass->GetEntry(entry);
    b_nvtx->GetEntry(entry);
    b_mjj->GetEntry(entry);
    b_type1_pfMETEt->GetEntry(entry);
    b_l1Eta->GetEntry(entry);
    b_l1Pt->GetEntry(entry);
    b_l2Eta->GetEntry(entry);
    b_l2Pt->GetEntry(entry);
    b_l3Eta->GetEntry(entry);
    b_l3Pt->GetEntry(entry);
    b_jetPt->GetEntry(entry);
    b_jetEta->GetEntry(entry);
    
    if (isMC_ && weight_info_ > 0) {
        b_scaleWeights->GetEntry(entry);
        lheWeights = *scaleWeights;
        if (weight_info_ == 2) {
            b_pdfWeights->GetEntry(entry);
            lheWeights.insert(lheWeights.begin(), pdfWeights->begin(), pdfWeights->end());
        }
    }
    dEtajj = 0;
    if (jetEta->size() >= 2)
        dEtajj = std::abs(jetEta->at(0) - jetEta->at(1));
}

bool WZSelector::PassesSelection(bool tightLeps) { 
    //if (type1_pfMETEt < 30)
    //    return true;
    //if (Mass < 100)
    //    return true;
    //if (l1Pt < 25 || l2Pt < 15)
    //    return true;
    //if (ZMass > 116.1876 || ZMass < 76.1876)
    //    return true;
    if (!passesLeptonVeto)
        return false;
    if (tightLeps && !(zlep1IsTight() && zlep2IsTight() && lepton3IsTight()))
        return false;
    return true;
}

void WZSelector::FillHistograms(float weight, bool noBlind) { 
    if (hists1D_["mjj"] != nullptr)
        hists1D_["mjj"]->Fill(mjj, weight*(isMC_ || mjj < 500 || noBlind));
    if (hists1D_["dEtajj"] != nullptr && dEtajj > 0)
        hists1D_["dEtajj"]->Fill(dEtajj, weight*(isMC_ || dEtajj < 2.5 || noBlind));
    if (hists1D_["Mass"] != nullptr)
        hists1D_["Mass"]->Fill(Mass, weight*(isMC_ || Mass < 400 || noBlind));
    if (hists1D_["ZMass"] != nullptr)
        hists1D_["ZMass"]->Fill(ZMass, weight*(isMC_ || Mass < 400 || noBlind));
    if (hists1D_["Zlep1_Pt"] != nullptr)
        hists1D_["Zlep1_Pt"]->Fill(l1Pt, weight);
    if (hists1D_["Zlep1_Eta"] != nullptr)
        hists1D_["Zlep1_Eta"]->Fill(l1Eta, weight);
    if (hists1D_["Zlep2_Pt"] != nullptr)
        hists1D_["Zlep2_Pt"]->Fill(l2Pt, weight);
    if (hists1D_["Zlep2_Eta"] != nullptr)
        hists1D_["Zlep2_Eta"]->Fill(l2Eta, weight);
    if (hists1D_["Wlep_Pt"] != nullptr)
        hists1D_["Wlep_Pt"]->Fill(l3Pt, weight);
    if (hists1D_["Wlep_Eta"] != nullptr)
        hists1D_["Wlep_Eta"]->Fill(l3Eta, weight);
    if (hists1D_["nvtx"] != nullptr)
        hists1D_["nvtx"]->Fill(nvtx, weight);
    for (size_t i = 0; i < lheWeights.size(); i++) {
        if (weighthists_["mjj"] != nullptr)
            weighthists_["mjj"]->Fill(mjj, i, lheWeights[i]*weight*(isMC_ || mjj < 500 || noBlind));
        if (weighthists_["dEtajj"] != nullptr && dEtajj > 0)
            weighthists_["dEtajj"]->Fill(dEtajj, i, lheWeights[i]*weight*(isMC_ || dEtajj < 2.5 || noBlind));
        if (weighthists_["Mass"] != nullptr)
            weighthists_["Mass"]->Fill(Mass, i, lheWeights[i]*weight*(isMC_ || Mass < 400 || noBlind));
        if (weighthists_["ZMass"] != nullptr)
            weighthists_["ZMass"]->Fill(ZMass, i, lheWeights[i]*weight*(isMC_ || Mass < 400 || noBlind));
        if (weighthists_["Zlep1_Pt"] != nullptr)
            weighthists_["Zlep1_Pt"]->Fill(l1Pt, i, lheWeights[i]*weight);
        if (weighthists_["Zlep1_Eta"] != nullptr)
            weighthists_["Zlep1_Eta"]->Fill(l1Eta, i, lheWeights[i]*weight);
        if (weighthists_["Zlep2_Pt"] != nullptr)
            weighthists_["Zlep2_Pt"]->Fill(l2Pt, i, lheWeights[i]*weight);
        if (weighthists_["Zlep2_Eta"] != nullptr)
            weighthists_["Zlep2_Eta"]->Fill(l2Eta, i, lheWeights[i]*weight);
        if (weighthists_["Wlep_Pt"] != nullptr)
            weighthists_["Wlep_Pt"]->Fill(l3Pt, i, lheWeights[i]*weight);
        if (weighthists_["Wlep_Eta"] != nullptr)
            weighthists_["Wlep_Eta"]->Fill(l3Eta, i, lheWeights[i]*weight);
        if (weighthists_["nvtx"] != nullptr)
            weighthists_["nvtx"]->Fill(nvtx, i, lheWeights[i]*weight);
    }
}

Bool_t WZSelector::Process(Long64_t entry)
{
    LoadBranches(entry);
    if (!PassesSelection(true))
        return true;

    if (isMC_) {
        if (channel_ == eee) {
            genWeight *= eIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
            genWeight *= eIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
            genWeight *= eIdSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
            genWeight *= pileupSF_->Evaluate1D(nTruePU);
        }
        else if (channel_ == eem) {
            genWeight *= eIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
            genWeight *= eIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
            genWeight *= mIdSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
            genWeight *= mIsoSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
            genWeight *= pileupSF_->Evaluate1D(nTruePU);
        }
        else if (channel_ == emm) {
            genWeight *= eIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
            genWeight *= mIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
            genWeight *= mIsoSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
            genWeight *= mIdSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
            genWeight *= mIsoSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
            genWeight *= pileupSF_->Evaluate1D(nTruePU);
        }
        else {
            genWeight *= mIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
            genWeight *= mIsoSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
            genWeight *= mIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
            genWeight *= mIsoSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
            genWeight *= mIdSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
            genWeight *= mIsoSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
            genWeight *= pileupSF_->Evaluate1D(nTruePU);
        }
    }
    
    FillHistograms(genWeight, false);
    
    return true;
}

std::vector<std::string> WZSelector::ReadHistData(std::string histDataString) {
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

void WZSelector::SetupNewDirectory()
{
    WZSelectorBase::SetupNewDirectory();
   
    TList* histInfo = (TList *) GetInputList()->FindObject("histinfo");
    if (histInfo == nullptr ) 
        Abort("Must pass histogram information");
    
    for (auto && entry : *histInfo) {  
        TNamed* currentHistInfo = dynamic_cast<TNamed*>(entry);
        std::string name = currentHistInfo->GetName();
        if (hists1D_.find(name) != hists1D_.end()) { 
            std::vector<std::string> histData = ReadHistData(currentHistInfo->GetTitle());
            if (histData.size() != 4) {
                std::cerr << "Malformed data string for histogram '" << name
                          << ".' Must have form: 'Title; (optional info) $ nbins, xmin, xmax'"
                          << std::endl;
                exit(1);
            }
            AddObject<TH1D>(hists1D_[name], 
                (name+"_"+channelName_).c_str(), histData[0].c_str(),
                std::stoi(histData[1]), std::stof(histData[2]), std::stof(histData[3]));
        }
        else
            std::cerr << "Skipping invalid histogram " << name << std::endl;
        if (isMC_ && (weighthists_.find(name) != weighthists_.end())) { 
            std::vector<std::string> histData = ReadHistData(currentHistInfo->GetTitle());
            AddObject<TH2D>(weighthists_[name], 
                (name+"_lheWeights_"+channelName_).c_str(), histData[0].c_str(),
                std::stoi(histData[1]), std::stof(histData[2]), std::stof(histData[3]),
                1000, 0, 1000);
        }
    }
}
