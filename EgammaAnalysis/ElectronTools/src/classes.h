#include "DataFormats/Common/interface/Wrapper.h"

//Add includes for your classes here
#include "EgammaAnalysis/ElectronTools/interface/VersionedGsfElectronSelector.h"

namespace EgammaAnalysis_ElectronTools {
  struct dictionary {    
    //for using the selectors in python
    VersionedGsfElectronSelector vGsfElectronSelector;    
  };
}
