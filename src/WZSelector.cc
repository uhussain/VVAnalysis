#include "Analysis/WZAnalysis/interface/WZSelector.h"
#include "TLorentzVector.h"
#include <boost/algorithm/string.hpp>

void WZSelector::Init(TTree *tree)
{
    WZSelectorBase::Init(tree);

    doSystematics_ = true;
    weight_info_ = 0;
    if (isMC_) {
        fChain->SetBranchAddress("nTruePU", &nTruePU, &b_nTruePU);
        weight_info_ = GetLheWeightInfo();
        if (weight_info_ > 0)
            fChain->SetBranchAddress("scaleWeights", &scaleWeights, &b_scaleWeights);
        if (weight_info_ == 2 || weight_info_ == 3)
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
    fChain->SetBranchAddress("jetCSVv2", &jetCSVv2, &b_jetCSVv2);
    fChain->SetBranchAddress("Mass", &Mass, &b_Mass);
    fChain->SetBranchAddress("Eta", &Eta, &b_Eta);
    fChain->SetBranchAddress("Pt", &Pt, &b_Pt);
    fChain->SetBranchAddress("nvtx", &nvtx, &b_nvtx);
    fChain->SetBranchAddress("mjj", &mjj, &b_mjj);
    //fChain->SetBranchAddress("MtToMET", &MtToMET, &b_MtToMET);

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
    std::vector<std::string> scaleAndPdfWeights = {
        "wz3lnu-powheg", "wz3lnu-mg5amcnlo",
        "wz3lnu-mgmlm-0j", "wz3lnu-mgmlm-1j",
        "wz3lnu-mgmlm-2j", "wz3lnu-mgmlm-3j", "wlljj-ewk", 
    };
    std::vector<std::string> allLheWeights = {
        "wzjj-aqgcft", "wzjj-aqgcfm", "wzjj-aqgcfs",
        //"wz-atgc_pt0-200", "wz-atgc_pt200-300",
        //"wz-atgc_pt300"
    };

    if (std::find(noLheWeights.begin(), noLheWeights.end(), name_) != noLheWeights.end())
        return 0;
    if (std::find(scaleAndPdfWeights.begin(), scaleAndPdfWeights.end(), name_) != scaleAndPdfWeights.end())
        return 2;
    if (std::find(allLheWeights.begin(), allLheWeights.end(), name_) != allLheWeights.end())
        return 3;
    return 1;
}

void WZSelector::LoadBranches(Long64_t entry, std::pair<Systematic, std::string> variation) { 
    WZSelectorBase::Process(entry);
    
    if (variation.first == Central) {
        b_ZPhi->GetEntry(entry);
        b_ZEta->GetEntry(entry);
        b_Mass->GetEntry(entry);
        //b_MtToMET->GetEntry(entry);
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
            if (isMC_ && weight_info_ > 0) {
                b_scaleWeights->GetEntry(entry);
                lheWeights = *scaleWeights;
                if (weight_info_ == 2) {
                    b_pdfWeights->GetEntry(entry);
                    // Only keep NNPDF weights
                    lheWeights.insert(lheWeights.end(), pdfWeights->begin(), 
                        pdfWeights->begin()+std::min(static_cast<size_t>(103), pdfWeights->size()));
                }
                else if (weight_info_ == 3) {
                    b_pdfWeights->GetEntry(entry);
                    lheWeights.insert(lheWeights.end(), pdfWeights->begin(), pdfWeights->end());
                }
            }
        }

        if (hists1D_["MTWZ"] != nullptr || hists1D_["M3lMET"] == nullptr) {
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
            MtWZ = std::sqrt(sumEt*sumEt - wzp4.Pt()*wzp4.Pt());
            M3lMET = (l1+l2+l3+met).M();
        }
    }
    else if (isMC_) {
        if (variation.first == jetEnergyScaleUp) {
            b_mjj_jesUp->GetEntry(entry);
            b_jetEta_jesUp->GetEntry(entry);
            b_jetPt_jesUp->GetEntry(entry);

            mjj = mjj_jesUp;
            jetEta = jetEta_jesUp;
            jetPt = jetPt_jesUp;
        }
        else if (variation.first == jetEnergyScaleDown) {
            b_mjj_jesDown->GetEntry(entry);
            b_jetEta_jesDown->GetEntry(entry);
            b_jetPt_jesDown->GetEntry(entry);

            mjj = mjj_jesDown;
            jetEta = jetEta_jesDown;
            jetPt = jetPt_jesDown;
        }
        else if (variation.first == jetEnergyResolutionUp) {
            b_mjj_jerUp->GetEntry(entry);
            b_jetEta_jerUp->GetEntry(entry);
            b_jetPt_jerUp->GetEntry(entry);

            mjj = mjj_jerUp;
            jetEta = jetEta_jerUp;
            jetPt = jetPt_jerUp;
        }
        else if (variation.first == jetEnergyResolutionDown) {
            b_mjj_jerDown->GetEntry(entry);
            b_jetEta_jerDown->GetEntry(entry);
            b_jetPt_jerDown->GetEntry(entry);

            mjj = mjj_jerDown;
            jetEta = jetEta_jerDown;
            jetPt = jetPt_jerDown;
        }
    }

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
        return std::abs(etaDiff);
    };

    auto zeppenfeld = [](std::vector<float>* jEta, float objEta) {
        if (jEta->size() < 2)
            return -999.;
        return objEta - 0.5*(jEta->at(1) + jEta->at(0));
    };


    dEtajj = deltaEtajj(jetEta);
    dRjj = deltaRjj(jetEta, jetPhi);
    zep3l = zeppenfeld(jetEta, Eta);
}
bool WZSelector::PassesVBSBackgroundControlSelection() {
    if (jetPt->size() != jetEta->size() || jetPt->size() < 2)
        return false;
    //return (dijetMass > 100 && (dijetMass < 500 || dEtajj < 2.5 || std::abs(zep3l) > 2.5));
    return (mjj > 100 && (mjj < 500 || dEtajj < 2.5 ));
    //return ((mjj > 500 && dEtajj < 2.5) || (mjj < 500 && dEtajj > 2.5));
}

bool WZSelector::PassesVBSSelection(bool noBlind) {
    if (!(isMC_ || noBlind))
        return false;
    if (jetPt->size() != jetEta->size() || jetPt->size() < 2)
        return false;

    // Use optimized point of pT(j1,j2) > 50 GeV
    if (selection_ != VBSselection_Loose && 
            selection_ != VBSBackgroundControl &&
            selection_ != VBSBackgroundControlLoose) { 
        if (jetPt->at(0) < 50 || jetPt->at(1) < 50)
            return false;
    }

    // mjj > 400
    if (selection_ == VBSselection_mjj_400_detajj_2) { 
        return mjj > 400 && dEtajj > 2.;
    }
    else if (selection_ == VBSselection_mjj_400_detajj_2p5) { 
        return mjj > 400 && dEtajj > 2.5;
    }
    else if (selection_ == VBSselection_mjj_400_detajj_3) { 
        return mjj > 400 && dEtajj > 3.;
    }
    else if (selection_ == VBSselection_mjj_400_detajj_3p5) { 
        return mjj > 400 && dEtajj > 3.5;
    }
    else if (selection_ == VBSselection_mjj_400_detajj_4) { 
        return mjj > 400 && dEtajj > 4.;
    }
    // mjj > 450
    else if (selection_ == VBSselection_mjj_450_detajj_2) { 
        return mjj > 450 && dEtajj > 2.;
    }
    else if (selection_ == VBSselection_mjj_450_detajj_2p5) { 
        return mjj > 450 && dEtajj > 2.5;
    }
    else if (selection_ == VBSselection_mjj_450_detajj_3) { 
        return mjj > 450 && dEtajj > 3.;
    }
    else if (selection_ == VBSselection_mjj_450_detajj_3p5) { 
        return mjj > 450 && dEtajj > 3.5;
    }
    else if (selection_ == VBSselection_mjj_450_detajj_4) { 
        return mjj > 450 && dEtajj > 4.;
    }
    // mjj > 500
    else if (selection_ == VBSselection_mjj_500_detajj_2) { 
        return mjj > 500 && dEtajj > 2.;
    }
    else if (selection_ == VBSselection_mjj_500_detajj_2p5) { 
        return mjj > 500 && dEtajj > 2.5;
    }
    else if (selection_ == VBSselection_mjj_500_detajj_3) { 
        return mjj > 500 && dEtajj > 3.;
    }
    else if (selection_ == VBSselection_mjj_500_detajj_3p5) { 
        return mjj > 500 && dEtajj > 3.5;
    }
    else if (selection_ == VBSselection_mjj_500_detajj_4) { 
        return mjj > 500 && dEtajj > 4.;
    }
    // mjj > 550
    else if (selection_ == VBSselection_mjj_550_detajj_2) { 
        return mjj > 550 && dEtajj > 2.;
    }
    else if (selection_ == VBSselection_mjj_550_detajj_2p5) { 
        return mjj > 550 && dEtajj > 2.5;
    }
    else if (selection_ == VBSselection_mjj_550_detajj_3) { 
        return mjj > 550 && dEtajj > 3.;
    }
    else if (selection_ == VBSselection_mjj_550_detajj_3p5) { 
        return mjj > 550 && dEtajj > 3.5;
    }
    else if (selection_ == VBSselection_mjj_550_detajj_4) { 
        return mjj > 550 && dEtajj > 4.;
    }

    // mjj > 600
    else if (selection_ == VBSselection_mjj_600_detajj_2) { 
        return mjj > 600 && dEtajj > 2.;
    }
    else if (selection_ == VBSselection_mjj_600_detajj_2p5) { 
        return mjj > 600 && dEtajj > 2.5;
    }
    else if (selection_ == VBSselection_mjj_600_detajj_3) { 
        return mjj > 600 && dEtajj > 3.;
    }
    else if (selection_ == VBSselection_mjj_600_detajj_3p5) { 
        return mjj > 600 && dEtajj > 3.5;
    }
    else if (selection_ == VBSselection_mjj_600_detajj_4) { 
        return mjj > 600 && dEtajj > 4.;
    }
    else if (selection_ == VBSselection_Tight) { 
        //if (type1_pfMETEt < 50)
        //    return false;

        if (std::abs(zep3l) > 2.5)
            return false;
        return mjj > 500 && dEtajj > 2.5;
    }
    // Backgournd control
    else if (selection_ == VBSBackgroundControl) { 
        return ((mjj > 500 && dEtajj < 2.5) ||
                (mjj < 500 && dEtajj > 2.5));
    }
    else if (selection_ == VBSBackgroundControlLoose) { 
        return PassesVBSBackgroundControlSelection();
    }
    return mjj > 500 && dEtajj > 2.5;
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

void WZSelector::FillVBSHistograms(float weight, bool noBlind,
        std::pair<Systematic, std::string> variation) { 
    // JES/JER uncertainties
    // Need to separate check VBS cuts using JER/JES variations
    SafeHistFill(hists2D_, getHistName("mjj_etajj_2D", variation.second), 
        mjj, dEtajj, weight*(isMC_ || noBlind || mjj < 500 || dEtajj < 2.5));
    SafeHistFill(hists1D_, getHistName("zep3l", variation.second), zep3l, weight);
    SafeHistFill(hists2D_, getHistName("mjj_dRjj_2D", variation.second), mjj, dRjj, weight*(isMC_ || noBlind || mjj < 500 || dEtajj < 2.5));

    SafeHistFill(hists1D_, getHistName("mjj", variation.second), mjj, weight*(isMC_ || (mjj < 500) || noBlind));
    SafeHistFill(hists1D_, getHistName("dEtajj", variation.second), dEtajj, weight*(isMC_ || (dEtajj < 2.5) || noBlind));

    if (jetPt->size() > 0 && jetPt->size() == jetEta->size()) {
        SafeHistFill(hists1D_, getHistName("jetPt[0]", variation.second), jetPt->at(0), weight);
        SafeHistFill(hists1D_, getHistName("jetEta[0]", variation.second), jetEta->at(0), weight);
    }
    if (jetPt->size() > 1 && jetPt->size() == jetEta->size()) {
        SafeHistFill(hists1D_, getHistName("jetPt[1]", variation.second), jetPt->at(1), weight);
        SafeHistFill(hists1D_, getHistName("jetEta[1]", variation.second), jetEta->at(1), weight);
    }
    if (jetPt->size() > 2 && jetPt->size() == jetEta->size()) {
        SafeHistFill(hists1D_, getHistName("jetPt[2]", variation.second), jetPt->at(2), weight);
        SafeHistFill(hists1D_, getHistName("jetEta[2]", variation.second), jetEta->at(2), weight);
    }
     
    if (jetEta->size() > 3)
        SafeHistFill(hists1D_, getHistName("zepj3", variation.second), jetEta->at(2) - 0.5*(jetEta->at(1) + jetEta->at(0)), weight);
    
    if (hists1D_[getHistName("jetEta12", variation.second)] != nullptr && jetEta->size() > 1) {
        hists1D_[getHistName("jetEta12", variation.second)]->Fill(jetEta->at(0), weight);
        hists1D_[getHistName("jetEta12", variation.second)]->Fill(jetEta->at(1), weight);
    }
}

std::string WZSelector::getHistName(std::string histName, std::string variationName) {
    return variationName == "" ? histName : histName + "_" + variationName;
}

void WZSelector::FillHistograms(Long64_t entry, float weight, bool noBlind, 
        std::pair<Systematic, std::string> variation) { 
    bool passesVBS = PassesVBSSelection(noBlind);
    if (hists1D_[getHistName("backgroundControlYield", variation.second)] != nullptr)
        if (PassesVBSBackgroundControlSelection())
            hists1D_[getHistName("backgroundControlYield", variation.second)]->Fill(1, weight);

    if (variation.first == Central && isMC_) {
        for (size_t i = 0; i < lheWeights.size(); i++) {
            if (PassesVBSBackgroundControlSelection())
                SafeHistFill(weighthists_, "backgroundControlYield", 1, i, lheWeights[i]/lheWeights[0]*weight);
            if (isVBS_ && !passesVBS)
                continue;
            SafeHistFill(weighthists_, "yield", 1, i, lheWeights[i]/lheWeights[0]*weight);
            SafeHistFill(weighthists_, "mjj", mjj, i, lheWeights[i]/lheWeights[0]*weight);
            SafeHistFill(weighthists_, "Mass", Mass, i, lheWeights[i]/lheWeights[0]*weight);
            SafeHistFill(weighthists_, "MTWZ", MtWZ, i, lheWeights[i]/lheWeights[0]*weight);
            SafeHistFill(weighthists_, "M3lMET", M3lMET, i, lheWeights[i]/lheWeights[0]*weight);
            SafeHistFill(weighthists_, "l1Pt", l1Pt, i, lheWeights[i]/lheWeights[0]*weight);
            SafeHistFill(weighthists_, "l2Pt", l2Pt, i, lheWeights[i]/lheWeights[0]*weight);
            SafeHistFill(weighthists_, "l3Pt", l3Pt, i, lheWeights[i]/lheWeights[0]*weight);
            SafeHistFill(weighthists_, "ZPt", ZPt, i, lheWeights[i]/lheWeights[0]*weight);
            SafeHistFill(weighthists_, "Pt", ZPt, i, lheWeights[i]/lheWeights[0]*weight);
            SafeHistFill(weighthists_, "Mass", ZPt, i, lheWeights[i]/lheWeights[0]*weight);
            SafeHistFill(weighthists2D_, "mjj_etajj_2D", mjj, dEtajj, i, lheWeights[i]/lheWeights[0]*weight);
            SafeHistFill(weighthists2D_, "mjj_dRjj_2D", mjj, dRjj, i, lheWeights[i]/lheWeights[0]*weight);
        }
    }
    if (isVBS_ && !passesVBS)
        return;
    FillVBSHistograms(weight, noBlind, variation);

    SafeHistFill(hists1D_, getHistName("yield", variation.second), 1, weight);
    SafeHistFill(hists1D_, getHistName("Mass", variation.second), Mass, 
        weight*(isMC_ || Mass < 400 || noBlind));
    SafeHistFill(hists1D_, getHistName("ZMass", variation.second), ZMass, weight);
    SafeHistFill(hists1D_, getHistName("ZPhi", variation.second), ZPhi, weight);
    SafeHistFill(hists1D_, getHistName("ZEta", variation.second), ZEta, weight);
    SafeHistFill(hists1D_, getHistName("Zlep1_Pt", variation.second), l1Pt, weight);
    SafeHistFill(hists1D_, getHistName("Zlep1_Eta", variation.second), l1Eta, weight);
    SafeHistFill(hists1D_, getHistName("Zlep1_Phi", variation.second), l1Phi, weight);
    SafeHistFill(hists1D_, getHistName("Zlep2_Pt", variation.second), l2Pt, weight);
    SafeHistFill(hists1D_, getHistName("Zlep2_Eta", variation.second), l2Eta, weight);
    SafeHistFill(hists1D_, getHistName("Zlep2_Phi", variation.second), l2Phi, weight);
    SafeHistFill(hists1D_, getHistName("Wlep_Pt", variation.second), l3Pt, weight);
    SafeHistFill(hists1D_, getHistName("WlepRelPFIsoDBR04", variation.second), l3Pt, weight);
    SafeHistFill(hists1D_, getHistName("Wlep_Eta", variation.second), l3Eta, weight);
    SafeHistFill(hists1D_, getHistName("Wlep_Phi", variation.second), l3Phi, weight);
    SafeHistFill(hists1D_, getHistName("MET", variation.second), type1_pfMETEt, weight);
    SafeHistFill(hists1D_, getHistName("MtW", variation.second), l3MtToMET, weight);
    SafeHistFill(hists1D_, getHistName("nJets", variation.second), jetPt->size(), weight);

    if (hists1D_[getHistName("dR_lW_Z", variation.second)] != nullptr) {
        float dPhi_lW_Z = ZPhi - l3Phi;
        float dEta_lW_Z = ZEta - l3Eta;
        float dR_lW_Z = std::sqrt(dPhi_lW_Z*dPhi_lW_Z + dEta_lW_Z*dEta_lW_Z);
        hists1D_[getHistName("dR_lW_Z", variation.second)]->Fill(dR_lW_Z, weight);
    }
    if (hists1D_[getHistName("nJetCSVv2T", variation.second)] != nullptr) {
        b_jetCSVv2->GetEntry(entry);
        unsigned int bjets = 0;
        for (const auto& jetCSVval : *jetCSVv2) {
            if (jetCSVval > 0.9535)
                bjets++;
        }
        hists1D_[getHistName("nJetCSVv2T", variation.second)]->Fill(bjets, weight);
    }
    if (hists1D_[getHistName("Pt", variation.second)] != nullptr) {
        b_Pt->GetEntry(entry);
        hists1D_[getHistName("Pt", variation.second)]->Fill(Pt, weight);
    }
    if (hists1D_[getHistName("nvtx", variation.second)] != nullptr) {
        b_nvtx->GetEntry(entry);
        hists1D_[getHistName("nvtx", variation.second)]->Fill(nvtx, weight);
    }
    if (hists1D_[getHistName("ZPt", variation.second)] != nullptr) {
        b_ZPt->GetEntry(entry);
        hists1D_[getHistName("ZPt", variation.second)]->Fill(ZPt, weight);
    }

    SafeHistFill(hists1D_, getHistName("MTWZ", variation.second), MtWZ, weight*(isMC_ || MtWZ < 300 || noBlind));
    SafeHistFill(hists1D_, getHistName("M3lMET", variation.second), M3lMET, weight*(isMC_ || M3lMET < 300 || noBlind));
}

Bool_t WZSelector::Process(Long64_t entry)
{
    bool blindVBS = (selection_ == Wselection || 
            (isVBS_ && 
                selection_ != VBSBackgroundControl && 
                selection_ != VBSBackgroundControlLoose));

    std::pair<Systematic, std::string> central_var = std::make_pair(Central, "");
    LoadBranches(entry, central_var);
    if (PassesBaseSelection(true, selection_))
        FillHistograms(entry, weight, !blindVBS, central_var);

    doSystematics_ = true;
    if (doSystematics_ && (isMC_ || isNonpromptEstimate_)) {
        for (const auto& systematic : systematics_) {
            LoadBranches(entry, systematic);
            if (!PassesBaseSelection(true, selection_))
                return true;
            FillHistograms(entry, weight, !blindVBS, systematic);
        }
    }
    
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
        if (doSystematics_ && std::find(systHists_.begin(), systHists_.end(), name) != systHists_.end()) {
            for (auto& syst : systematics_) {
                std::string syst_hist_name = name+"_"+syst.second;
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
        if (doSystematics_ && std::find(systHists2D_.begin(), systHists2D_.end(), name) != systHists2D_.end()) {
            for (auto& syst : systematics_) {
                std::string syst_hist_name = name+"_"+syst.second;
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
