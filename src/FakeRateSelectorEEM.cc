// The class definition in FakeRateSelectorEEM.h has been generated automatically
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
// root> T->Process("FakeRateSelectorEEM.C")
// root> T->Process("FakeRateSelectorEEM.C","some options")
// root> T->Process("FakeRateSelectorEEM.C+")
//


#include "Analysis/WZAnalysis/interface/FakeRateSelectorEEM.h"
#include <TStyle.h>

void FakeRateSelectorEEM::Begin(TTree * /*tree*/)
{
    // The Begin() function is called at the start of the query.
    // When running with PROOF Begin() is only called on the client.
    // The tree argument is deprecated (on PROOF 0 is passed).
    TString option = GetOption();
}

void FakeRateSelectorEEM::Init(TTree *tree)
{
    fReader.SetTree(tree);
}

void FakeRateSelectorEEM::SlaveBegin(TTree * /*tree*/)
{
   // const char* dataset = "Unnamed";
   // if (GetInputList() != nullptr) {
   //     TNamed* name = (TNamed *) GetInputList()->FindObject("name");
   //     if (name != nullptr)
   //         dataset = name->GetTitle();
   // }
   // const char* channel = "eem";
   // passingTight2D_= new TH2D((std::string("passingTight2D_")+channel+"_"+dataset).c_str(),
   //     "Tight leptons; p_{T} [GeV]; Eta", 8, 10, 50, 5, -2.5, 2.5);
   // passingTight2D_->SetDirectory(0);
   // fOutput->Add(passingTight2D_);
   // passingTight1DPt_= new TH1D((std::string("passingTight1DPt_")+channel+"_"+dataset).c_str(),
   //     "Tight leptons; p_{T} [GeV]", 8, 10, 50);
   // passingTight1DPt_->SetDirectory(0);
   // fOutput->Add(passingTight1DPt_);
   // passingTight1DEta_ = new TH1D( (std::string("passingTight1DEta_")+channel+"_"+dataset).c_str(), 
   //     "Tight leptons; #eta; Events", 5, -2.5, 2.5);
   // passingTight1DEta_->SetDirectory(0);
   // fOutput->Add(passingTight1DEta_);
   // passingLoose2D_ = new TH2D((std::string("passingLoose2D_")+channel+"_"+dataset).c_str(), 
   //     "Leptons passing tight failing loose; p_{T} [GeV]; Eta", 8, 10, 50, 5, -2.5, 2.5);
   // passingLoose2D_->SetDirectory(0);
   // fOutput->Add(passingLoose2D_);
   // passingLoose1DPt_ = new TH1D((std::string("passingLoose1DPt_")+channel+"_"+dataset).c_str(), 
   //     "Loose leptons; p_{T} [GeV]; Events", 8, 10, 50);
   // passingLoose1DPt_->SetDirectory(0);
   // fOutput->Add(passingLoose1DPt_);
   // passingLoose1DEta_ = new TH1D((std::string("passingLoose1DEta_")+channel+"_"+dataset).c_str(), 
   //     "Loose leptons; #eta; Events", 5, -2.5, 2.5);
   // passingLoose1DEta_->SetDirectory(0);
   // fOutput->Add(passingLoose1DEta_);

    TString option = GetOption();

}

Bool_t FakeRateSelectorEEM::Process(Long64_t entry)
{
    fReader.SetEntry(entry);
//    if (std::abs(static_cast<int>(*nWWLooseElec + *nWZLooseMuon) - 3) > 0.1)
//        return false;
    if (*type1_pfMETEt > 30)
        return true;
//    if (!(*e1IsCBVIDTight &&
//            ( *e1IsEB ? *e1PVDXY < 0.05 : *e1PVDXY < 0.1) &&
//            ( *e1IsEB ? *e1PVDZ < 0.1 : *e1PVDZ < 0.2)) &&
//            (*e2IsCBVIDTight &&
//            ( *e2IsEB ? *e2PVDXY < 0.05 : *e2PVDXY < 0.1) &&
//            ( *e2IsEB ? *e2PVDZ < 0.1 : *e2PVDZ < 0.2)))
//        return false;
//    passingLoose2D_->Fill(*mPt, *mEta);
//    passingLoose1DPt_->Fill(*mPt);
//    passingLoose1DEta_->Fill(*mEta);
//    if (*mIsTightMuon && *mRelPFIsoDBR04 < 0.15) {
//        passingTight2D_->Fill(*mPt, *mEta);
//        passingTight1DPt_->Fill(*mPt);
//        passingTight1DEta_->Fill(*mEta);
//    }
//
    return true;
}

void FakeRateSelectorEEM::SlaveTerminate()
{
}

void FakeRateSelectorEEM::Terminate()
{
}

Bool_t FakeRateSelectorEEM::Notify()
{
    return kTRUE;
}

FakeRateSelectorEEM::~FakeRateSelectorEEM() {}
