#include "Analysis/WZAnalysis/interface/FakeRateSelector.h"
#include "Analysis/WZAnalysis/interface/WZSelector.h"
#include "Analysis/WZAnalysis/interface/WZSelectorBase.h"
#include "Analysis/WZAnalysis/interface/WZBackgroundSelector.h"
#include "Analysis/WZAnalysis/interface/ScaleFactor.h"
namespace{
  namespace{
    FakeRateSelector pFakeRateSelector;
    WZSelectorBase pWZSelectorBase;
    WZSelector pWZSelector;
    WZBackgroundSelector pWZBackgroundSelector;
    ScaleFactor pScaleFactor;
  }
}
