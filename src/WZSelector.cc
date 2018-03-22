#include "Analysis/WZAnalysis/interface/WZSelector.h"
#include "TLorentzVector.h"
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
    fChain->SetBranchAddress("jetCSVv2", &jetCSVv2, &b_jetCSVv2);
    fChain->SetBranchAddress("jetEta", &jetEta, &b_jetEta);
    fChain->SetBranchAddress("Mass", &Mass, &b_Mass);
    fChain->SetBranchAddress("Eta", &Eta, &b_Eta);
    fChain->SetBranchAddress("Pt", &Pt, &b_Pt);
    fChain->SetBranchAddress("nvtx", &nvtx, &b_nvtx);
    fChain->SetBranchAddress("mjj", &mjj, &b_mjj);
    fChain->SetBranchAddress("MtToMET", &MtToMET, &b_MtToMET);

    if (channel_ == eee) {
        fChain->SetBranchAddress("e1_e2_Mass", &ZMass, &b_ZMass);
        fChain->SetBranchAddress("e1_e2_Pt", &ZPt, &b_ZPt);
        fChain->SetBranchAddress("e1_e2_Eta", &ZEta, &b_ZEta);
        fChain->SetBranchAddress("e1_e2_Phi", &ZPhi, &b_ZPhi);
        fChain->SetBranchAddress("e1Phi", &l1Phi, &b_l1Phi);
        fChain->SetBranchAddress("e2Phi", &l2Phi, &b_l2Phi);
        fChain->SetBranchAddress("e3Phi", &l3Phi, &b_l3Phi);
    }
    else if (channel_ == eem) { 
        fChain->SetBranchAddress("e1_e2_Mass", &ZMass, &b_ZMass);
        fChain->SetBranchAddress("e1_e2_Pt", &ZPt, &b_ZPt);
        fChain->SetBranchAddress("e1_e2_Eta", &ZEta, &b_ZEta);
        fChain->SetBranchAddress("e1_e2_Phi", &ZPhi, &b_ZPhi);
        fChain->SetBranchAddress("mPhi", &l3Phi, &b_l3Phi);
        fChain->SetBranchAddress("e1Phi", &l1Phi, &b_l1Phi);
        fChain->SetBranchAddress("e2Phi", &l2Phi, &b_l2Phi);
    }
    else if (channel_ == emm) { 
        fChain->SetBranchAddress("m1_m2_Mass", &ZMass, &b_ZMass);
        fChain->SetBranchAddress("m1_m2_Pt", &ZPt, &b_ZPt);
        fChain->SetBranchAddress("m1_m2_Eta", &ZEta, &b_ZEta);
        fChain->SetBranchAddress("m1_m2_Phi", &ZPhi, &b_ZPhi);
        fChain->SetBranchAddress("ePhi", &l3Phi, &b_l3Phi);
        fChain->SetBranchAddress("m1Phi", &l1Phi, &b_l1Phi);
        fChain->SetBranchAddress("m2Phi", &l2Phi, &b_l2Phi);
    }
    else if (channel_ == mmm) { 
        fChain->SetBranchAddress("m1_m2_Mass", &ZMass, &b_ZMass);
        fChain->SetBranchAddress("m1_m2_Pt", &ZPt, &b_ZPt);
        fChain->SetBranchAddress("m1_m2_Eta", &ZEta, &b_ZEta);
        fChain->SetBranchAddress("m1_m2_Phi", &ZPhi, &b_ZPhi);
        fChain->SetBranchAddress("m1Phi", &l1Phi, &b_l1Phi);
        fChain->SetBranchAddress("m2Phi", &l2Phi, &b_l2Phi);
        fChain->SetBranchAddress("m3Phi", &l3Phi, &b_l3Phi);
    }
}

unsigned int WZSelector::GetLheWeightInfo() {
    std::vector<std::string> noLheWeights = {
        "ggZZ2e2mu", "ggZZ4e", "ggZZ4m", "wzjj-vbfnlo-of",
        "wzjj-vbfnlo-sf", "st-schan", "st-tchan", "st-tchan-t", "st-tchan-tbar"
    };
    std::vector<std::string> allLheWeights = {
        // PDF weights are saved in the ntuples but not really used at this point
        "wz3lnu-powheg", "wz3lnu-mg5amcnlo",
        "wz3lnu-mgmlm-0j", "wz3lnu-mgmlm-1j",
        "wz3lnu-mgmlm-2j", "wz3lnu-mgmlm-3j", "wlljj-ewk", 
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

void WZSelector::LoadBranches(Long64_t entry) { 
    WZSelectorBase::Process(entry);
    
    b_ZPhi->GetEntry(entry);
    b_ZEta->GetEntry(entry);
    b_Mass->GetEntry(entry);
    b_MtToMET->GetEntry(entry);
    b_l1Eta->GetEntry(entry);
    b_l1Phi->GetEntry(entry);
    b_l2Eta->GetEntry(entry);
    b_l2Phi->GetEntry(entry);
    b_l3Phi->GetEntry(entry);
    b_jetPt->GetEntry(entry);
    b_jetEta->GetEntry(entry);
    b_Eta->GetEntry(entry);
    b_mjj->GetEntry(entry);
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
        zep3l_jesUp = -1;
        if (jetEta_jesUp->size() >= 2) {
            dEtajj_jesUp = std::abs(jetEta_jesUp->at(0) - jetEta_jesUp->at(1));
            zep3l_jesUp = Eta - 0.5*(jetEta_jesUp->at(1) + jetEta_jesUp->at(0));
        }

        dEtajj_jesDown = -1;
        zep3l_jesDown = -1;
        if (jetEta_jesDown->size() >= 2) {
            dEtajj_jesDown = std::abs(jetEta_jesDown->at(0) - jetEta_jesDown->at(1));
            zep3l_jesDown = Eta - 0.5*(jetEta_jesDown->at(1) + jetEta_jesDown->at(0));
        }

        dEtajj_jerUp = -1;
        zep3l_jerUp = -1;
        if (jetEta_jerUp->size() >= 2) {
            dEtajj_jerUp = std::abs(jetEta_jerUp->at(0) - jetEta_jerUp->at(1));
            zep3l_jerUp = Eta - 0.5*(jetEta_jerUp->at(1) + jetEta_jerUp->at(0));
        }

        dEtajj_jerDown = -1;
        zep3l_jerDown = -1;
        if (jetEta_jerDown->size() >= 2) {
            dEtajj_jerDown = std::abs(jetEta_jerDown->at(0) - jetEta_jerDown->at(1));
            zep3l_jerDown = Eta - 0.5*(jetEta_jerDown->at(1) + jetEta_jerDown->at(0));
        }
    }
    
    if (isMC_ && weight_info_ > 0) {
        b_scaleWeights->GetEntry(entry);
        lheWeights = *scaleWeights;
        if (weight_info_ == 2) {
            b_pdfWeights->GetEntry(entry);
            lheWeights.insert(lheWeights.end(), pdfWeights->begin(), pdfWeights->end());
        }
    }
    dEtajj = -1;
    zep3l = -1;

    if (jetEta->size() >= 2) {
        dEtajj = std::abs(jetEta->at(0) - jetEta->at(1));
        zep3l = Eta - 0.5*(jetEta->at(1) + jetEta->at(0));
    }
}
bool WZSelector::PassesVBSBackgroundControlSelection(float dijetMass, 
        std::vector<float>* jPt, std::vector<float>* jEta) { 
    if (jPt->size() != jEta->size() || jPt->size() < 2)
        return false;
    float deltaEtajj = std::abs(jEta->at(0) - jEta->at(1));
    //return (dijetMass > 100 && (dijetMass < 500 || deltaEtajj < 2.5 || std::abs(zep3l) > 2.5));
    return (dijetMass > 100 && (dijetMass < 500 || deltaEtajj < 2.5 ));
    //return ((dijetMass > 500 && deltaEtajj < 2.5) || (dijetMass < 500 && deltaEtajj > 2.5));
}

bool WZSelector::PassesVBSSelection(bool noBlind, float dijetMass, 
        std::vector<float>* jPt, std::vector<float>* jEta) { 
    if (!(isMC_ || noBlind))
        return false;
    if (jPt->size() != jEta->size() || jPt->size() < 2)
        return false;

    // Use optimized point of pT(j1,j2) > 50 GeV
    if (selection_ != VBSselection_Loose && 
            selection_ != VBSBackgroundControl &&
            selection_ != VBSBackgroundControlLoose) { 
        if (jPt->at(0) < 50 || jPt->at(1) < 50)
            return false;
    }

    float deltaEtajj = std::abs(jEta->at(0) - jEta->at(1));

    // mjj > 400
    if (selection_ == VBSselection_mjj_400_detajj_2) { 
        return dijetMass > 400 && deltaEtajj > 2.;
    }
    else if (selection_ == VBSselection_mjj_400_detajj_2p5) { 
        return dijetMass > 400 && deltaEtajj > 2.5;
    }
    else if (selection_ == VBSselection_mjj_400_detajj_3) { 
        return dijetMass > 400 && deltaEtajj > 3.;
    }
    else if (selection_ == VBSselection_mjj_400_detajj_3p5) { 
        return dijetMass > 400 && deltaEtajj > 3.5;
    }
    else if (selection_ == VBSselection_mjj_400_detajj_4) { 
        return dijetMass > 400 && deltaEtajj > 4.;
    }
    // mjj > 450
    else if (selection_ == VBSselection_mjj_450_detajj_2) { 
        return dijetMass > 450 && deltaEtajj > 2.;
    }
    else if (selection_ == VBSselection_mjj_450_detajj_2p5) { 
        return dijetMass > 450 && deltaEtajj > 2.5;
    }
    else if (selection_ == VBSselection_mjj_450_detajj_3) { 
        return dijetMass > 450 && deltaEtajj > 3.;
    }
    else if (selection_ == VBSselection_mjj_450_detajj_3p5) { 
        return dijetMass > 450 && deltaEtajj > 3.5;
    }
    else if (selection_ == VBSselection_mjj_450_detajj_4) { 
        return dijetMass > 450 && deltaEtajj > 4.;
    }
    // mjj > 500
    else if (selection_ == VBSselection_mjj_500_detajj_2) { 
        return dijetMass > 500 && deltaEtajj > 2.;
    }
    else if (selection_ == VBSselection_mjj_500_detajj_2p5) { 
        return dijetMass > 500 && deltaEtajj > 2.5;
    }
    else if (selection_ == VBSselection_mjj_500_detajj_3) { 
        return dijetMass > 500 && deltaEtajj > 3.;
    }
    else if (selection_ == VBSselection_mjj_500_detajj_3p5) { 
        return dijetMass > 500 && deltaEtajj > 3.5;
    }
    else if (selection_ == VBSselection_mjj_500_detajj_4) { 
        return dijetMass > 500 && deltaEtajj > 4.;
    }
    // mjj > 550
    else if (selection_ == VBSselection_mjj_550_detajj_2) { 
        return dijetMass > 550 && deltaEtajj > 2.;
    }
    else if (selection_ == VBSselection_mjj_550_detajj_2p5) { 
        return dijetMass > 550 && deltaEtajj > 2.5;
    }
    else if (selection_ == VBSselection_mjj_550_detajj_3) { 
        return dijetMass > 550 && deltaEtajj > 3.;
    }
    else if (selection_ == VBSselection_mjj_550_detajj_3p5) { 
        return dijetMass > 550 && deltaEtajj > 3.5;
    }
    else if (selection_ == VBSselection_mjj_550_detajj_4) { 
        return dijetMass > 550 && deltaEtajj > 4.;
    }

    // mjj > 600
    else if (selection_ == VBSselection_mjj_600_detajj_2) { 
        return dijetMass > 600 && deltaEtajj > 2.;
    }
    else if (selection_ == VBSselection_mjj_600_detajj_2p5) { 
        return dijetMass > 600 && deltaEtajj > 2.5;
    }
    else if (selection_ == VBSselection_mjj_600_detajj_3) { 
        return dijetMass > 600 && deltaEtajj > 3.;
    }
    else if (selection_ == VBSselection_mjj_600_detajj_3p5) { 
        return dijetMass > 600 && deltaEtajj > 3.5;
    }
    else if (selection_ == VBSselection_mjj_600_detajj_4) { 
        return dijetMass > 600 && deltaEtajj > 4.;
    }
    else if (selection_ == VBSselection_Tight) { 
        //if (type1_pfMETEt < 50)
        //    return false;

        if (std::abs(zep3l) > 2.5)
            return false;
        if (dijetMass == -1)
            return true;
        return dijetMass > 500 && deltaEtajj > 2.5;
    }
    // Backgournd control
    else if (selection_ == VBSBackgroundControl) { 
        return ((dijetMass > 500 && deltaEtajj < 2.5) ||
                (dijetMass < 500 && deltaEtajj > 2.5));
    }
    else if (selection_ == VBSBackgroundControlLoose) { 
        return PassesVBSBackgroundControlSelection(mjj, jetPt, jetEta);
    }
    // Make selection with no mjj and etajj conditions
    if (dijetMass == -1)
        return true;
    return dijetMass > 500 && deltaEtajj > 2.5;
}

bool WZSelector::PassesBaseSelection(bool tightLeps, Selection selection) { 
    if (!(Flag_BadChargedCandidateFilterPass
            && Flag_HBHENoiseFilterPass 
            && Flag_HBHENoiseIsoFilterPass 
            && Flag_BadPFMuonFilterPass
            && Flag_EcalDeadCellTriggerPrimitiveFilterPass 
            && Flag_goodVerticesPass 
            && Flag_globalTightHalo2016FilterPass
            && (isMC_ || (Flag_eeBadScFilterPass
                && !Flag_duplicateMuonsPass 
                && !Flag_badMuonsPass)
            )
        )
    ) 
        return false;
    if (!passesLeptonVeto)
        return false;
    if (selection == FakeRateSelectionLoose || selection == FakeRateSelectionTight) {
        if (l1Pt < 25 || l2Pt < 15)
            return false;
        if (ZMass > 111.1876 || ZMass < 81.1876)
            return false;
        if (type1_pfMETEt > 25)
            return false;
        if (l3MtToMET > 30)
            return false;
        if (!tightZLeptons())
            return false;
        if (selection == FakeRateSelectionLoose)
            tightLeps = false;
    }
    if (selection == Inclusive2Jet && jetPt->size() < 2)
        return false;
    
    if (tightLeps && !(zlep1IsTight() && zlep2IsTight() && lepton3IsTight()))
        return false;
    if (!IsGenMatched3l()) {
        return false;
    }

    return true;
}

void WZSelector::FillVBSBackgroundControlHistograms(float weight, bool noBlind) { 
    if (PassesVBSBackgroundControlSelection(mjj, jetPt, jetEta)) {
        hists1D_["backgroundControlYield"]->Fill(1, weight);
        if (isMC_)
            for (size_t i = 0; i < lheWeights.size(); i++) {
                weighthists_["backgroundControlYield"]->Fill(1, i, lheWeights[i]/lheWeights[0]*weight);
        }
        // Useful nonprompt estimation
        if (!isMC_ && noBlind) {
            hists1D_["backgroundControlYield_jesUp"]->Fill(1, weight);
            hists1D_["backgroundControlYield_jesDown"]->Fill(1, weight);
            hists1D_["backgroundControlYield_jerUp"]->Fill(1, weight);
            hists1D_["backgroundControlYield_jerDown"]->Fill(1, weight);
            return;
        }
    }
    if (!isMC_)
        return;
    if (PassesVBSBackgroundControlSelection(mjj_jesUp, jetPt_jesUp, jetEta_jesUp)) {
        hists1D_["backgroundControlYield_jesUp"]->Fill(1, weight);
    }
    if (PassesVBSBackgroundControlSelection(mjj_jesDown, jetPt_jesDown, jetEta_jesDown)) {
        hists1D_["backgroundControlYield_jesDown"]->Fill(1, weight);
    }
    if (PassesVBSBackgroundControlSelection(mjj_jerUp, jetPt_jerUp, jetEta_jerUp)) {
        hists1D_["backgroundControlYield_jerUp"]->Fill(1, weight);
    }
    if (PassesVBSBackgroundControlSelection(mjj_jerDown, jetPt_jerDown, jetEta_jerDown)) {
        hists1D_["backgroundControlYield_jerDown"]->Fill(1, weight);
    }
}

void WZSelector::FillVBSHistograms(Long64_t entry, float weight, bool noBlind) { 
    // JES/JER uncertainties
    // Need to separate check VBS cuts using JER/JES variations
    if (hists1D_["backgroundControlYield"] != nullptr)
        FillVBSBackgroundControlHistograms(weight, noBlind);
    if (!isVBS_|| PassesVBSSelection(noBlind, -1, jetPt, jetEta)) {
        mjj_etajj_2Dhist_->Fill(mjj, dEtajj, weight*(isMC_ || noBlind || mjj < 500 || dEtajj < 2.5));
    }

    if (isMC_) {
        if (isMC_ && (!isVBS_|| PassesVBSSelection(noBlind, -1, jetPt_jesUp, jetEta_jesUp))) {
            mjj_etajj_2Dhist_jesUp_->Fill(mjj_jesUp, dEtajj_jesUp, weight);
        }
        if (isMC_ && (!isVBS_|| PassesVBSSelection(noBlind, -1, jetPt_jesDown, jetEta_jesDown))) {
            mjj_etajj_2Dhist_jesDown_->Fill(mjj_jesDown, dEtajj_jesDown, weight);
        }
        if (isMC_ && (!isVBS_|| PassesVBSSelection(noBlind, -1, jetPt_jerUp, jetEta_jerUp))) {
            mjj_etajj_2Dhist_jerUp_->Fill(mjj_jerUp, dEtajj_jerUp, weight);
        }
        if (isMC_ && (!isVBS_|| PassesVBSSelection(noBlind, -1, jetPt_jerDown, jetEta_jerDown))) {
            mjj_etajj_2Dhist_jerDown_->Fill(mjj_jerDown, dEtajj_jerDown, weight);
        }
    }
    else if (noBlind && (!isVBS_ || PassesVBSSelection(noBlind, -1, jetPt, jetEta))) {
        mjj_etajj_2Dhist_jesUp_->Fill(mjj,dEtajj, weight);
        mjj_etajj_2Dhist_jesDown_->Fill(mjj, dEtajj, weight);
        mjj_etajj_2Dhist_jerUp_->Fill(mjj, dEtajj, weight);
        mjj_etajj_2Dhist_jerDown_->Fill(mjj, dEtajj, weight);
    }

    if (isMC_ && (!isVBS_|| PassesVBSSelection(noBlind, mjj_jesUp, jetPt_jesUp, jetEta_jesUp))) {
        hists1D_["yield_jesUp"]->Fill(1, weight);
        mjj_mtwz_2Dhist_jesUp_->Fill(mjj_jesUp, MtToMET, weight);
        if (hists1D_["mjj_jesUp"] != nullptr) {
            hists1D_["mjj_jesUp"]->Fill(mjj_jesUp, weight);
        }
        if (hists1D_["MTWZ_jesUp"] != nullptr) {
            hists1D_["MTWZ_jesUp"]->Fill(MtToMET, weight);
        }
        if (hists1D_["dEtajj_jesUp"] != nullptr) {
            hists1D_["dEtajj_jesUp"]->Fill(dEtajj_jesUp, weight);
        }
        if (hists1D_["jetPt_jesUp[0]"] != nullptr && jetPt_jesUp->size() > 0)
            hists1D_["jetPt_jesUp[0]"]->Fill(jetPt_jesUp->at(0), weight);
        if (hists1D_["jetPt_jesUp[1]"] != nullptr && jetPt_jesUp->size() > 1)
            hists1D_["jetPt_jesUp[1]"]->Fill(jetPt_jesUp->at(1), weight);
        if (hists1D_["jetPt_jesUp[2]"] != nullptr && jetPt_jesUp->size() > 2)
            hists1D_["jetPt_jesUp[2]"]->Fill(jetPt_jesUp->at(2), weight);
        if (hists1D_["jetEta_jesUp[0]"] != nullptr && jetEta_jesUp->size() > 0)
            hists1D_["jetEta_jesUp[0]"]->Fill(jetEta_jesUp->at(0), weight);
        if (hists1D_["jetEta_jesUp[1]"] != nullptr && jetEta_jesUp->size() > 1)
            hists1D_["jetEta_jesUp[1]"]->Fill(jetEta_jesUp->at(1), weight);
        if (hists1D_["jetEta_jesUp[2]"] != nullptr && jetEta_jesUp->size() > 2)
            hists1D_["jetEta_jesUp[2]"]->Fill(jetEta_jesUp->at(2), weight);
    }
    if (isMC_ && (!isVBS_|| PassesVBSSelection(noBlind, mjj_jesDown, jetPt_jesDown, jetEta_jesDown))) {
        mjj_mtwz_2Dhist_jesDown_->Fill(mjj_jesDown, MtToMET, weight);
        hists1D_["yield_jesDown"]->Fill(1, weight);

        if (hists1D_["mjj_jesDown"] != nullptr) {
            hists1D_["mjj_jesDown"]->Fill(mjj_jesDown, weight);
        }
        if (hists1D_["MTWZ_jesDown"] != nullptr) {
            hists1D_["MTWZ_jesDown"]->Fill(MtToMET, weight);
        }
        if (hists1D_["dEtajj_jesDown"] != nullptr) {
            hists1D_["dEtajj_jesDown"]->Fill(dEtajj_jesDown, weight);
        }
        if (hists1D_["jetPt_jesDown[0]"] != nullptr && jetPt_jesDown->size() > 0)
            hists1D_["jetPt_jesDown[0]"]->Fill(jetPt_jesDown->at(0), weight);
        if (hists1D_["jetPt_jesDown[1]"] != nullptr && jetPt_jesDown->size() > 1)
            hists1D_["jetPt_jesDown[1]"]->Fill(jetPt_jesDown->at(1), weight);
        if (hists1D_["jetPt_jesDown[2]"] != nullptr && jetPt_jesDown->size() > 2)
            hists1D_["jetPt_jesDown[2]"]->Fill(jetPt_jesDown->at(2), weight);
        if (hists1D_["jetEta_jesDown[0]"] != nullptr && jetEta_jesDown->size() > 0)
            hists1D_["jetEta_jesDown[0]"]->Fill(jetEta_jesDown->at(0), weight);
        if (hists1D_["jetEta_jesDown[1]"] != nullptr && jetEta_jesDown->size() > 1)
            hists1D_["jetEta_jesDown[1]"]->Fill(jetEta_jesDown->at(1), weight);
        if (hists1D_["jetEta_jesDown[2]"] != nullptr && jetEta_jesDown->size() > 2)
            hists1D_["jetEta_jesDown[2]"]->Fill(jetEta_jesDown->at(2), weight);
    }   
    if (isMC_&& (!isVBS_|| PassesVBSSelection(noBlind, mjj_jerUp, jetPt_jerUp, jetEta_jerUp))) {
        mjj_mtwz_2Dhist_jerUp_->Fill(mjj_jerUp, MtToMET, weight);
        hists1D_["yield_jerUp"]->Fill(1, weight);

        if (hists1D_["mjj_jerUp"] != nullptr) {
            hists1D_["mjj_jerUp"]->Fill(mjj_jerUp, weight);
        }
        if (hists1D_["MTWZ_jerUp"] != nullptr) {
            hists1D_["MTWZ_jerUp"]->Fill(MtToMET, weight);
        }
        if (hists1D_["dEtajj_jerUp"] != nullptr) {
            hists1D_["dEtajj_jerUp"]->Fill(dEtajj_jerUp, weight);
        }
        if (hists1D_["jetPt_jerUp[0]"] != nullptr && jetPt_jerUp->size() > 0)
            hists1D_["jetPt_jerUp[0]"]->Fill(jetPt_jerUp->at(0), weight);
        if (hists1D_["jetPt_jerUp[1]"] != nullptr && jetPt_jerUp->size() > 1)
            hists1D_["jetPt_jerUp[1]"]->Fill(jetPt_jerUp->at(1), weight);
        if (hists1D_["jetPt_jerUp[2]"] != nullptr && jetPt_jerUp->size() > 2)
            hists1D_["jetPt_jerUp[2]"]->Fill(jetPt_jerUp->at(2), weight);
        if (hists1D_["jetEta_jerUp[0]"] != nullptr && jetEta_jerUp->size() > 0)
            hists1D_["jetEta_jerUp[0]"]->Fill(jetEta_jerUp->at(0), weight);
        if (hists1D_["jetEta_jerUp[1]"] != nullptr && jetEta_jerUp->size() > 1)
            hists1D_["jetEta_jerUp[1]"]->Fill(jetEta_jerUp->at(1), weight);
        if (hists1D_["jetEta_jerUp[2]"] != nullptr && jetEta_jerUp->size() > 2)
            hists1D_["jetEta_jerUp[2]"]->Fill(jetEta_jerUp->at(2), weight);
    }
    if (isMC_ && (!isVBS_ || PassesVBSSelection(noBlind, mjj_jerDown, jetPt_jerDown, jetEta_jerDown))) {
        mjj_mtwz_2Dhist_jerDown_->Fill(mjj_jerDown, MtToMET, weight);
        hists1D_["yield_jerDown"]->Fill(1, weight);

        if (hists1D_["mjj_jerDown"] != nullptr) {
            hists1D_["mjj_jerDown"]->Fill(mjj_jerDown, weight);
        }
        if (hists1D_["MTWZ_jerDown"] != nullptr) {
            hists1D_["MTWZ_jerDown"]->Fill(MtToMET, weight);
        }
        if (hists1D_["dEtajj_jerDown"] != nullptr) {
            hists1D_["dEtajj_jerDown"]->Fill(dEtajj_jerDown, weight);
        }
        if (hists1D_["jetPt_jerDown[0]"] != nullptr && jetPt_jerDown->size() > 0)
            hists1D_["jetPt_jerDown[0]"]->Fill(jetPt_jerDown->at(0), weight);
        if (hists1D_["jetPt_jerDown[1]"] != nullptr && jetPt_jerDown->size() > 1)
            hists1D_["jetPt_jerDown[1]"]->Fill(jetPt_jerDown->at(1), weight);
        if (hists1D_["jetPt_jerDown[2]"] != nullptr && jetPt_jerDown->size() > 2)
            hists1D_["jetPt_jerDown[2]"]->Fill(jetPt_jerDown->at(2), weight);
        if (hists1D_["jetEta_jerDown[0]"] != nullptr && jetEta_jerDown->size() > 0)
            hists1D_["jetEta_jerDown[0]"]->Fill(jetEta_jerDown->at(0), weight);
        if (hists1D_["jetEta_jerDown[1]"] != nullptr && jetEta_jerDown->size() > 1)
            hists1D_["jetEta_jerDown[1]"]->Fill(jetEta_jerDown->at(1), weight);
        if (hists1D_["jetEta_jerDown[2]"] != nullptr && jetEta_jerDown->size() > 2)
            hists1D_["jetEta_jerDown[2]"]->Fill(jetEta_jerDown->at(2), weight);
    }
    
    if (isVBS_ && !PassesVBSSelection(noBlind, mjj, jetPt, jetEta))
        return;
    mjj_mtwz_2Dhist_->Fill(mjj, MtToMET, weight*(isMC_ || noBlind || mjj < 500 || dEtajj < 2.5));
    mjj_zep3l_2Dhist_->Fill(mjj, std::abs(zep3l), weight*(isMC_ || noBlind || mjj < 500 || dEtajj < 2.5));
    etajj_zep3l_2Dhist_->Fill(dEtajj, std::abs(zep3l), weight*(isMC_ || noBlind || mjj < 500 || dEtajj < 2.5));
    // Useful for the nonprompt estimation
    if (!isMC_ && noBlind) {
        mjj_mtwz_2Dhist_jesUp_->Fill(mjj, MtToMET, weight);
        mjj_mtwz_2Dhist_jerUp_->Fill(mjj, MtToMET, weight);
        mjj_mtwz_2Dhist_jesUp_->Fill(mjj, MtToMET, weight);
        mjj_mtwz_2Dhist_jerUp_->Fill(mjj, MtToMET, weight);

        for (auto& var : systematicNames_) { 
            hists1D_["yield_"+var]->Fill(1, weight);
            if (hists1D_["mjj_"+var] != nullptr) {
                hists1D_["mjj_"+var ]->Fill(mjj, weight);
            }
            if (hists1D_["MTWZ_"+var ] != nullptr) {
                hists1D_["MTWZ_"+var ]->Fill(MtToMET, weight);
            }
            if (hists1D_["dEtajj_"+var ] != nullptr) {
                hists1D_["dEtajj_"+var ]->Fill(dEtajj, weight);
            }
        }
    }
    if (hists1D_["mjj"] != nullptr)
        hists1D_["mjj"]->Fill(mjj, weight*(isMC_ || (mjj < 500) || noBlind));
    if (hists1D_["dEtajj"] != nullptr)
        hists1D_["dEtajj"]->Fill(dEtajj, weight*(isMC_ || (dEtajj < 2.5) || noBlind));

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
    if (isVBS_ && !PassesVBSSelection(noBlind, mjj, jetPt, jetEta))
        return;

    if (hists1D_["yield"] == nullptr)
        throw std::invalid_argument("Yield histogram is required!");
    hists1D_["yield"]->Fill(1, weight);
    if (hists1D_["Mass"] != nullptr)
        hists1D_["Mass"]->Fill(Mass, weight*(isMC_ || Mass < 400 || noBlind));
    if (hists1D_["ZMass"] != nullptr)
        hists1D_["ZMass"]->Fill(ZMass, weight);
    if (hists1D_["ZPhi"] != nullptr)
        hists1D_["ZPhi"]->Fill(ZPhi, weight);
    if (hists1D_["ZEta"] != nullptr)
        hists1D_["ZEta"]->Fill(ZEta, weight);
    if (hists1D_["Zlep1_Pt"] != nullptr)
        hists1D_["Zlep1_Pt"]->Fill(l1Pt, weight);
    if (hists1D_["Zlep1_Eta"] != nullptr)
        hists1D_["Zlep1_Eta"]->Fill(l1Eta, weight);
    if (hists1D_["Zlep1_Phi"] != nullptr)
        hists1D_["Zlep1_Phi"]->Fill(l1Phi, weight);
    if (hists1D_["Zlep2_Pt"] != nullptr)
        hists1D_["Zlep2_Pt"]->Fill(l2Pt, weight);
    if (hists1D_["Zlep2_Eta"] != nullptr)
        hists1D_["Zlep2_Eta"]->Fill(l2Eta, weight);
    if (hists1D_["Zlep2_Phi"] != nullptr)
        hists1D_["Zlep2_Phi"]->Fill(l2Phi, weight);
    if (hists1D_["Wlep_Pt"] != nullptr)
        hists1D_["Wlep_Pt"]->Fill(l3Pt, weight);
    if (hists1D_["WlepRelPFIsoDBR04"] != nullptr)
        hists1D_["WlepRelPFIsoDBR04"]->Fill(l3Pt, weight);
    if (hists1D_["Wlep_Eta"] != nullptr)
        hists1D_["Wlep_Eta"]->Fill(l3Eta, weight);
    if (hists1D_["Wlep_Phi"] != nullptr)
        hists1D_["Wlep_Phi"]->Fill(l3Phi, weight);
    if (hists1D_["MET"] != nullptr) {
        hists1D_["MET"]->Fill(type1_pfMETEt, weight);
    }
    if (hists1D_["dR_lW_Z"] != nullptr) {
        float dPhi_lW_Z = ZPhi - l3Phi;
        float dEta_lW_Z = ZEta - l3Eta;
        float dR_lW_Z = std::sqrt(dPhi_lW_Z*dPhi_lW_Z + dEta_lW_Z*dEta_lW_Z);
        hists1D_["dR_lW_Z"]->Fill(dR_lW_Z, weight);
    }
    if (hists1D_["nJets"] != nullptr) {
        b_nJets->GetEntry(entry);
        hists1D_["nJets"]->Fill(nJets, weight);
    }
    if (hists1D_["nJetCSVv2T"] != nullptr) {
        b_jetCSVv2->GetEntry(entry);
        unsigned int bjets = 0;
        for (const auto& jetCSVval : *jetCSVv2) {
            if (jetCSVval > 0.9535)
                bjets++;
        }
        hists1D_["nJetCSVv2T"]->Fill(bjets, weight);
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
        hists1D_["MtW"]->Fill(l3MtToMET, weight);
    }
    if (hists1D_["MTWZ"] != nullptr) {
        TLorentzVector l1 = TLorentzVector();
        l1.SetPtEtaPhiM(l1Pt, l1Eta, l1Phi, 0);
        TLorentzVector l2 = TLorentzVector();
        l2.SetPtEtaPhiM(l2Pt, l2Eta, l2Phi, 0);
        TLorentzVector l3 = TLorentzVector();
        l3.SetPtEtaPhiM(l3Pt, l3Eta, l3Phi, 0);
        TLorentzVector met = TLorentzVector();
        met.SetPtEtaPhiM(type1_pfMETEt, 0, type1_pfMETPhi, 0);
        TLorentzVector zp4 = l1+l2;
        TLorentzVector wp4 = l3+met;
        TLorentzVector wzp4 = l1+l2+l3+met;
        float sumEt = zp4.Et() + wp4.Et();
        float mtwz = std::sqrt(sumEt*sumEt - wzp4.Pt()*wzp4.Pt());
        hists1D_["MTWZ"]->Fill(mtwz, weight*(isMC_ || MtToMET < 300 || noBlind));
    }
    float m3lmet = 0;
    if (hists1D_["M3lMET"] != nullptr) {
        TLorentzVector l1 = TLorentzVector();
        l1.SetPtEtaPhiM(l1Pt, l1Eta, l1Phi, 0);
        TLorentzVector l2 = TLorentzVector();
        l2.SetPtEtaPhiM(l2Pt, l2Eta, l2Phi, 0);
        TLorentzVector l3 = TLorentzVector();
        l3.SetPtEtaPhiM(l3Pt, l3Eta, l3Phi, 0);
        TLorentzVector met = TLorentzVector();
        met.SetPtEtaPhiM(type1_pfMETEt, 0, type1_pfMETPhi, 0);
        m3lmet= (l1+l2+l3+met).M();
        hists1D_["M3lMET"]->Fill(m3lmet, weight*(isMC_ || m3lmet < 400 || noBlind));
    }
    if (hists1D_["ZPt"] != nullptr) {
        b_ZPt->GetEntry(entry);
        hists1D_["ZPt"]->Fill(ZPt, weight);
    }
    
    for (size_t i = 0; i < lheWeights.size(); i++) {
        weighthists_["yield"]->Fill(1, i, lheWeights[i]/lheWeights[0]*weight);
        if (weighthists_["mjj"] != nullptr)
            weighthists_["mjj"]->Fill(mjj, i, lheWeights[i]/lheWeights[0]*weight);
        if (weighthists_["Mass"] != nullptr)
            weighthists_["Mass"]->Fill(Mass, i, lheWeights[i]/lheWeights[0]*weight);
        if (weighthists_["MTWZ"] != nullptr)
            weighthists_["MTWZ"]->Fill(MtToMET, i, lheWeights[i]/lheWeights[0]*weight);
        if (weighthists_["M3lMET"] != nullptr)
            weighthists_["M3lMET"]->Fill(m3lmet, i, lheWeights[i]/lheWeights[0]*weight);
        if (weighthists_["l1Pt"] != nullptr)
            weighthists_["l1Pt"]->Fill(l1Pt, i, lheWeights[i]/lheWeights[0]*weight);
        if (weighthists_["l2Pt"] != nullptr)
            weighthists_["l2Pt"]->Fill(l2Pt, i, lheWeights[i]/lheWeights[0]*weight);
        if (weighthists_["l3Pt"] != nullptr)
            weighthists_["l3Pt"]->Fill(l3Pt, i, lheWeights[i]/lheWeights[0]*weight);
        if (weighthists_["ZPt"] != nullptr)
            weighthists_["ZPt"]->Fill(ZPt, i, lheWeights[i]/lheWeights[0]*weight);
        if (weighthists_["Pt"] != nullptr)
            weighthists_["Pt"]->Fill(ZPt, i, lheWeights[i]/lheWeights[0]*weight);
        if (weighthists_["Mass"] != nullptr)
            weighthists_["Mass"]->Fill(ZPt, i, lheWeights[i]/lheWeights[0]*weight);

        mjj_etajj_lheWeights_3Dhist_->Fill(mjj, dEtajj, i, lheWeights[i]/lheWeights[0]*weight);
        mjj_mtwz_lheWeights_3Dhist_->Fill(mjj, MtToMET, i, lheWeights[i]/lheWeights[0]*weight);
    }
}

Bool_t WZSelector::Process(Long64_t entry)
{
    LoadBranches(entry);
    if (!PassesBaseSelection(true, selection_))
        return true;

    bool blindVBS = (selection_ == Wselection || 
            (isVBS_ && 
                selection_ != VBSBackgroundControl && 
                selection_ != VBSBackgroundControlLoose));

    FillHistograms(entry, weight, !blindVBS);
    
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
            std::string hist_name = name+"_"+channelName_;
            int nbins = std::stoi(histData[1]);
            float xmin = std::stof(histData[2]);
            float xmax = std::stof(histData[3]);
            AddObject<TH1D>(hists1D_[name], hist_name.c_str(), histData[0].c_str(),nbins, xmin, xmax);
            if (std::find(systHists_.begin(), systHists_.end(), name) != systHists_.end()) {
                for (auto& syst : systematicNames_) {
                    std::string syst_hist_name = name+"_"+syst;
                    hists1D_[syst_hist_name] = {};
                    AddObject<TH1D>(hists1D_[syst_hist_name], (syst_hist_name+"_"+channelName_).c_str(), 
                        histData[0].c_str(),nbins, xmin, xmax);
                }
            }
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
    AddObject<TH2D>(mjj_etajj_2Dhist_, ("mjj_etajj_2D_"+channelName_).c_str(), "#Delta#eta(j_{1}, j_{2}) vs. m_{jj}" , 
        50, 0, 2500, 28, 0, 7);
    AddObject<TH2D>(mjj_etajj_2Dhist_jesUp_, ("mjj_etajj_2D_jesUp_"+channelName_).c_str(), "#Delta#eta(j_{1}, j_{2}) vs. m_{jj}" , 
        50, 0, 2500, 28, 0, 7);
    AddObject<TH2D>(mjj_etajj_2Dhist_jesDown_, ("mjj_etajj_2D_jesDown_"+channelName_).c_str(), "#Delta#eta(j_{1}, j_{2}) vs. m_{jj}" , 
        50, 0, 2500, 28, 0, 7);
    AddObject<TH2D>(mjj_etajj_2Dhist_jerUp_, ("mjj_etajj_2D_jerUp_"+channelName_).c_str(), "#Delta#eta(j_{1}, j_{2}) vs. m_{jj}" , 
        50, 0, 2500, 28, 0, 7);
    AddObject<TH2D>(mjj_etajj_2Dhist_jerDown_, ("mjj_etajj_2D_jerDown_"+channelName_).c_str(), "#Delta#eta(j_{1}, j_{2}) vs. m_{jj}" , 
        50, 0, 2500, 28, 0, 7);
    AddObject<TH3D>(mjj_etajj_lheWeights_3Dhist_, ("mjj_etajj_2D_lheWeights_"+channelName_).c_str(), "#Delta#eta(j_{1}, j_{2}) vs. m_{jj}" , 
        50, 0, 2500, 28, 0, 7, 1000, 0, 1000);

    AddObject<TH2D>(mjj_mtwz_2Dhist_, ("mjj_mtwz_2D_"+channelName_).c_str(), "m_{T}(3\\ell, p_{T}^{miss}) vs. m_{jj}" , 
        50, 0, 2500, 200, 0, 1000);
    AddObject<TH2D>(mjj_mtwz_2Dhist_jesUp_, ("mjj_mtwz_2D_jesUp_"+channelName_).c_str(), "m_{T}(3\\ell, p_{T}^{miss}) vs. m_{jj}" ,
        50, 0, 2500, 200, 0, 1000);
    AddObject<TH2D>(mjj_mtwz_2Dhist_jesDown_, ("mjj_mtwz_2D_jesDown_"+channelName_).c_str(),"m_{T}(3\\ell, p_{T}^{miss}) vs. m_{jj}" ,
        50, 0, 2500, 200, 0, 1000);
    AddObject<TH2D>(mjj_mtwz_2Dhist_jerUp_, ("mjj_mtwz_2D_jerUp_"+channelName_).c_str(),"m_{T}(3\\ell, p_{T}^{miss}) vs. m_{jj}" ,
        50, 0, 2500, 200, 0, 1000);
    AddObject<TH2D>(mjj_mtwz_2Dhist_jerDown_, ("mjj_mtwz_2D_jerDown_"+channelName_).c_str(),"m_{T}(3\\ell, p_{T}^{miss}) vs. m_{jj}" ,
        50, 0, 2500, 200, 0, 1000);
    AddObject<TH3D>(mjj_mtwz_lheWeights_3Dhist_, ("mjj_mtwz_2D_lheWeights_"+channelName_).c_str(),"m_{T}(3\\ell, p_{T}^{miss}) vs. m_{jj}" ,
        50, 0, 2500, 200, 0, 1000, 1000, 0, 1000);

    AddObject<TH2D>(mjj_zep3l_2Dhist_, ("mjj_zep3l_2D_"+channelName_).c_str(), "zep(3\\ell) vs. m_{jj}" , 
        50, 0, 2500, 20, 0, 5);
    AddObject<TH2D>(etajj_zep3l_2Dhist_, ("etajj_zep3l_2D_"+channelName_).c_str(), "#Delta#eta(j_{1}, j_{2}) vs zep(3\\ell)" , 
        28, 0, 7, 20, 0, 5);
}
