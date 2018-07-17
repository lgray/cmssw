/** \file
 *
 *  \author L. Gray - FNAL
 */

#include <Geometry/MTDGeometry/interface/BTLGeometry.h>
#include <Geometry/CommonDetUnit/interface/GeomDet.h>

#include <algorithm>
#include <iostream>

BTLGeometry::BTLGeometry() {}

BTLGeometry::~BTLGeometry(){
  // delete all the chambers (which will delete the SL which will delete the
  // layers)
  for (auto & theChamber : theChambers) delete theChamber;
}

const BTLGeometry::DetTypeContainer&  BTLGeometry::detTypes() const{
  // FIXME - fill it at runtime
  return theDetTypes;
}


void BTLGeometry::add(DTChamber* ch) {
  theDets.emplace_back(ch);
  theChambers.emplace_back(ch);
  theMap.insert(DTDetMap::value_type(ch->geographicalId(),ch));
}


void BTLGeometry::add(DTSuperLayer* sl) {
  theDets.emplace_back(sl);
  theSuperLayers.emplace_back(sl);
  theMap.insert(DTDetMap::value_type(sl->geographicalId(),sl));
}


void BTLGeometry::add(DTLayer* l) {
  theDetUnits.emplace_back(l);
  theDets.emplace_back(l);
  theLayers.emplace_back(l); 
  theMap.insert(DTDetMap::value_type(l->geographicalId(),l));
}


const BTLGeometry::DetContainer& BTLGeometry::detUnits() const{
  return theDetUnits;
}


const BTLGeometry::DetContainer& BTLGeometry::dets() const{
  return theDets; 
}


const BTLGeometry::DetIdContainer& BTLGeometry::detUnitIds() const{
  // FIXME - fill it at runtime
  return theDetUnitIds;
}


const BTLGeometry::DetIdContainer& BTLGeometry::detIds() const{
  // FIXME - fill it at runtime
  return theDetIds;
}


const GeomDet* BTLGeometry::idToDetUnit(DetId id) const{
  return dynamic_cast<const GeomDet*>(idToDet(id));
}


const GeomDet* BTLGeometry::idToDet(DetId id) const{
  // Strip away wire#, if any!
  DTLayerId lId(id.rawId());
  DTDetMap::const_iterator i = theMap.find(lId);
  return (i != theMap.end()) ?
    i->second : nullptr ;
}


const std::vector<const DTChamber*>& BTLGeometry::chambers() const{
  return theChambers;
}


const std::vector<const DTSuperLayer*>& BTLGeometry::superLayers() const{
  return theSuperLayers;
}


const std::vector<const DTLayer*>& BTLGeometry::layers() const{
  return theLayers;
}


const DTChamber* BTLGeometry::chamber(const DTChamberId& id) const {
  return (const DTChamber*)(idToDet(id));
}


const DTSuperLayer* BTLGeometry::superLayer(const DTSuperLayerId& id) const {
  return (const DTSuperLayer*)(idToDet(id));
}


const DTLayer* BTLGeometry::layer(const DTLayerId& id) const {
  return (const DTLayer*)(idToDet(id));
}
