#include "Geometry/MTDGeometry/interface/ETLGeometry.h"
#include "Geometry/MTDGeometry/interface/CSCChamber.h"
#include "Geometry/MTDGeometry/interface/CSCChamberSpecs.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/GeometrySurface/interface/TrapezoidalPlaneBounds.h"

#include <string>

ETLGeometry::ETLGeometry():  debugV_(false), gangedstripsME1a_(true), 
   onlywiresME1a_(false), realWireGeometry_(true), useCentreTIOffsets_(false) {
   if ( debugV_ ) queryModelling();
}

ETLGeometry::ETLGeometry( bool dbgv, bool gangedstripsME1a, bool onlywiresME1a, bool realWireGeometry, bool useCentreTIOffsets ) :  
   debugV_(dbgv), gangedstripsME1a_( gangedstripsME1a ), onlywiresME1a_( onlywiresME1a ), 
   realWireGeometry_( realWireGeometry ), useCentreTIOffsets_( useCentreTIOffsets ) {
   if ( debugV_ ) queryModelling();
}

ETLGeometry::~ETLGeometry(){

  // delete all the chambers (which will delete the layers)
  for (ChamberContainer::const_iterator ich=theChambers.begin();
       ich!=theChambers.end(); ++ich) delete (*ich);

  // delete specs
  for ( CSCSpecsContainer::const_iterator it =
	   specsContainer.begin(); it!=specsContainer.end(); ++it) {
    delete (*it).second; // they are never shared per chamber type so should be no possible double deletion.
  }

}  


void ETLGeometry::addChamber(CSCChamber* ch){
  theChambers.emplace_back(ch);
  addDet(ch);
}


void ETLGeometry::addLayer(CSCLayer* l) {
  theDetUnits.emplace_back(l);
  theLayers.emplace_back(l);
  theDetTypes.emplace_back(l->chamber()->specs());
  theDetUnitIds.emplace_back(l->geographicalId());
  addDet(l);
}


void ETLGeometry::addDetType(GeomDetType* type) {
  theDetTypes.emplace_back(type);
}


void ETLGeometry::addDet(GeomDet* det){
  theDets.emplace_back(det);  
  theDetIds.emplace_back(det->geographicalId());
  theMap.insert(CSCDetMap::value_type(det->geographicalId(),det));
}


const ETLGeometry::DetTypeContainer& ETLGeometry::detTypes() const 
{
  return theDetTypes;
}


const ETLGeometry::DetContainer& ETLGeometry::detUnits() const
{
  return theDetUnits;
}


const ETLGeometry::DetContainer& ETLGeometry::dets() const
{
  return theDets;
}


const ETLGeometry::DetIdContainer& ETLGeometry::detUnitIds() const 
{
  return theDetUnitIds;
}


const ETLGeometry::DetIdContainer& ETLGeometry::detIds() const 
{
  return theDetIds;
}


const GeomDet* ETLGeometry::idToDetUnit(DetId id) const
{
  return dynamic_cast<const GeomDet*>(idToDet(id));
}


const GeomDet* ETLGeometry::idToDet(DetId id) const{
  CSCDetMap::const_iterator i = theMap.find(id);
  return (i != theMap.end()) ?
    i->second : nullptr ;
}


const ETLGeometry::ChamberContainer& ETLGeometry::chambers() const
{
  return theChambers;
}


const ETLGeometry::LayerContainer& ETLGeometry::layers() const
{
  return theLayers;
}


const CSCChamber* ETLGeometry::chamber(CSCDetId id) const {
  CSCDetId id1(id.endcap(), id.station(), id.ring(), id.chamber(), 0);
  return dynamic_cast<const CSCChamber*>(idToDet(id1));
}


const CSCLayer* ETLGeometry::layer(CSCDetId id) const {
  return dynamic_cast<const CSCLayer*>(idToDetUnit(id));
}

void ETLGeometry::queryModelling() const {
  // Dump user-selected overall modelling parameters.
  // Only requires calling once per job.

  LogTrace("ETLGeometry|ETL")  << "ETLGeometry::queryModelling entered...";

  edm::LogInfo("ETL") << "ETLGeometry version 18-Oct-2012 queryModelling...\n";

  std::string gs = " ";
  if ( gangedstripsME1a_ )
    gs = "GANGED";
  else
    gs = "UNGANGED";

  edm::LogInfo("ETL") << "ETLGeometry: in ME1a use " << gs << " strips" << "\n";

  std::string wo = " ";
  if ( onlywiresME1a_ )
    wo = "WIRES ONLY";
  else
    wo = "WIRES & STRIPS";

  edm::LogInfo("ETL") << "ETLGeometry: in ME1a use  " << wo << "\n";

  std::string wg = " ";
  if ( realWireGeometry_ )
    wg = "REAL";
  else
    wg = "PSEUDO";

  edm::LogInfo("ETL") << "ETLGeometry: wires are modelled using " << wg << " wire geometry " << "\n";

  std::string cti = " ";
  if ( useCentreTIOffsets_ )
    cti = "WITH";
  else
    cti = "WITHOUT";

  edm::LogInfo("ETL") << "ETLGeometry: strip plane centre-to-intersection ideal " << cti << " corrections " << "\n";
}

const CSCChamberSpecs* ETLGeometry::findSpecs( int iChamberType ) {
  const CSCChamberSpecs* aSpecs = nullptr;
  CSCSpecsContainer::const_iterator it = specsContainer.find( iChamberType );
  if (  it != specsContainer.end() )  aSpecs = (*it).second;
  return aSpecs;
} 

const CSCChamberSpecs* ETLGeometry::buildSpecs( int iChamberType,
					 const std::vector<float>& fpar,
					 const std::vector<float>& fupar,
					 const CSCWireGroupPackage& wg ) {

  // Note arg list order is hbot, htop, apothem, hthickness
  TrapezoidalPlaneBounds bounds( fpar[0], fpar[1], fpar[3], fpar[2] );
  const CSCChamberSpecs* aSpecs = new CSCChamberSpecs( this, iChamberType, bounds, fupar, wg );
  specsContainer[ iChamberType ] = aSpecs;
  return aSpecs;
}
