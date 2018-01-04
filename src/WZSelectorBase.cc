#include "Analysis/WZAnalysis/interface/WZSelectorBase.h"
#include <TStyle.h>

void WZSelectorBase::Begin(TTree * /*tree*/)
{
    TString option = GetOption();
}

void WZSelectorBase::SlaveBegin(TTree * /*tree*/)
{
}
void WZSelectorBase::Init(TTree *tree)
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
        if (chan != nullptr) {
            channelName_ = chan->GetTitle();
        }
        if (selection != nullptr) {
            selectionName_ = selection->GetTitle();
        }
    }
    std::cout << "Processing " << name_ << std::endl;
    
    isMC_ = false;
    if (name_.find("data") == std::string::npos){
        isMC_ = true;
        fChain->SetBranchAddress("genWeight", &genWeight, &b_genWeight);
    }
    else {
        fChain->SetBranchAddress("Flag_duplicateMuonsPass", &Flag_duplicateMuonsPass, &b_Flag_duplicateMuonsPass);
        fChain->SetBranchAddress("Flag_badMuonsPass", &Flag_badMuonsPass, &b_Flag_badMuonsPass);
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
            Abort(Form("WZSelectorBase: Size of allObjects has changed!: %lu to %lu", existingObjectPtrsSize, allObjects_.size()));
        }
    }
    UpdateDirectory();

    if (channelName_ == "eee") {
        channel_ = eee;
        fChain->SetBranchAddress("e1IsCBVIDTight", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("e2IsCBVIDTight", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("e3IsCBVIDTight", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("e1_e2_Mass", &ZMass, &b_ZMass);
        fChain->SetBranchAddress("e1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("e2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("e3Pt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("e3Eta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("e3MtToMET", &l3MtToMET, &b_l3MtToMET);
    }
    else if (channelName_ == "eem") { 
        channel_ = eem;
        fChain->SetBranchAddress("e1IsCBVIDTight", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("e2IsCBVIDTight", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("mIsWZTight", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("e1_e2_Mass", &ZMass, &b_ZMass);
        fChain->SetBranchAddress("e1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("e2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("mPt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("mEta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("mMtToMET", &l3MtToMET, &b_l3MtToMET);
    }
    else if (channelName_ == "emm") { 
        channel_ = emm;
        fChain->SetBranchAddress("eIsCBVIDTight", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("m1IsWZTight", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("m2IsWZTight", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("m1_m2_Mass", &ZMass, &b_ZMass);
        fChain->SetBranchAddress("m1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("m2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("ePt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("eEta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("eMtToMET", &l3MtToMET, &b_l3MtToMET);
    }
    else if (channelName_ == "mmm") { 
        channel_ = mmm;
        fChain->SetBranchAddress("m1IsWZTight", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("m2IsWZTight", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("m3IsWZTight", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("m1_m2_Mass", &ZMass, &b_ZMass);
        fChain->SetBranchAddress("m1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("m2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("m3Pt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("m3Eta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("m3MtToMET", &l3MtToMET, &b_l3MtToMET);
    }
    else
        throw std::invalid_argument("Invalid channel choice!");

    if (selectionName_ == "tightleptons")
        selection_ = tightleptons;
    else if (selectionName_ == "Wselection")
        selection_ = Wselection;
    else if (selectionName_ == "FakeRateSelectionLoose")
        selection_ = FakeRateSelectionLoose;
    else if (selectionName_ == "FakeRateSelectionTight")
        selection_ = FakeRateSelectionTight;
    else if (selectionName_ == "VBSselection_Loose")
        selection_ = VBSselection_Loose;
    else if (selectionName_ == "VBSselection_Tight")
        selection_ = VBSselection_Tight;
    else if (selectionName_ == "VBSBackgroundControl")
        selection_ = VBSBackgroundControl;
    else if (selectionName_ == "VBSselection_mjj_400_detajj_2")
        selection_ = VBSselection_mjj_400_detajj_2;
    else if (selectionName_ == "VBSselection_mjj_400_detajj_2p5")
        selection_ = VBSselection_mjj_400_detajj_2p5;
    else if (selectionName_ == "VBSselection_mjj_400_detajj_3")
        selection_ = VBSselection_mjj_400_detajj_3;
    else if (selectionName_ == "VBSselection_mjj_400_detajj_3p5")
        selection_ = VBSselection_mjj_400_detajj_3p5;
    else if (selectionName_ == "VBSselection_mjj_400_detajj_4")
        selection_ = VBSselection_mjj_400_detajj_4;
    else if (selectionName_ == "VBSselection_mjj_450_detajj_2")
        selection_ = VBSselection_mjj_450_detajj_2;
    else if (selectionName_ == "VBSselection_mjj_450_detajj_2p5")
        selection_ = VBSselection_mjj_450_detajj_2p5;
    else if (selectionName_ == "VBSselection_mjj_450_detajj_3")
        selection_ = VBSselection_mjj_450_detajj_3;
    else if (selectionName_ == "VBSselection_mjj_450_detajj_3p5")
        selection_ = VBSselection_mjj_450_detajj_3p5;
    else if (selectionName_ == "VBSselection_mjj_450_detajj_4")
        selection_ = VBSselection_mjj_450_detajj_4;
    else if (selectionName_ == "VBSselection_mjj_500_detajj_2")
        selection_ = VBSselection_mjj_500_detajj_2;
    else if (selectionName_ == "VBSselection_mjj_500_detajj_2p5")
        selection_ = VBSselection_mjj_500_detajj_2p5;
    else if (selectionName_ == "VBSselection_mjj_500_detajj_3")
        selection_ = VBSselection_mjj_500_detajj_3;
    else if (selectionName_ == "VBSselection_mjj_500_detajj_3p5")
        selection_ = VBSselection_mjj_500_detajj_3p5;
    else if (selectionName_ == "VBSselection_mjj_500_detajj_4")
        selection_ = VBSselection_mjj_500_detajj_4;
    else if (selectionName_ == "VBSselection_mjj_550_detajj_2")
        selection_ = VBSselection_mjj_550_detajj_2;
    else if (selectionName_ == "VBSselection_mjj_550_detajj_2p5")
        selection_ = VBSselection_mjj_550_detajj_2p5;
    else if (selectionName_ == "VBSselection_mjj_550_detajj_3")
        selection_ = VBSselection_mjj_550_detajj_3;
    else if (selectionName_ == "VBSselection_mjj_550_detajj_3p5")
        selection_ = VBSselection_mjj_550_detajj_3p5;
    else if (selectionName_ == "VBSselection_mjj_550_detajj_4")
        selection_ = VBSselection_mjj_550_detajj_4;
    else if (selectionName_ == "VBSselection_mjj_600_detajj_2")
        selection_ = VBSselection_mjj_600_detajj_2;
    else if (selectionName_ == "VBSselection_mjj_600_detajj_2p5")
        selection_ = VBSselection_mjj_600_detajj_2p5;
    else if (selectionName_ == "VBSselection_mjj_600_detajj_3")
        selection_ = VBSselection_mjj_600_detajj_3;
    else if (selectionName_ == "VBSselection_mjj_600_detajj_3p5")
        selection_ = VBSselection_mjj_600_detajj_3p5;
    else if (selectionName_ == "VBSselection_mjj_600_detajj_4")
        selection_ = VBSselection_mjj_600_detajj_4;
    else {
        std::cerr << "INFO: Selection set to default value: TightLeptons" << std::endl;
        selection_ = tightleptons;
    }
    isVBS_ = (selection_ == VBSselection_Loose || 
        selection_ == VBSselection_Tight || 
        selection_ == VBSBackgroundControl || 
        selection_ == VBSselection_mjj_400_detajj_2 || 
        selection_ == VBSselection_mjj_400_detajj_2p5 || 
        selection_ == VBSselection_mjj_400_detajj_3 || 
        selection_ == VBSselection_mjj_400_detajj_3p5 || 
        selection_ == VBSselection_mjj_400_detajj_4 || 
        selection_ == VBSselection_mjj_450_detajj_2 || 
        selection_ == VBSselection_mjj_450_detajj_2p5 || 
        selection_ == VBSselection_mjj_450_detajj_3 || 
        selection_ == VBSselection_mjj_450_detajj_3p5 || 
        selection_ == VBSselection_mjj_450_detajj_4 || 
        selection_ == VBSselection_mjj_500_detajj_2 || 
        selection_ == VBSselection_mjj_500_detajj_2p5 || 
        selection_ == VBSselection_mjj_500_detajj_3 || 
        selection_ == VBSselection_mjj_500_detajj_3p5 || 
        selection_ == VBSselection_mjj_500_detajj_4 || 
        selection_ == VBSselection_mjj_550_detajj_2 || 
        selection_ == VBSselection_mjj_550_detajj_2p5 || 
        selection_ == VBSselection_mjj_550_detajj_3 || 
        selection_ == VBSselection_mjj_550_detajj_3p5 || 
        selection_ == VBSselection_mjj_550_detajj_4 || 
        selection_ == VBSselection_mjj_600_detajj_2 || 
        selection_ == VBSselection_mjj_600_detajj_2p5 || 
        selection_ == VBSselection_mjj_600_detajj_3 || 
        selection_ == VBSselection_mjj_600_detajj_3p5 || 
        selection_ == VBSselection_mjj_600_detajj_4
        );

    fChain->SetBranchAddress("type1_pfMETEt", &type1_pfMETEt, &b_type1_pfMETEt);
    fChain->SetBranchAddress("nCBVIDTightElec", &nCBVIDTightElec, &b_nCBVIDTightElec);
    fChain->SetBranchAddress("nCBVIDHLTSafeElec", &nCBVIDHLTSafeElec, &b_nCBVIDHLTSafeElec);
    fChain->SetBranchAddress("nWZTightMuon", &nWZTightMuon, &b_nWZTightMuon);
    fChain->SetBranchAddress("nWZMediumMuon", &nWZMediumMuon, &b_nWZMediumMuon);
    fChain->SetBranchAddress("Flag_BadChargedCandidateFilterPass", &Flag_BadChargedCandidateFilterPass, &b_Flag_BadChargedCandidateFilterPass);
    fChain->SetBranchAddress("Flag_BadPFMuonFilterPass", &Flag_BadPFMuonFilterPass, &b_Flag_BadPFMuonFilterPass);
    fChain->SetBranchAddress("Flag_HBHENoiseFilterPass", &Flag_HBHENoiseFilterPass, &b_Flag_HBHENoiseFilterPass);
    fChain->SetBranchAddress("Flag_HBHENoiseIsoFilterPass", &Flag_HBHENoiseIsoFilterPass, &b_Flag_HBHENoiseIsoFilterPass);
    fChain->SetBranchAddress("Flag_EcalDeadCellTriggerPrimitiveFilterPass", &Flag_EcalDeadCellTriggerPrimitiveFilterPass, &b_Flag_EcalDeadCellTriggerPrimitiveFilterPass);
    fChain->SetBranchAddress("Flag_goodVerticesPass", &Flag_goodVerticesPass, &b_Flag_goodVerticesPass);
    fChain->SetBranchAddress("Flag_eeBadScFilterPass", &Flag_eeBadScFilterPass, &b_Flag_eeBadScFilterPass);
    fChain->SetBranchAddress("Flag_globalTightHalo2016FilterPass", &Flag_globalTightHalo2016FilterPass, &b_Flag_globalTightHalo2016FilterPass);
}

Bool_t WZSelectorBase::Notify()
{
    return kTRUE;
}

Bool_t WZSelectorBase::Process(Long64_t entry)
{
    genWeight = 1;
    if (isMC_)
        b_genWeight->GetEntry(entry);
    else {
        b_Flag_duplicateMuonsPass->GetEntry(entry);          
        b_Flag_badMuonsPass->GetEntry(entry);          
    }
    b_ZMass->GetEntry(entry);
    b_l1IsTight->GetEntry(entry);
    b_l2IsTight->GetEntry(entry);
    b_l3IsTight->GetEntry(entry);
    b_l1Pt->GetEntry(entry);
    b_l2Pt->GetEntry(entry);
    b_l3Pt->GetEntry(entry);
    b_l3Eta->GetEntry(entry);
    b_l3MtToMET->GetEntry(entry);
    b_type1_pfMETEt->GetEntry(entry);
    b_nCBVIDTightElec->GetEntry(entry);
    b_nCBVIDHLTSafeElec->GetEntry(entry);
    b_nWZTightMuon->GetEntry(entry);
    b_nWZMediumMuon->GetEntry(entry);
    b_Flag_BadPFMuonFilterPass->GetEntry(entry);                    
    b_Flag_BadChargedCandidateFilterPass->GetEntry(entry);          
    b_Flag_HBHENoiseFilterPass->GetEntry(entry);                    
    b_Flag_HBHENoiseIsoFilterPass->GetEntry(entry);                 
    b_Flag_EcalDeadCellTriggerPrimitiveFilterPass->GetEntry(entry); 
    b_Flag_goodVerticesPass->GetEntry(entry);                       
    b_Flag_eeBadScFilterPass->GetEntry(entry);                      
    b_Flag_globalTightHalo2016FilterPass->GetEntry(entry);          

    //passesLeptonVeto = nWZTightMuon + nCBVIDTightElec <= 3;
    passesLeptonVeto = std::abs(nWZMediumMuon + nCBVIDHLTSafeElec - 3) < 0.1;
    
    return kTRUE;
}

// Meant to be a wrapper for the tight ID just in case it changes
// To be a function of multiple variables
bool WZSelectorBase::zlep1IsTight() {
    return l1IsTight; 
}

bool WZSelectorBase::zlep2IsTight() {
    return l2IsTight; 
}

bool WZSelectorBase::tightZLeptons() {
    return zlep1IsTight() && zlep2IsTight(); 
}

bool WZSelectorBase::lepton3IsTight() {
    return l3IsTight;
}

void WZSelectorBase::Terminate()
{
}
    
void WZSelectorBase::SlaveTerminate()
{
}
void WZSelectorBase::UpdateDirectory()
{
  for(TNamed** objPtrPtr : allObjects_) {
    if ( *objPtrPtr == nullptr ) Abort("WZSelectorBase: Call to UpdateObject but existing pointer is null");
    *objPtrPtr = (TNamed *) currentHistDir_->FindObject((*objPtrPtr)->GetName());
    if ( *objPtrPtr == nullptr ) Abort("WZSelectorBase: Call to UpdateObject but current directory has no instance");
  }
}

void WZSelectorBase::SetupNewDirectory()
{
}
