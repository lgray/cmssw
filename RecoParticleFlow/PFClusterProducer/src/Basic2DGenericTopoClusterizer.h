#ifndef __Basic2DGenericTopoClusterizer_H__
#define __Basic2DGenericTopoClusterizer_H__

#include "RecoParticleFlow/PFClusterProducer/interface/TopoClusterBuilderBase.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecHitFraction.h"

class Basic2DGenericTopoClusterizer : public TopoClusterBuilderBase {
  typedef Basic2DGenericTopoClusterizer B2DGT;
 public:
  Basic2DGenericTopoClusterizer(const edm::ParameterSet& conf) :
    TopoClusterBuilderBase(conf),
    _useCornerCells(conf.getParameter<bool>("useCornerCells")) { }
  virtual ~Basic2DGenericTopoClusterizer() {}
  Basic2DGenericTopoClusterizer(const B2DGT&) = delete;
  B2DGT& operator=(const B2DGT&) = delete;

  void buildTopoClusters(const edm::Handle<reco::PFRecHitCollection>&,
			 const std::vector<bool>&,
			 const std::vector<bool>&, 
			 reco::PFClusterCollection&);
  
 private:  
  const bool _useCornerCells;
  void buildTopoCluster(const edm::Handle<reco::PFRecHitCollection>&,
			const std::vector<bool>&, // masked rechits
			const reco::PFRecHitRef&, //present rechit
			std::vector<bool>&, // hit usage state
			reco::PFCluster&); // the topocluster
  
};

#include "RecoParticleFlow/PFClusterProducer/interface/TopoClusterBuilderFactory.h"
DEFINE_EDM_PLUGIN(TopoClusterBuilderFactory,
		  Basic2DGenericTopoClusterizer,
		  "Basic2DGenericTopoClusterizer");

#endif
