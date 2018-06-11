/** \file
 *
 *  \author L. Gray - FNAL
 *
 */

#include <RecoMTD/DetLayers/plugins/MTDDetLayerGeometryESProducer.h>
#include <Geometry/Records/interface/MTDGeometryRecord.h>

#include <Geometry/HGCalGeometry/interface/FastTimeGeometry.h>

#include <RecoMTD/DetLayers/src/ETLDetLayerGeometryBuilder.h>
#include <RecoMTD/DetLayers/src/BTLDetLayerGeometryBuilder.h>

#include <FWCore/Framework/interface/EventSetup.h>
#include <FWCore/Framework/interface/ESHandle.h>

#include <FWCore/MessageLogger/interface/MessageLogger.h>
#include <FWCore/Framework/interface/NoProxyException.h>

#include <memory>

using namespace edm;

MTDDetLayerGeometryESProducer::MTDDetLayerGeometryESProducer(const edm::ParameterSet & p){
  setWhatProduced(this);
}


MTDDetLayerGeometryESProducer::~MTDDetLayerGeometryESProducer(){}


std::unique_ptr<MTDDetLayerGeometry>
MTDDetLayerGeometryESProducer::produce(const MuonRecoGeometryRecord & record) {

  const std::string metname = "MTD|RecoMTD|RecoMTDDetLayers|MTDDetLayerGeometryESProducer";
  auto mtdDetLayerGeometry = std::make_unique<MTDDetLayerGeometry>();
  
  // Build DT layers  
  edm::ESHandle<DTGeometry> dt;
  record.getRecord<MTDGeometryRecord>().get(dt);
  if (dt.isValid()) {
    mtdDetLayerGeometry->addDTLayers(MuonDTDetLayerGeometryBuilder::buildLayers(*dt));
  } else {
    LogInfo(metname) << "No BTL geometry is available."; 
  }

  // Build CSC layers
  edm::ESHandle<CSCGeometry> csc;
  record.getRecord<MTDGeometryRecord>().get(csc);
  if (csc.isValid()) {
    mtdDetLayerGeometry->addCSCLayers(MuonCSCDetLayerGeometryBuilder::buildLayers(*csc));
  } else {
    LogInfo(metname) << "No ETL geometry is available.";
  }

  
  

  // Sort layers properly
  mtdDetLayerGeometry->sortLayers();

  return mtdDetLayerGeometry;
}
