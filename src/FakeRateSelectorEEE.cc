// The class definition in FakeRateSelectorEEE.h has been generated automatically
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
// To use this file, try the following session on your Tree T:
//
// root> T->Process("FakeRateSelectorEEE.C")
// root> T->Process("FakeRateSelectorEEE.C","some options")
// root> T->Process("FakeRateSelectorEEE.C+")
//


#include "Analysis/WZAnalysis/interface/FakeRateSelectorEEE.h"
#include <TStyle.h>

void FakeRateSelectorEEE::Begin(TTree * /*tree*/)
{
    TString option = GetOption();
}

void FakeRateSelectorEEE::SlaveBegin(TTree *tree)
{
    const char* dataset = "Unnamed";
    if (GetInputList() != nullptr) {
        TNamed* name = (TNamed *) GetInputList()->FindObject("name");
        if (name != nullptr)
            dataset = name->GetTitle();
    }
    const char* channel = "eee";
    passingTight2D_= new TH2D((std::string("passingTight2D_")+channel+"_"+dataset).c_str(),
        "Tight leptons; p_{T} [GeV]; Eta", 8, 10, 50, 5, -2.5, 2.5);
    passingTight2D_->SetDirectory(0);
    //fOutput->Add(passingTight2D_);
    passingTight1DPt_= new TH1D((std::string("passingTight1DPt_")+channel+"_"+dataset).c_str(),
        "Tight leptons; p_{T} [GeV]", 8, 10, 50);
    passingTight1DPt_->SetDirectory(0);
    fOutput->Add(passingTight1DPt_);
    passingTight1DEta_ = new TH1D( (std::string("passingTight1DEta_")+channel+"_"+dataset).c_str(), 
        "Tight leptons; #eta; Events", 5, -2.5, 2.5);
    passingTight1DEta_->SetDirectory(0);
    fOutput->Add(passingTight1DEta_);
    passingLoose2D_ = new TH2D((std::string("passingLoose2D_")+channel+"_"+dataset).c_str(), 
        "Leptons passing tight failing loose; p_{T} [GeV]; Eta", 8, 10, 50, 5, -2.5, 2.5);
    passingLoose2D_->SetDirectory(0);
    fOutput->Add(passingLoose2D_);
    passingLoose1DPt_ = new TH1D((std::string("passingLoose1DPt_")+channel+"_"+dataset).c_str(), 
        "Loose leptons; p_{T} [GeV]; Events", 8, 10, 50);
    passingLoose1DPt_->SetDirectory(0);
    fOutput->Add(passingLoose1DPt_);
    passingLoose1DEta_ = new TH1D((std::string("passingLoose1DEta_")+channel+"_"+dataset).c_str(), 
        "Loose leptons; #eta; Events", 5, -2.5, 2.5);
    passingLoose1DEta_->SetDirectory(0);
    fOutput->Add(passingLoose1DEta_);

    TString option = GetOption();
}

Bool_t FakeRateSelectorEEE::Process(Long64_t entry)
{
    fReader.SetEntry(entry);
    if (std::abs(static_cast<int>(*nWWLooseElec + *nWZLooseMuon) - 3) > 0.1)
        return false;
    if (*type1_pfMETEt > 30)
        return false;
    if (!(*e1IsCBVIDTight &&
            ( *e1IsEB ? *e1PVDXY < 0.05 : *e1PVDXY < 0.1) &&
            ( *e1IsEB ? *e1PVDZ < 0.1 : *e1PVDZ < 0.2)) &&
            (*e2IsCBVIDTight &&
            ( *e2IsEB ? *e2PVDXY < 0.05 : *e2PVDXY < 0.1) &&
            ( *e2IsEB ? *e2PVDZ < 0.1 : *e2PVDZ < 0.2)))
        return false;
    std::cout << "Pass loose" << std::endl;
    passingLoose2D_->Fill(*e3Pt, *e3Eta);
    passingLoose1DPt_->Fill(*e3Pt);
    passingLoose1DEta_->Fill(*e3Eta);
    if (*e3IsCBVIDTight &&
        ( *e3IsEB ? *e3PVDXY < 0.05 : *e3PVDXY < 0.1) &&
        ( *e3IsEB ? *e3PVDZ < 0.1 : *e3PVDZ < 0.2)) {
        std::cout << "Pass tight" << std::endl;
        passingTight2D_->Fill(*e3Pt, *e3Eta);
        passingTight1DPt_->Fill(*e3Pt);
        passingTight1DEta_->Fill(*e3Eta);
    }

    return kTRUE;
}

void FakeRateSelectorEEE::SlaveTerminate()
{
}

void FakeRateSelectorEEE::Terminate()
{
}

void FakeRateSelectorEEE::Init(TTree * tree)
{
    fReader.SetTree(tree);
}

Bool_t FakeRateSelectorEEE::Notify()
{
    return kTRUE;
}
