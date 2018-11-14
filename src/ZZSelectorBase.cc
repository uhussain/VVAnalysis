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

std::string ZZSelectorBase::GetNameFromFile() {
    std::regex expr = std::regex("201[0-9]-[0-9][0-9]-[0-9][0-9]-(.*)-WZxsec2016");
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
        //std::cout<<"channelName: "<<channelName_<<std::endl;
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
        //std::cout<<"enum channel_: "<<channel_<<std::endl;
        fChain->SetBranchAddress("e1ZZTightIDNoVtx", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("e1ZZIsoPass", &l1IsIso, &b_l1IsIso);
        fChain->SetBranchAddress("e2ZZTightIDNoVtx", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("e2ZZIsoPass", &l2IsIso, &b_l2IsIso);
        fChain->SetBranchAddress("e3ZZTightIDNoVtx", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("e3ZZIsoPass", &l3IsIso, &b_l3IsIso);
        fChain->SetBranchAddress("e4ZZTightIDNoVtx", &l4IsTight, &b_l4IsTight);
        fChain->SetBranchAddress("e4ZZIsoPass", &l4IsIso, &b_l4IsIso);
        fChain->SetBranchAddress("e1_e2_Mass", &Z1mass, &b_Z1mass);
        fChain->SetBranchAddress("e3_e4_Mass", &Z2mass, &b_Z2mass);
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
        fChain->SetBranchAddress("e1ZZTightIDNoVtx", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("e1ZZIsoPass", &l1IsIso, &b_l1IsIso);
        fChain->SetBranchAddress("e2ZZTightIDNoVtx", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("e2ZZIsoPass", &l2IsIso, &b_l2IsIso);
        fChain->SetBranchAddress("m1ZZTightIDNoVtx", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("m1ZZIsoPass", &l3IsIso, &b_l3IsIso);
        fChain->SetBranchAddress("m2ZZTightIDNoVtx", &l4IsTight, &b_l4IsTight);
        fChain->SetBranchAddress("m2ZZIsoPass", &l4IsIso, &b_l4IsIso);
        fChain->SetBranchAddress("e1_e2_Mass", &Z1mass, &b_Z1mass);
        fChain->SetBranchAddress("m1_m2_Mass", &Z2mass, &b_Z2mass);
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
        //if (isMC_) {
        //    fChain->SetBranchAddress("e1GenPt", &l1GenPt, &b_l1GenPt);
        //    fChain->SetBranchAddress("e2GenPt", &l2GenPt, &b_l2GenPt);
        //    fChain->SetBranchAddress("e3GenPt", &l3GenPt, &b_l3GenPt);
        //    fChain->SetBranchAddress("e4GenPt", &l4GenPt, &b_l4GenPt);
        //}
    }
    else if (channelName_ == "mmmm") {
        channel_ = mmmm;
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
        //if (isMC_) {
        //    fChain->SetBranchAddress("m1GenPt", &l1GenPt, &b_l1GenPt);
        //    fChain->SetBranchAddress("m2GenPt", &l2GenPt, &b_l2GenPt);
        //    fChain->SetBranchAddress("m3GenPt", &l3GenPt, &b_l3GenPt);
        //    fChain->SetBranchAddress("m4GenPt", &l4GenPt, &b_l4GenPt);
        //}
    }
    else if (channelName_ == "eee") {
        channel_ = eee;
        fChain->SetBranchAddress("e1ZZTightIDNoVtx", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("e1ZZIsoPass", &l1IsIso, &b_l1IsIso);
        fChain->SetBranchAddress("e2ZZTightIDNoVtx", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("e2ZZIsoPass", &l2IsIso, &b_l2IsIso);
        fChain->SetBranchAddress("e3ZZTightIDNoVtx", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("e3ZZIsoPass", &l3IsIso, &b_l3IsIso);
        fChain->SetBranchAddress("e1_e2_Mass", &Z1mass, &b_Z1mass);
        fChain->SetBranchAddress("e1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("e2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("e3Pt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("e1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("e2Eta", &l2Eta, &b_l2Eta);
        fChain->SetBranchAddress("e3Eta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("e1Phi", &l1Phi, &b_l1Phi);
        fChain->SetBranchAddress("e2Phi", &l2Phi, &b_l2Phi);
        fChain->SetBranchAddress("e3Phi", &l3Phi, &b_l3Phi);
        fChain->SetBranchAddress("e3MtToMET", &l3MtToMET, &b_l3MtToMET);
        //if (isMC_) {
        //    fChain->SetBranchAddress("e1GenPt", &l1GenPt, &b_l1GenPt);
        //    fChain->SetBranchAddress("e2GenPt", &l2GenPt, &b_l2GenPt);
        //    fChain->SetBranchAddress("e3GenPt", &l3GenPt, &b_l3GenPt);
        //}
    }
    else if (channelName_ == "eem") { 
        channel_ = eem;
        fChain->SetBranchAddress("e1ZZTightIDNoVtx", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("e1ZZIsoPass", &l1IsIso, &b_l1IsIso);
        fChain->SetBranchAddress("e2ZZTightIDNoVtx", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("e2ZZIsoPass", &l2IsIso, &b_l2IsIso);
        fChain->SetBranchAddress("mZZTightIDNoVtx", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("mZZIsoPass", &l3IsIso, &b_l3IsIso);
        fChain->SetBranchAddress("e1_e2_Mass", &Z1mass, &b_Z1mass); 
        fChain->SetBranchAddress("e1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("e2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("e1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("e2Eta", &l2Eta, &b_l2Eta);
        fChain->SetBranchAddress("e1Phi", &l1Phi, &b_l1Phi);
        fChain->SetBranchAddress("e2Phi", &l2Phi, &b_l2Phi);
        fChain->SetBranchAddress("mPt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("mEta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("mPhi", &l3Phi, &b_l3Phi);
        fChain->SetBranchAddress("mMtToMET", &l3MtToMET, &b_l3MtToMET);
        //if (isMC_) {
        //    fChain->SetBranchAddress("mGenPt", &l3GenPt, &b_l3GenPt);
        //    fChain->SetBranchAddress("e1GenPt", &l1GenPt, &b_l1GenPt);
        //    fChain->SetBranchAddress("e2GenPt", &l2GenPt, &b_l2GenPt);
        //}
    }
    else if (channelName_ == "emm") { 
        channel_ = emm;
        fChain->SetBranchAddress("eZZTightIDNoVtx", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("eZZIsoPass", &l3IsIso, &b_l3IsIso);
        fChain->SetBranchAddress("m1ZZTightIDNoVtx", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("m1ZZIsoPass", &l1IsIso, &b_l1IsIso);
        fChain->SetBranchAddress("m2ZZTightIDNoVtx", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("m2ZZIsoPass", &l2IsIso, &b_l2IsIso);
        fChain->SetBranchAddress("m1_m2_Mass", &Z1mass, &b_Z1mass);
        fChain->SetBranchAddress("m1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("m2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("m1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("m2Eta", &l2Eta, &b_l2Eta);
        fChain->SetBranchAddress("m1Phi", &l1Phi, &b_l1Phi);
        fChain->SetBranchAddress("m2Phi", &l2Phi, &b_l2Phi);
        fChain->SetBranchAddress("ePt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("eEta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("ePhi", &l3Phi, &b_l3Phi);
        fChain->SetBranchAddress("eMtToMET", &l3MtToMET, &b_l3MtToMET);
        //if (isMC_) {
        //    fChain->SetBranchAddress("eGenPt", &l3GenPt, &b_l3GenPt);
        //    fChain->SetBranchAddress("m1GenPt", &l1GenPt, &b_l1GenPt);
        //    fChain->SetBranchAddress("m2GenPt", &l2GenPt, &b_l2GenPt);
        //}
    }
    else if (channelName_ == "mmm") { 
        channel_ = mmm;
        fChain->SetBranchAddress("m1ZZTightIDNoVtx", &l1IsTight, &b_l1IsTight);
        fChain->SetBranchAddress("m1ZZIsoPass", &l1IsIso, &b_l1IsIso);
        fChain->SetBranchAddress("m2ZZTightIDNoVtx", &l2IsTight, &b_l2IsTight);
        fChain->SetBranchAddress("m2ZZIsoPass", &l2IsIso, &b_l2IsIso);
        fChain->SetBranchAddress("m3ZZTightIDNoVtx", &l3IsTight, &b_l3IsTight);
        fChain->SetBranchAddress("m3ZZIsoPass", &l3IsIso, &b_l3IsIso);
        fChain->SetBranchAddress("m1_m2_Mass", &Z1mass, &b_Z1mass);
        fChain->SetBranchAddress("m1Pt", &l1Pt, &b_l1Pt);
        fChain->SetBranchAddress("m2Pt", &l2Pt, &b_l2Pt);
        fChain->SetBranchAddress("m3Pt", &l3Pt, &b_l3Pt);
        fChain->SetBranchAddress("m1Eta", &l1Eta, &b_l1Eta);
        fChain->SetBranchAddress("m2Eta", &l2Eta, &b_l2Eta);
        fChain->SetBranchAddress("m3Eta", &l3Eta, &b_l3Eta);
        fChain->SetBranchAddress("m1Phi", &l1Phi, &b_l1Phi);
        fChain->SetBranchAddress("m2Phi", &l2Phi, &b_l2Phi);
        fChain->SetBranchAddress("m3Phi", &l3Phi, &b_l3Phi);
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
    //std::cout<<"IsMC: "<<isMC_<<std::endl;
    if (isMC_) {
        //b_duplicated->GetEntry(entry);
        b_genWeight->GetEntry(entry);
        //b_l1GenPt->GetEntry(entry);
        //b_l2GenPt->GetEntry(entry);
        //b_l3GenPt->GetEntry(entry);
        //std::cout<<"Its fine before nTruePU" <<std::endl;
        //b_nTruePU->GetEntry(entry);
        //if (channel_ == eeee || channel_ == eemm || channel_ == mmmm) {
        //  b_l4GenPt->GetEntry(entry);}
        //std::cout<<"It fails before weight=genWeight assignment" <<std::endl;
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
    //std::cout<<"Is the ZZSelectorBase fine until here"<<std::endl;
    if (channel_ == eeee || channel_ == eemm || channel_ == mmmm) {
      b_l4Pt->GetEntry(entry);
      b_l4Eta->GetEntry(entry);
      b_l4Phi->GetEntry(entry);
      b_l4SIP3D->GetEntry(entry);
      b_l4IsTight->GetEntry(entry);
      b_l4IsIso->GetEntry(entry);
      //b_l4PVDXY->GetEntry(entry);
      //b_l4PVDZ->GetEntry(entry);
      b_Z2mass->GetEntry(entry);
    }
    b_Z1mass->GetEntry(entry);
    b_type1_pfMETEt->GetEntry(entry); 
    b_l1IsTight->GetEntry(entry);
    b_l1IsIso->GetEntry(entry);
    b_l2IsTight->GetEntry(entry);
    b_l2IsIso->GetEntry(entry);
    b_l3IsTight->GetEntry(entry);
    b_l3IsIso->GetEntry(entry); 
    
    //b_l1PVDXY->GetEntry(entry);
    //b_l2PVDXY->GetEntry(entry);
    //b_l3PVDXY->GetEntry(entry);
    //b_l1PVDZ->GetEntry(entry);
    //b_l2PVDZ->GetEntry(entry);
    //b_l3PVDZ->GetEntry(entry);

    if (channel_ == eee || channel_ == eem || channel_ == emm || channel_ == mmm) {
      b_l3MtToMET->GetEntry(entry);
    }
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
      if(fabs(Z1mass-91.1876) < fabs(Z2mass-91.1876)){
        return true;}
      else{
        return false;}
}
// Meant to be a wrapper for the tight ID just in case it changes
// To be a function of multiple variables
bool ZZSelectorBase::lep1IsTight() {
    return (l1IsTight && l1IsIso); 
}

bool ZZSelectorBase::lep2IsTight() {
    return (l2IsTight && l2IsIso); 
}

bool ZZSelectorBase::tightZ1Leptons() {
    return lep1IsTight() && lep2IsTight(); 
}
//This is explicitly for 2P2F CR
bool ZZSelectorBase::lep3IsLoose() {
    return (!(l3IsTight) && !(l3IsIso));
}
bool ZZSelectorBase::lep3IsTight() {
    return (l3IsTight && l3IsIso);
}
//This is explicitly for 2P2F CR and 3P1F CR
bool ZZSelectorBase::lep4IsLoose() {
    return (!(l4IsTight) && !(l4IsIso));
}
bool ZZSelectorBase::lep4IsTight() {
    return (l4IsTight && l4IsIso);
}
bool ZZSelectorBase::tightZ2Leptons() {
    return lep3IsTight() && lep4IsTight(); 
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
