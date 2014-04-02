#include "RecoParticleFlow/PFProducer/interface/BlockElementLinkerBase.h"
#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementTrack.h"
#include "RecoParticleFlow/PFClusterTools/interface/LinkByRecHit.h"

class TrackAndTrackLinker : public BlockElementLinkerBase {
public:
  TrackAndTrackLinker(const edm::ParameterSet& conf) :
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
		  TrackAndTrackLinker, 
		  "TrackAndTrackLinker");

double TrackAndTrackLinker::operator()
  ( const std::unique_ptr<reco::PFBlockElement>& elem1,
    const std::unique_ptr<reco::PFBlockElement>& elem2) const { 
  constexpr reco::PFBlockElement::TrackType T_TO_DISP = 
    reco::PFBlockElement::T_TO_DISP;
  constexpr reco::PFBlockElement::TrackType T_FROM_DISP = 
    reco::PFBlockElement::T_FROM_DISP;
  double dist = -1.0;
  
  const reco::PFDisplacedTrackerVertexRef& ni1_TO_DISP = 
    elem1->displacedVertexRef(T_TO_DISP);
  const reco::PFDisplacedTrackerVertexRef& ni2_TO_DISP = 
    elem2->displacedVertexRef(T_TO_DISP);
  const reco::PFDisplacedTrackerVertexRef& ni1_FROM_DISP = 
    elem1->displacedVertexRef(T_FROM_DISP);
  const reco::PFDisplacedTrackerVertexRef& ni2_FROM_DISP = 
    elem2->displacedVertexRef(T_FROM_DISP);

  if( ni1_TO_DISP.isNonnull() && ni2_FROM_DISP.isNonnull())
    if( ni1_TO_DISP == ni2_FROM_DISP ) { dist = 1.0; }

  if( ni1_FROM_DISP.isNonnull() && ni2_TO_DISP.isNonnull())
    if( ni1_FROM_DISP == ni2_TO_DISP ) { dist = 1.0; }

  if( ni1_FROM_DISP.isNonnull() && ni2_FROM_DISP.isNonnull())
    if( ni1_FROM_DISP == ni2_FROM_DISP ) { dist = 1.0; }

  if (  elem1->trackType(reco::PFBlockElement::T_FROM_GAMMACONV)  &&
	elem2->trackType(reco::PFBlockElement::T_FROM_GAMMACONV)  ) {    
    if ( elem1->convRef().isNonnull() && elem2->convRef().isNonnull() ) {
      if ( elem1->convRef() ==  elem2->convRef() ) {
	dist=1.0;
      }
    }     
  }
  
  if (  elem1->trackType(reco::PFBlockElement::T_FROM_V0)  &&
	elem2->trackType(reco::PFBlockElement::T_FROM_V0)  ) {
    if ( elem1->V0Ref().isNonnull() && elem2->V0Ref().isNonnull() ) {      
      if ( elem1->V0Ref() ==  elem2->V0Ref() ) {
	dist=1.0;
      }
    }
  }
  
  return dist;
}
