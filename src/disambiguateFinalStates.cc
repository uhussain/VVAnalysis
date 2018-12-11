/*
 * TSelector deduplicate events in combinatorical ntuple
 * by choosing event with l1_l2_Mass pairing closest to
 * M_PDG(Z) = 91.1876 
 * with the highest scalar Pt sum of the remaining leptons
 * used as a tiebreaker. 
 *
 * Modified from K. Long, U. Wisconsin
 *
 * https://https://github.com/kdlong/VVAnalysis/blob/master/src/disambiguateFinalStates.cc
 */
#include "Analysis/VVAnalysis/interface/disambiguateFinalStates.h"

void disambiguateFinalStates::Init(TTree *tree)
{
  if (!tree) return;
  fChain = tree;

  fChain->SetBranchAddress(l1_l2_Cand_mass, &l1_l2_Mass, &b_l1_l2_Mass);
  fChain->SetBranchAddress(l1_Cand_pt, &l1_Pt, &b_l1_Pt);
  fChain->SetBranchAddress(l2_Cand_pt, &l2_Pt, &b_l2_Pt);
  fChain->SetBranchAddress(l3_l4_Cand_mass, &l3_l4_Mass, &b_l3_l4_Mass);
  fChain->SetBranchAddress(l3_Cand_pt, &l3_Pt, &b_l3_Pt); 
  fChain->SetBranchAddress(l4_Cand_pt, &l4_Pt, &b_l4_Pt);
  fChain->SetBranchAddress(l1_Cand_Tight, &l1IsTight, &b_l1IsTight);
  fChain->SetBranchAddress(l1_Cand_Iso, &l1IsIso, &b_l1IsIso);
  fChain->SetBranchAddress(l2_Cand_Tight, &l2IsTight, &b_l2IsTight);
  fChain->SetBranchAddress(l2_Cand_Iso, &l2IsIso, &b_l2IsIso);
  fChain->SetBranchAddress(l3_Cand_Tight, &l3IsTight, &b_l3IsTight);
  fChain->SetBranchAddress(l3_Cand_Iso, &l3IsIso, &b_l3IsIso);
  fChain->SetBranchAddress(l4_Cand_Tight, &l4IsTight, &b_l4IsTight);
  fChain->SetBranchAddress(l4_Cand_Iso, &l4IsIso, &b_l4IsIso);
  fChain->SetBranchAddress("evt", &evt, &b_evt);
  fChain->SetBranchAddress("run", &run, &b_run);

  SafeDelete(fCutFormula);
  fCutFormula = new TTreeFormula("CutFormula", fOption, fChain);
  fCutFormula->SetQuickLoad(kTRUE);
  if (!fCutFormula->GetNdim()) { delete fCutFormula; fCutFormula = 0; }
}

Bool_t disambiguateFinalStates::Notify()
{
  return kTRUE;
}

void disambiguateFinalStates::Begin(TTree * /*tree*/)
{
}

void disambiguateFinalStates::SlaveBegin(TTree * /*tree*/)
{
  fBestCandidateEntryList = new TEntryList("bestCandidates", "Entry List of disambiguated combinatoric candidates after leptons pass tight ID");
  fOutput->Add(fBestCandidateEntryList);
}

Bool_t disambiguateFinalStates::Process(Long64_t entry)
{
  b_evt->GetEntry(entry);
  b_run->GetEntry(entry);

  if ( !(run == fCurrentRun && evt == fCurrentEvt) )
  {
    findBestEntry();
  }

  fCurrentRun = run;
  fCurrentEvt = evt;
  // TODO Understand why this gives segfault for chains
  // with multiple entries
  if ( fCutFormula && fCutFormula->EvalInstance() > 0. )
    {
    b_l1_l2_Mass->GetEntry(entry);
    b_l1_Pt->GetEntry(entry);
    b_l2_Pt->GetEntry(entry);
    b_l3_l4_Mass->GetEntry(entry);
    b_l3_Pt->GetEntry(entry); 
    b_l4_Pt->GetEntry(entry);
    b_l1IsTight->GetEntry(entry);
    b_l1IsIso->GetEntry(entry);
    b_l2IsTight->GetEntry(entry);
    b_l2IsIso->GetEntry(entry);
    b_l3IsTight->GetEntry(entry);
    b_l3IsIso->GetEntry(entry);
    b_l4IsTight->GetEntry(entry);
    b_l4IsIso->GetEntry(entry); 
    
    float mass_discriminant,Z2ptSum;

    //This condition identifies the Z1 candidate
    //Required for the 2e2mu state but redundant for the 4e,4mu state however it should be quick comparison
    if(fabs(l1_l2_Mass-91.1876) < fabs(l3_l4_Mass-91.1876)){
    //if(l1l2IsZ1(entry)){
      mass_discriminant = fabs(l1_l2_Mass-91.1876);
      Z2ptSum = l3_Pt+l4_Pt;}
    else{ 
      mass_discriminant = fabs(l3_l4_Mass-91.1876);
      Z2ptSum = l1_Pt+l2_Pt;
      //bool templ1IsTight = l1IsTight;
      //l1IsTight = l3IsTight;
      //l3IsTight = templ1IsTight;
      //bool templ2IsTight = l2IsTight;
      //l2IsTight = l4IsTight;
      //l4IsTight = templ2IsTight;
      //bool templ1IsIso = l1IsIso;
      //l1IsIso = l3IsIso;
      //l3IsIso = templ1IsIso;
      //bool templ2IsIso = l2IsIso;
      //l2IsIso = l4IsIso;
      //l4IsIso = templ2IsIso;
    }  
   
    if(tightZ1Leptons() && tightZ2Leptons()){ 
      fEntriesToCompareTight.push_back(entry);
      fEntryDiscriminantsTight.push_back(mass_discriminant);
      fEntryZ2PtSumTight.push_back(Z2ptSum);
    }
    //
    //else if(tightZ1Leptons()){
    else{
      fEntriesToCompareLoose.push_back(entry);
      fEntryDiscriminantsLoose.push_back(mass_discriminant);
      fEntryZ2PtSumLoose.push_back(Z2ptSum);
    }
    //else{ 
    //  fEntriesToCompareVLoose.push_back(entry);
    //  fEntryDiscriminantsVLoose.push_back(mass_discriminant);
    //  fEntryZ2PtSumVLoose.push_back(Z2ptSum);
    //}
  }

  if ( entry == fChain->GetEntries()-1 ) {
    findBestEntry();
  }

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

void disambiguateFinalStates::findBestEntry()
{
  //The correct row is the one with Z1 closest
  //to on-shell, with the highest scalar Pt sum of the remaining leptons
  // used as a tiebreaker. 
  Long64_t bestEntryTight = -1L;
  float lowestDiscriminantTight = 1e100;
  float MaxPtSumTight = 0.0;

  Long64_t bestEntryLoose = -1L;
  float lowestDiscriminantLoose = 1e100;
  float MaxPtSumLoose = 0.0;
  
  //Long64_t bestEntryVLoose = -1L;
  //float lowestDiscriminantVLoose = 1e100;
  //float MaxPtSumVLoose = 0.0;

  for (size_t i=0; i<fEntriesToCompareTight.size(); ++i)
  {
    if ((fEntryDiscriminantsTight[i] < lowestDiscriminantTight) || ((fEntryDiscriminantsTight[i] == lowestDiscriminantTight) && (fEntryZ2PtSumTight[i] > MaxPtSumTight)))
    {
      MaxPtSumTight = fEntryZ2PtSumTight[i];
      lowestDiscriminantTight = fEntryDiscriminantsTight[i];
      bestEntryTight = fEntriesToCompareTight[i];
    }
  }
 
  for (size_t i=0; i<fEntriesToCompareLoose.size(); ++i)
  {
    if ((fEntryDiscriminantsLoose[i] < lowestDiscriminantLoose) || ((fEntryDiscriminantsLoose[i] == lowestDiscriminantLoose) && (fEntryZ2PtSumLoose[i] > MaxPtSumLoose)))
    {
      MaxPtSumLoose = fEntryZ2PtSumLoose[i];
      lowestDiscriminantLoose = fEntryDiscriminantsLoose[i];
      bestEntryLoose = fEntriesToCompareLoose[i];
    }
  }

 // for (size_t i=0; i<fEntriesToCompareVLoose.size(); ++i)
 // {
 //   if ((fEntryDiscriminantsVLoose[i] < lowestDiscriminantVLoose) || ((fEntryDiscriminantsVLoose[i] == lowestDiscriminantVLoose) && (fEntryZ2PtSumVLoose[i] > MaxPtSumVLoose)))
 //   {
 //     MaxPtSumVLoose = fEntryZ2PtSumVLoose[i];
 //     lowestDiscriminantVLoose = fEntryDiscriminantsVLoose[i];
 //     bestEntryVLoose = fEntriesToCompareVLoose[i];
 //   }
 // }
    if ( bestEntryTight >= 0 )
    {
      fBestCandidateEntryList->Enter(bestEntryTight);
    }
    else if ( bestEntryLoose >=0 ){ 
      fBestCandidateEntryList->Enter(bestEntryLoose);
    }
    //else if ( bestEntryVLoose >=0 ){ 
    //  fBestCandidateEntryList->Enter(bestEntryVLoose);
    //}

    fEntriesToCompareTight.clear();
    fEntryDiscriminantsTight.clear();
    fEntryZ2PtSumTight.clear();

    fEntriesToCompareLoose.clear();
    fEntryDiscriminantsLoose.clear();
    fEntryZ2PtSumLoose.clear();

    //fEntriesToCompareVLoose.clear();
    //fEntryDiscriminantsVLoose.clear();
    //fEntryZ2PtSumVLoose.clear();
}

bool disambiguateFinalStates::l1l2IsZ1(Long64_t entry){
  if(fabs(l1_l2_Mass-91.1876) < fabs(l3_l4_Mass-91.1876))
    return true;
  else
    return false;
}
bool disambiguateFinalStates::lep1IsTight() {
    return (l1IsTight && l1IsIso); 
}
bool disambiguateFinalStates::lep2IsTight() {
    return (l2IsTight && l2IsIso); 
}
bool disambiguateFinalStates::tightZ1Leptons() {
    return lep1IsTight() && lep2IsTight(); 
}
bool disambiguateFinalStates::lep3IsTight() {
    return (l3IsTight && l3IsIso);
}
bool disambiguateFinalStates::lep4IsTight() {
    return (l4IsTight && l4IsIso);
}
bool disambiguateFinalStates::tightZ2Leptons() {
    return lep3IsTight() && lep4IsTight(); 
}
