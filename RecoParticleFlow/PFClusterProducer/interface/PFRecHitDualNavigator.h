#ifndef RecoParticleFlow_PFClusterProducer_PFRecHitDualNavigator_h
#define RecoParticleFlow_PFClusterProducer_PFRecHitDualNavigator_h


#include "RecoParticleFlow/PFClusterProducer/interface/PFRecHitNavigatorBase.h"
#include "DataFormats/ParticleFlowReco/interface/PFLayer.h"



template <PFLayer::Layer D1, typename barrel,PFLayer::Layer D2, typename endcap>
class PFRecHitDualNavigator : public PFRecHitNavigatorBase {
 public:
  PFRecHitDualNavigator() {
  }



  PFRecHitDualNavigator(const edm::ParameterSet& iConfig){
    barrelNav_ = new barrel(iConfig);
    endcapNav_ = new endcap(iConfig);
  }

  void beginEvent(const edm::EventSetup& iSetup) {
    barrelNav_->beginEvent(iSetup); 
    endcapNav_->beginEvent(iSetup);

  }

  void associateNeighbours(reco::PFRecHit& hit,std::auto_ptr<reco::PFRecHitCollection>& hits) {
      if (hit.layer() ==  D1)
	barrelNav_->associateNeighbours(hit,hits);
      else if (hit.layer() ==  D2)
	endcapNav_->associateNeighbours(hit,hits);
  }

 protected:
      barrel *barrelNav_;
      endcap *endcapNav_;


};

#include "RecoParticleFlow/PFClusterProducer/interface/PFRecHitCaloNavigator.h"

typedef  PFRecHitDualNavigator<PFLayer::ECAL_BARREL,PFRecHitEcalBarrelNavigator,PFLayer::ECAL_ENDCAP,PFRecHitEcalEndcapNavigator> PFRecHitEcalNavigator;



#endif


