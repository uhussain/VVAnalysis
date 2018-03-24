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
    fChain->SetBranchAddress("jetPhi", &jetPhi, &b_jetPhi);
    fChain->SetBranchAddress("jetEta", &jetEta, &b_jetEta);
    fChain->SetBranchAddress("nJets", &nJets, &b_nJets);
    fChain->SetBranchAddress("jetCSVv2", &jetCSVv2, &b_jetCSVv2);
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
    b_jetPhi->GetEntry(entry);
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

        auto deltaRjj = [](std::vector<float>* jEta, std::vector<float>* jPhi) {
            if (jEta->size() != jPhi->size() || jEta->size() < 2)
                return -1.;
            double etaDiff = jEta->at(0) - jEta->at(1);
            double phiDiff = jPhi->at(0) - jPhi->at(1);
            return std::sqrt(etaDiff*etaDiff - phiDiff*phiDiff);
        };

        auto deltaEtajj = [](std::vector<float>* jEta) {
            if (jEta->size() < 2)
                return -1.;
            double etaDiff = jEta->at(0) - jEta->at(1);
            return etaDiff;
        };

        auto zeppenfeld = [](std::vector<float>* jEta, float objEta) {
            if (jEta->size() < 2)
                return -999.;
            return objEta - 0.5*(jEta->at(1) + jEta->at(0));
        };

        dEtajj = deltaEtajj(jetEta);
        zep3l = zeppenfeld(jetEta, Eta);
        dRjj = deltaRjj(jetEta, jetPhi);

        dEtajj_jesUp = deltaEtajj(jetEta_jesUp);
        zep3l_jesUp = zeppenfeld(jetEta_jesUp, Eta);
        dRjj = deltaRjj(jetEta_jesUp, jetPhi);

        dEtajj_jesDown = deltaEtajj(jetEta_jesDown);
        zep3l_jesDown = zeppenfeld(jetEta_jesDown, Eta);
        dRjj = deltaRjj(jetEta_jesDown, jetPhi);

        dEtajj_jerUp = deltaEtajj(jetEta_jerUp);
        zep3l_jerUp = zeppenfeld(jetEta_jerUp, Eta);
        dRjj = deltaRjj(jetEta_jerUp, jetPhi);

        dEtajj_jerDown = deltaEtajj(jetEta_jerDown);
        zep3l_jerDown = zeppenfeld(jetEta_jerDown, Eta);
        dRjj = deltaRjj(jetEta_jerDown, jetPhi);
    }
    
    if (isMC_ && weight_info_ > 0) {
        b_scaleWeights->GetEntry(entry);
        lheWeights = *scaleWeights;
        if (weight_info_ == 2) {
            b_pdfWeights->GetEntry(entry);
            lheWeights.insert(lheWeights.end(), pdfWeights->begin(), pdfWeights->end());
        }
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
        SafeHistFill(hists2D_, "mjj_etajj_2D", mjj, dEtajj, weight*(isMC_ || noBlind || mjj < 500 || dEtajj < 2.5));
        if (noBlind)
            std::cout << weight;
    }

    // Make 2D plots without cuts on eta and mjj
    if (isMC_) {
        if (!isVBS_|| PassesVBSSelection(noBlind, -1, jetPt_jesUp, jetEta_jesUp)) {
            SafeHistFill(hists2D_, "mjj_etajj_2D_jesUp", mjj_jesUp, dEtajj_jesUp, weight);
        }
        if (!isVBS_|| PassesVBSSelection(noBlind, -1, jetPt_jesDown, jetEta_jesDown)) {
            SafeHistFill(hists2D_, "mjj_etajj_2D_jesDown", mjj_jesDown, dEtajj_jesDown, weight);
        }
        if (!isVBS_|| PassesVBSSelection(noBlind, -1, jetPt_jerUp, jetEta_jerUp)) {
            SafeHistFill(hists2D_, "mjj_etajj_2D_jerUp", mjj_jerUp, dEtajj_jerUp, weight);
        }
        if (!isVBS_|| PassesVBSSelection(noBlind, -1, jetPt_jerDown, jetEta_jerDown)) {
            SafeHistFill(hists2D_, "mjj_etajj_2D_jerDown", mjj_jerDown, dEtajj_jerDown, weight);
        }
    }
    else if (noBlind && (!isVBS_ || PassesVBSSelection(noBlind, -1, jetPt, jetEta))) {
        SafeHistFill(hists2D_, "mjj_etajj_2D_jesUp", mjj,dEtajj, weight);
        SafeHistFill(hists2D_, "mjj_etajj_2D_jesDown", mjj, dEtajj, weight);
        SafeHistFill(hists2D_, "mjj_etajj_2D_jerUp", mjj, dEtajj, weight);
        SafeHistFill(hists2D_, "mjj_etajj_2D_jerDown", mjj, dEtajj, weight);
    }

    if (isMC_ && (!isVBS_|| PassesVBSSelection(noBlind, mjj_jesUp, jetPt_jesUp, jetEta_jesUp))) {
        SafeHistFill(hists1D_, "yield_jesUp", 1, weight);
        SafeHistFill(hists1D_, "mjj_jesUp", mjj_jesUp, weight);
        SafeHistFill(hists1D_, "MTWZ_jesUp", MtToMET, weight);
        SafeHistFill(hists1D_, "dEtajj_jesUp", dEtajj_jesUp, weight);
        SafeHistFill(hists2D_, "mjj_dRjj_2D_jesUp", mjj_jesUp, dRjj_jesUp, weight);
        if (jetPt_jesUp->size() > 0 && jetPt_jesUp->size() == jetEta_jesUp->size()) {
            SafeHistFill(hists1D_, "jetPt_jesUp[0]", jetPt_jesUp->at(0), weight);
            SafeHistFill(hists1D_, "jetEta_jesUp[0]", jetEta_jesUp->at(0), weight);
        }
        if (jetPt_jesUp->size() > 1 && jetPt_jesUp->size() == jetEta_jesUp->size()) {
            SafeHistFill(hists1D_, "jetPt_jesUp[1]", jetPt_jesUp->at(1), weight);
            SafeHistFill(hists1D_, "jetEta_jesUp[1]", jetEta_jesUp->at(1), weight);
        }
        if (jetPt_jesUp->size() > 2 && jetPt_jesUp->size() == jetEta_jesUp->size()) {
            SafeHistFill(hists1D_, "jetPt_jesUp[2]", jetPt_jesUp->at(2), weight);
            SafeHistFill(hists1D_, "jetEta_jesUp[2]", jetEta_jesUp->at(2), weight);
        }
    }
    if (isMC_ && (!isVBS_|| PassesVBSSelection(noBlind, mjj_jesDown, jetPt_jesDown, jetEta_jesDown))) {
        SafeHistFill(hists1D_, "yield_jesDown", 1, weight);
        SafeHistFill(hists1D_, "mjj_jesDown", mjj_jesDown, weight);
        SafeHistFill(hists1D_, "MTWZ_jesDown", MtToMET, weight);
        SafeHistFill(hists1D_, "dEtajj_jesDown", dEtajj_jesDown, weight);
        SafeHistFill(hists2D_, "mjj_dRjj_2D_jesDown", mjj_jesDown, dRjj_jesDown, weight);
        if (jetPt_jesDown->size() > 0 && jetPt_jesDown->size() == jetEta_jesDown->size()) {
            SafeHistFill(hists1D_, "jetPt_jesDown[0]", jetPt_jesDown->at(0), weight);
            SafeHistFill(hists1D_, "jetEta_jesDown[0]", jetEta_jesDown->at(0), weight);
        }
        if (jetPt_jesDown->size() > 1 && jetPt_jesDown->size() == jetEta_jesDown->size()) {
            SafeHistFill(hists1D_, "jetPt_jesDown[1]", jetPt_jesDown->at(1), weight);
            SafeHistFill(hists1D_, "jetEta_jesDown[1]", jetEta_jesDown->at(1), weight);
        }
        if (jetPt_jesDown->size() > 2 && jetPt_jesDown->size() == jetEta_jesDown->size()) {
            SafeHistFill(hists1D_, "jetPt_jesDown[2]", jetPt_jesDown->at(2), weight);
            SafeHistFill(hists1D_, "jetEta_jesDown[2]", jetEta_jesDown->at(2), weight);
        }
    }   
    if (isMC_&& (!isVBS_|| PassesVBSSelection(noBlind, mjj_jerUp, jetPt_jerUp, jetEta_jerUp))) {
        SafeHistFill(hists1D_, "yield_jerUp", 1, weight);
        SafeHistFill(hists1D_, "mjj_jerUp", mjj_jerUp, weight);
        SafeHistFill(hists1D_, "MTWZ_jerUp", MtToMET, weight);
        SafeHistFill(hists1D_, "dEtajj_jerUp", dEtajj_jerUp, weight);
        SafeHistFill(hists2D_, "mjj_dRjj_2D_jerUp", mjj_jerUp, dRjj_jerUp, weight);
        if (jetPt_jerUp->size() > 0 && jetPt_jerUp->size() == jetEta_jerUp->size()) {
            SafeHistFill(hists1D_, "jetPt_jerUp[0]", jetPt_jerUp->at(0), weight);
            SafeHistFill(hists1D_, "jetEta_jerUp[0]", jetEta_jerUp->at(0), weight);
        }
        if (jetPt_jerUp->size() > 1 && jetPt_jerUp->size() == jetEta_jerUp->size()) {
            SafeHistFill(hists1D_, "jetPt_jerUp[1]", jetPt_jerUp->at(1), weight);
            SafeHistFill(hists1D_, "jetEta_jerUp[1]", jetEta_jerUp->at(1), weight);
        }
        if (jetPt_jerUp->size() > 2 && jetPt_jerUp->size() == jetEta_jerUp->size()) {
            SafeHistFill(hists1D_, "jetPt_jerUp[2]", jetPt_jerUp->at(2), weight);
            SafeHistFill(hists1D_, "jetEta_jerUp[2]", jetEta_jerUp->at(2), weight);
        }
    }
    if (isMC_ && (!isVBS_ || PassesVBSSelection(noBlind, mjj_jerDown, jetPt_jerDown, jetEta_jerDown))) {
        SafeHistFill(hists1D_, "yield_jerDown", 1, weight);
        SafeHistFill(hists1D_, "jetPt_jerDown[0]", jetPt_jerDown->at(0), weight);
        SafeHistFill(hists1D_, "mjj_jerDown", mjj_jerDown, weight);
        SafeHistFill(hists1D_, "MTWZ_jerDown", MtToMET, weight);
        SafeHistFill(hists1D_, "dEtajj_jerDown", dEtajj_jerDown, weight);
        SafeHistFill(hists2D_, "mjj_dRjj_2D_jerDown", mjj_jerDown, dRjj_jerDown, weight);
        if (jetPt_jerDown->size() > 0 && jetPt_jerDown->size() == jetEta_jerDown->size()) {
            SafeHistFill(hists1D_, "jetPt_jerDown[0]", jetPt_jerDown->at(0), weight);
            SafeHistFill(hists1D_, "jetEta_jerDown[0]", jetEta_jerDown->at(0), weight);
        }
        if (jetPt_jerDown->size() > 1 && jetPt_jerDown->size() == jetEta_jerDown->size()) {
            SafeHistFill(hists1D_, "jetPt_jerDown[1]", jetPt_jerDown->at(1), weight);
            SafeHistFill(hists1D_, "jetEta_jerDown[1]", jetEta_jerDown->at(1), weight);
        }
        if (jetPt_jerDown->size() > 2 && jetPt_jerDown->size() == jetEta_jerDown->size()) {
            SafeHistFill(hists1D_, "jetPt_jerDown[2]", jetPt_jerDown->at(2), weight);
            SafeHistFill(hists1D_, "jetEta_jerDown[2]", jetEta_jerDown->at(2), weight);
        }
    }
    
    if (isVBS_ && !PassesVBSSelection(noBlind, mjj, jetPt, jetEta))
        return;
    // Useful for the nonprompt estimation
    if (!isMC_ && noBlind) {
        for (auto& var : systematicNames_) { 
            SafeHistFill(hists1D_, "yield_"+var, 1, weight);
            SafeHistFill(hists1D_,"mjj_"+var, mjj, weight);
            SafeHistFill(hists1D_, "MTWZ_"+var, MtToMET, weight);
            SafeHistFill(hists1D_, "dEtajj_"+var , dEtajj, weight);
            SafeHistFill(hists2D_, "mjj_dRjj_2D_"+var, mjj, dRjj, weight);
        }
    }
    SafeHistFill(hists1D_, "mjj", mjj, weight*(isMC_ || (mjj < 500) || noBlind));
    SafeHistFill(hists1D_, "dEtajj", dEtajj, weight*(isMC_ || (dEtajj < 2.5) || noBlind));

    if (jetPt->size() > 0 && jetPt->size() == jetEta->size()) {
        SafeHistFill(hists1D_, "jetPt[0]", jetPt->at(0), weight);
        SafeHistFill(hists1D_, "jetEta[0]", jetEta->at(0), weight);
    }
    if (jetPt->size() > 1 && jetPt->size() == jetEta->size()) {
        SafeHistFill(hists1D_, "jetPt[1]", jetPt->at(1), weight);
        SafeHistFill(hists1D_, "jetEta[1]", jetEta->at(1), weight);
    }
    if (jetPt->size() > 2 && jetPt->size() == jetEta->size()) {
        SafeHistFill(hists1D_, "jetPt[2]", jetPt->at(2), weight);
        SafeHistFill(hists1D_, "jetEta[2]", jetEta->at(2), weight);
    }
     
    // VBS Variables
    SafeHistFill(hists1D_, "zep3l", zep3l, weight);
    SafeHistFill(hists2D_, "mjj_dRjj_2D", mjj, dRjj, weight*(isMC_ || noBlind || mjj < 500 || dEtajj < 2.5));
    if (jetEta->size() > 3)
        SafeHistFill(hists1D_, "zepj3", jetEta->at(2) - 0.5*(jetEta->at(1) + jetEta->at(0)), weight);
    
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

    SafeHistFill(hists1D_, "yield", 1, weight);
    SafeHistFill(hists1D_, "Mass", Mass, weight*(isMC_ || Mass < 400 || noBlind));
    SafeHistFill(hists1D_, "ZMass", ZMass, weight);
    SafeHistFill(hists1D_, "ZPhi", ZPhi, weight);
    SafeHistFill(hists1D_, "ZEta", ZEta, weight);
    SafeHistFill(hists1D_, "Zlep1_Pt", l1Pt, weight);
    SafeHistFill(hists1D_, "Zlep1_Eta", l1Eta, weight);
    SafeHistFill(hists1D_, "Zlep1_Phi", l1Phi, weight);
    SafeHistFill(hists1D_, "Zlep2_Pt", l2Pt, weight);
    SafeHistFill(hists1D_, "Zlep2_Eta", l2Eta, weight);
    SafeHistFill(hists1D_, "Zlep2_Phi", l2Phi, weight);
    SafeHistFill(hists1D_, "Wlep_Pt", l3Pt, weight);
    SafeHistFill(hists1D_, "WlepRelPFIsoDBR04", l3Pt, weight);
    SafeHistFill(hists1D_, "Wlep_Eta", l3Eta, weight);
    SafeHistFill(hists1D_, "Wlep_Phi", l3Phi, weight);
    SafeHistFill(hists1D_, "MET", type1_pfMETEt, weight);
    SafeHistFill(hists1D_, "MtW", l3MtToMET, weight);

    if (hists1D_["dR_lW_Z"] != nullptr) {
        float dPhi_lW_Z = ZPhi - l3Phi;
        float dEta_lW_Z = ZEta - l3Eta;
        float dR_lW_Z = std::sqrt(dPhi_lW_Z*dPhi_lW_Z + dEta_lW_Z*dEta_lW_Z);
        hists1D_["dR_lW_Z"]->Fill(dR_lW_Z, weight);
    }
    //if (hists1D_["nJets"] != nullptr) {
    //    b_nJets->GetEntry(entry);
    //    hists1D_["nJets"]->Fill(nJets, weight);
    //}
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
        SafeHistFill(weighthists_, "yield", 1, i, lheWeights[i]/lheWeights[0]*weight);
        SafeHistFill(weighthists_, "mjj", mjj, i, lheWeights[i]/lheWeights[0]*weight);
        SafeHistFill(weighthists_, "Mass", Mass, i, lheWeights[i]/lheWeights[0]*weight);
        SafeHistFill(weighthists_, "MTWZ", MtToMET, i, lheWeights[i]/lheWeights[0]*weight);
        SafeHistFill(weighthists_, "M3lMET", m3lmet, i, lheWeights[i]/lheWeights[0]*weight);
        SafeHistFill(weighthists_, "l1Pt", l1Pt, i, lheWeights[i]/lheWeights[0]*weight);
        SafeHistFill(weighthists_, "l2Pt", l2Pt, i, lheWeights[i]/lheWeights[0]*weight);
        SafeHistFill(weighthists_, "l3Pt", l3Pt, i, lheWeights[i]/lheWeights[0]*weight);
        SafeHistFill(weighthists_, "ZPt", ZPt, i, lheWeights[i]/lheWeights[0]*weight);
        SafeHistFill(weighthists_, "Pt", ZPt, i, lheWeights[i]/lheWeights[0]*weight);
        SafeHistFill(weighthists_, "Mass", ZPt, i, lheWeights[i]/lheWeights[0]*weight);
        SafeHistFill(weighthists2D_, "mjj_etajj_2D", mjj, dEtajj, i, lheWeights[i]/lheWeights[0]*weight);
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

void WZSelector::InitialzeHistogram(std::string name, std::vector<std::string> histData) {
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
        if (std::find(systHists_.begin(), systHists_.end(), name) != systHists_.end()) {
            for (auto& syst : systematicNames_) {
                std::string syst_hist_name = name+"_"+syst;
                hists1D_[syst_hist_name] = {};
                AddObject<TH1D>(hists1D_[syst_hist_name], (syst_hist_name+"_"+channelName_).c_str(), 
                    histData[0].c_str(),nbins, xmin, xmax);
            }
        }
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
        if (std::find(systHists2D_.begin(), systHists2D_.end(), name) != systHists2D_.end()) {
            for (auto& syst : systematicNames_) {
                std::string syst_hist_name = name+"_"+syst;
                hists2D_[syst_hist_name] = {};
                AddObject<TH2D>(hists2D_[syst_hist_name], (syst_hist_name+"_"+channelName_).c_str(), 
                    histData[0].c_str(),nbins, xmin, xmax, nbinsy, ymin, ymax);
            }
        }
        // 3D weight hists must be subset of 2D hists!
        if (isMC_ && (weighthists2D_.find(name) != weighthists2D_.end())) { 
            AddObject<TH3D>(weighthists2D_[name], 
                (name+"_lheWeights_"+channelName_).c_str(), histData[0].c_str(),
                nbins, xmin, xmax, nbinsy, ymin, ymax, 1000, 0, 1000);
        }
    }
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
        std::vector<std::string> histData = ReadHistData(currentHistInfo->GetTitle());
        if (hists2D_.find(name) != hists2D_.end() || hists1D_.find(name) != hists1D_.end()) { 
            InitialzeHistogram(name, histData);
        }
        else
            std::cerr << "Skipping invalid histogram " << name << std::endl;
    }
}
