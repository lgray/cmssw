/** \file
 *
 *  \author L. Gray - FNAL
 *
 */

#include <RecoMTD/DetLayers/plugins/MTDDetLayerGeometryESProducer.h>
#include <Geometry/Records/interface/MTDGeometryRecord.h>

#include <Geometry/MTDGeometry/interface/BTLGeometry.h>
#include <Geometry/MTDGeometry/interface/ETLGeometry.h>

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
MTDDetLayerGeometryESProducer::produce(const MTDRecoGeometryRecord & record) {

  const std::string metname = "MTD|RecoMTD|RecoMTDDetLayers|MTDDetLayerGeometryESProducer";
  auto mtdDetLayerGeometry = std::make_unique<MTDDetLayerGeometry>();
  
  // Build DT layers  
  edm::ESHandle<BTLGeometry> btl;
  record.getRecord<MTDGeometryRecord>().get(btl);
  if (btl.isValid()) {
    mtdDetLayerGeometry->addBTLLayers(BTLDetLayerGeometryBuilder::buildLayers(*btl));
  } else {
    LogInfo(metname) << "No BTL geometry is available."; 
  }

  // Build CSC layers
  edm::ESHandle<ETLGeometry> etl;
  record.getRecord<MTDGeometryRecord>().get(etl);
  if (etl.isValid()) {
    mtdDetLayerGeometry->addETLLayers(ETLDetLayerGeometryBuilder::buildLayers(*etl));
  } else {
    LogInfo(metname) << "No ETL geometry is available.";
  }
  
  // Sort layers properly
  mtdDetLayerGeometry->sortLayers();

  return mtdDetLayerGeometry;
}
