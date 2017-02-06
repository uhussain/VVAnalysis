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
// To use this file, try the following seFakeRateSelector.C")
// root> T->Process("FakeRateSelector.C","some options")
// root> T->Process("FakeRateSelector.C+")
//


#include "FakeRateSelector.h"
#include <TStyle.h>

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
    if (! SelectorBase::Process(entry)) return false;
    fMassHist->Fill(*Mass);
    return kTRUE;
}

void FakeRateSelector::SetupNewDirectory()
{
    // Must call base class setup
    SelectorBase::SetupNewDirectory();
    AddObject<TH1D>(fMassHist, "Mass", "Mass [GeV]; Events / 20 GeV ;Counts", 25, 0, 400);
}
