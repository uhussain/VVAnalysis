#include "Analysis/VVAnalysis/interface/ZZSelectorBase.h"
#include <TStyle.h>
#include <regex>
#include "TParameter.h"

void ZZSelectorBase::Begin(TTree * /*tree*/)
{
    TString option = GetOption();
}

void ZZSelectorBase::SlaveBegin(TTree * /*tree*/)
{
    pileupSF_ = (ScaleFactor *) GetInputList()->FindObject("pileupSF");
    if (pileupSF_ == nullptr ) 
        Abort("Must pass pileup weights SF");
    eLowRecoSF_ = (ScaleFactor *) GetInputList()->FindObject("electronLowReco18SF");
    if (eLowRecoSF_ == nullptr ) 
        Abort("Must pass electron LowReco18 SF");
    eRecoSF_ = (ScaleFactor *) GetInputList()->FindObject("electronReco18SF");
    if (eRecoSF_ == nullptr ) 
        Abort("Must pass electron Reco18 SF");
    eIdSF_ = (ScaleFactor *) GetInputList()->FindObject("electronMoriond18SF");
    if (eIdSF_ == nullptr ) 
        Abort("Must pass electron Moriond18 SF");
    eGapIdSF_ = (ScaleFactor *) GetInputList()->FindObject("electronMoriond18GapSF");
    if (eGapIdSF_ == nullptr ) 
        Abort("Must pass electronGap Moriond18 SF");
    mIdSF_ = (ScaleFactor *) GetInputList()->FindObject("muonMoriond18SF");
    if (mIdSF_ == nullptr ) 
        Abort("Must pass muon Moriond18 SF");
    //mIsoSF_ = (ScaleFactor *) GetInputList()->FindObject("muonIsoSF");
    //if (mIsoSF_ == nullptr ) 
    //    Abort("Must pass muon Iso SF");
    TParameter<bool>* addSum = (TParameter<bool>*) GetInputList()->FindObject("addSumweights");
    if (addSum != nullptr) 
        addSumweights_ = addSum->GetVal();
    else
        addSumweights_ = false;
}

std::string ZZSelectorBase::GetNameFromFile() {
    std::regex expr = std::regex("201[0-9]-[0-9][0-9]-[0-9][0-9]-(.*)-ZZ4l2018");
    std::smatch matches;
    std::string fileName = fChain->GetTree()->GetDirectory()->GetFile()->GetName(); 

    std::regex_search(fileName, matches, expr);
    return std::string(matches.str(1));
}

void ZZSelectorBase::Init(TTree *tree)
{
    if (!tree) return;
    fChain = tree;
    
    TString option = GetOption();

    if (GetInputList() != nullptr) {
        TNamed* name = (TNamed *) GetInputList()->FindObject("name");
        //std::cout<<"name is added"<<std::endl;
        TNamed* chan = (TNamed *) GetInputList()->FindObject("channel");
        //std::cout<<"channel is added and passed" <<std::endl;
        TNamed* selection = (TNamed *) GetInputList()->FindObject("selection");
        if (name != nullptr) {
            name_ = name->GetTitle();
        }
        else {
            name_ = GetNameFromFile();
        }
        //std::cout<<"name: "<<name_<<std::endl; 
        if (chan != nullptr) {
            channelName_ = chan->GetTitle();
        }
        else
            channelName_ = fChain->GetTree()->GetDirectory()->GetName();
        if (selection != nullptr) {
            selectionName_ = selection->GetTitle();
        }
    }
    //std::cout<<"channelName: "<<channelName_<<std::endl;
    //std::cout<<"fChain: "<< fChain->Print() <<std::endl;
    if (selectionName_ == "tightleptons")
        selection_ = tightleptons;
    else if (selectionName_ == "FakeRateSelectionLoose")
        selection_ = FakeRateSelectionLoose;
    else if (selectionName_ == "FakeRateSelectionTight")
        selection_ = FakeRateSelectionTight;
    else {
        //std::cerr << "INFO: Selection set to default value: TightLeptons" << std::endl;
        selection_ = tightleptons;
    }
    //isVBS_ = (selection_ == VBSselection_Loose || 
    //    selection_ == VBSselection_Loose_Full || 
    //    selection_ == VBSselection_NoZeppenfeld || 
    //    selection_ == VBSselection_NoZeppenfeld_Full || 
    //    selection_ == VBSselection_Tight || 
    //    selection_ == VBSselection_Tight_Full || 
    //    selection_ == VBSBackgroundControl || 
    //    selection_ == VBSBackgroundControl_Full || 
    //    selection_ == VBSBackgroundControlATLAS || 
    //    selection_ == VBSBackgroundControlLoose ||
    //    selection_ == VBSBackgroundControlLoose_Full
    //    );

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
            Abort(Form("ZZSelectorBase: Size of allObjects has changed!: %lu to %lu", existingObjectPtrsSize, allObjects_.size()));
        }
    }
    UpdateDirectory();
    //std::cout<<"channelName: "<<channelName_<<std::endl;
    //std::cout<<"enum channel_: "<<channel_<<std::endl;
    //std::cout<<"isMC: "<<isMC_<<std::endl;
    if (channelName_ == "eeee") {
        channel_ = eeee;
        fChain->SetBranchAddress("run", &run, &b_run);
        fChain->SetBranchAddress("lumi", &lumi, &b_lumi);
        fChain->SetBranchAddress("evt", &evt, &b_evt);
        //fChain->SetBranchAddress("duplicated", &duplicated, &b_duplicated);
        //std::cout<<"enum channel_: "<<channel_<<std::endl;
        //fChain->SetBranchAddress("e1ZZTightIDNoVtx", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("e1ZZTightIDNoVtxHZZWP", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("e1ZZIsoPass", &l1IsIso, &b_l1IsIso);
        //fChain->SetBranchAddress("e2ZZTightIDNoVtx", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("e2ZZTightIDNoVtxHZZWP", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("e2ZZIsoPass", &l2IsIso, &b_l2IsIso);
        //fChain->SetBranchAddress("e3ZZTightIDNoVtx", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("e3ZZTightIDNoVtxHZZWP", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("e3ZZIsoPass", &l3IsIso, &b_l3IsIso);
        //fChain->SetBranchAddress("e4ZZTightIDNoVtx", &l4IsTight, &b_l4IsTight);
        fChain->SetBranchAddress("e4ZZTightIDNoVtxHZZWP", &l4IsTight, &b_l4IsTight);
        fChain->SetBranchAddress("e4ZZIsoPass", &l4IsIso, &b_l4IsIso);
        fChain->SetBranchAddress("e1_e2_Mass", &Z1mass, &b_Z1mass);
        fChain->SetBranchAddress("e3_e4_Mass", &Z2mass, &b_Z2mass);
        fChain->SetBranchAddress("e1_e2_Pt", &Z1pt, &b_Z1pt);
        fChain->SetBranchAddress("e3_e4_Pt", &Z2pt, &b_Z2pt);
        fChain->SetBranchAddress("e1_e2_Phi", &Z1Phi, &b_Z1Phi);
        fChain->SetBranchAddress("e3_e4_Phi", &Z2Phi, &b_Z2Phi);
        fChain->SetBranchAddress("e1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("e2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("e3Pt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("e4Pt", &l4Pt, &b_l4Pt);
        fChain->SetBranchAddress("e1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("e2Eta", &l2Eta, &b_l2Eta);
        fChain->SetBranchAddress("e3Eta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("e4Eta", &l4Eta, &b_l4Eta);
        fChain->SetBranchAddress("e1Phi", &l1Phi, &b_l1Phi);
        fChain->SetBranchAddress("e2Phi", &l2Phi, &b_l2Phi);
        fChain->SetBranchAddress("e3Phi", &l3Phi, &b_l3Phi);
        fChain->SetBranchAddress("e4Phi", &l4Phi, &b_l4Phi);
        fChain->SetBranchAddress("e1SIP3D", &l1SIP3D, &b_l1SIP3D);
        fChain->SetBranchAddress("e2SIP3D", &l2SIP3D, &b_l2SIP3D);
        fChain->SetBranchAddress("e3SIP3D", &l3SIP3D, &b_l3SIP3D);
        fChain->SetBranchAddress("e4SIP3D", &l4SIP3D, &b_l4SIP3D);
        fChain->SetBranchAddress("e1PdgId", &l1PdgId, &b_l1PdgId);
        fChain->SetBranchAddress("e2PdgId", &l2PdgId, &b_l2PdgId);
        fChain->SetBranchAddress("e3PdgId", &l3PdgId, &b_l3PdgId);
        fChain->SetBranchAddress("e4PdgId", &l4PdgId, &b_l4PdgId);
        fChain->SetBranchAddress("e1Mass", &l1Mass, &b_l1Mass);
        fChain->SetBranchAddress("e2Mass", &l2Mass, &b_l2Mass);
        fChain->SetBranchAddress("e3Mass", &l3Mass, &b_l3Mass);
        fChain->SetBranchAddress("e4Mass", &l4Mass, &b_l4Mass);
        fChain->SetBranchAddress("e1IsGap", &l1IsGap, &b_l1IsGap);
        fChain->SetBranchAddress("e2IsGap", &l2IsGap, &b_l2IsGap);
        fChain->SetBranchAddress("e3IsGap", &l3IsGap, &b_l3IsGap);
        fChain->SetBranchAddress("e4IsGap", &l4IsGap, &b_l4IsGap);
        //if (isMC_) {
        //    fChain->SetBranchAddress("e1GenPt", &l1GenPt, &b_l1GenPt);
        //    fChain->SetBranchAddress("e2GenPt", &l2GenPt, &b_l2GenPt);
        //    fChain->SetBranchAddress("e3GenPt", &l3GenPt, &b_l3GenPt);
        //    fChain->SetBranchAddress("e4GenPt", &l4GenPt, &b_l4GenPt);
        //}
    }
    //Add 2e2mu channel also but it still needs to differentiate which one is Z1Mass and which one is Z2Mass leptons
    //This is done with a flag at the time of Process for each event on the fly
    else if (channelName_ == "eemm") {
        channel_ = eemm;
        fChain->SetBranchAddress("run", &run, &b_run);
        fChain->SetBranchAddress("lumi", &lumi, &b_lumi);
        fChain->SetBranchAddress("evt", &evt, &b_evt);
        //fChain->SetBranchAddress("duplicated", &duplicated, &b_duplicated);
        //fChain->SetBranchAddress("e1ZZTightIDNoVtx", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("e1ZZTightIDNoVtxHZZWP", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("e1ZZIsoPass", &l1IsIso, &b_l1IsIso);
        //fChain->SetBranchAddress("e2ZZTightIDNoVtx", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("e2ZZTightIDNoVtxHZZWP", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("e2ZZIsoPass", &l2IsIso, &b_l2IsIso);
        fChain->SetBranchAddress("m1ZZTightIDNoVtx", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("m1ZZIsoPass", &l3IsIso, &b_l3IsIso);
        fChain->SetBranchAddress("m2ZZTightIDNoVtx", &l4IsTight, &b_l4IsTight);
        fChain->SetBranchAddress("m2ZZIsoPass", &l4IsIso, &b_l4IsIso);
        fChain->SetBranchAddress("e1_e2_Mass", &Z1mass, &b_Z1mass);
        fChain->SetBranchAddress("m1_m2_Mass", &Z2mass, &b_Z2mass);
        fChain->SetBranchAddress("e1_e2_Pt", &Z1pt, &b_Z1pt);
        fChain->SetBranchAddress("m1_m2_Pt", &Z2pt, &b_Z2pt);
        fChain->SetBranchAddress("e1_e2_Phi", &Z1Phi, &b_Z1Phi);
        fChain->SetBranchAddress("m1_m2_Phi", &Z2Phi, &b_Z2Phi);
        fChain->SetBranchAddress("e1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("e2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("m1Pt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("m2Pt", &l4Pt, &b_l4Pt);
        fChain->SetBranchAddress("e1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("e2Eta", &l2Eta, &b_l2Eta);
        fChain->SetBranchAddress("m1Eta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("m2Eta", &l4Eta, &b_l4Eta);
        fChain->SetBranchAddress("e1Phi", &l1Phi, &b_l1Phi);
        fChain->SetBranchAddress("e2Phi", &l2Phi, &b_l2Phi);
        fChain->SetBranchAddress("e1SIP3D", &l1SIP3D, &b_l1SIP3D);
        fChain->SetBranchAddress("e2SIP3D", &l2SIP3D, &b_l2SIP3D);
        fChain->SetBranchAddress("m1Phi", &l3Phi, &b_l3Phi);
        fChain->SetBranchAddress("m2Phi", &l4Phi, &b_l4Phi);
        fChain->SetBranchAddress("m1SIP3D", &l3SIP3D, &b_l3SIP3D);
        fChain->SetBranchAddress("m2SIP3D", &l4SIP3D, &b_l4SIP3D);
        fChain->SetBranchAddress("e1PdgId", &l1PdgId, &b_l1PdgId);
        fChain->SetBranchAddress("e2PdgId", &l2PdgId, &b_l2PdgId);
        fChain->SetBranchAddress("m1PdgId", &l3PdgId, &b_l3PdgId);
        fChain->SetBranchAddress("m2PdgId", &l4PdgId, &b_l4PdgId);
        fChain->SetBranchAddress("e1Mass", &l1Mass, &b_l1Mass);
        fChain->SetBranchAddress("e2Mass", &l2Mass, &b_l2Mass);
        fChain->SetBranchAddress("m1Mass", &l3Mass, &b_l3Mass);
        fChain->SetBranchAddress("m2Mass", &l4Mass, &b_l4Mass);
        fChain->SetBranchAddress("e1IsGap", &l1IsGap, &b_l1IsGap);
        fChain->SetBranchAddress("e2IsGap", &l2IsGap, &b_l2IsGap);
        //There are 2 different SFs for electrons depending on whether the electron is from the crack 
        //in ECAL crystals or not but we need a dummy for the muons? Makes life easier later
        fChain->SetBranchAddress("m1IsLoose", &l3IsGap, &b_l3IsGap);
        fChain->SetBranchAddress("m2IsLoose", &l4IsGap, &b_l4IsGap);
        //if (isMC_) {
        //    fChain->SetBranchAddress("e1GenPt", &l1GenPt, &b_l1GenPt);
        //    fChain->SetBranchAddress("e2GenPt", &l2GenPt, &b_l2GenPt);
        //    fChain->SetBranchAddress("e3GenPt", &l3GenPt, &b_l3GenPt);
        //    fChain->SetBranchAddress("e4GenPt", &l4GenPt, &b_l4GenPt);
        //}
    }
    else if (channelName_ == "mmee") {
        channel_ = mmee;
        fChain->SetBranchAddress("run", &run, &b_run);
        fChain->SetBranchAddress("lumi", &lumi, &b_lumi);
        fChain->SetBranchAddress("evt", &evt, &b_evt);
        //fChain->SetBranchAddress("duplicated", &duplicated, &b_duplicated);
        //fChain->SetBranchAddress("e1ZZTightIDNoVtx", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("e1ZZTightIDNoVtxHZZWP", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("e1ZZIsoPass", &l1IsIso, &b_l1IsIso);
        //fChain->SetBranchAddress("e2ZZTightIDNoVtx", &l2IsTight, &b_l2IsTight); 
        fChain->SetBranchAddress("e2ZZTightIDNoVtxHZZWP", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("e2ZZIsoPass", &l2IsIso, &b_l2IsIso);
        fChain->SetBranchAddress("m1ZZTightIDNoVtx", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("m1ZZIsoPass", &l3IsIso, &b_l3IsIso);
        fChain->SetBranchAddress("m2ZZTightIDNoVtx", &l4IsTight, &b_l4IsTight);
        fChain->SetBranchAddress("m2ZZIsoPass", &l4IsIso, &b_l4IsIso);
        fChain->SetBranchAddress("e1_e2_Mass", &Z1mass, &b_Z1mass);
        fChain->SetBranchAddress("m1_m2_Mass", &Z2mass, &b_Z2mass);
        fChain->SetBranchAddress("e1_e2_Pt", &Z1pt, &b_Z1pt);
        fChain->SetBranchAddress("m1_m2_Pt", &Z2pt, &b_Z2pt);
        fChain->SetBranchAddress("e1_e2_Phi", &Z1Phi, &b_Z1Phi);
        fChain->SetBranchAddress("m1_m2_Phi", &Z2Phi, &b_Z2Phi);
        fChain->SetBranchAddress("e1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("e2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("m1Pt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("m2Pt", &l4Pt, &b_l4Pt);
        fChain->SetBranchAddress("e1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("e2Eta", &l2Eta, &b_l2Eta);
        fChain->SetBranchAddress("m1Eta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("m2Eta", &l4Eta, &b_l4Eta);
        fChain->SetBranchAddress("e1Phi", &l1Phi, &b_l1Phi);
        fChain->SetBranchAddress("e2Phi", &l2Phi, &b_l2Phi);
        fChain->SetBranchAddress("e1SIP3D", &l1SIP3D, &b_l1SIP3D);
        fChain->SetBranchAddress("e2SIP3D", &l2SIP3D, &b_l2SIP3D);
        fChain->SetBranchAddress("m1Phi", &l3Phi, &b_l3Phi);
        fChain->SetBranchAddress("m2Phi", &l4Phi, &b_l4Phi);
        fChain->SetBranchAddress("m1SIP3D", &l3SIP3D, &b_l3SIP3D);
        fChain->SetBranchAddress("m2SIP3D", &l4SIP3D, &b_l4SIP3D);
        fChain->SetBranchAddress("e1PdgId", &l1PdgId, &b_l1PdgId);
        fChain->SetBranchAddress("e2PdgId", &l2PdgId, &b_l2PdgId);
        fChain->SetBranchAddress("m1PdgId", &l3PdgId, &b_l3PdgId);
        fChain->SetBranchAddress("m2PdgId", &l4PdgId, &b_l4PdgId);
        fChain->SetBranchAddress("e1Mass", &l1Mass, &b_l1Mass);
        fChain->SetBranchAddress("e2Mass", &l2Mass, &b_l2Mass);
        fChain->SetBranchAddress("m1Mass", &l3Mass, &b_l3Mass);
        fChain->SetBranchAddress("m2Mass", &l4Mass, &b_l4Mass);
        fChain->SetBranchAddress("e1IsGap", &l1IsGap, &b_l1IsGap);
        fChain->SetBranchAddress("e2IsGap", &l2IsGap, &b_l2IsGap);
        fChain->SetBranchAddress("m1IsLoose", &l3IsGap, &b_l3IsGap);
        fChain->SetBranchAddress("m2IsLoose", &l4IsGap, &b_l4IsGap);
        //if (isMC_) {
        //    fChain->SetBranchAddress("e1GenPt", &l1GenPt, &b_l1GenPt);
        //    fChain->SetBranchAddress("e2GenPt", &l2GenPt, &b_l2GenPt);
        //    fChain->SetBranchAddress("e3GenPt", &l3GenPt, &b_l3GenPt);
        //    fChain->SetBranchAddress("e4GenPt", &l4GenPt, &b_l4GenPt);
        //}
    }
    else if (channelName_ == "mmmm") {
        channel_ = mmmm;
        fChain->SetBranchAddress("run", &run, &b_run);
        fChain->SetBranchAddress("lumi", &lumi, &b_lumi);
        fChain->SetBranchAddress("evt", &evt, &b_evt);
        //fChain->SetBranchAddress("duplicated", &duplicated, &b_duplicated);
        fChain->SetBranchAddress("m1ZZTightIDNoVtx", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("m1ZZIsoPass", &l1IsIso, &b_l1IsIso);
        fChain->SetBranchAddress("m2ZZTightIDNoVtx", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("m2ZZIsoPass", &l2IsIso, &b_l2IsIso);
        fChain->SetBranchAddress("m3ZZTightIDNoVtx", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("m3ZZIsoPass", &l3IsIso, &b_l3IsIso);
        fChain->SetBranchAddress("m4ZZTightIDNoVtx", &l4IsTight, &b_l4IsTight);
        fChain->SetBranchAddress("m4ZZIsoPass", &l4IsIso, &b_l4IsIso);
        fChain->SetBranchAddress("m1_m2_Mass", &Z1mass, &b_Z1mass);
        fChain->SetBranchAddress("m3_m4_Mass", &Z2mass, &b_Z2mass);
        fChain->SetBranchAddress("m1_m2_Pt", &Z1pt, &b_Z1pt);
        fChain->SetBranchAddress("m3_m4_Pt", &Z2pt, &b_Z2pt);
        fChain->SetBranchAddress("m1_m2_Phi", &Z1Phi, &b_Z1Phi);
        fChain->SetBranchAddress("m3_m4_Phi", &Z2Phi, &b_Z2Phi);
        fChain->SetBranchAddress("m1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("m2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("m3Pt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("m4Pt", &l4Pt, &b_l4Pt);
        fChain->SetBranchAddress("m1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("m2Eta", &l2Eta, &b_l2Eta);
        fChain->SetBranchAddress("m3Eta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("m4Eta", &l4Eta, &b_l4Eta);
        fChain->SetBranchAddress("m1Phi", &l1Phi, &b_l1Phi);
        fChain->SetBranchAddress("m2Phi", &l2Phi, &b_l2Phi);
        fChain->SetBranchAddress("m3Phi", &l3Phi, &b_l3Phi);
        fChain->SetBranchAddress("m4Phi", &l4Phi, &b_l4Phi);
        fChain->SetBranchAddress("m1SIP3D", &l1SIP3D, &b_l1SIP3D);
        fChain->SetBranchAddress("m2SIP3D", &l2SIP3D, &b_l2SIP3D);
        fChain->SetBranchAddress("m3SIP3D", &l3SIP3D, &b_l3SIP3D);
        fChain->SetBranchAddress("m4SIP3D", &l4SIP3D, &b_l4SIP3D);
        fChain->SetBranchAddress("m1PdgId", &l1PdgId, &b_l1PdgId);
        fChain->SetBranchAddress("m2PdgId", &l2PdgId, &b_l2PdgId);
        fChain->SetBranchAddress("m3PdgId", &l3PdgId, &b_l3PdgId);
        fChain->SetBranchAddress("m4PdgId", &l4PdgId, &b_l4PdgId);
        fChain->SetBranchAddress("m1Mass", &l1Mass, &b_l1Mass);
        fChain->SetBranchAddress("m2Mass", &l2Mass, &b_l2Mass);
        fChain->SetBranchAddress("m3Mass", &l3Mass, &b_l3Mass);
        fChain->SetBranchAddress("m4Mass", &l4Mass, &b_l4Mass);
        //if (isMC_) {
        //    fChain->SetBranchAddress("m1GenPt", &l1GenPt, &b_l1GenPt);
        //    fChain->SetBranchAddress("m2GenPt", &l2GenPt, &b_l2GenPt);
        //    fChain->SetBranchAddress("m3GenPt", &l3GenPt, &b_l3GenPt);
        //    fChain->SetBranchAddress("m4GenPt", &l4GenPt, &b_l4GenPt);
        //}
    }
    else if (channelName_ == "eee") {
        channel_ = eee;
        fChain->SetBranchAddress("run", &run, &b_run);
        fChain->SetBranchAddress("lumi", &lumi, &b_lumi);
        fChain->SetBranchAddress("evt", &evt, &b_evt);
        //fChain->SetBranchAddress("duplicated", &duplicated, &b_duplicated);
        fChain->SetBranchAddress("e1ZZTightIDNoVtx", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("e1ZZIsoPass", &l1IsIso, &b_l1IsIso);
        fChain->SetBranchAddress("e2ZZTightIDNoVtx", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("e2ZZIsoPass", &l2IsIso, &b_l2IsIso);
        fChain->SetBranchAddress("e3ZZTightIDNoVtx", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("e3ZZIsoPass", &l3IsIso, &b_l3IsIso);
        fChain->SetBranchAddress("e1_e2_Mass", &Z1mass, &b_Z1mass);
        fChain->SetBranchAddress("e1_e2_Pt", &Z1pt, &b_Z1pt);
        fChain->SetBranchAddress("e1_e2_Phi", &Z1Phi, &b_Z1Phi);
        fChain->SetBranchAddress("e1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("e2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("e3Pt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("e1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("e2Eta", &l2Eta, &b_l2Eta);
        fChain->SetBranchAddress("e3Eta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("e1Phi", &l1Phi, &b_l1Phi);
        fChain->SetBranchAddress("e2Phi", &l2Phi, &b_l2Phi);
        fChain->SetBranchAddress("e3Phi", &l3Phi, &b_l3Phi);
        fChain->SetBranchAddress("e1SIP3D", &l1SIP3D, &b_l1SIP3D);
        fChain->SetBranchAddress("e2SIP3D", &l2SIP3D, &b_l2SIP3D);
        fChain->SetBranchAddress("e3SIP3D", &l3SIP3D, &b_l3SIP3D);
        fChain->SetBranchAddress("e1PdgId", &l1PdgId, &b_l1PdgId);
        fChain->SetBranchAddress("e2PdgId", &l2PdgId, &b_l2PdgId);
        fChain->SetBranchAddress("e3PdgId", &l3PdgId, &b_l3PdgId);
        fChain->SetBranchAddress("e3MtToMET", &l3MtToMET, &b_l3MtToMET);
        //if (isMC_) {
        //    fChain->SetBranchAddress("e1GenPt", &l1GenPt, &b_l1GenPt);
        //    fChain->SetBranchAddress("e2GenPt", &l2GenPt, &b_l2GenPt);
        //    fChain->SetBranchAddress("e3GenPt", &l3GenPt, &b_l3GenPt);
        //}
    }
    else if (channelName_ == "eem") { 
        channel_ = eem;
        fChain->SetBranchAddress("run", &run, &b_run);
        fChain->SetBranchAddress("lumi", &lumi, &b_lumi);
        fChain->SetBranchAddress("evt", &evt, &b_evt);
        //fChain->SetBranchAddress("duplicated", &duplicated, &b_duplicated);
        fChain->SetBranchAddress("e1ZZTightIDNoVtx", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("e1ZZIsoPass", &l1IsIso, &b_l1IsIso);
        fChain->SetBranchAddress("e2ZZTightIDNoVtx", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("e2ZZIsoPass", &l2IsIso, &b_l2IsIso);
        fChain->SetBranchAddress("mZZTightIDNoVtx", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("mZZIsoPass", &l3IsIso, &b_l3IsIso);
        fChain->SetBranchAddress("e1_e2_Mass", &Z1mass, &b_Z1mass);
        fChain->SetBranchAddress("e1_e2_Pt", &Z1pt, &b_Z1pt);
        fChain->SetBranchAddress("e1_e2_Phi", &Z1Phi, &b_Z1Phi);
        fChain->SetBranchAddress("e1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("e2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("e1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("e2Eta", &l2Eta, &b_l2Eta);
        fChain->SetBranchAddress("e1Phi", &l1Phi, &b_l1Phi);
        fChain->SetBranchAddress("e2Phi", &l2Phi, &b_l2Phi);
        fChain->SetBranchAddress("mPt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("mEta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("mPhi", &l3Phi, &b_l3Phi);
        fChain->SetBranchAddress("e1SIP3D", &l1SIP3D, &b_l1SIP3D);
        fChain->SetBranchAddress("e2SIP3D", &l2SIP3D, &b_l2SIP3D);
        fChain->SetBranchAddress("mSIP3D", &l3SIP3D, &b_l3SIP3D);
        fChain->SetBranchAddress("e1PdgId", &l1PdgId, &b_l1PdgId);
        fChain->SetBranchAddress("e2PdgId", &l2PdgId, &b_l2PdgId);
        fChain->SetBranchAddress("mPdgId", &l3PdgId, &b_l3PdgId);
        fChain->SetBranchAddress("mMtToMET", &l3MtToMET, &b_l3MtToMET);
        //if (isMC_) {
        //    fChain->SetBranchAddress("mGenPt", &l3GenPt, &b_l3GenPt);
        //    fChain->SetBranchAddress("e1GenPt", &l1GenPt, &b_l1GenPt);
        //    fChain->SetBranchAddress("e2GenPt", &l2GenPt, &b_l2GenPt);
        //}
    }
    else if (channelName_ == "emm") { 
        channel_ = emm;
        fChain->SetBranchAddress("run", &run, &b_run);
        fChain->SetBranchAddress("lumi", &lumi, &b_lumi);
        fChain->SetBranchAddress("evt", &evt, &b_evt);
        //fChain->SetBranchAddress("duplicated", &duplicated, &b_duplicated);
        fChain->SetBranchAddress("eZZTightIDNoVtx", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("eZZIsoPass", &l3IsIso, &b_l3IsIso);
        fChain->SetBranchAddress("m1ZZTightIDNoVtx", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("m1ZZIsoPass", &l1IsIso, &b_l1IsIso);
        fChain->SetBranchAddress("m2ZZTightIDNoVtx", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("m2ZZIsoPass", &l2IsIso, &b_l2IsIso);
        fChain->SetBranchAddress("m1_m2_Mass", &Z1mass, &b_Z1mass);
        fChain->SetBranchAddress("m1_m2_Pt", &Z1pt, &b_Z1pt);
        fChain->SetBranchAddress("m1_m2_Phi", &Z1Phi, &b_Z1Phi);
        fChain->SetBranchAddress("m1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("m2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("m1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("m2Eta", &l2Eta, &b_l2Eta);
        fChain->SetBranchAddress("m1Phi", &l1Phi, &b_l1Phi);
        fChain->SetBranchAddress("m2Phi", &l2Phi, &b_l2Phi);
        fChain->SetBranchAddress("ePt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("eEta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("ePhi", &l3Phi, &b_l3Phi);
        fChain->SetBranchAddress("m1SIP3D", &l1SIP3D, &b_l1SIP3D);
        fChain->SetBranchAddress("m2SIP3D", &l2SIP3D, &b_l2SIP3D);
        fChain->SetBranchAddress("eSIP3D", &l3SIP3D, &b_l3SIP3D);
        fChain->SetBranchAddress("m1PdgId", &l1PdgId, &b_l1PdgId);
        fChain->SetBranchAddress("m2PdgId", &l2PdgId, &b_l2PdgId);
        fChain->SetBranchAddress("ePdgId", &l3PdgId, &b_l3PdgId);
        fChain->SetBranchAddress("eMtToMET", &l3MtToMET, &b_l3MtToMET);
        //if (isMC_) {
        //    fChain->SetBranchAddress("eGenPt", &l3GenPt, &b_l3GenPt);
        //    fChain->SetBranchAddress("m1GenPt", &l1GenPt, &b_l1GenPt);
        //    fChain->SetBranchAddress("m2GenPt", &l2GenPt, &b_l2GenPt);
        //}
    }
    else if (channelName_ == "mmm") { 
        channel_ = mmm;
        fChain->SetBranchAddress("run", &run, &b_run);
        fChain->SetBranchAddress("lumi", &lumi, &b_lumi);
        fChain->SetBranchAddress("evt", &evt, &b_evt);
        //fChain->SetBranchAddress("duplicated", &duplicated, &b_duplicated);
        fChain->SetBranchAddress("m1ZZTightIDNoVtx", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("m1ZZIsoPass", &l1IsIso, &b_l1IsIso);
        fChain->SetBranchAddress("m2ZZTightIDNoVtx", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("m2ZZIsoPass", &l2IsIso, &b_l2IsIso);
        fChain->SetBranchAddress("m3ZZTightIDNoVtx", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("m3ZZIsoPass", &l3IsIso, &b_l3IsIso);
        fChain->SetBranchAddress("m1_m2_Mass", &Z1mass, &b_Z1mass);
        fChain->SetBranchAddress("m1_m2_Pt", &Z1pt, &b_Z1pt);
        fChain->SetBranchAddress("m1_m2_Phi", &Z1Phi, &b_Z1Phi);
        fChain->SetBranchAddress("m1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("m2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("m3Pt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("m1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("m2Eta", &l2Eta, &b_l2Eta);
        fChain->SetBranchAddress("m3Eta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("m1Phi", &l1Phi, &b_l1Phi);
        fChain->SetBranchAddress("m2Phi", &l2Phi, &b_l2Phi);
        fChain->SetBranchAddress("m3Phi", &l3Phi, &b_l3Phi);
        fChain->SetBranchAddress("m1SIP3D", &l1SIP3D, &b_l1SIP3D);
        fChain->SetBranchAddress("m2SIP3D", &l2SIP3D, &b_l2SIP3D);
        fChain->SetBranchAddress("m3SIP3D", &l3SIP3D, &b_l3SIP3D);
        fChain->SetBranchAddress("m1PdgId", &l1PdgId, &b_l1PdgId);
        fChain->SetBranchAddress("m2PdgId", &l2PdgId, &b_l2PdgId);
        fChain->SetBranchAddress("m3PdgId", &l3PdgId, &b_l3PdgId);
        fChain->SetBranchAddress("m3MtToMET", &l3MtToMET, &b_l3MtToMET);
        //if (isMC_) {
        //    fChain->SetBranchAddress("m1GenPt", &l1GenPt, &b_l1GenPt);
        //    fChain->SetBranchAddress("m2GenPt", &l2GenPt, &b_l2GenPt);
        //    fChain->SetBranchAddress("m3GenPt", &l3GenPt, &b_l3GenPt);
        //}
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


Bool_t ZZSelectorBase::Notify()
{
    return kTRUE;
}

Bool_t ZZSelectorBase::Process(Long64_t entry)
{
    weight = 1;
    b_l1Pt->GetEntry(entry);
    b_l2Pt->GetEntry(entry);
    b_l3Pt->GetEntry(entry);
    b_l1Eta->GetEntry(entry);
    b_l2Eta->GetEntry(entry);
    b_l3Eta->GetEntry(entry);
    b_l1Phi->GetEntry(entry);
    b_l2Phi->GetEntry(entry);
    b_l3Phi->GetEntry(entry);
    b_l1SIP3D->GetEntry(entry);
    b_l2SIP3D->GetEntry(entry);
    b_l3SIP3D->GetEntry(entry);
    b_l1PdgId->GetEntry(entry);
    b_l2PdgId->GetEntry(entry);
    b_l3PdgId->GetEntry(entry);
    b_run->GetEntry(entry);
    b_lumi->GetEntry(entry);
    b_evt->GetEntry(entry);
    //std::cout<<"Is the ZZSelectorBase fine until here"<<std::endl;
    if (channel_ == eeee || channel_ == eemm || channel_ == mmee || channel_ == mmmm) {
      b_l4Pt->GetEntry(entry);
      b_l4Eta->GetEntry(entry);
      b_l4Phi->GetEntry(entry);
      b_l4SIP3D->GetEntry(entry);
      b_l4PdgId->GetEntry(entry);
      b_l1Mass->GetEntry(entry);
      b_l2Mass->GetEntry(entry);
      b_l3Mass->GetEntry(entry);
      b_l4Mass->GetEntry(entry);
      b_l4IsTight->GetEntry(entry);
      b_l4IsIso->GetEntry(entry);
      //b_l4PVDXY->GetEntry(entry);
      //b_l4PVDZ->GetEntry(entry);
      b_Z2mass->GetEntry(entry);
      b_Z2pt->GetEntry(entry);
      b_Z2Phi->GetEntry(entry);
    }
    b_Z1mass->GetEntry(entry);
    b_Z1pt->GetEntry(entry);
    b_Z1Phi->GetEntry(entry);
    b_type1_pfMETEt->GetEntry(entry); 
    b_l1IsTight->GetEntry(entry);
    b_l1IsIso->GetEntry(entry);
    b_l2IsTight->GetEntry(entry);
    b_l2IsIso->GetEntry(entry);
    b_l3IsTight->GetEntry(entry);
    b_l3IsIso->GetEntry(entry); 
    if (channel_ == eeee || channel_ == eemm || channel_ == mmee){
      b_l1IsGap->GetEntry(entry); 
      b_l2IsGap->GetEntry(entry); 
      b_l3IsGap->GetEntry(entry); 
      b_l4IsGap->GetEntry(entry);
    }
    if (channel_ == eee || channel_ == eem || channel_ == emm || channel_ == mmm) {
      b_l3MtToMET->GetEntry(entry);
    }
    //std::cout<<"IsMC: "<<isMC_<<std::endl;
    if (isMC_) {
        //b_duplicated->GetEntry(entry);
        b_genWeight->GetEntry(entry);
        //b_l1GenPt->GetEntry(entry);
        //b_l2GenPt->GetEntry(entry);
        //b_l3GenPt->GetEntry(entry);
        //std::cout<<"Its fine before nTruePU" <<std::endl;
        b_nTruePU->GetEntry(entry);
        //if (channel_ == eeee || channel_ == eemm || channel_ == mmmm) {
        //  b_l4GenPt->GetEntry(entry);}
        //std::cout<<"It fails before weight=genWeight assignment" <<std::endl;
        weight = genWeight;
    }
    else {
        //b_Flag_duplicateMuonsPass->GetEntry(entry);          
        //b_Flag_badMuonsPass->GetEntry(entry);          
    } 
    if(channel_ == mmee){
      if(e1e2IsZ1(entry))
        weight=0.0;
        //Makes weight 0 if Z1 is ee hence should not go in _mmee histos
    }
    else if(channel_ == eemm){
      if(!(e1e2IsZ1(entry)))
        weight=0.0;
        //Makes weight 0 if Z1 is mm hence should not go in _eemm 
    }
    //b_l1PVDXY->GetEntry(entry);
    //b_l2PVDXY->GetEntry(entry);
    //b_l3PVDXY->GetEntry(entry);
    //b_l1PVDZ->GetEntry(entry);
    //b_l2PVDZ->GetEntry(entry);
    //b_l3PVDZ->GetEntry(entry);

    //b_Flag_BadPFMuonFilterPass->GetEntry(entry);                    
    //b_Flag_BadChargedCandidateFilterPass->GetEntry(entry);          
    //b_Flag_HBHENoiseFilterPass->GetEntry(entry);                    
    //b_Flag_HBHENoiseIsoFilterPass->GetEntry(entry);                 
    //b_Flag_EcalDeadCellTriggerPrimitiveFilterPass->GetEntry(entry); 
    //b_Flag_goodVerticesPass->GetEntry(entry);                       
    //b_Flag_eeBadScFilterPass->GetEntry(entry);                      
    //b_Flag_globalTightHalo2016FilterPass->GetEntry(entry);          

    
    //This condition identifies the Z1 candidate
    //Required for the 2e2mu state 
    //Can I save some kind of flag to identify the order of leptons for each event
    //I think this part might need to be added to the ZZSelector and BackgroundSelector and the condition checked at
    //process time for each event.
    //if (channel_ == eemm) {
    //  bool e1e2IsZ1;
    //  if(fabs(Z1mass-91.1876) < fabs(Z2mass-91.1876)){
    //    e1e2IsZ1=1;}
    //  else{ 
    //    e1e2IsZ1=0;}
    //}
    //std::cout<<"Does the ZZSelectorBase finish processing"<<std::endl;
    return kTRUE;
}

//This condition identifies the Z1 candidate
//Required for the 2e2mu state 
//Can I save some kind of flag to identify the order of leptons for each event
//I think this part might need to be added to the ZZSelector and BackgroundSelector and the condition checked at
//process time for each event.
bool ZZSelectorBase::e1e2IsZ1(Long64_t entry){
  //4P Signal region logic where I need to differentiate between two tight pairs and assign Z1 depending on which is closer to mZ
  if(tightZ1Leptons() && tightZ2Leptons()){
    if(fabs(Z1mass-91.1876) < fabs(Z2mass-91.1876))
      return true;
    else
      return false;
  }
  //In CRs it doesn't matter, the tight pair builds the Z and the other pair is X in Z+X.
  else if(tightZ1Leptons() && !tightZ2Leptons()){
    return true;}

  else if(!tightZ1Leptons() && tightZ2Leptons()){
    return false;}
  else
    return false;
}
// Meant to be a wrapper for the tight ID just in case it changes
// To be a function of multiple variables
bool ZZSelectorBase::lep1IsTight() {
    return (l1IsTight && l1IsIso); 
}

bool ZZSelectorBase::lep2IsTight() {
    return (l2IsTight && l2IsIso); 
}

bool ZZSelectorBase::tightSIP() {
    if ((l1SIP3D < 4.0 && l2SIP3D < 4.0 && l3SIP3D < 4.0))
        return true;
    else
        return false;
}
bool ZZSelectorBase::tightZ1Leptons() {
    return lep1IsTight() && lep2IsTight(); 
}
bool ZZSelectorBase::lep3IsTight() {
    return (l3IsTight && l3IsIso);
}
bool ZZSelectorBase::lep4IsTight() {
    return (l4IsTight && l4IsIso);
}
bool ZZSelectorBase::tightZ2Leptons() {
    return lep3IsTight() && lep4IsTight(); 
}
bool ZZSelectorBase::Z1PF(){
  if(lep1IsTight() && !lep2IsTight())
    return true;
  else
    return false;
}
bool ZZSelectorBase::Z1FP(){
  if(lep2IsTight() && !lep1IsTight())
    return true;
  else
    return false;
}
bool ZZSelectorBase::Z1FF(){
  if(!lep1IsTight() && !lep2IsTight())
    return true;
  else
    return false;
}
bool ZZSelectorBase::Z2PF(){
  if(lep3IsTight() && !lep4IsTight())
    return true;
  else
    return false;
}
bool ZZSelectorBase::Z2FP(){
  if(lep4IsTight() && !lep3IsTight())
    return true;
  else
    return false;
}
bool ZZSelectorBase::Z2FF(){
  if(!lep3IsTight() && !lep4IsTight())
    return true;
  else
    return false;
}
//bool ZZSelectorBase::IsGenMatched3l() {
//    //return true;
//    return (!isMC_ || isNonpromptMC_ || 
//        (isZgamma_ && l1GenPt > 0 && l2GenPt > 0) ||
//        (l1GenPt > 0 && l2GenPt > 0 && l3GenPt > 0));
//}

void ZZSelectorBase::Terminate()
{
}
    
void ZZSelectorBase::SlaveTerminate()
{
}
void ZZSelectorBase::UpdateDirectory()
{
  for(TNamed** objPtrPtr : allObjects_) {
    if ( *objPtrPtr == nullptr ) Abort("ZZSelectorBase: Call to UpdateObject but existing pointer is null");
    *objPtrPtr = (TNamed *) currentHistDir_->FindObject((*objPtrPtr)->GetName());
    if ( *objPtrPtr == nullptr ) Abort("ZZSelectorBase: Call to UpdateObject but current directory has no instance");
  }
}

void ZZSelectorBase::SetupNewDirectory()
{
    if (addSumweights_)
        AddObject<TH1D>(sumWeightsHist_, "sumweights", "sumweights", 1, 0, 10);
}
