#ifndef ZZBackgroundSelector_h
#define ZZBackgroundSelector_h

#include <vector>
#include "Analysis/VVAnalysis/interface/ScaleFactor.h"
#include "Analysis/VVAnalysis/interface/ZZSelector.h"

class ZZBackgroundSelector : public ZZSelector {
public :
    TH2D* fakeRate_allE_;
    TH2D* fakeRate_allMu_;
  
    //PPPF represents 3P1F CR and PPFF represents 2P1F CR
    TH1D* Z1MassHistPPPF_;
    TH1D* Z1MassHistPPFF_;
    //4lMass Histogram
    TH1D* MassHistPPPF_;
    TH1D* MassHistPPFF_;
    
    ZZBackgroundSelector(TTree * /*tree*/ =0) { }
    virtual ~ZZBackgroundSelector() { }
    virtual Bool_t  Process(Long64_t entry) override;
    virtual void    SetupNewDirectory() override;
    virtual void    SlaveBegin(TTree *tree) override;

    ClassDefOverride(ZZBackgroundSelector,0);
private:
    float getl3FakeRate(Long64_t entry);
    float getl4FakeRate(Long64_t entry);
    //In ZZ we only care about 3P1F and 2P2F, contributions are extremely low from other regions like FFFF,PFFF etc
    bool IsPPPFRegion();
    bool IsPPFFRegion();
    float getEventWeight(Long64_t entry);
};

#endif
