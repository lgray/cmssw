#ifndef RecoEcal_EgammaClusterProducers_PFECALSuperClusterProducer_h_
#define RecoEcal_EgammaClusterProducers_PFECALSuperClusterProducer_h_

// system include files
#include <memory>
#include <vector>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/ParticleFlowReco/interface/PFRecHitFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PFClusterFwd.h"

#include "RecoEcal/EgammaClusterAlgos/interface/PFECALSuperClusterAlgo.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"
#include "Geometry/CaloTopology/interface/CaloTopology.h" 
#include "RecoEgamma/EgammaTools/interface/EcalClusterLocal.h"

#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"

/**\class PFECALSuperClusterProducer 

\author Nicolas Chanon
Additional authors for Mustache: Y. Gershtein, R. Patel, L. Gray
\date   July 2012
*/

class CaloSubdetectorTopology;
class CaloSubdetectorGeometry;
class DetId;
class GBRForest;
class GBRWrapperRcd;
class EcalClusterTools;


class PFECALSuperClusterProducer : public edm::stream::EDProducer<> {
 public:  
  explicit PFECALSuperClusterProducer(const edm::ParameterSet&);
  ~PFECALSuperClusterProducer();

  virtual void beginRun(const edm::Run& iR, const edm::EventSetup& iE);
  virtual void produce(edm::Event&, const edm::EventSetup&);
  

 private:  
  // ----------member data ---------------------------

  /// clustering algorithm 
  PFECALSuperClusterAlgo                  superClusterAlgo_;
  PFECALSuperClusterAlgo::clustering_type _theclusteringtype;
  PFECALSuperClusterAlgo::energy_weight   _theenergyweight;

  std::shared_ptr<PFEnergyCalibration> thePFEnergyCalibration_;

  /// verbose ?
  bool   verbose_;
  // regression
  bool use_regression;
  float rinputs[33];
  EcalClusterLocal ecl_;
  std::string eb_reg_key, ee_reg_key;
  const GBRWrapperRcd* gbr_record;
  edm::ESHandle<GBRForest> eb_reg, ee_reg;
  const CaloTopologyRecord* topo_record;
  edm::ESHandle<CaloTopology> calotopo;
  double getRegressionCorrection(const reco::SuperCluster&, 
				 const edm::Handle<reco::VertexCollection>&,
				 const edm::Handle<EcalRecHitCollection>&,
				 const edm::Handle<EcalRecHitCollection>&,
				 const edm::EventSetup& );
  
  edm::EDGetTokenT<edm::View<reco::PFCluster> >   inputTagPFClusters_;
  edm::EDGetTokenT<reco::PFCluster::EEtoPSAssociation>   inputTagPFClustersES_;
  edm::EDGetTokenT<EcalRecHitCollection>          inputTagEBRecHits_;
  edm::EDGetTokenT<EcalRecHitCollection>          inputTagEERecHits_;
  edm::EDGetTokenT<reco::VertexCollection>        inputTagVertices_;

  std::string PFBasicClusterCollectionBarrel_;
  std::string PFSuperClusterCollectionBarrel_;
  std::string PFBasicClusterCollectionEndcap_;
  std::string PFSuperClusterCollectionEndcap_;
  std::string PFBasicClusterCollectionPreshower_;
  std::string PFSuperClusterCollectionEndcapWithPreshower_;

};

#endif
