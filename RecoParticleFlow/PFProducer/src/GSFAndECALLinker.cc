#include "RecoParticleFlow/PFProducer/interface/BlockElementLinkerBase.h"
#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementGsfTrack.h"
#include "RecoParticleFlow/PFClusterTools/interface/LinkByRecHit.h"

class GSFAndECALLinker : public BlockElementLinkerBase {
public:
  GSFAndECALLinker(const edm::ParameterSet& conf) :
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
		  GSFAndECALLinker, 
		  "GSFAndECALLinker");

double GSFAndECALLinker::operator()
  ( const std::unique_ptr<reco::PFBlockElement>& elem1,
    const std::unique_ptr<reco::PFBlockElement>& elem2) const {  
  constexpr reco::PFTrajectoryPoint::LayerType ECALShowerMax =
    reco::PFTrajectoryPoint::ECALShowerMax;
  const reco::PFBlockElementCluster  *ecalelem(NULL);
  const reco::PFBlockElementGsfTrack *gsfelem(NULL);
  double dist(-1.0);
  if( elem1->type() < elem2->type() ) {
    ecalelem = static_cast<const reco::PFBlockElementCluster*>(elem1.get());
    gsfelem  = static_cast<const reco::PFBlockElementGsfTrack*>(elem2.get());
  } else {
    ecalelem = static_cast<const reco::PFBlockElementCluster*>(elem2.get());
    gsfelem  = static_cast<const reco::PFBlockElementGsfTrack*>(elem1.get());
  }
  const reco::PFRecTrack& track = gsfelem->GsftrackPF();
  const reco::PFClusterRef& clusterref = ecalelem->clusterRef();
  const reco::PFTrajectoryPoint& tkAtECAL =
    track.extrapolatedPoint( ECALShowerMax );
  if( tkAtECAL.isValid() ) {
    dist = LinkByRecHit::testTrackAndClusterByRecHit( track, *clusterref, 
						      false, _debug );
  }
  if ( _debug ) {
    if ( dist > 0. ) {
      std::cout << " Here a link has been established" 
		<< " between a GSF track an Ecal with dist  " 
		<< dist <<  std::endl;
    } else {
      if( _debug ) std::cout << " No link found " << std::endl;
    }
  }
  
  return dist;
}
