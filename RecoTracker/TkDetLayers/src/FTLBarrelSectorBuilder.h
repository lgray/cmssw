#ifndef TkDetLayers_FTLBarrelSectorBuilder_h
#define TkDetLayers_FTLBarrelSectorBuilder_h


#include "FTLBarrelSector.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/TrackerNumberingBuilder/interface/GeometricDet.h"

/** A concrete builder for FTLBarrelSector
 */

#pragma GCC visibility push(hidden)
class FTLBarrelSectorBuilder {  
 public:
  FTLBarrelSectorBuilder(){};
  FTLBarrelSector* build(const GeometricDet* theFTLBarrelSector,
			 const TrackerGeometry* theGeomDetGeometry) __attribute__ ((cold));

  
};


#pragma GCC visibility pop
#endif 
