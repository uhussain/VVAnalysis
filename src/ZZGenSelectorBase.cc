#include "Analysis/VVAnalysis/interface/ZZGenSelectorBase.h"
#include <TStyle.h>
#include <regex>
#include "TParameter.h"

void ZZGenSelectorBase::Begin(TTree * /*tree*/)
{
    TString option = GetOption();
}

void ZZGenSelectorBase::SlaveBegin(TTree * /*tree*/)
{
}

std::string ZZGenSelectorBase::GetNameFromFile() {
    std::regex expr = std::regex("201[0-9]-[0-9][0-9]-[0-9][0-9]-(.*)-ZZ4l2018");
    std::smatch matches;
    std::string fileName = fChain->GetTree()->GetDirectory()->GetFile()->GetName(); 

    std::regex_search(fileName, matches, expr);
    return std::string(matches.str(1));
}

void ZZGenSelectorBase::Init(TTree *tree)
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
    isMC_ = false;
    if (name_.find("data") == std::string::npos){
        isMC_ = true;
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
            Abort(Form("ZZGenSelectorBase: Size of allObjects has changed!: %lu to %lu", existingObjectPtrsSize, allObjects_.size()));
        }
    }
    UpdateDirectory();
    //std::cout<<"channelName: "<<channelName_<<std::endl;
    //std::cout<<"enum channel_: "<<channel_<<std::endl;
    //std::cout<<"isMC: "<<isMC_<<std::endl;
    if (channelName_ == "eeee") {
        channel_ = eeee;
        //std::cout<<"enum channel_: "<<channel_<<std::endl;
        fChain->SetBranchAddress("e1_e2_Mass", &GenZ1mass, &b_GenZ1mass);
        fChain->SetBranchAddress("e3_e4_Mass", &GenZ2mass, &b_GenZ2mass);
        fChain->SetBranchAddress("e1_e2_Pt", &GenZ1pt, &b_GenZ1pt);
        fChain->SetBranchAddress("e3_e4_Pt", &GenZ2pt, &b_GenZ2pt);
        fChain->SetBranchAddress("e1_e2_Phi", &GenZ1Phi, &b_GenZ1Phi);
        fChain->SetBranchAddress("e3_e4_Phi", &GenZ2Phi, &b_GenZ2Phi);
        fChain->SetBranchAddress("e1Pt", &Genl1Pt, &b_Genl1Pt);
        fChain->SetBranchAddress("e2Pt", &Genl2Pt, &b_Genl2Pt);
        fChain->SetBranchAddress("e3Pt", &Genl3Pt, &b_Genl3Pt);
        fChain->SetBranchAddress("e4Pt", &Genl4Pt, &b_Genl4Pt);
        fChain->SetBranchAddress("e1Eta", &Genl1Eta, &b_Genl1Eta);
        fChain->SetBranchAddress("e2Eta", &Genl2Eta, &b_Genl2Eta);
        fChain->SetBranchAddress("e3Eta", &Genl3Eta, &b_Genl3Eta);
        fChain->SetBranchAddress("e4Eta", &Genl4Eta, &b_Genl4Eta);
        fChain->SetBranchAddress("e1Phi", &Genl1Phi, &b_Genl1Phi);
        fChain->SetBranchAddress("e2Phi", &Genl2Phi, &b_Genl2Phi);
        fChain->SetBranchAddress("e3Phi", &Genl3Phi, &b_Genl3Phi);
        fChain->SetBranchAddress("e4Phi", &Genl4Phi, &b_Genl4Phi);
    }
    //Add 2e2mu channel also but it still needs to differentiate which one is Z1Mass and which one is Z2Mass leptons
    //This is done with a flag at the time of Process for each event on the fly
    else if (channelName_ == "eemm") {
        channel_ = eemm;
        fChain->SetBranchAddress("e1_e2_Mass", &GenZ1mass, &b_GenZ1mass);
        fChain->SetBranchAddress("m1_m2_Mass", &GenZ2mass, &b_GenZ2mass);
        fChain->SetBranchAddress("e1_e2_Pt", &GenZ1pt, &b_GenZ1pt);
        fChain->SetBranchAddress("m1_m2_Pt", &GenZ2pt, &b_GenZ2pt);
        fChain->SetBranchAddress("e1_e2_Phi", &GenZ1Phi, &b_GenZ1Phi);
        fChain->SetBranchAddress("m1_m2_Phi", &GenZ2Phi, &b_GenZ2Phi);
        fChain->SetBranchAddress("e1Pt", &Genl1Pt, &b_Genl1Pt);
        fChain->SetBranchAddress("e2Pt", &Genl2Pt, &b_Genl2Pt);
        fChain->SetBranchAddress("m1Pt", &Genl3Pt, &b_Genl3Pt);
        fChain->SetBranchAddress("m2Pt", &Genl4Pt, &b_Genl4Pt);
        fChain->SetBranchAddress("e1Eta", &Genl1Eta, &b_Genl1Eta);
        fChain->SetBranchAddress("e2Eta", &Genl2Eta, &b_Genl2Eta);
        fChain->SetBranchAddress("m1Eta", &Genl3Eta, &b_Genl3Eta);
        fChain->SetBranchAddress("m2Eta", &Genl4Eta, &b_Genl4Eta);
        fChain->SetBranchAddress("e1Phi", &Genl1Phi, &b_Genl1Phi);
        fChain->SetBranchAddress("e2Phi", &Genl2Phi, &b_Genl2Phi);
        fChain->SetBranchAddress("m1Phi", &Genl3Phi, &b_Genl3Phi);
        fChain->SetBranchAddress("m2Phi", &Genl4Phi, &b_Genl4Phi);
    }
    else if (channelName_ == "mmee") {
        channel_ = mmee;
        fChain->SetBranchAddress("e1_e2_Mass", &GenZ1mass, &b_GenZ1mass);
        fChain->SetBranchAddress("m1_m2_Mass", &GenZ2mass, &b_GenZ2mass);
        fChain->SetBranchAddress("e1_e2_Pt", &GenZ1pt, &b_GenZ1pt);
        fChain->SetBranchAddress("m1_m2_Pt", &GenZ2pt, &b_GenZ2pt);
        fChain->SetBranchAddress("e1_e2_Phi", &GenZ1Phi, &b_GenZ1Phi);
        fChain->SetBranchAddress("m1_m2_Phi", &GenZ2Phi, &b_GenZ2Phi);
        fChain->SetBranchAddress("e1Pt", &Genl1Pt, &b_Genl1Pt);
        fChain->SetBranchAddress("e2Pt", &Genl2Pt, &b_Genl2Pt);
        fChain->SetBranchAddress("m1Pt", &Genl3Pt, &b_Genl3Pt);
        fChain->SetBranchAddress("m2Pt", &Genl4Pt, &b_Genl4Pt);
        fChain->SetBranchAddress("e1Eta", &Genl1Eta, &b_Genl1Eta);
        fChain->SetBranchAddress("e2Eta", &Genl2Eta, &b_Genl2Eta);
        fChain->SetBranchAddress("m1Eta", &Genl3Eta, &b_Genl3Eta);
        fChain->SetBranchAddress("m2Eta", &Genl4Eta, &b_Genl4Eta);
        fChain->SetBranchAddress("e1Phi", &Genl1Phi, &b_Genl1Phi);
        fChain->SetBranchAddress("e2Phi", &Genl2Phi, &b_Genl2Phi);
        fChain->SetBranchAddress("m1Phi", &Genl3Phi, &b_Genl3Phi);
        fChain->SetBranchAddress("m2Phi", &Genl4Phi, &b_Genl4Phi);
    }
    else if (channelName_ == "mmmm") {
        channel_ = mmmm;
        fChain->SetBranchAddress("m1_m2_Mass", &GenZ1mass, &b_GenZ1mass);
        fChain->SetBranchAddress("m3_m4_Mass", &GenZ2mass, &b_GenZ2mass);
        fChain->SetBranchAddress("m1_m2_Pt", &GenZ1pt, &b_GenZ1pt);
        fChain->SetBranchAddress("m3_m4_Pt", &GenZ2pt, &b_GenZ2pt);
        fChain->SetBranchAddress("m1_m2_Phi", &GenZ1Phi, &b_GenZ1Phi);
        fChain->SetBranchAddress("m3_m4_Phi", &GenZ2Phi, &b_GenZ2Phi);
        fChain->SetBranchAddress("m1Pt", &Genl1Pt, &b_Genl1Pt);
        fChain->SetBranchAddress("m2Pt", &Genl2Pt, &b_Genl2Pt);
        fChain->SetBranchAddress("m3Pt", &Genl3Pt, &b_Genl3Pt);
        fChain->SetBranchAddress("m4Pt", &Genl4Pt, &b_Genl4Pt);
        fChain->SetBranchAddress("m1Eta", &Genl1Eta, &b_Genl1Eta);
        fChain->SetBranchAddress("m2Eta", &Genl2Eta, &b_Genl2Eta);
        fChain->SetBranchAddress("m3Eta", &Genl3Eta, &b_Genl3Eta);
        fChain->SetBranchAddress("m4Eta", &Genl4Eta, &b_Genl4Eta);
        fChain->SetBranchAddress("m1Phi", &Genl1Phi, &b_Genl1Phi);
        fChain->SetBranchAddress("m2Phi", &Genl2Phi, &b_Genl2Phi);
        fChain->SetBranchAddress("m3Phi", &Genl3Phi, &b_Genl3Phi);
        fChain->SetBranchAddress("m4Phi", &Genl4Phi, &b_Genl4Phi);
    }
    else
        throw std::invalid_argument("Invalid channel choice!");

}


Bool_t ZZGenSelectorBase::Notify()
{
    return kTRUE;
}

Bool_t ZZGenSelectorBase::Process(Long64_t entry)
{
    Genweight = 1;
    b_Genl1Pt->GetEntry(entry);
    b_Genl2Pt->GetEntry(entry);
    b_Genl3Pt->GetEntry(entry);
    b_Genl1Eta->GetEntry(entry);
    b_Genl2Eta->GetEntry(entry);
    b_Genl3Eta->GetEntry(entry);
    b_Genl1Phi->GetEntry(entry);
    b_Genl2Phi->GetEntry(entry);
    b_Genl3Phi->GetEntry(entry);
    //std::cout<<"Is the ZZGenSelectorBase fine until here"<<std::endl;
    if (channel_ == eeee || channel_ == eemm || channel_ == mmee || channel_ == mmmm) {
      b_Genl4Pt->GetEntry(entry);
      b_Genl4Eta->GetEntry(entry);
      b_Genl4Phi->GetEntry(entry);
      b_GenZ2mass->GetEntry(entry);
      b_GenZ2pt->GetEntry(entry);
      b_GenZ2Phi->GetEntry(entry);
    }
    b_GenZ1mass->GetEntry(entry);
    b_GenZ1pt->GetEntry(entry);
    b_GenZ1Phi->GetEntry(entry);
    if(channel_ == mmee){
      if(e1e2IsZ1(entry))
        Genweight=0.0;
        //Makes Genweight 0 if Z1 is ee hence should not go in _mmee histos
    }
    else if(channel_ == eemm){
      if(!(e1e2IsZ1(entry)))
        Genweight=0.0;
        //Makes Genweight 0 if Z1 is mm hence should not go in _eemm 
    } 
    return kTRUE;
}

//This condition identifies the Z1 candidate
//Required for the 2e2mu state 
bool ZZGenSelectorBase::e1e2IsZ1(Long64_t entry){
  if(fabs(GenZ1mass-91.1876) < fabs(GenZ2mass-91.1876))
    return true;
  else
    return false;
  }
void ZZGenSelectorBase::Terminate()
{
}
    
void ZZGenSelectorBase::SlaveTerminate()
{
}
void ZZGenSelectorBase::UpdateDirectory()
{
  for(TNamed** objPtrPtr : allObjects_) {
    if ( *objPtrPtr == nullptr ) Abort("ZZGenSelectorBase: Call to UpdateObject but existing pointer is null");
    *objPtrPtr = (TNamed *) currentHistDir_->FindObject((*objPtrPtr)->GetName());
    if ( *objPtrPtr == nullptr ) Abort("ZZGenSelectorBase: Call to UpdateObject but current directory has no instance");
  }
}

void ZZGenSelectorBase::SetupNewDirectory()
{
}
