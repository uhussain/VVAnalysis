// The class definition in FakeRateSelectorEEETest.h has been generated automatically
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
// root> T->Process("FakeRateSelectorEEETest.C")
// root> T->Process("FakeRateSelectorEEETest.C","some options")
// root> T->Process("FakeRateSelectorEEETest.C+")
//


#include "Analysis/WZAnalysis/interface/FakeRateSelectorEEETest.h"
#include <TStyle.h>

void FakeRateSelectorEEETest::Begin(TTree * /*tree*/)
{
    // The Begin() function is called at the start of the query.
    // When running with PROOF Begin() is only called on the client.
    // The tree argument is deprecated (on PROOF 0 is passed).
    const char* dataset = "Unnamed";
    if (GetInputList() != nullptr) {
        TNamed* name = (TNamed *) GetInputList()->FindObject("name");
        if (name != nullptr)
            dataset = name->GetTitle();
    }
    histDir_ = new TList();
    histDir_->SetName(dataset);
    fOutput->Add(histDir_);
    std::cout << "Commencer";
    const char* channel = "eee";
    passingTight2D_= new TH2D((std::string("passingTight2D_") + channel).c_str(),
        "Tight leptons; p_{T} [GeV]; Eta", 8, 10, 50, 5, -2.5, 2.5);
    histDir_->Add(passingTight2D_);
    passingTight1DPt_= new TH1D((std::string("passingTight1DPt_") + channel).c_str(),
        "Tight leptons; p_{T} [GeV]", 8, 10, 50);
    histDir_->Add(passingTight1DPt_);
    passingTight1DEta_ = new TH1D( (std::string("passingTight1DEta_") + channel).c_str(), 
        "Tight leptons; #eta; Events", 5, -2.5, 2.5);
    histDir_->Add(passingTight1DEta_);
    passingLoose2D_ = new TH2D((std::string("passingLoose2D_") + channel).c_str(), 
        "Leptons passing tight failing loose; p_{T} [GeV]; Eta", 8, 10, 50, 5, -2.5, 2.5);
    histDir_->Add(passingLoose2D_);
    passingLoose1DPt_ = new TH1D((std::string("passingLoose1DPt_") + channel).c_str(), 
        "Loose leptons; p_{T} [GeV]; Events", 8, 10, 50);
    histDir_->Add(passingLoose1DPt_);
    passingLoose1DEta_ = new TH1D((std::string("passingLoose1DEta_") + channel).c_str(), 
        "Loose leptons; #eta; Events", 5, -2.5, 2.5);
    histDir_->Add(passingLoose1DEta_);
    TString option = GetOption();
}

void FakeRateSelectorEEETest::SlaveBegin(TTree * /*tree*/)
{
    // The SlaveBegin() function is called after the Begin() function.
    // When running with PROOF SlaveBegin() is called on each slave server.
    // The tree argument is deprecated (on PROOF 0 is passed).

    TString option = GetOption();

}

Bool_t FakeRateSelectorEEETest::Process(Long64_t entry)
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

void FakeRateSelectorEEETest::SlaveTerminate()
{
    // The SlaveTerminate() function is called after all entries or objects
    // have been processed. When running with PROOF SlaveTerminate() is called
    // on each slave server.

}

void FakeRateSelectorEEETest::Terminate()
{
    // The Terminate() function is the last function to be called during
    // a query. It always runs on the client, it can be used to present
    // the results graphically or save the results to file.

}

void FakeRateSelectorEEETest::Init(TTree *tree)
{
    // The Init() function is called when the selector needs to initialize
    // a new tree or chain. Typically here the reader is initialized.
    // It is normally not necessary to make changes to the generated
    // code, but the routine can be extended by the user if needed.
    // Init() will be called many times when running on PROOF
    // (once per file to be processed).

    fReader.SetTree(tree);
}

Bool_t FakeRateSelectorEEETest::Notify()
{
    // The Notify() function is called when a new file is opened. This
    // can be either for a new TTree in a TChain or when when a new TTree
    // is started when using PROOF. It is normally not necessary to make changes
    // to the generated code, but the routine can be extended by the
    // user if needed. The return value is currently not used.

    return kTRUE;
}
