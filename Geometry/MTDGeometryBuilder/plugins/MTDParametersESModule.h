#ifndef Geometry_MTDGeometryBuilder_MTDParametersESModule_H
#define Geometry_MTDGeometryBuilder_MTDParametersESModule_H

#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include <memory>

namespace edm {
  class ConfigurationDescriptions;
}
class PMTDParameters;
class PMTDParametersRcd;

class  MTDParametersESModule: public edm::ESProducer
{
 public:
  MTDParametersESModule( const edm::ParameterSet & );
  ~MTDParametersESModule( void ) override;

  typedef std::shared_ptr<PMTDParameters> ReturnType;

  static void fillDescriptions( edm::ConfigurationDescriptions & );
  
  ReturnType produce( const PMTDParametersRcd & );
};

#endif
