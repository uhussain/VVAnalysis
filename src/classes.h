#include "Analysis/VVAnalysis/interface/FakeRateSelector.h"
#include "Analysis/VVAnalysis/interface/ZZSelector.h"
#include "Analysis/VVAnalysis/interface/ZZSelectorBase.h"
#include "Analysis/VVAnalysis/interface/ZZBackgroundSelector.h"
#include "Analysis/VVAnalysis/interface/ScaleFactor.h"
#include "Analysis/VVAnalysis/interface/disambiguateFinalStates.h"

namespace{
  namespace{
    FakeRateSelector pFakeRateSelector;
    ZZSelectorBase pZZSelectorBase;
    ZZSelector pZZSelector;
    ZZBackgroundSelector pZZBackgroundSelector;
    ScaleFactor pScaleFactor;
    disambiguateFinalStates pDisambiguator;
  }
}
