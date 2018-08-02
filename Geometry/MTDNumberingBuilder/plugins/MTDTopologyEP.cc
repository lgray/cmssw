#include "MTDTopologyEP.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "Geometry/Records/interface/PMTDParametersRcd.h"

MTDTopologyEP::MTDTopologyEP( const edm::ParameterSet& conf )
{
  edm::LogInfo("MTD") << "MTDTopologyEP::MTDTopologyEP";

  setWhatProduced(this);
}

MTDTopologyEP::~MTDTopologyEP()
{ 
}

void
MTDTopologyEP::fillDescriptions( edm::ConfigurationDescriptions & descriptions ) 
{
  edm::ParameterSetDescription ttc;
  descriptions.add( "mtdTopology", ttc );
}

MTDTopologyEP::ReturnType
MTDTopologyEP::produce( const MTDTopologyRcd& iRecord )
{
  edm::LogInfo("MTDTopologyEP") <<  "MTDTopologyEP::produce(const MTDTopologyRcd& iRecord)";
  edm::ESHandle<PMTDParameters> ptp;
  iRecord.getRecord<PMTDParametersRcd>().get( ptp );
  fillParameters( *ptp );
  
  ReturnType myTopo( new MTDTopology( btlVals_, etlVals_ ));

  return myTopo ;
}

void
MTDTopologyEP::fillParameters( const PMTDParameters& ptp )
{  
  btlVals_.sideStartBit_ = ptp.vitems[0].vpars[0]; // 16
  btlVals_.layerStartBit_ = ptp.vitems[0].vpars[1]; // 16
  btlVals_.trayStartBit_ = ptp.vitems[0].vpars[2]; // 8
  btlVals_.moduleStartBit_ = ptp.vitems[0].vpars[3]; // 2
  btlVals_.sideMask_ = ptp.vitems[0].vpars[4]; // 0xF
  btlVals_.layerMask_ = ptp.vitems[0].vpars[5]; // 0xF
  btlVals_.trayMask_ = ptp.vitems[0].vpars[6]; // 0xFF
  btlVals_.moduleMask_ = ptp.vitems[0].vpars[7]; // 0x3F
  
  etlVals_.sideStartBit_ = ptp.vitems[1].vpars[0];
  etlVals_.layerStartBit_ = ptp.vitems[1].vpars[1];
  etlVals_.ringStartBit_ = ptp.vitems[1].vpars[2];
  etlVals_.moduleStartBit_ = ptp.vitems[1].vpars[3];
  etlVals_.sideMask_ = ptp.vitems[1].vpars[4];
  etlVals_.layerMask_ = ptp.vitems[1].vpars[5];
  etlVals_.ringMask_ = ptp.vitems[1].vpars[6];
  etlVals_.moduleMask_ = ptp.vitems[1].vpars[7];   
}

DEFINE_FWK_EVENTSETUP_MODULE( MTDTopologyEP);

