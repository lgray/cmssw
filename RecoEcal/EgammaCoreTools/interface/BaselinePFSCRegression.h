#ifndef __BASELINEPFSCREGRESSION_H__
#define __BASELINEPFSCREGRESSION_H__

#include "RecoEcal/EgammaCoreTools/interface/SCRegressionCalculator.h"

#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"
#include "Geometry/CaloTopology/interface/CaloTopology.h" 
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

#include <vector>

class BaselinePFSCRegression {
 public:
  BaselinePFSCRegression() : topo_record(NULL) {};
  void update(const edm::EventSetup&);
  void set(const reco::SuperCluster&, std::vector<float>&) const;
  void setEvent(const edm::Event&);

 private:
  const CaloTopologyRecord* topo_record;
  edm::ESHandle<CaloTopology> calotopo;
  edm::Handle<reco::VertexCollection> vertices;
  edm::Handle<EcalRecHitCollection>  rechitsEB,rechitsEE;
};

typedef SCRegressionCalculator<BaselinePFSCRegression> PFSCRegressionCalc;

#endif
