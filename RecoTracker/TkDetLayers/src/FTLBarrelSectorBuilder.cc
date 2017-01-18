#include "FTLBarrelSectorBuilder.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

using namespace edm;
using namespace std;

FTLBarrelSector* FTLBarrelSectorBuilder::build(const GeometricDet* theFTLBarrelSector,
					       const TrackerGeometry* theGeomDetGeometry)
{  
  const std::vector<const GeometricDet*>& allGeometricDets = theFTLBarrelSector->components();
  LogDebug("TkDetLayers") << "FTLBarrelSectorBuilder with #Modules: " << allGeometricDets.size() << std::endl;

  std::vector<const GeomDet*> innerGeomDets;
  std::vector<const GeomDet*> outerGeomDets;
  
  double meanR = 0;
  for(auto ptr : allGeometricDets){
    const auto& compGeometricDets = ptr->components(); 
    if (compGeometricDets.size() != 1){
      LogDebug("FTLDetLayers") << " Stack not with one component but with " << compGeometricDets.size() << std::endl;
    } else {
      //LogTrace("TkDetLayers") << " compGeometricDets[0]->positionBounds().perp() " << compGeometricDets[0]->positionBounds().perp() << std::endl;
      //LogTrace("TkDetLayers") << " compGeometricDets[1]->positionBounds().perp() " << compGeometricDets[1]->positionBounds().perp() << std::endl;
      meanR = meanR + compGeometricDets[0]->positionBounds().perp();
    }

  }
  meanR = meanR/allGeometricDets.size();
  LogDebug("FTLDetLayers") << " meanR Lower " << meanR << std::endl;

  for(auto ptr : allGeometricDets){
    const auto& compGeometricDets = ptr->components(); 
    const GeomDet* theGeomDet = theGeomDetGeometry->idToDet( compGeometricDets[0]->geographicalID() );
    LogTrace("FTLDetLayers") << " inserisco " << compGeometricDets[0]->geographicalID().rawId() << std::endl;

    if( compGeometricDets[0]->positionBounds().perp() < meanR)
      innerGeomDets.push_back(theGeomDet);

    if( compGeometricDets[0]->positionBounds().perp() >= meanR)
      outerGeomDets.push_back(theGeomDet);
    
  }

  LogDebug("FTLDetLayers") << "innerGeomDets.size(): " << innerGeomDets.size() ;
  LogDebug("FTLDetLayers") << "outerGeomDets.size(): " << outerGeomDets.size() ;
  
  return new FTLBarrelSector(innerGeomDets,outerGeomDets);
 
}
