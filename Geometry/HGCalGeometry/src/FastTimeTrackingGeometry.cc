#include "Geometry/HGCalGeometry/interface/FastTimeTrackingGeometry.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/GeometrySurface/interface/RectangularPlaneBounds.h"

#include <string>

FastTimeTrackingGeometry::FastTimeTrackingGeometry():  theGeom(nullptr) {
}

FastTimeTrackingGeometry::FastTimeTrackingGeometry( const FastTimeGeometry* geom ) :  
  theGeom(geom) {
}

FastTimeTrackingGeometry::~FastTimeTrackingGeometry(){

}  

void FastTimeTrackingGeometry::addDetType(GeomDetType* type) {
  theDetTypes.push_back(type);
}


void FastTimeTrackingGeometry::addDet(GeomDet* det){
  theDets.push_back(det);  
  theDetIds.push_back(det->geographicalId());
  theMap.insert(FTLDetMap::value_type(det->geographicalId(),det));
}


const FastTimeTrackingGeometry::DetTypeContainer& FastTimeTrackingGeometry::detTypes() const 
{
  return theDetTypes;
}


const FastTimeTrackingGeometry::DetUnitContainer& FastTimeTrackingGeometry::detUnits() const
{
  return theDetUnits;
}


const FastTimeTrackingGeometry::DetContainer& FastTimeTrackingGeometry::dets() const
{
  return theDets;
}


const FastTimeTrackingGeometry::DetIdContainer& FastTimeTrackingGeometry::detUnitIds() const 
{
  return theDetUnitIds;
}


const FastTimeTrackingGeometry::DetIdContainer& FastTimeTrackingGeometry::detIds() const 
{
  return theDetIds;
}


const GeomDetUnit* FastTimeTrackingGeometry::idToDetUnit(DetId id) const
{
  return dynamic_cast<const GeomDetUnit*>(idToDet(id));
}


const GeomDet* FastTimeTrackingGeometry::idToDet(DetId id) const{
  FTLDetMap::const_iterator i = theMap.find(id);
  return (i != theMap.end()) ?
    i->second : 0 ;
}
