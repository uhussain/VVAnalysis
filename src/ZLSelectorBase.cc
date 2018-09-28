#include "Analysis/VVAnalysis/interface/ZLSelectorBase.h"
#include <TStyle.h>
#include <regex>
#include "TParameter.h"

void ZLSelectorBase::Begin(TTree * /*tree*/)
{
    TString option = GetOption();
}

void ZLSelectorBase::SlaveBegin(TTree * /*tree*/)
{
    //pileupSF_ = (ScaleFactor *) GetInputList()->FindObject("pileupSF");
    //if (pileupSF_ == nullptr ) 
    //    Abort("Must pass pileup weights SF");
    //eIdSF_ = (ScaleFactor *) GetInputList()->FindObject("electronTightIdSF");
    //if (eIdSF_ == nullptr ) 
    //    Abort("Must pass electron ID SF");
    //eGsfSF_ = (ScaleFactor *) GetInputList()->FindObject("electronGsfSF");
    //if (eGsfSF_ == nullptr ) 
    //    Abort("Must pass electron GSF SF");
    //mIdSF_ = (ScaleFactor *) GetInputList()->FindObject("muonTightIdSF");
    //if (mIdSF_ == nullptr ) 
    //    Abort("Must pass muon ID SF");
    //mIsoSF_ = (ScaleFactor *) GetInputList()->FindObject("muonIsoSF");
    //if (mIsoSF_ == nullptr ) 
    //    Abort("Must pass muon Iso SF");
    TParameter<bool>* addSum = (TParameter<bool>*) GetInputList()->FindObject("addSumweights");
    if (addSum != nullptr) 
        addSumweights_ = addSum->GetVal();
    else
        addSumweights_ = false;
}

std::string ZLSelectorBase::GetNameFromFile() {
    std::regex expr = std::regex("201[0-9]-[0-9][0-9]-[0-9][0-9]-(.*)-WZxsec2016");
    std::smatch matches;
    std::string fileName = fChain->GetTree()->GetDirectory()->GetFile()->GetName(); 

    std::regex_search(fileName, matches, expr);
    return std::string(matches.str(1));
}

void ZLSelectorBase::Init(TTree *tree)
{
    if (!tree) return;
    fChain = tree;
    
    TString option = GetOption();

    if (GetInputList() != nullptr) {
        TNamed* name = (TNamed *) GetInputList()->FindObject("name");
        TNamed* chan = (TNamed *) GetInputList()->FindObject("channel");
        TNamed* selection = (TNamed *) GetInputList()->FindObject("selection");
        if (name != nullptr) {
            name_ = name->GetTitle();
        }
        else {
            name_ = GetNameFromFile();
        }
        if (chan != nullptr) {
            channelName_ = chan->GetTitle();
        }
        else
            channelName_ = fChain->GetTree()->GetDirectory()->GetName();
        if (selection != nullptr) {
            selectionName_ = selection->GetTitle();
        }
    }

    if (selectionName_ == "tightleptons")
        selection_ = tightleptons;
    else if (selectionName_ == "Wselection")
        selection_ = Wselection;
    else if (selectionName_ == "Wselection_Full")
        selection_ = Wselection_Full;
    else if (selectionName_ == "FakeRateSelectionLoose")
        selection_ = FakeRateSelectionLoose;
    else if (selectionName_ == "FakeRateSelectionTight")
        selection_ = FakeRateSelectionTight;
    else if (selectionName_ == "VBSselection_Loose")
        selection_ = VBSselection_Loose;
    else if (selectionName_ == "VBSselection_Loose_Full")
        selection_ = VBSselection_Loose_Full;
    else if (selectionName_ == "VBSselection_NoZeppenfeld")
        selection_ = VBSselection_NoZeppenfeld;
    else if (selectionName_ == "VBSselection_NoZeppenfeld_Full")
        selection_ = VBSselection_NoZeppenfeld_Full;
    else if (selectionName_ == "VBSselection_Tight")
        selection_ = VBSselection_Tight;
    else if (selectionName_ == "VBSselection_Tight_Full")
        selection_ = VBSselection_Tight_Full;
    else if (selectionName_ == "VBSBackgroundControl")
        selection_ = VBSBackgroundControl;
    else if (selectionName_ == "VBSBackgroundControlATLAS")
        selection_ = VBSBackgroundControlATLAS;
    else if (selectionName_ == "VBSBackgroundControl_Full")
        selection_ = VBSBackgroundControl_Full;
    else if (selectionName_ == "VBSBackgroundControlLoose")
        selection_ = VBSBackgroundControlLoose;
    else if (selectionName_ == "VBSBackgroundControlLoose_Full")
        selection_ = VBSBackgroundControlLoose_Full;
    else if (selectionName_ == "Inclusive2Jet")
        selection_ = Inclusive2Jet;
    else if (selectionName_ == "Inclusive2Jet_Full")
        selection_ = Inclusive2Jet_Full;
    else {
        std::cerr << "INFO: Selection set to default value: TightLeptons" << std::endl;
        selection_ = tightleptons;
    }
    isVBS_ = (selection_ == VBSselection_Loose || 
        selection_ == VBSselection_Loose_Full || 
        selection_ == VBSselection_NoZeppenfeld || 
        selection_ == VBSselection_NoZeppenfeld_Full || 
        selection_ == VBSselection_Tight || 
        selection_ == VBSselection_Tight_Full || 
        selection_ == VBSBackgroundControl || 
        selection_ == VBSBackgroundControl_Full || 
        selection_ == VBSBackgroundControlATLAS || 
        selection_ == VBSBackgroundControlLoose ||
        selection_ == VBSBackgroundControlLoose_Full
        );

    
    isNonpromptEstimate_ = false;
    isMC_ = false;
    if (name_.find("data") == std::string::npos){
        isMC_ = true;
        isNonpromptMC_ = false;
        isZgamma_ = false;
        fChain->SetBranchAddress("genWeight", &genWeight, &b_genWeight);
        if (std::find(nonprompt3l_.begin(), nonprompt3l_.end(), name_) != nonprompt3l_.end()) {
            isNonpromptMC_ = true;
        }
        else if (name_ == "zg") {
            isZgamma_ = true;
        }
        if (addSumweights_) {
            TFile* file = fChain->GetTree()->GetDirectory()->GetFile(); 
            TTree* metaInfo = dynamic_cast<TTree*>(file->Get("metaInfo/metaInfo"));
            if (metaInfo == nullptr)
                std::cerr << "WARNING: Failed to add sumWeights histogram" << std::endl;
            else {
                metaInfo->Draw("1>>sumweights", "summedWeights");
            }
        }
    }
    else {
        //fChain->SetBranchAddress("Flag_duplicateMuonsPass", &Flag_duplicateMuonsPass, &b_Flag_duplicateMuonsPass);
        //fChain->SetBranchAddress("Flag_badMuonsPass", &Flag_badMuonsPass, &b_Flag_badMuonsPass);
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
            Abort(Form("ZLSelectorBase: Size of allObjects has changed!: %lu to %lu", existingObjectPtrsSize, allObjects_.size()));
        }
    }
    UpdateDirectory();

    if (channelName_ == "eee") {
        channel_ = eee;
        fChain->SetBranchAddress("e1ZZTightIDNoVtx", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("e1ZZIsoPass", &l1IsIso, &b_l1IsIso);
        fChain->SetBranchAddress("e2ZZTightIDNoVtx", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("e2ZZIsoPass", &l2IsIso, &b_l2IsIso);
        fChain->SetBranchAddress("e3ZZTightIDNoVtx", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("e3ZZIsoPass", &l3IsIso, &b_l3IsIso);
        fChain->SetBranchAddress("e1_e2_Mass", &Zmass, &b_Zmass);
        fChain->SetBranchAddress("e1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("e2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("e3Pt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("e3Eta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("e3MtToMET", &l3MtToMET, &b_l3MtToMET);
    }
    else if (channelName_ == "eem") { 
        channel_ = eem;
        fChain->SetBranchAddress("e1ZZTightIDNoVtx", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("e1ZZIsoPass", &l1IsIso, &b_l1IsIso);
        fChain->SetBranchAddress("e2ZZTightIDNoVtx", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("e2ZZIsoPass", &l2IsIso, &b_l2IsIso);
        fChain->SetBranchAddress("mZZTightIDNoVtx", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("mZZIsoPass", &l3IsIso, &b_l3IsIso);
        fChain->SetBranchAddress("e1_e2_Mass", &Zmass, &b_Zmass); 
        fChain->SetBranchAddress("e1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("e2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("mPt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("mEta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("mMtToMET", &l3MtToMET, &b_l3MtToMET);
    }
    else if (channelName_ == "emm") { 
        channel_ = emm;
        fChain->SetBranchAddress("eZZTightIDNoVtx", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("eZZIsoPass", &l3IsIso, &b_l3IsIso);
        fChain->SetBranchAddress("m1ZZTightIDNoVtx", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("m1ZZIsoPass", &l1IsIso, &b_l1IsIso);
        fChain->SetBranchAddress("m2ZZTightIDNoVtx", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("m2ZZIsoPass", &l2IsIso, &b_l2IsIso);
        fChain->SetBranchAddress("m1_m2_Mass", &Zmass, &b_Zmass);
        fChain->SetBranchAddress("m1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("m2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("ePt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("eEta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("eMtToMET", &l3MtToMET, &b_l3MtToMET);
    }
    else if (channelName_ == "mmm") { 
        channel_ = mmm;
        fChain->SetBranchAddress("m1ZZTightIDNoVtx", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("m1ZZIsoPass", &l1IsIso, &b_l1IsIso);
        fChain->SetBranchAddress("m2ZZTightIDNoVtx", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("m2ZZIsoPass", &l2IsIso, &b_l2IsIso);
        fChain->SetBranchAddress("m3ZZTightIDNoVtx", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("m3ZZIsoPass", &l3IsIso, &b_l3IsIso);
        fChain->SetBranchAddress("m1_m2_Mass", &Zmass, &b_Zmass);
        fChain->SetBranchAddress("m1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("m2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("m3Pt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("m3Eta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("m3MtToMET", &l3MtToMET, &b_l3MtToMET);
    }
    else
        throw std::invalid_argument("Invalid channel choice!");

    fChain->SetBranchAddress("type1_pfMETEt", &type1_pfMETEt, &b_type1_pfMETEt);
    //fChain->SetBranchAddress("type1_pfMETPhi", &type1_pfMETPhi, &b_type1_pfMETPhi);
    //fChain->SetBranchAddress("Flag_BadChargedCandidateFilterPass", &Flag_BadChargedCandidateFilterPass, &b_Flag_BadChargedCandidateFilterPass);
    //fChain->SetBranchAddress("Flag_BadPFMuonFilterPass", &Flag_BadPFMuonFilterPass, &b_Flag_BadPFMuonFilterPass);
    //fChain->SetBranchAddress("Flag_HBHENoiseFilterPass", &Flag_HBHENoiseFilterPass, &b_Flag_HBHENoiseFilterPass);
    //fChain->SetBranchAddress("Flag_HBHENoiseIsoFilterPass", &Flag_HBHENoiseIsoFilterPass, &b_Flag_HBHENoiseIsoFilterPass);
    //fChain->SetBranchAddress("Flag_EcalDeadCellTriggerPrimitiveFilterPass", &Flag_EcalDeadCellTriggerPrimitiveFilterPass, &b_Flag_EcalDeadCellTriggerPrimitiveFilterPass);
    //fChain->SetBranchAddress("Flag_goodVerticesPass", &Flag_goodVerticesPass, &b_Flag_goodVerticesPass);
    //fChain->SetBranchAddress("Flag_eeBadScFilterPass", &Flag_eeBadScFilterPass, &b_Flag_eeBadScFilterPass);
    //fChain->SetBranchAddress("Flag_globalTightHalo2016FilterPass", &Flag_globalTightHalo2016FilterPass, &b_Flag_globalTightHalo2016FilterPass);
}


Bool_t ZLSelectorBase::Notify()
{
    return kTRUE;
}

Bool_t ZLSelectorBase::Process(Long64_t entry)
{
    weight = 1;
    b_l1Pt->GetEntry(entry);
    b_l2Pt->GetEntry(entry);
    b_l3Pt->GetEntry(entry);
    b_l3Eta->GetEntry(entry);

    if (isMC_) {
        b_genWeight->GetEntry(entry);
        //b_l1GenPt->GetEntry(entry);
        //b_l2GenPt->GetEntry(entry);
        //b_l3GenPt->GetEntry(entry);
        //b_nTruePU->GetEntry(entry);
        weight = genWeight;
        //if (channel_ == eee) {
        //    weight *= eIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
        //    weight *= eGsfSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
        //    weight *= eIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
        //    weight *= eGsfSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
        //    weight *= eIdSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
        //    weight *= eGsfSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
        //}
        //else if (channel_ == eem) {
        //    weight *= eIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
        //    weight *= eGsfSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
        //    weight *= eIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
        //    weight *= eGsfSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
        //    weight *= mIdSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
        //    weight *= mIsoSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
        //}
        //else if (channel_ == emm) {
        //    weight *= eIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
        //    weight *= eGsfSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
        //    weight *= mIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
        //    weight *= mIsoSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
        //    weight *= mIdSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
        //    weight *= mIsoSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
        //}
        //else {
        //    weight *= mIdSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
        //    weight *= mIsoSF_->Evaluate2D(std::abs(l1Eta), l1Pt);
        //    weight *= mIdSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
        //    weight *= mIsoSF_->Evaluate2D(std::abs(l2Eta), l2Pt);
        //    weight *= mIdSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
        //    weight *= mIsoSF_->Evaluate2D(std::abs(l3Eta), l3Pt);
        //}
        //weight *= pileupSF_->Evaluate1D(nTruePU);
    }
    else {
        //b_Flag_duplicateMuonsPass->GetEntry(entry);          
        //b_Flag_badMuonsPass->GetEntry(entry);          
    }

    b_Zmass->GetEntry(entry);
    b_type1_pfMETEt->GetEntry(entry);
    
    b_l1IsTight->GetEntry(entry);
    b_l1IsIso->GetEntry(entry);
    b_l2IsTight->GetEntry(entry);
    b_l2IsIso->GetEntry(entry);
    b_l3IsTight->GetEntry(entry);
    b_l3IsIso->GetEntry(entry);
    
    b_l3MtToMET->GetEntry(entry);
    //b_Flag_BadPFMuonFilterPass->GetEntry(entry);                    
    //b_Flag_BadChargedCandidateFilterPass->GetEntry(entry);          
    //b_Flag_HBHENoiseFilterPass->GetEntry(entry);                    
    //b_Flag_HBHENoiseIsoFilterPass->GetEntry(entry);                 
    //b_Flag_EcalDeadCellTriggerPrimitiveFilterPass->GetEntry(entry); 
    //b_Flag_goodVerticesPass->GetEntry(entry);                       
    //b_Flag_eeBadScFilterPass->GetEntry(entry);                      
    //b_Flag_globalTightHalo2016FilterPass->GetEntry(entry);          

    // Veto on tight leptons
    // Make sure tight leptons also pass loose
    // passesLeptonVeto = nWZTightMuon + nCBVIDTightElec <= 3 &&
    //                      (nWZMediumMuon + nCBVIDHLTSafeElec) >= (nWZTightMuon+nCBVIDTightElec);
    // If tight isn't required to include loose
    // passesLeptonVeto = std::abs(nWZMediumMuon + nCBVIDHLTSafeElec - (l1IsLoose +l2IsLoose +l3IsLoose)) < 0.1;
    
    return kTRUE;
}

// Meant to be a wrapper for the tight ID just in case it changes
// To be a function of multiple variables
bool ZLSelectorBase::zlep1IsTight() {
    return (l1IsTight && l1IsIso); 
}

bool ZLSelectorBase::zlep2IsTight() {
    return (l2IsTight && l2IsIso); 
}

bool ZLSelectorBase::tightZLeptons() {
    return zlep1IsTight() && zlep2IsTight(); 
}

bool ZLSelectorBase::lepton3IsTight() {
    return (l3IsTight && l3IsIso);
}
//bool ZLSelectorBase::IsGenMatched3l() {
//    //return true;
//    return (!isMC_ || isNonpromptMC_ || 
//        (isZgamma_ && l1GenPt > 0 && l2GenPt > 0) ||
//        (l1GenPt > 0 && l2GenPt > 0 && l3GenPt > 0));
//}

void ZLSelectorBase::Terminate()
{
}
    
void ZLSelectorBase::SlaveTerminate()
{
}
void ZLSelectorBase::UpdateDirectory()
{
  for(TNamed** objPtrPtr : allObjects_) {
    if ( *objPtrPtr == nullptr ) Abort("ZLSelectorBase: Call to UpdateObject but existing pointer is null");
    *objPtrPtr = (TNamed *) currentHistDir_->FindObject((*objPtrPtr)->GetName());
    if ( *objPtrPtr == nullptr ) Abort("ZLSelectorBase: Call to UpdateObject but current directory has no instance");
  }
}

void ZLSelectorBase::SetupNewDirectory()
{
    if (addSumweights_)
        AddObject<TH1D>(sumWeightsHist_, "sumweights", "sumweights", 1, 0, 10);
}
