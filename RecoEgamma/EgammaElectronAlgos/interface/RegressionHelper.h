
#ifndef RegressionHelper_h
#define RegressionHelper_h

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "CondFormats/EgammaObjects/interface/GBRForest.h"


class RegressionHelper {
  public:
  struct Configuration
  {
      // weight files for the regression
    std::vector<std::string> ecalRegressionWeightFiles;
    std::vector<std::string> combinationRegressionWeightFiles;
    
  };
  
  RegressionHelper( const Configuration & ) ;
  void checkSetup( const edm::EventSetup & ) ;
  void readEvent( const edm::Event & ) ;
  ~RegressionHelper() ;
  
 private:
  
  const Configuration cfg_ ;
  const CaloTopology * caloTopology_;
  bool ecalRegressionInitialized_;
  bool combinationRegressionInitialized_;

  unsigned long long caloTopologyCacheId_;
  unsigned long long regressionCacheId_;

  edm::ESHandle<GBRForest> ecalRegBarrel_ ;
  edm::ESHandle<GBRForest> ecalRegEndcap_ ;
  edm::ESHandle<GBRForest> ecalRegErrorBarrel_ ;  
  edm::ESHandle<GBRForest> ecalRegErrorEndcap_ ;  
  edm::ESHandle<GBRForest> combinationReg_ ;
};

#endif



