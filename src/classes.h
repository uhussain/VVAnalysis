#include "Analysis/VVAnalysis/interface/FakeRateSelector.h"
#include "Analysis/VVAnalysis/interface/WZSelector.h"
#include "Analysis/VVAnalysis/interface/ZZSelector.h"
#include "Analysis/VVAnalysis/interface/WZSelectorBase.h"
#include "Analysis/VVAnalysis/interface/ZZSelectorBase.h"
#include "Analysis/VVAnalysis/interface/WZBackgroundSelector.h"
#include "Analysis/VVAnalysis/interface/ZZBackgroundSelector.h"
#include "Analysis/VVAnalysis/interface/ScaleFactor.h"
#include "Analysis/VVAnalysis/interface/disambiguateFinalStates.h"

namespace{
  namespace{
    FakeRateSelector pFakeRateSelector;
    WZSelectorBase pWZSelectorBase;
    ZZSelectorBase pZZSelectorBase;
    WZSelector pWZSelector;
    ZZSelector pZZSelector;
    WZBackgroundSelector pWZBackgroundSelector;
    ZZBackgroundSelector pZZBackgroundSelector;
    ScaleFactor pScaleFactor;
    disambiguateFinalStates pDisambiguator;
  }
}
