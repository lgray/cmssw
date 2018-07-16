/** \file
 *
 *  \author L. Gray - FNAL
 *
 */

#include <RecoMTD/DetLayers/interface/MTDDetLayerGeometry.h>

#include <FWCore/Utilities/interface/Exception.h>
#include <TrackingTools/DetLayers/interface/DetLayer.h>
#include <DataFormats/MTDDetId/interface/CSCDetId.h>
#include <DataFormats/ForwardDetId/interface/BTLDetId.h"
#include <DataFormats/ForwardDetId/interface/ETLDetId.h"

#include <Utilities/General/interface/precomputed_value_sort.h>
#include <DataFormats/GeometrySurface/interface/GeometricSorting.h>

#include <algorithm>

using namespace std;
using namespace geomsort;

MTDDetLayerGeometry::MTDDetLayerGeometry() {}

MTDDetLayerGeometry::~MTDDetLayerGeometry(){  
}

void MTDDetLayerGeometry::addETLLayers(const pair<vector<DetLayer*>, vector<DetLayer*> >& etllayers) {
    
  for(auto const it : etllayers.first) {
    etlLayers_fw.push_back(it);
    allForward.push_back(it);
    
    detLayersMap[ makeDetLayerId(it) ] = it;
  }

  for(auto const it: etllayers.second) {
    etlLayers_bk.push_back(it);
    allBackward.push_back(it);
    
    detLayersMap[ makeDetLayerId(it) ] = it;
  }    
}    

void MTDDetLayerGeometry::addBTLLayers(const vector<DetLayer*>& dtlayers) {

    for(auto const it : dtlayers) {
        btlLayers.push_back(it);
        allBarrel.push_back(it);

	detLayersMap[ makeDetLayerId(it) ] = it;
    }
}    

DetId MTDDetLayerGeometry::makeDetLayerId(const DetLayer* detLayer) const{

  if(detLayer->subDetector() ==  GeomDetEnumerators::TimingEndcap) {
    ETLDetId id( detLayer->basicComponents().front()->geographicalId().rawId() ) ;
    return ETLDetId(id.endcap(),id.station(),0,0,0);    
  }
  else if(detLayer->subDetector() == GeomDetEnumerators::TimingBarrel) {
    BTLDetId id( detLayer->basicComponents().front()->geographicalId().rawId() ) ;
    return BTLDetId(0,id.station(),0);
  }  
  else throw cms::Exception("InvalidModuleIdentification"); // << detLayer->module();
}


const vector<const DetLayer*>& 
MTDDetLayerGeometry::allBTLLayers() const {    
    return dtLayers; 
}

const vector<const DetLayer*>&
MTDDetLayerGeometry::allETLLayers() const {
    return cscLayers_all;
}


////////////////////////////////////////////////////

const DetLayer* MTDDetLayerGeometry::idToLayer(const DetId &detId) const{

  DetId id;
  
  if(detId.subdetId() == MuonSubdetId::CSC){
    ETLDetId etlId( detId.rawId() );
    id = ETLDetId(etlId.endcap(),etlId.station(),0,0,0);
  }  
  else if (detId.subdetId() == MuonSubdetId::DT){
    BTLDetId btlId( detId.rawId() );
    id = BTLDetId(0,btlId.station(),0);
  }  
  else throw cms::Exception("InvalidSubdetId")<< detId.subdetId();

  std::map<DetId,const DetLayer*>::const_iterator layer = detLayersMap.find(id);
  if (layer == detLayersMap.end()) return nullptr;
  return layer->second; 
}


// Quick way to sort barrel det layers by increasing R,
// do not abuse!
#include <TrackingTools/DetLayers/interface/BarrelDetLayer.h>
struct ExtractBarrelDetLayerR {
  typedef Surface::Scalar result_type;
  result_type operator()(const DetLayer* p) const {
    const BarrelDetLayer * bdl = dynamic_cast<const BarrelDetLayer*>(p);
    if (bdl) return bdl->specificSurface().radius();
    else return -1.;
  }
};

void MTDDetLayerGeometry::sortLayers() {

  // The following are filled inside-out, no need to re-sort
  // precomputed_value_sort(dtLayers.begin(), dtLayers.end(),ExtractR<DetLayer,float>());
  // precomputed_value_sort(cscLayers_fw.begin(), cscLayers_fw.end(),ExtractAbsZ<DetLayer,float>());
  // precomputed_value_sort(cscLayers_bk.begin(), cscLayers_bk.end(),ExtractAbsZ<DetLayer,float>());
  // precomputed_value_sort(rpcLayers_fw.begin(), rpcLayers_fw.end(),ExtractAbsZ<DetLayer,float>());
  // precomputed_value_sort(rpcLayers_bk.begin(), rpcLayers_bk.end(),ExtractAbsZ<DetLayer,float>());
  // precomputed_value_sort(rpcLayers_barrel.begin(), rpcLayers_barrel.end(), ExtractR<DetLayer,float>());

  // Sort these inside-out
  precomputed_value_sort(allBarrel.begin(), allBarrel.end(), ExtractBarrelDetLayerR());
  precomputed_value_sort(allBackward.begin(), allBackward.end(), ExtractAbsZ<DetLayer,float>());
  precomputed_value_sort(allForward.begin(), allForward.end(), ExtractAbsZ<DetLayer,float>());  

  // Build more complicated vectors with correct sorting

  //cscLayers_all: from -Z to +Z
  cscLayers_all.reserve(cscLayers_bk.size()+cscLayers_fw.size());
  std::copy(cscLayers_bk.begin(),cscLayers_bk.end(),back_inserter(cscLayers_all));
  std::reverse(cscLayers_all.begin(),cscLayers_all.end());
  std::copy(cscLayers_fw.begin(),cscLayers_fw.end(),back_inserter(cscLayers_all));

  //gemLayers_all: from -Z to +Z
  gemLayers_all.reserve(gemLayers_bk.size()+gemLayers_fw.size());
  std::copy(gemLayers_bk.begin(),gemLayers_bk.end(),back_inserter(gemLayers_all));
  std::reverse(gemLayers_all.begin(),gemLayers_all.end());
  std::copy(gemLayers_fw.begin(),gemLayers_fw.end(),back_inserter(gemLayers_all));

  //me0Layers_all: from -Z to +Z
  me0Layers_all.reserve(me0Layers_bk.size()+me0Layers_fw.size());
  std::copy(me0Layers_bk.begin(),me0Layers_bk.end(),back_inserter(me0Layers_all));
  std::reverse(me0Layers_all.begin(),me0Layers_all.end());
  std::copy(me0Layers_fw.begin(),me0Layers_fw.end(),back_inserter(me0Layers_all));

  //rpcLayers_endcap: from -Z to +Z
  rpcLayers_endcap.reserve(rpcLayers_bk.size()+rpcLayers_fw.size());
  std::copy(rpcLayers_bk.begin(),rpcLayers_bk.end(),back_inserter(rpcLayers_endcap));
  std::reverse(rpcLayers_endcap.begin(),rpcLayers_endcap.end());
  std::copy(rpcLayers_fw.begin(),rpcLayers_fw.end(),back_inserter(rpcLayers_endcap));

  //rpcLayers_all: order is bw, barrel, fw
  rpcLayers_all.reserve(rpcLayers_bk.size()+rpcLayers_barrel.size()+rpcLayers_fw.size());
  std::copy(rpcLayers_bk.begin(),rpcLayers_bk.end(),back_inserter(rpcLayers_all));
  std::reverse(rpcLayers_all.begin(),rpcLayers_all.end());
  std::copy(rpcLayers_barrel.begin(),rpcLayers_barrel.end(),back_inserter(rpcLayers_all));
  std::copy(rpcLayers_fw.begin(),rpcLayers_fw.end(),back_inserter(rpcLayers_all));

  // allEndcap: order is  all bw, all fw
  allEndcap.reserve(allBackward.size()+allForward.size());
  std::copy(allBackward.begin(),allBackward.end(),back_inserter(allEndcap));
  std::reverse(allEndcap.begin(),allEndcap.end());
  std::copy(allForward.begin(),allForward.end(),back_inserter(allEndcap));

  // allEndcapCSCGEM: order is  all bw, all fw
  allEndcapCscGem.reserve(cscLayers_bk.size()+cscLayers_fw.size()+gemLayers_bk.size()+gemLayers_fw.size());
  std::copy(cscLayers_bk.begin(),cscLayers_bk.end(),back_inserter(allEndcapCscGem));
  std::copy(gemLayers_bk.begin(),gemLayers_bk.end(),back_inserter(allEndcapCscGem));
  std::reverse(allEndcapCscGem.begin(),allEndcapCscGem.end());
  std::copy(cscLayers_fw.begin(),cscLayers_fw.end(),back_inserter(allEndcapCscGem));
  std::copy(gemLayers_fw.begin(),gemLayers_fw.end(),back_inserter(allEndcapCscGem));

  // allCscGemForward
  allCscGemForward.reserve(cscLayers_fw.size()+gemLayers_fw.size());
  std::copy(cscLayers_fw.begin(),cscLayers_fw.end(),back_inserter(allCscGemForward));
  std::copy(gemLayers_fw.begin(),gemLayers_fw.end(),back_inserter(allCscGemForward));

  // allCscGemBackward
  allCscGemBackward.reserve(cscLayers_bk.size()+gemLayers_bk.size());
  std::copy(cscLayers_bk.begin(),cscLayers_bk.end(),back_inserter(allCscGemBackward));
  std::copy(gemLayers_bk.begin(),gemLayers_bk.end(),back_inserter(allCscGemBackward));

  // allCscME0Forward
  allCscME0Forward.reserve(cscLayers_fw.size()+me0Layers_fw.size());
  std::copy(cscLayers_fw.begin(),cscLayers_fw.end(),back_inserter(allCscME0Forward));
  std::copy(me0Layers_fw.begin(),me0Layers_fw.end(),back_inserter(allCscME0Forward));

  // allCscME0Backward
  allCscME0Backward.reserve(cscLayers_bk.size()+me0Layers_bk.size());
  std::copy(cscLayers_bk.begin(),cscLayers_bk.end(),back_inserter(allCscME0Backward));
  std::copy(me0Layers_bk.begin(),me0Layers_bk.end(),back_inserter(allCscME0Backward));

  // allEndcapCSCME0: order is  all bw, all fw
  allEndcapCscME0.reserve(cscLayers_bk.size()+cscLayers_fw.size()+me0Layers_bk.size()+me0Layers_fw.size());
  std::copy(cscLayers_bk.begin(),cscLayers_bk.end(),back_inserter(allEndcapCscME0));
  std::copy(me0Layers_bk.begin(),me0Layers_bk.end(),back_inserter(allEndcapCscME0));
  std::reverse(allEndcapCscME0.begin(),allEndcapCscME0.end());
  std::copy(cscLayers_fw.begin(),cscLayers_fw.end(),back_inserter(allEndcapCscME0));
  std::copy(me0Layers_fw.begin(),me0Layers_fw.end(),back_inserter(allEndcapCscME0));

  // allDetLayers: order is  all bw, all barrel, all fw
  allDetLayers.reserve(allBackward.size()+allBarrel.size()+allForward.size());
  std::copy(allBackward.begin(),allBackward.end(),back_inserter(allDetLayers));
  std::reverse(allDetLayers.begin(),allDetLayers.end());
  std::copy(allBarrel.begin(),allBarrel.end(),back_inserter(allDetLayers));
  std::copy(allForward.begin(),allForward.end(),back_inserter(allDetLayers));

  // number layers
  int sq=0;
  for (auto l : allDetLayers) 
    (*const_cast<DetLayer*>(l)).setSeqNum(sq++);


}
