#include <RecoMTD/DetLayers/src/BTLDetLayerGeometryBuilder.h>

#include <DataFormats/MuonDetId/interface/DTChamberId.h>
#include <Geometry/CommonDetUnit/interface/GeomDet.h>
#include <RecoMTD/DetLayers/interface/MTDTrayBarrelLayer.h>
#include <RecoMTD/DetLayers/interface/MTDDetTray.h>

#include <Utilities/General/interface/precomputed_value_sort.h>
#include <Geometry/CommonDetUnit/interface/DetSorting.h>

#include <FWCore/MessageLogger/interface/MessageLogger.h>

#include <iostream>

using namespace std;

BTLDetLayerGeometryBuilder::BTLDetLayerGeometryBuilder() {
}

BTLDetLayerGeometryBuilder::~BTLDetLayerGeometryBuilder() {
}

vector<DetLayer*> 
BTLDetLayerGeometryBuilder::buildLayers(const DTGeometry& geo) {
        
  const std::string metname = "MTD|RecoMTD|RecoMTDDetLayers|BTLDetLayerGeometryBuilder";

  vector<DetLayer*> detlayers;
  vector<MTDTrayBarrelLayer*> result;
            
  for(int station = DTChamberId::minStationId; station <= DTChamberId::maxStationId; station++) {
    
    vector<const DetRod*> muDetRods;
    for(int sector = DTChamberId::minSectorId; sector <= DTChamberId::maxSectorId; sector++) {
      
      vector<const GeomDet*> geomDets;
      for(int wheel = DTChamberId::minWheelId; wheel <= DTChamberId::maxWheelId; wheel++) {		  
        const GeomDet* geomDet = geo.idToDet(DTChamberId(wheel, station, sector));
        if (geomDet) {
          geomDets.push_back(geomDet);
          LogTrace(metname) << "get DT chamber " <<  DTChamberId(wheel, station, sector)
                            << " at R=" << geomDet->position().perp()
                            << ", phi=" << geomDet->position().phi() ;
        }
      }
      
      if (!geomDets.empty()) {
        precomputed_value_sort(geomDets.begin(), geomDets.end(), geomsort::DetZ());
        muDetRods.push_back(new MTDDetTray(geomDets));
        LogTrace(metname) << "  New MuDetRod with " << geomDets.size()
                          << " chambers at R=" << muDetRods.back()->position().perp()
                          << ", phi=" << muDetRods.back()->position().phi();
      }
    }
    precomputed_value_sort(muDetRods.begin(), muDetRods.end(), geomsort::ExtractPhi<GeometricSearchDet,float>());
    result.push_back(new MTDTrayBarrelLayer(muDetRods));  
    LogDebug(metname) << "    New MuRodBarrelLayer with " << muDetRods.size()
                      << " rods, at R " << result.back()->specificSurface().radius();
  }
  
  for(vector<MTDTrayBarrelLayer*>::const_iterator it = result.begin(); it != result.end(); it++)
    detlayers.push_back((DetLayer*)(*it));
  
  return detlayers;
}
