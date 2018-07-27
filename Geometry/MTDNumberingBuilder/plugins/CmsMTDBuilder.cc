#include "Geometry/MTDNumberingBuilder/plugins/CmsMTDBuilder.h"
#include "DetectorDescription/Core/interface/DDFilteredView.h"
#include "Geometry/MTDNumberingBuilder/interface/GeometricTimingDet.h"
#include "Geometry/MTDNumberingBuilder/plugins/ExtractStringFromDDD.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "Geometry/MTDNumberingBuilder/plugins/CmsMTDSubStrctBuilder.h"
#include "Geometry/MTDNumberingBuilder/plugins/CmsMTDEndcapBuilder.h"

#include <bitset>

CmsMTDBuilder::CmsMTDBuilder()
{}

void
CmsMTDBuilder::buildComponent( DDFilteredView& fv, GeometricTimingDet* g, std::string s )
{
  CmsMTDSubStrctBuilder theCmsMTDSubStrctBuilder;
  CmsMTDEndcapBuilder theCmsMTDEndcapBuilder;
  
  GeometricTimingDet* subdet = new GeometricTimingDet( &fv, theCmsMTDStringToEnum.type( ExtractStringFromDDD::getString( s, &fv )));
  
  switch( theCmsMTDStringToEnum.type( fv.logicalPart().name() ) )
  {  
  case GeometricTimingDet::ETL:
    theCmsMTDEndcapBuilder.build( fv, subdet, s );      
    break;  
  case GeometricTimingDet::BTL:
    theCmsMTDSubStrctBuilder.build( fv, subdet, s ); 
    break;  
  default:
    throw cms::Exception("CmsMTDBuilder") << " ERROR - I was expecting a SubDet, I got a " << fv.logicalPart().name();   
  }
  
  g->addComponent( subdet );
}

void
CmsMTDBuilder::sortNS( DDFilteredView& fv, GeometricTimingDet* det )
{  
  GeometricTimingDet::ConstGeometricTimingDetContainer & comp = det->components();
  std::stable_sort( comp.begin(), comp.end(), subDetByType());
  
  for( uint32_t i = 0; i < comp.size(); i++ )
  {
    uint32_t temp= comp[i]->type();
    det->component(i)->setGeographicalID(temp);  // it relies on the fact that the GeometricTimingDet::GDEnumType enumerators used to identify the subdetectors in the upgrade geometries are equal to the ones of the present detector + n*100
  }
}




