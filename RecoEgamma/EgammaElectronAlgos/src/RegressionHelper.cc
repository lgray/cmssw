#include "RecoEgamma/EgammaElectronAlgos/interface/RegressionHelper.h"
#include "Geometry/Records/interface/CaloTopologyRecord.h"
#include "CondFormats/DataRecord/interface/GBRWrapperRcd.h"

RegressionHelper::RegressionHelper( const Configuration & config):
  cfg_(config),ecalRegressionInitialized_(false),combinationRegressionInitialized_(false),
  caloTopologyCacheId_(0), regressionCacheId_(0)
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

void readEvent( const edm::Event &) {;}
