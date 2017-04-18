#ifndef WZBackgroundSelector_h
#define WZBackgroundSelector_h

#include <vector>
#include "Analysis/WZAnalysis/interface/ScaleFactor.h"
#include "Analysis/WZAnalysis/interface/WZSelector.h"

class WZBackgroundSelector : public WZSelector {
public :
    ScaleFactor * fakeRate_allE_;
    ScaleFactor * fakeRate_allMu_;
    
    TH1D* zmassHistPPF_;
    TH1D* zmassHistPFP_;
    TH1D* zmassHistFPP_;
    TH1D* zmassHistFFP_;
    TH1D* zmassHistFPF_;
    TH1D* zmassHistPFF_;
    TH1D* zmassHistFFF_;
    
    WZBackgroundSelector(TTree * /*tree*/ =0) { }
    virtual ~WZBackgroundSelector() { }
    virtual Bool_t  Process(Long64_t entry) override;
    virtual void    SetupNewDirectory() override;
    virtual void    SlaveBegin(TTree *tree) override;

    ClassDefOverride(WZBackgroundSelector,0);
private:
    float getl1FakeRate();
    float getl2FakeRate();
    float getl3FakeRate();
    bool IsFPPRegion();
    bool IsPFPRegion();
    bool IsPPFRegion();
    bool IsFPFRegion();
    bool IsFFPRegion();
    bool IsPFFRegion();
    bool IsFFFRegion();
};

#endif
