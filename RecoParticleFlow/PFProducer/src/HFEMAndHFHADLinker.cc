#include "RecoParticleFlow/PFProducer/interface/BlockElementLinkerBase.h"
#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementCluster.h"
#include "RecoParticleFlow/PFClusterTools/interface/LinkByRecHit.h"

class HFEMAndHFHADLinker : public BlockElementLinkerBase {
public:
  HFEMAndHFHADLinker(const edm::ParameterSet& conf) :
    BlockElementLinkerBase(conf),
    _useKDTree(conf.getParameter<bool>("useKDTree")),
    _debug(conf.getUntrackedParameter<bool>("debug",false)) {}
  
  double operator() 
  ( const std::unique_ptr<reco::PFBlockElement>&,
    const std::unique_ptr<reco::PFBlockElement>& ) const override;

private:
  bool _useKDTree,_debug;
};

DEFINE_EDM_PLUGIN(BlockElementLinkerFactory, 
		  HFEMAndHFHADLinker, 
		  "HFEMAndHFHADLinker");

double HFEMAndHFHADLinker::operator()
  ( const std::unique_ptr<reco::PFBlockElement>& elem1,
    const std::unique_ptr<reco::PFBlockElement>& elem2) const {  
  const reco::PFBlockElementCluster *hfemelem(NULL), *hfhadelem(NULL);
  if( elem1->type() < elem2->type() ) {
    hfemelem = static_cast<const reco::PFBlockElementCluster*>(elem1.get());
    hfhadelem = static_cast<const reco::PFBlockElementCluster*>(elem2.get());
  } else {
    hfemelem = static_cast<const reco::PFBlockElementCluster*>(elem2.get());
    hfhadelem = static_cast<const reco::PFBlockElementCluster*>(elem1.get());
  }
  const reco::PFClusterRef& hfemref  = hfemelem->clusterRef();
  const reco::PFClusterRef& hfhadref = hfhadelem->clusterRef();
  if( hfemref.isNull() || hfhadref.isNull() ) {
    throw cms::Exception("BadClusterRefs") 
      << "PFBlockElementCluster's refs are null!";
  }    
  return LinkByRecHit::testHFEMAndHFHADByRecHit( *hfemref, *hfhadref, _debug );
}
