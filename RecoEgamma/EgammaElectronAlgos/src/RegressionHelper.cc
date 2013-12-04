#include "Geometry/Records/interface/CaloTopologyRecord.h"
#include "CondFormats/DataRecord/interface/GBRWrapperRcd.h"

#include "RecoEgamma/EgammaElectronAlgos/interface/RegressionHelper.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterTools.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "TVector2.h"

RegressionHelper::RegressionHelper( const Configuration & config):
  cfg_(config),ecalRegressionInitialized_(false),combinationRegressionInitialized_(false),
  caloTopologyCacheId_(0), caloGeometryCacheId_(0),regressionCacheId_(0)
{;}

RegressionHelper::~RegressionHelper(){;}

void RegressionHelper::checkSetup(const edm::EventSetup & es) {

  // Topology 
  unsigned long long newCaloTopologyCacheId 
    = es.get<CaloTopologyRecord>().cacheIdentifier() ;
  if (!caloTopologyCacheId_ || ( caloTopologyCacheId_ != newCaloTopologyCacheId ) )
    {
      caloTopologyCacheId_ = newCaloTopologyCacheId;
      edm::ESHandle<CaloTopology> caloTopo;
      es.get<CaloTopologyRecord>().get(caloTopo);
      caloTopology_ = &(*caloTopo);
    }
    
  // Geometry
  unsigned long long newCaloGeometryCacheId
    = es.get<CaloGeometryRecord>().cacheIdentifier() ;
  if (!caloGeometryCacheId_ || ( caloGeometryCacheId_ != newCaloGeometryCacheId ) ) 
    {
      caloGeometryCacheId_ = newCaloGeometryCacheId;
      edm::ESHandle<CaloGeometry> caloGeom;
      es.get<CaloGeometryRecord>().get(caloGeom);
      caloGeometry_ = &(*caloGeom);
    }

  // Ecal regression 
  
  unsigned long long newRegressionCacheId
    = es.get<GBRWrapperRcd>().cacheIdentifier() ;
  if ( !regressionCacheId_ || (newRegressionCacheId != regressionCacheId_ ) ) {
    
    const GBRWrapperRcd& gbrRcd = es.get<GBRWrapperRcd>();

    // ECAL barrel
    gbrRcd.get(cfg_.ecalRegressionWeightFiles[0].c_str(),ecalRegBarrel_);

    // ECAL endcaps
    gbrRcd.get(cfg_.ecalRegressionWeightFiles[1].c_str(),ecalRegEndcap_);

    // ECAL barrel error
    gbrRcd.get(cfg_.ecalRegressionWeightFiles[2].c_str(),ecalRegErrorBarrel_);

    // ECAL endcap error
    gbrRcd.get(cfg_.ecalRegressionWeightFiles[3].c_str(),ecalRegErrorEndcap_);

    ecalRegressionInitialized_ = true;



    // Combination
    gbrRcd.get(cfg_.combinationRegressionWeightFiles[0].c_str(),combinationReg_);

    combinationRegressionInitialized_ = true;
  }

}

void RegressionHelper::readEvent( const edm::Event &) {;}

double RegressionHelper::getEcalRegression(const reco::SuperCluster & sc,
			 const edm::Handle<reco::VertexCollection>& vertices,
			 const edm::Handle<EcalRecHitCollection>& rechitsEB,
			 const edm::Handle<EcalRecHitCollection>& rechitsEE) const {
  std::vector<float> rInputs;
  rInputs.resize(33);
  
  const double rawEnergy = sc.rawEnergy();
  const double calibEnergy = sc.energy();
  const edm::Ptr<reco::CaloCluster> seed(sc.seed());
  const size_t nVtx = vertices->size();
  float maxDR=999., maxDRDPhi=999., maxDRDEta=999., maxDRRawEnergy=0.;
  float subClusRawE[3], subClusDPhi[3], subClusDEta[3];
  memset(subClusRawE,0,3*sizeof(float));
  memset(subClusDPhi,0,3*sizeof(float));
  memset(subClusDEta,0,3*sizeof(float));
  size_t iclus=0;
  for( auto clus = sc.clustersBegin()+1; clus != sc.clustersEnd(); ++clus ) {
    const float this_dr = reco::deltaR(**clus, *seed);
    if(this_dr > maxDR || maxDR == 999.0f) {
      maxDR = this_dr;
      maxDRDEta = (*clus)->eta() - seed->eta();
      maxDRDPhi = TVector2::Phi_mpi_pi((*clus)->phi() - seed->phi());
      maxDRRawEnergy = (*clus)->energy();
    }
    if( iclus++ < 3 ) {
      subClusRawE[iclus] = (*clus)->energy();
      subClusDEta[iclus] = (*clus)->eta() - seed->eta();
      subClusDPhi[iclus] = TVector2::Phi_mpi_pi((*clus)->phi() - seed->phi());
    }
  }
  float scPreshowerSum = 0.0;
  for( auto psclus = sc.preshowerClustersBegin();
       psclus != sc.preshowerClustersEnd(); ++psclus ) {
    scPreshowerSum += (*psclus)->energy();
  }

  if ( seed->seed().subdetId()==EcalBarrel ) {
    const float eMax = EcalClusterTools::eMax( *seed, &*rechitsEB );
    const float e2nd = EcalClusterTools::e2nd( *seed, &*rechitsEB );
    const float e3x3 = EcalClusterTools::e3x3( *seed,
					       &*rechitsEB,
					       &*caloTopology_ );
    const float eTop = EcalClusterTools::eTop( *seed,
					       &*rechitsEB,
					       &*caloTopology_ );
    const float eBottom = EcalClusterTools::eBottom( *seed,
						     &*rechitsEB,
						     &*caloTopology_ );
    const float eLeft = EcalClusterTools::eLeft( *seed,
                                                 &*rechitsEB,
                                                 &*caloTopology_ );
    const float eRight = EcalClusterTools::eRight( *seed,
						   &*rechitsEB,
						   &*caloTopology_ );
    const float eLpeR = eLeft + eRight;
    const float eTpeB = eTop + eBottom;
    const float eLmeR = eLeft - eRight;
    const float eTmeB = eTop - eBottom;
    std::vector<float> vCov =
      EcalClusterTools::localCovariances( *seed, &*rechitsEB, &*caloTopology_ );
    const float see = (isnan(vCov[0]) ? 0. : sqrt(vCov[0]));
    const float spp = (isnan(vCov[2]) ? 0. : sqrt(vCov[2]));
    float sep = 0.;
    if (see*spp > 0)
      sep = vCov[1] / (see * spp);
    else if (vCov[1] > 0)
      sep = 1.0;
    else
      sep = -1.0;
    float cryPhi, cryEta, thetatilt, phitilt;
    int ieta, iphi;
    ecl_.localCoordsEB(*seed, *caloGeometry_, cryEta, cryPhi,
		       ieta, iphi, thetatilt, phitilt);
    rInputs[0] = nVtx; //nVtx
    rInputs[1] = sc.eta(); //scEta
    rInputs[2] = sc.phi(); //scPhi
    rInputs[3] = sc.etaWidth(); //scEtaWidth
    rInputs[4] = sc.phiWidth(); //scPhiWidth
    rInputs[5] = e3x3/rawEnergy; //scSeedR9
    rInputs[6] = sc.seed()->energy()/rawEnergy; //scSeedRawEnergy/scRawEnergy
    rInputs[7] = eMax/rawEnergy; //scSeedEmax/scRawEnergy
    rInputs[8] = e2nd/rawEnergy; //scSeedE2nd/scRawEnergy
    rInputs[9] = (eLpeR!=0. ? eLmeR/eLpeR : 0.);//scSeedLeftRightAsym
    rInputs[10] = (eTpeB!=0.? eTmeB/eTpeB : 0.);//scSeedTopBottomAsym
    rInputs[11] = see; //scSeedSigmaIetaIeta
    rInputs[12] = sep; //scSeedSigmaIetaIphi
    rInputs[13] = spp; //scSeedSigmaIphiIphi
    rInputs[14] = sc.clustersSize()-1; //N_ECALClusters
    rInputs[15] = maxDR; //clusterMaxDR
    rInputs[16] = maxDRDPhi; //clusterMaxDRDPhi
    rInputs[17] = maxDRDEta; //clusterMaxDRDEta
    rInputs[18] = maxDRRawEnergy/rawEnergy; //clusMaxDRRawEnergy/scRawEnergy
    rInputs[19] = subClusRawE[0]/rawEnergy; //clusterRawEnergy[0]/scRawEnergy
    rInputs[20] = subClusRawE[1]/rawEnergy; //clusterRawEnergy[1]/scRawEnergy
    rInputs[21] = subClusRawE[2]/rawEnergy; //clusterRawEnergy[2]/scRawEnergy
    rInputs[22] = subClusDPhi[0]; //clusterDPhiToSeed[0]
    rInputs[23] = subClusDPhi[1]; //clusterDPhiToSeed[1]
    rInputs[24] = subClusDPhi[2]; //clusterDPhiToSeed[2]
    rInputs[25] = subClusDEta[0]; //clusterDEtaToSeed[0]
    rInputs[26] = subClusDEta[1]; //clusterDEtaToSeed[1]
    rInputs[27] = subClusDEta[2]; //clusterDEtaToSeed[2]
    rInputs[28] = cryEta; //scSeedCryEta
    rInputs[29] = cryPhi; //scSeedCryPhi
    rInputs[30] = ieta; //scSeedCryIeta
    rInputs[31] = iphi; //scSeedCryIphi
    rInputs[32] = calibEnergy; //scCalibratedEnergy
    return ecalRegBarrel_->GetResponse(&rInputs[0]);
  }
  else if(seed->seed().subdetId()==EcalEndcap)
    {
      const float eMax = EcalClusterTools::eMax( *seed, &*rechitsEE );
      const float e2nd = EcalClusterTools::e2nd( *seed, &*rechitsEE );
      const float e3x3 = EcalClusterTools::e3x3( *seed,
                                                 &*rechitsEE,
                                                 &*caloTopology_ );
      const float eTop = EcalClusterTools::eTop( *seed,
                                                 &*rechitsEE,
                                                 &*caloTopology_ );
      const float eBottom = EcalClusterTools::eBottom( *seed,
						       &*rechitsEE,
						       &*caloTopology_ );
      const float eLeft = EcalClusterTools::eLeft( *seed,
						   &*rechitsEE,
						   &*caloTopology_ );
      const float eRight = EcalClusterTools::eRight( *seed,
						     &*rechitsEE,
						     &*caloTopology_ );
      const float eLpeR = eLeft + eRight;
      const float eTpeB = eTop + eBottom;
      const float eLmeR = eLeft - eRight;
      const float eTmeB = eTop - eBottom;
      std::vector<float> vCov =
        EcalClusterTools::localCovariances( *seed, &*rechitsEE, &*caloTopology_ );
      const float see = (isnan(vCov[0]) ? 0. : sqrt(vCov[0]));
      const float spp = (isnan(vCov[2]) ? 0. : sqrt(vCov[2]));
      float sep = 0.;
      if (see*spp > 0)
        sep = vCov[1] / (see * spp);
      else if (vCov[1] > 0)
        sep = 1.0;
      else
        sep = -1.0;
      rInputs[0] = nVtx; //nVtx
      rInputs[1] = sc.eta(); //scEta
      rInputs[2] = sc.phi(); //scPhi
      rInputs[3] = sc.etaWidth(); //scEtaWidth
      rInputs[4] = sc.phiWidth(); //scPhiWidth
      rInputs[5] = e3x3/rawEnergy; //scSeedR9
      rInputs[6] = sc.seed()->energy()/rawEnergy; //scSeedRawEnergy/scRawEnergy
      rInputs[7] = eMax/rawEnergy; //scSeedEmax/scRawEnergy
      rInputs[8] = e2nd/rawEnergy; //scSeedE2nd/scRawEnergy
      rInputs[9] = (eLpeR!=0. ? eLmeR/eLpeR : 0.);//scSeedLeftRightAsym
      rInputs[10] = (eTpeB!=0.? eTmeB/eTpeB : 0.);//scSeedTopBottomAsym
      rInputs[11] = see; //scSeedSigmaIetaIeta
      rInputs[12] = sep; //scSeedSigmaIetaIphi
      rInputs[13] = spp; //scSeedSigmaIphiIphi
      rInputs[14] = sc.clustersSize()-1; //N_ECALClusters
      rInputs[15] = maxDR; //clusterMaxDR
      rInputs[16] = maxDRDPhi; //clusterMaxDRDPhi
      rInputs[17] = maxDRDEta; //clusterMaxDRDEta
      rInputs[18] = maxDRRawEnergy/rawEnergy; //clusMaxDRRawEnergy/scRawEnergy
      rInputs[19] = subClusRawE[0]/rawEnergy; //clusterRawEnergy[0]/scRawEnergy
      rInputs[20] = subClusRawE[1]/rawEnergy; //clusterRawEnergy[1]/scRawEnergy
      rInputs[21] = subClusRawE[2]/rawEnergy; //clusterRawEnergy[2]/scRawEnergy
      rInputs[22] = subClusDPhi[0]; //clusterDPhiToSeed[0]
      rInputs[23] = subClusDPhi[1]; //clusterDPhiToSeed[1]
      rInputs[24] = subClusDPhi[2]; //clusterDPhiToSeed[2]
      rInputs[25] = subClusDEta[0]; //clusterDEtaToSeed[0]
      rInputs[26] = subClusDEta[1]; //clusterDEtaToSeed[1]
      rInputs[27] = subClusDEta[2]; //clusterDEtaToSeed[2]
      rInputs[28] = scPreshowerSum/rawEnergy; //scPreshowerEnergy/scRawEnergy
      rInputs[29] = calibEnergy; //scCalibratedEnergy
      return ecalRegEndcap_->GetResponse(&rInputs[0]);
    }
  else
    {
      throw cms::Exception("RegressionHelper::calculateRegressedEnergy")
	<< "Supercluster seed is either EB nor EE!" << std::endl;
    }
  return -1.;
}
