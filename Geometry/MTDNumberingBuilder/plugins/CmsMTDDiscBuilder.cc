#include "Geometry/MTDNumberingBuilder/plugins/CmsMTDDiscBuilder.h"
#include "DetectorDescription/Core/interface/DDFilteredView.h"
#include "Geometry/MTDNumberingBuilder/interface/GeometricTimingDet.h"
#include "Geometry/MTDNumberingBuilder/plugins/ExtractStringFromDDD.h"
#include "Geometry/MTDNumberingBuilder/plugins/CmsMTDETLRingBuilder.h"
#include "Geometry/MTDNumberingBuilder/plugins/MTDStablePhiSort.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <vector>
#include <algorithm>

using namespace std;

void
CmsMTDDiscBuilder::buildComponent( DDFilteredView& fv, GeometricTimingDet* g, std::string s )
{
  CmsMTDETLRingBuilder theCmsMTDETLRingBuilder;
  GeometricTimingDet * subdet = new GeometricTimingDet( &fv, theCmsMTDStringToEnum.type( fv.logicalPart().name().fullname().substr(0,8) ));

  switch( theCmsMTDStringToEnum.type( fv.logicalPart().name().fullname().substr(0,8) ))
  {
  case GeometricTimingDet::ETLRing:
    theCmsMTDETLRingBuilder.build( fv, subdet, s );
    break;
  default:
    throw cms::Exception( "CmsMTDDiscBuilder" ) << " ERROR - I was expecting a Ring, I got a " <<  fv.logicalPart().name().fullname().substr(0,8);   
  }  
  g->addComponent( subdet );
}

void
CmsMTDDiscBuilder::sortNS( DDFilteredView& fv, GeometricTimingDet* det )
{


  GeometricTimingDet::ConstGeometricTimingDetContainer & comp = det->components();

  switch(det->components().front()->type()){
  case GeometricTimingDet::ETLRing:
    // nothing to be done because the rings (here named panels) are already sorted ??
    break;
  default:
    edm::LogError("CmsMTDDiscBuilder")<<"ERROR - wrong SubDet to sort..... "<<det->components().front()->type();
  }
  
  GeometricTimingDet::GeometricTimingDetContainer rings;
  uint32_t  totalrings = comp.size();


  for ( uint32_t rn=0; rn<totalrings; rn++) {
    rings.emplace_back(det->component(rn));
    uint32_t blade = rn+1;
    uint32_t panel = 1;
    uint32_t temp = (blade<<2) | panel;
    rings[rn]->setGeographicalID(temp);

  }

  det->clearComponents();
  det->addComponents(rings);

}

