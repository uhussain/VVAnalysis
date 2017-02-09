#define FakeRateSelector_cxx
// The class definition in FakeRateSelector.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.


// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following se                                              FakeRateSelector.C")
// root> T->Process("FakeRateSelector.C","some options")
// root> T->Process("FakeRateSelector.C+")
//


#include "Analysis/WZAnalysis/interface/FakeRateSelector.h"
#include <TStyle.h>
#include <iostream>

void FakeRateSelector::Begin(TTree * /*tree*/)
{
    // The Begin() function is called at the start of the query.
    // When running with PROOF Begin() is only called on the client.
    // The tree argument is deprecated (on PROOF 0 is passed).

    TString option = GetOption();
}

void FakeRateSelector::SlaveBegin(TTree * /*tree*/)
{
    // The SlaveBegin() function is called after the Begin() function.
    // When running with PROOF SlaveBegin() is called on each slave server.
    // The tree argument is deprecated (on PROOF 0 is passed).

    if (GetInputList() != nullptr) {
        TNamed* name = (TNamed *) GetInputList()->FindObject("name");
        TNamed* chan = (TNamed *) GetInputList()->FindObject("channel");
        if (name != nullptr) {
            name_ = name->GetTitle();
        }
        if (chan != nullptr) {
            channel_ = chan->GetTitle();
        }
    }
    histDir_ = new TList();
    histDir_->SetName(name_);
    fOutput->Add(histDir_);
    passingTight2D_= new TH2D((std::string("passingTight2D_")+channel_).c_str(),
        "Tight leptons; p_{T} [GeV]; Eta", 8, 10, 50, 10, -2.5, 2.5);
    passingTight2D_->SetDirectory(0);
    histDir_->Add(passingTight2D_);
    passingTight1DPt_= new TH1D((std::string("passingTight1DPt_")+channel_).c_str(),
        "Tight leptons; p_{T} [GeV]", 8, 10, 50);
    passingTight1DPt_->SetDirectory(0);
    histDir_->Add(passingTight1DPt_);
    passingTight1DEta_ = new TH1D( (std::string("passingTight1DEta_")+channel_).c_str(), 
        "Tight leptons; #eta; Events", 10, -2.5, 2.5);
    passingTight1DEta_->SetDirectory(0);
    histDir_->Add(passingTight1DEta_);
    passingLoose2D_ = new TH2D((std::string("passingLoose2D_")+channel_).c_str(), 
        "Leptons passing tight failing loose; p_{T} [GeV]; Eta", 8, 10, 50, 10, -2.5, 2.5);
    passingLoose2D_->SetDirectory(0);
    histDir_->Add(passingLoose2D_);
    
    passingLoose1DPt_ = new TH1D((std::string("passingLoose1DPt_")+channel_).c_str(), 
        "Loose leptons; p_{T} [GeV]; Events", 8, 10, 50);
    passingLoose1DPt_->SetDirectory(0);
    histDir_->Add(passingLoose1DPt_);
    
    passingLoose1DEta_ = new TH1D((std::string("passingLoose1DEta_")+channel_).c_str(), 
        "Loose leptons; #eta; Events", 10, -2.5, 2.5);
    passingLoose1DEta_->SetDirectory(0);
    histDir_->Add(passingLoose1DEta_);
    TString option = GetOption();

}

Bool_t FakeRateSelector::Process(Long64_t entry)
{
    // The Process() function is called for each entry in the tree (or possibly
    // keyed object in the case of PROOF) to be processed. The entry argument
    // specifies which entry in the currently loaded tree is to be processed.
    // When processing keyed objects with PROOF, the object is already loaded
    // and is available via the fObject pointer.
    //
    // This function should contain the \"body\" of the analysis. It can contain
    // simple or elaborate selection criteria, run algorithms on the data
    // of the event and typically fill histograms.
    //
    // The processing can be stopped by calling Abort().
    //
    // Use fStatus to set the return value of TTree::Process().
    //
    // The return value is currently not used.

    b_type1_pfMETEt->GetEntry(entry);
    b_nWWLooseElec->GetEntry(entry);
    b_nWZLooseMuon->GetEntry(entry);
    b_l1IsTight->GetEntry(entry);
    b_l2IsTight->GetEntry(entry);
    b_l3IsTight->GetEntry(entry);
    b_l3Eta->GetEntry(entry);
    b_l3Pt->GetEntry(entry);
    if (channel_ == "eee") {
        b_e1IsEB->GetEntry(entry);
        b_e2IsEB->GetEntry(entry);
        b_e3IsEB->GetEntry(entry);
        b_e1PVDXY->GetEntry(entry);
        b_e2PVDXY->GetEntry(entry);
        b_e3PVDXY->GetEntry(entry);
        b_e1PVDZ->GetEntry(entry);
        b_e2PVDZ->GetEntry(entry);
        b_e3PVDZ->GetEntry(entry);
    }
    else if (channel_ == "eem") {
        b_e1IsEB->GetEntry(entry);
        b_e2IsEB->GetEntry(entry);
        b_e1PVDXY->GetEntry(entry);
        b_e2PVDXY->GetEntry(entry);
        b_e1PVDZ->GetEntry(entry);
        b_e2PVDZ->GetEntry(entry);
        b_m3RelPFIsoDBR04->GetEntry(entry);
    }
    else if (channel_ == "emm") {
        b_e3IsEB->GetEntry(entry);
        b_e3PVDXY->GetEntry(entry);
        b_e3PVDZ->GetEntry(entry);
        b_m1RelPFIsoDBR04->GetEntry(entry);
        b_m2RelPFIsoDBR04->GetEntry(entry);
    }
    else if (channel_ == "mmm") {
        b_m1RelPFIsoDBR04->GetEntry(entry);
        b_m2RelPFIsoDBR04->GetEntry(entry);
        b_m3RelPFIsoDBR04->GetEntry(entry);
    }
    if (std::abs(static_cast<int>(nWWLooseElec + nWZLooseMuon) - 3) > 0.1)
        return true;
    if (type1_pfMETEt > 30)
        return true;
    if (!tightZLeptons())
        return true;
    passingLoose2D_->Fill(l3Pt, l3Eta);
    passingLoose1DPt_->Fill(l3Pt);
    passingLoose1DEta_->Fill(l3Eta);
    if (lepton3IsTight()) {
        passingTight2D_->Fill(l3Pt, l3Eta);
        passingTight1DPt_->Fill(l3Pt);
        passingTight1DEta_->Fill(l3Eta);
    }
    return kTRUE;
}

bool FakeRateSelector::tightZLeptons() {
    if (channel_ == "eem" || channel_ == "eee") {
        return (l1IsTight &&
            (e1IsEB ? e1PVDXY < 0.05 : e1PVDXY < 0.1) &&
            (e1IsEB ? e1PVDZ < 0.1 : e1PVDZ < 0.2) &&
            l2IsTight &&
            (e2IsEB ? e2PVDXY < 0.05 : e2PVDXY < 0.1) &&
            (e2IsEB ? e2PVDZ < 0.1 : e2PVDZ < 0.2));
    }
    else 
        return (l1IsTight && m1RelPFIsoDBR04 &&
            l2IsTight && m2RelPFIsoDBR04);
}

bool FakeRateSelector::lepton3IsTight() {
    if (channel_ == "eee" || channel_ == "emm") {
        return (l3IsTight &&
            (e3IsEB ? e3PVDZ < 0.1 : e3PVDZ < 0.2) &&
            (e3IsEB ? e3PVDZ < 0.1 : e3PVDZ < 0.2));
    }
    else if (channel_ == "mmm" || channel_ == "eem") {
        return (l3IsTight && m3RelPFIsoDBR04);
    }
    else
        return false;
}
void FakeRateSelector::SlaveTerminate()
{
    // The SlaveTerminate() function is called after all entries or objects
    // have been processed. When running with PROOF SlaveTerminate() is called
    // on each slave server.

}

void FakeRateSelector::Terminate()
{
    // The Terminate() function is the last function to be called during
    // a query. It always runs on the client, it can be used to present
    // the results graphically or save the results to file.
    ratio2D_ = dynamic_cast<TH2D*>(passingTight2D_->Clone());
    ratio2D_->Divide(passingLoose2D_);
    ratio2D_->SetName((std::string("ratio2D_") + channel_).c_str());
    ratio2D_->SetTitle("Ratio tight/loose");
    ratio2D_->SetDirectory(0);
    histDir_->Add(ratio2D_);

    ratio1DPt_ = dynamic_cast<TH1D*>(passingTight1DPt_->Clone());
    ratio1DPt_->Divide(passingLoose1DPt_);
    ratio1DPt_->SetName((std::string("ratio1DPt_") + channel_).c_str());
    ratio1DPt_->SetTitle("Ratio tight/loose");
    ratio1DPt_->SetDirectory(0);
    histDir_->Add(ratio1DPt_);

    ratio1DEta_ = dynamic_cast<TH1D*>(passingTight1DEta_->Clone());
    ratio1DEta_->Divide(passingLoose1DEta_);
    ratio1DEta_->SetName((std::string("ratio1DEta_") + channel_).c_str());
    ratio1DEta_->SetTitle("Ratio tight/loose");
    ratio1DEta_->SetDirectory(0);
    histDir_->Add(ratio1DEta_);
}
