#include "Analysis/WZAnalysis/interface/WZSelector.h"
#include <boost/algorithm/string.hpp>

void WZSelector::Init(TTree *tree)
{
    WZSelectorBase::Init(tree);
    
    weight_info_ = 0;
    if (isMC_) {
        fChain->SetBranchAddress("nTruePU", &nTruePU, &b_nTruePU);
        weight_info_ = GetLheWeightInfo();
        if (weight_info_ > 0)
            fChain->SetBranchAddress("scaleWeights", &scaleWeights, &b_scaleWeights);
        if (weight_info_ == 2)
            fChain->SetBranchAddress("pdfWeights", &pdfWeights, &b_pdfWeights);
        fChain->SetBranchAddress("mjj_jesUp", &mjj_jesUp, &b_mjj_jesUp);
        fChain->SetBranchAddress("mjj_jesDown", &mjj_jesDown, &b_mjj_jesDown);
        fChain->SetBranchAddress("mjj_jerUp", &mjj_jerUp, &b_mjj_jerUp);
        fChain->SetBranchAddress("mjj_jerDown", &mjj_jerDown, &b_mjj_jerDown);
        fChain->SetBranchAddress("jetEta_jesUp", &jetEta_jesUp, &b_jetEta_jesUp);
        fChain->SetBranchAddress("jetEta_jesDown", &jetEta_jesDown, &b_jetEta_jesDown);
        fChain->SetBranchAddress("jetEta_jerUp", &jetEta_jerUp, &b_jetEta_jerUp);
        fChain->SetBranchAddress("jetEta_jerDown", &jetEta_jerDown, &b_jetEta_jerDown);
        fChain->SetBranchAddress("jetPt_jesUp", &jetPt_jesUp, &b_jetPt_jesUp);
        fChain->SetBranchAddress("jetPt_jesDown", &jetPt_jesDown, &b_jetPt_jesDown);
        fChain->SetBranchAddress("jetPt_jerUp", &jetPt_jerUp, &b_jetPt_jerUp);
        fChain->SetBranchAddress("jetPt_jerDown", &jetPt_jerDown, &b_jetPt_jerDown);
    }
    
    fChain->SetBranchAddress("jetPt", &jetPt, &b_jetPt);
    fChain->SetBranchAddress("nJets", &nJets, &b_nJets);
    fChain->SetBranchAddress("nJetCSVv2T", &nJetCSVv2T, &b_nJetCSVv2T);
    fChain->SetBranchAddress("jetEta", &jetEta, &b_jetEta);
    fChain->SetBranchAddress("Mass", &Mass, &b_Mass);
    fChain->SetBranchAddress("Eta", &Eta, &b_Eta);
    fChain->SetBranchAddress("Pt", &Pt, &b_Pt);
    fChain->SetBranchAddress("nvtx", &nvtx, &b_nvtx);
    fChain->SetBranchAddress("mjj", &mjj, &b_mjj);
    fChain->SetBranchAddress("type1_pfMETEt", &type1_pfMETEt, &b_type1_pfMETEt);
    fChain->SetBranchAddress("MtToMET", &MtToMET, &b_MtToMET);

    if (channel_ == eee) {
        fChain->SetBranchAddress("e3MtToMET", &l3MtToMET, &b_l3MtToMET);
        fChain->SetBranchAddress("e1_e2_Mass", &ZMass, &b_ZMass);
        fChain->SetBranchAddress("e1_e2_Pt", &ZPt, &b_ZPt);
        fChain->SetBranchAddress("e1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("e1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("e2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("e2Eta", &l2Eta, &b_l2Eta);
        fChain->SetBranchAddress("e3Pt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("e3Eta", &l3Eta, &b_l3Eta);
    }
    else if (channel_ == eem) { 
        fChain->SetBranchAddress("mMtToMET", &l3MtToMET, &b_l3MtToMET);
        fChain->SetBranchAddress("e1_e2_Mass", &ZMass, &b_ZMass);
        fChain->SetBranchAddress("e1_e2_Pt", &ZPt, &b_ZPt);
        fChain->SetBranchAddress("mPt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("mEta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("e1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("e1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("e2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("e2Eta", &l2Eta, &b_l2Eta);
    }
    else if (channel_ == emm) { 
        fChain->SetBranchAddress("eMtToMET", &l3MtToMET, &b_l3MtToMET);
        fChain->SetBranchAddress("m1_m2_Mass", &ZMass, &b_ZMass);
        fChain->SetBranchAddress("m1_m2_Pt", &ZPt, &b_ZPt);
        fChain->SetBranchAddress("ePt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("eEta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("m1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("m1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("m2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("m2Eta", &l2Eta, &b_l2Eta);
    }
    else if (channel_ == mmm) { 
        fChain->SetBranchAddress("m3MtToMET", &l3MtToMET, &b_l3MtToMET);
        fChain->SetBranchAddress("m1_m2_Mass", &ZMass, &b_ZMass);
        fChain->SetBranchAddress("m1_m2_Pt", &ZPt, &b_ZPt);
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
        "ggZZ2e2mu", "ggZZ4e", "ggZZ4m", "wzjj-vbfnlo-of",
        "wzjj-vbfnlo-sf", "st-schan", "st-tchan"
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
    b_mjj->GetEntry(entry);
    b_l1Eta->GetEntry(entry);
    b_l1Pt->GetEntry(entry);
    b_l2Eta->GetEntry(entry);
    b_l2Pt->GetEntry(entry);
    b_l3Eta->GetEntry(entry);
    b_l3Pt->GetEntry(entry);
    b_jetPt->GetEntry(entry);
    b_jetEta->GetEntry(entry);
    b_type1_pfMETEt->GetEntry(entry);
    b_Eta->GetEntry(entry);
    if (isMC_) {
        b_nTruePU->GetEntry(entry);
        b_mjj_jesUp->GetEntry(entry);
        b_mjj_jesDown->GetEntry(entry);
        b_mjj_jerUp->GetEntry(entry);
        b_mjj_jerDown->GetEntry(entry);
        b_jetEta_jesUp->GetEntry(entry);
        b_jetEta_jesDown->GetEntry(entry);
        b_jetEta_jerUp->GetEntry(entry);
        b_jetEta_jerDown->GetEntry(entry);
        b_jetPt_jesUp->GetEntry(entry);
        b_jetPt_jesDown->GetEntry(entry);
        b_jetPt_jerUp->GetEntry(entry);
        b_jetPt_jerDown->GetEntry(entry);

        dEtajj_jesUp = -1;
        if (jetEta_jesUp->size() >= 2)
            dEtajj_jesUp = std::abs(jetEta_jesUp->at(0) - jetEta_jesUp->at(1));

        dEtajj_jesDown = -1;
        if (jetEta_jesDown->size() >= 2)
            dEtajj_jesDown = std::abs(jetEta_jesDown->at(0) - jetEta_jesDown->at(1));

        dEtajj_jerUp = -1;
        if (jetEta_jerUp->size() >= 2)
            dEtajj_jerUp = std::abs(jetEta_jerUp->at(0) - jetEta_jerUp->at(1));

        dEtajj_jerDown = -1;
        if (jetEta_jerDown->size() >= 2)
            dEtajj_jerDown = std::abs(jetEta_jerDown->at(0) - jetEta_jerDown->at(1));
    }
    
    if (isMC_ && weight_info_ > 0) {
        b_scaleWeights->GetEntry(entry);
        lheWeights = *scaleWeights;
        if (weight_info_ == 2) {
            b_pdfWeights->GetEntry(entry);
            lheWeights.insert(lheWeights.begin(), pdfWeights->begin(), pdfWeights->end());
        }
    }
    dEtajj = -1;

    if (jetEta->size() >= 2) {
        dEtajj = std::abs(jetEta->at(0) - jetEta->at(1));
        if (isMC_) {
        }
    }
}

bool WZSelector::PassesVBSSelection(bool noBlind, float dijetMass, 
        std::vector<float>* jPt, std::vector<float>* jEta) { 
    if (jPt->size() != jEta->size() || jPt->size() < 2)
        return false;
    if (jPt->at(0) < 50 || jPt->at(1) < 50)
        return false;

    float deltaEtajj = std::abs(jEta->at(0) - jEta->at(1));
    float zep3l = Eta - 0.5*(jEta->at(1) + jEta->at(0));
    if (dijetMass < 500 && deltaEtajj < 2.5)
        return false;
    if (std::abs(zep3l) < 2.5)
        return false;

    return ((isMC_ || noBlind) && type1_pfMETEt > 50);
}

bool WZSelector::PassesSelection(bool tightLeps, Selection selection) { 
    //if (type1_pfMETEt < 30)
    //    return true;
    //if (Mass < 100)
    //    return true;
    //if (l1Pt < 25 || l2Pt < 15)
    //    return true;
    //if (ZMass > 116.1876 || ZMass < 76.1876)
    //    return true;
    if (!(Flag_BadChargedCandidateFilterPass
            && Flag_BadPFMuonFilterPass 
            && Flag_HBHENoiseFilterPass 
            && Flag_HBHENoiseIsoFilterPass 
            && Flag_EcalDeadCellTriggerPrimitiveFilterPass 
            && Flag_goodVerticesPass 
            && Flag_globalTightHalo2016FilterPass
            && (isMC_ || Flag_eeBadScFilterPass))
    ) 
        return false;
    if (!passesLeptonVeto)
        return false;
    if (tightLeps && !(zlep1IsTight() && zlep2IsTight() && lepton3IsTight()))
        return false;
    // Don't blind background estimation
    if (selection == VBSselection) {
        if (!PassesVBSSelection(!tightLeps, mjj, jetPt, jetEta))
            return false;
    }
    return true;
}

void WZSelector::FillVBSHistograms(Long64_t entry, float weight, bool noBlind) { 
    // JES/JER uncertainties
    // Need to separate check VBS cuts using JER/JES variations
    if (isMC_) {
        if (selection_ != VBSselection || PassesVBSSelection(noBlind, mjj_jesUp, jetPt_jesUp, jetEta_jesUp)) {
            if (hists1D_["mjj_jesUp"] != nullptr) {
                hists1D_["mjj_jesUp"]->Fill(mjj_jesUp, weight);
            }
            if (hists1D_["dEtajj_jesUp"] != nullptr) {
                hists1D_["dEtajj_jesUp"]->Fill(dEtajj_jesUp, weight);
            }
        }
        if (selection_ != VBSselection || PassesVBSSelection(noBlind, mjj_jesDown, jetPt_jesDown, jetEta_jesDown)) {
            if (hists1D_["mjj_jesDown"] != nullptr) {
                hists1D_["mjj_jesDown"]->Fill(mjj_jesDown, weight);
            }
            if (hists1D_["dEtajj_jesDown"] != nullptr) {
                hists1D_["dEtajj_jesDown"]->Fill(dEtajj_jesDown, weight);
            }
        }   
        if (selection_ != VBSselection || PassesVBSSelection(noBlind, mjj_jerUp, jetPt_jerUp, jetEta_jerUp)) {
            if (hists1D_["mjj_jerUp"] != nullptr) {
                hists1D_["mjj_jerUp"]->Fill(mjj_jerUp, weight);
            }
            if (hists1D_["dEtajj_jerUp"] != nullptr) {
                hists1D_["dEtajj_jerUp"]->Fill(dEtajj_jerUp, weight);
            }
        }
        if (selection_ != VBSselection || PassesVBSSelection(noBlind, mjj_jerDown, jetPt_jerDown, jetEta_jerDown)) {
            if (hists1D_["mjj_jerDown"] != nullptr) {
                hists1D_["mjj_jerDown"]->Fill(mjj_jerDown, weight);
            }
            if (hists1D_["dEtajj_jerDown"] != nullptr) {
                hists1D_["dEtajj_jerDown"]->Fill(dEtajj_jerDown, weight);
            }
        }   
    }
    
    if (selection_ == VBSselection && !PassesVBSSelection(noBlind, mjj, jetPt, jetEta))
        return;
    if (hists1D_["mjj"] != nullptr)
        hists1D_["mjj"]->Fill(mjj, weight*(isMC_ || (mjj < 500) || noBlind));
    if (hists1D_["dEtajj"] != nullptr)
        hists1D_["dEtajj"]->Fill(dEtajj, weight*(isMC_ || (dEtajj < 2.5) || noBlind));

    if (hists1D_["mjj_dEtajj2"] != nullptr && dEtajj < 2)
        hists1D_["mjj_dEtajj2"]->Fill(mjj, weight*(isMC_ || noBlind));
    if (hists1D_["mjj_dEtajj2p25"] != nullptr && dEtajj < 2.25)
        hists1D_["mjj_dEtajj2p25"]->Fill(mjj, weight*(isMC_ || noBlind));
    if (hists1D_["mjj_dEtajj2p5"] != nullptr && dEtajj < 2.5)
        hists1D_["mjj_dEtajj2p5"]->Fill(mjj, weight*(isMC_ || noBlind));
    if (hists1D_["mjj_dEtajj2p75"] != nullptr && dEtajj < 2.75)
        hists1D_["mjj_dEtajj2p75"]->Fill(mjj, weight*(isMC_ || noBlind));
    if (hists1D_["mjj_dEtajj3"] != nullptr && dEtajj < 3)
        hists1D_["mjj_dEtajj3"]->Fill(mjj, weight*(isMC_ || noBlind));
    if (hists1D_["mjj_dEtajj3p25"] != nullptr && dEtajj < 3.25)
        hists1D_["mjj_dEtajj3p25"]->Fill(mjj, weight*(isMC_ || noBlind));
    if (hists1D_["mjj_dEtajj3p5"] != nullptr && dEtajj < 3.5)
        hists1D_["mjj_dEtajj3p5"]->Fill(mjj, weight*(isMC_ || noBlind));
    if (hists1D_["mjj_dEtajj3p75"] != nullptr && dEtajj < 3.75)
        hists1D_["mjj_dEtajj3p75"]->Fill(mjj, weight*(isMC_ || noBlind));
    if (hists1D_["mjj_dEtajj4"] != nullptr && dEtajj < 4)
        hists1D_["mjj_dEtajj4"]->Fill(mjj, weight*(isMC_ || noBlind));

    if (hists1D_["dEtajj_mjj400"] != nullptr && mjj < 400)
        hists1D_["dEtajj_mjj400"]->Fill(dEtajj, weight*(isMC_ || noBlind));
    if (hists1D_["dEtajj_mjj450"] != nullptr && mjj < 450)
        hists1D_["dEtajj_mjj450"]->Fill(dEtajj, weight*(isMC_ || noBlind));
    if (hists1D_["dEtajj_mjj500"] != nullptr && mjj < 500)
        hists1D_["dEtajj_mjj500"]->Fill(dEtajj, weight*(isMC_ || noBlind));
    if (hists1D_["dEtajj_mjj550"] != nullptr && mjj < 550)
        hists1D_["dEtajj_mjj550"]->Fill(dEtajj, weight*(isMC_ || noBlind));
    if (hists1D_["dEtajj_mjj600"] != nullptr && mjj < 600)
        hists1D_["dEtajj_mjj600"]->Fill(dEtajj, weight*(isMC_ || noBlind));
    
    if (selection_ == VBSselection && !PassesVBSSelection(noBlind, mjj, jetPt, jetEta))
        return;

    if (hists1D_["jetPt[0]"] != nullptr && jetPt->size() > 0)
        hists1D_["jetPt[0]"]->Fill(jetPt->at(0), weight);
    if (hists1D_["jetPt[1]"] != nullptr && jetPt->size() > 1)
        hists1D_["jetPt[1]"]->Fill(jetPt->at(1), weight);
    if (hists1D_["jetPt[2]"] != nullptr && jetPt->size() > 2)
        hists1D_["jetPt[2]"]->Fill(jetPt->at(2), weight);
    if (hists1D_["jetEta[0]"] != nullptr && jetEta->size() > 0)
        hists1D_["jetEta[0]"]->Fill(jetEta->at(0), weight);
    if (hists1D_["jetEta[1]"] != nullptr && jetEta->size() > 1)
        hists1D_["jetEta[1]"]->Fill(jetEta->at(1), weight);
    if (hists1D_["jetEta[2]"] != nullptr && jetEta->size() > 2)
        hists1D_["jetEta[2]"]->Fill(jetEta->at(2), weight);
     
    // VBS Variables
    //if (hists1D_["zepj3"] != nullptr && jetEta->size() > 2)
    //    std::abs(Phi - dijetPhi) < 3.14159 ? abs(Phi - dijetPhi) : abs(Phi - dijetPhi) - 3.14159
    if (hists1D_["zep3l"] != nullptr && jetEta->size() >= 2) {
        hists1D_["zep3l"]->Fill(Eta - 0.5*(jetEta->at(1) + jetEta->at(0)), weight);
    }
    if (hists1D_["zepj3"] != nullptr && jetEta->size() > 2)
        hists1D_["zepj3"]->Fill(jetEta->at(2) - 0.5*(jetEta->at(1) + jetEta->at(0)), weight);
    
    
    // VBS Variables for cut optimization
    if (hists1D_["jetEta12"] != nullptr && jetEta->size() > 1) {
        hists1D_["jetEta12"]->Fill(jetEta->at(0), weight);
        hists1D_["jetEta12"]->Fill(jetEta->at(1), weight);
    }
}

void WZSelector::FillHistograms(Long64_t entry, float weight, bool noBlind) { 
    FillVBSHistograms(entry, weight, noBlind);
    if (selection_ == VBSselection && !PassesVBSSelection(noBlind, mjj, jetPt, jetEta))
        return;

    if (hists1D_["Mass"] != nullptr)
        hists1D_["Mass"]->Fill(Mass, weight*(isMC_ || Mass < 400 || noBlind));
    if (hists1D_["ZMass"] != nullptr)
        hists1D_["ZMass"]->Fill(ZMass, weight);
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
    if (hists1D_["MET"] != nullptr) {
        hists1D_["MET"]->Fill(type1_pfMETEt, weight);
    }
    if (hists1D_["nJets"] != nullptr) {
        b_nJets->GetEntry(entry);
        hists1D_["nJets"]->Fill(nJets, weight);
    }
    if (hists1D_["nJetCSVv2T"] != nullptr) {
        b_nJetCSVv2T->GetEntry(entry);
        hists1D_["nJetCSVv2T"]->Fill(nJetCSVv2T, weight);
    }
    if (hists1D_["Pt"] != nullptr) {
        b_Pt->GetEntry(entry);
        hists1D_["Pt"]->Fill(Pt, weight);
    }
    if (hists1D_["nvtx"] != nullptr) {
        b_nvtx->GetEntry(entry);
        hists1D_["nvtx"]->Fill(nvtx, weight);
    }
    if (hists1D_["MtW"] != nullptr) {
        b_l3MtToMET->GetEntry(entry);
        hists1D_["MtW"]->Fill(l3MtToMET, weight);
    }
    if (hists1D_["MTWZ"] != nullptr) {
        b_MtToMET->GetEntry(entry);
        hists1D_["MTWZ"]->Fill(MtToMET*(MtToMET < 340 || isMC_ || noBlind), weight);
    }
    if (hists1D_["ZPt"] != nullptr) {
        b_ZPt->GetEntry(entry);
        hists1D_["ZPt"]->Fill(ZPt, weight);
    }
    
        //for (size_t i = 0; i < lheWeights.size(); i++) {
    //    if (weighthists_["mjj"] != nullptr)
    //        weighthists_["mjj"]->Fill(mjj, i, lheWeights[i]*weight*(isMC_ || mjj < 500 || noBlind));
    //    if (weighthists_["dEtajj"] != nullptr && dEtajj > 0)
    //        weighthists_["dEtajj"]->Fill(dEtajj, i, lheWeights[i]*weight*(isMC_ || dEtajj < 2.5 || noBlind));
    //    if (weighthists_["Mass"] != nullptr)
    //        weighthists_["Mass"]->Fill(Mass, i, lheWeights[i]*weight*(isMC_ || Mass < 400 || noBlind));
    //    if (weighthists_["ZMass"] != nullptr)
    //        weighthists_["ZMass"]->Fill(ZMass, i, lheWeights[i]*weight*(isMC_ || Mass < 400 || noBlind));
    //    if (weighthists_["Zlep1_Pt"] != nullptr)
    //        weighthists_["Zlep1_Pt"]->Fill(l1Pt, i, lheWeights[i]*weight);
    //    if (weighthists_["Zlep1_Eta"] != nullptr)
    //        weighthists_["Zlep1_Eta"]->Fill(l1Eta, i, lheWeights[i]*weight);
    //    if (weighthists_["Zlep2_Pt"] != nullptr)
    //        weighthists_["Zlep2_Pt"]->Fill(l2Pt, i, lheWeights[i]*weight);
    //    if (weighthists_["Zlep2_Eta"] != nullptr)
    //        weighthists_["Zlep2_Eta"]->Fill(l2Eta, i, lheWeights[i]*weight);
    //    if (weighthists_["Wlep_Pt"] != nullptr)
    //        weighthists_["Wlep_Pt"]->Fill(l3Pt, i, lheWeights[i]*weight);
    //    if (weighthists_["Wlep_Eta"] != nullptr)
    //        weighthists_["Wlep_Eta"]->Fill(l3Eta, i, lheWeights[i]*weight);
    //    if (weighthists_["nvtx"] != nullptr)
    //        weighthists_["nvtx"]->Fill(nvtx, i, lheWeights[i]*weight);
    //    if (weighthists_["MtW"] != nullptr)
    //        weighthists_["MtW"]->Fill(l3MtToMET, i, lheWeights[i]*weight);
    //    if (weighthists_["ZPt"] != nullptr)
    //        weighthists_["ZPt"]->Fill(ZPt, i, lheWeights[i]*weight);
    //}
}

Bool_t WZSelector::Process(Long64_t entry)
{
    LoadBranches(entry);
    if (selection_ == VBSselection && !PassesSelection(true, tightleptons))
        return true;
    else if (!PassesSelection(true, selection_))
        return true;

    if (isMC_) {
        if (channel_ == eee) {
            genWeight *= eIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
            genWeight *= eIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
            genWeight *= eIdSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
        }
        else if (channel_ == eem) {
            genWeight *= eIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
            genWeight *= eIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
            genWeight *= mIdSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
            genWeight *= mIsoSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
        }
        else if (channel_ == emm) {
            genWeight *= eIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
            genWeight *= mIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
            genWeight *= mIsoSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
            genWeight *= mIdSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
            genWeight *= mIsoSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
        }
        else {
            genWeight *= mIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
            genWeight *= mIsoSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
            genWeight *= mIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
            genWeight *= mIsoSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
            genWeight *= mIdSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
            genWeight *= mIsoSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
        }
        genWeight *= pileupSF_->Evaluate1D(nTruePU);
    }
    
    FillHistograms(entry, genWeight, false);
    
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
