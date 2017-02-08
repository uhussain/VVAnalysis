#include "Analysis/WZAnalysis/interface/FakeRateSelectorEEE.h"
#include <TStyle.h>

void FakeRateSelectorEEE::SlaveBegin(TTree* /*tree*/) {
    std::cout << "Commencer";
    //const char* channel = "eee";
    //passingTight2D_= new TH2D( (std::string("passingTight2D_") + channel).c_str(),
    //    "Tight leptons; p_{T} [GeV]; Eta", 8, 10, 50, 5, -2.5, 2.5);
    //fOutput->Add(passingTight2D_);
    //passingTight1DPt_= new TH1D( (std::string("passingTight1DPt_") + channel).c_str(),
    //    "Tight leptons; p_{T} [GeV]", 8, 10, 50);
    //passingTight1DEta_ = new TH1D( (std::string("passingTight1DEta_") + channel).c_str(), 
    //    "Tight leptons; #eta; Events", 5, -2.5, 2.5);
    //fOutput->Add(passingLoose1DPt_);
    //passingLoose2D_ = new TH2D( (std::string("passingLoose2D_") + channel).c_str(), 
    //    "Leptons passing tight failing loose; p_{T} [GeV]; Eta", 8, 10, 50, 5, -2.5, 2.5);
    //fOutput->Add(passingLoose2D_);
    //passingLoose1DPt_ = new TH1D( (std::string("passingLoose1DPt_") + channel).c_str(), 
    //    "Loose leptons; p_{T} [GeV]; Events", 8, 10, 50);
    //fOutput->Add(passingLoose1DPt_);
    //passingLoose1DEta_ = new TH1D( (std::string("passingLoose1DEta_") + channel).c_str(), 
    //    "Loose leptons; #eta; Events", 5, -2.5, 2.5);
    //fOutput->Add(passingLoose1DEta_);
}

Bool_t FakeRateSelectorEEE::Process(Long64_t entry)
{
    fReader.SetEntry(entry);
    //if (std::abs(static_cast<int>(*nWWLooseElec + *nWZLooseMuon) - 3) > 0.1)
    //    return false;
    //if (*type1_pfMETEt > 30)
    //    return false;
    //if (!(*e1IsCBVIDTight &&
    //        ( *e1IsEB ? *e1PVDXY < 0.05 : *e1PVDXY < 0.1) &&
    //        ( *e1IsEB ? *e1PVDZ < 0.1 : *e1PVDZ < 0.2)) &&
    //        (*e2IsCBVIDTight &&
    //        ( *e2IsEB ? *e2PVDXY < 0.05 : *e2PVDXY < 0.1) &&
    //        ( *e2IsEB ? *e2PVDZ < 0.1 : *e2PVDZ < 0.2)))
    //    return false;
    //passingLoose2D_->Fill(*e3Pt, *e3Eta);
    //passingLoose1DPt_->Fill(*e3Pt);
    //passingLoose1DEta_->Fill(*e3Eta);
    //if (*e3IsCBVIDTight &&
    //    ( *e3IsEB ? *e3PVDXY < 0.05 : *e3PVDXY < 0.1) &&
    //    ( *e3IsEB ? *e3PVDZ < 0.1 : *e3PVDZ < 0.2)) {
    //    passingTight2D_->Fill(*e3Pt, *e3Eta);
    //    passingTight1DPt_->Fill(*e3Pt);
    //    passingTight1DEta_->Fill(*e3Eta);
    //}
    //std::cout << "Yes is here";

    return true;
}

Bool_t FakeRateSelectorEEE::Notify()
{
      return kTRUE;
}

void FakeRateSelectorEEE::Init(TTree *tree)
{
  fReader.SetTree(tree);
}

void FakeRateSelectorEEE::Begin(TTree * /*tree*/)
{
}

void FakeRateSelectorEEE::SlaveTerminate()
{
}
void FakeRateSelectorEEE::Terminate()
{
}

//void FakeRateSelectorEEE::SetupNewDirectory()
//{
//    // Must call base class setup
//    SelectorBase::SetupNewDirectory();
////    const std::string channel = "eee";
////    = new TH2D>(passingTight2D_, (std::string("passingTight2D_") + channel).c_str(), 
////        "Tight leptons; p_{T} [GeV]; Eta", 8, 10, 50, 5, -2.5, 2.5);
////    = new TH1D>(passingTight1DPt_, (std::string("passingTight1DPt_") + channel).c_str(), 
////        "Tight leptons; p_{T} [GeV]", 8, 10, 50);
////    = new TH1D>(passingTight1DEta_, (std::string("passingTight1DEta_") + channel).c_str(), 
////        "Tight leptons; #eta; Events", 5, -2.5, 2.5);
////    = new TH2D>(passingLoose2D_, (std::string("passingLoose2D_") + channel).c_str(), 
////        "Leptons passing tight failing loose; p_{T} [GeV]; Eta", 8, 10, 50, 5, -2.5, 2.5);
////    = new TH1D>(passingLoose1DPt_, (std::string("passingLoose1DPt_") + channel).c_str(), 
////        "Loose leptons; p_{T} [GeV]; Events", 8, 10, 50);
////    = new TH1D>(passingLoose1DEta_, (std::string("passingLoose1DEta_") + channel).c_str(), 
////        "Loose leptons; #eta; Events", 5, -2.5, 2.5);
//}
