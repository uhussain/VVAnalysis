#define disambiguateFinalStates_cxx

#include "disambiguateFinalStates.h"
#include <iostream>

void disambiguateFinalStates::Begin(TTree * /*tree*/)
{
}

void disambiguateFinalStates::SlaveBegin(TTree * /*tree*/)
{
  fBestCandidateEntryList = new TEntryList("bestCandidates", "Entry List of disambiguated combinatoric candidates");
  fOutput->Add(fBestCandidateEntryList);
}

Bool_t disambiguateFinalStates::Process(Long64_t entry)
{
  b_Mass->GetEntry(entry);
  b_evt->GetEntry(entry);
  b_run->GetEntry(entry);

  if ( !fPostInit && !(run == fCurrentRun && evt == fCurrentEvt) )
  {
    Long64_t bestEntry = -1L;
    Float_t lowestDiscriminant = 1e10;
    for (size_t i=0; i<fEntriesToCompare.size(); ++i)
    {
      if ( lowestDiscriminant > fEntryDiscriminants[i] )
      {
        lowestDiscriminant = fEntryDiscriminants[i];
        bestEntry = fEntriesToCompare[i];
      }
    }

    if ( bestEntry > 0 )
      fBestCandidateEntryList->Enter(bestEntry);

    fEntriesToCompare.clear();
    fEntryDiscriminants.clear();
  }
  else if ( fPostInit )
  {
    fPostInit = false;
  }

  fCurrentRun = run;
  fCurrentEvt = evt;
  
  if ( fCutFormula && fCutFormula->EvalInstance() != 1. )
  {
    return kFALSE;
  }
  //std::cout << "The mass is " << Mass;
  Float_t discriminant = fabs(Mass-91.);
  fEntriesToCompare.push_back(entry);
  fEntryDiscriminants.push_back(discriminant);

  return kTRUE;
}

void disambiguateFinalStates::SlaveTerminate()
{
  fBestCandidateEntryList->OptimizeStorage();
  // Pointer is owned by fOutput, dereference
  fBestCandidateEntryList = nullptr;
}

void disambiguateFinalStates::Terminate()
{
}
