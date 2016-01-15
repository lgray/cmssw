#include "SimDataFormats/CaloAnalysis/interface/CaloParticleFwd.h"
#include "SimDataFormats/CaloAnalysis/interface/CaloParticle.h"

namespace SimDataFormats {
  namespace CaloAnalysis {
    CaloParticle cp;
    CaloParticleCollection vcp;
    edm::Wrapper<CaloParticleCollection> wvcp;

    CaloParticleRef cpr;
    CaloParticleRefVector cprv;
    CaloParticleRefProd cprp;
    CaloParticleContainer cpc;    
  }
}
