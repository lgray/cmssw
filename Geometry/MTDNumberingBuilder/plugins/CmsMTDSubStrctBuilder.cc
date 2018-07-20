#include "Geometry/MTDNumberingBuilder/plugins/CmsMTDSubStrctBuilder.h"
#include "DetectorDescription/Core/interface/DDFilteredView.h"
#include "Geometry/MTDNumberingBuilder/interface/GeometricTimingDet.h"
#include "Geometry/MTDNumberingBuilder/plugins/ExtractStringFromDDD.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "Geometry/MTDNumberingBuilder/plugins/CmsMTDTrayBuilder.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <vector>

#include <bitset>

CmsMTDSubStrctBuilder::CmsMTDSubStrctBuilder()
{}

void
CmsMTDSubStrctBuilder::buildComponent( DDFilteredView& fv, GeometricTimingDet* g, std::string s )
{
  CmsMTDTrayBuilder theCmsMTDTrayBuilder;

  std::cout << "CmsMTDSubStrctBuilder: " << fv.logicalPart().name() << std::endl;
  
  GeometricTimingDet * subdet = new GeometricTimingDet( &fv, theCmsMTDStringToEnum.type( fv.logicalPart().name() ));

  std::cout << "CmsMTDSubStrctBuilder: " << fv.logicalPart().name() << std::endl;

  switch( theCmsMTDStringToEnum.type( fv.logicalPart().name() ) )
  {  
  case GeometricTimingDet::BTLLayer:
    theCmsMTDTrayBuilder.build(fv,subdet,s);      
    break;  

  default:
    throw cms::Exception("CmsMTDSubStrctBuilder")<<" ERROR - I was expecting a BTLLayer... I got a "<< fv.logicalPart().name();
  }  
  
  g->addComponent(subdet);

}

void
CmsMTDSubStrctBuilder::sortNS( DDFilteredView& fv, GeometricTimingDet* det )
{
  GeometricTimingDet::ConstGeometricTimingDetContainer & comp = det->components();

  switch( comp.front()->type())
  {  
  case GeometricTimingDet::BTLLayer:
    std::sort( comp.begin(), comp.end(), LessR());
    break;    
  default:
    edm::LogError( "CmsMTDSubStrctBuilder" ) << "ERROR - wrong SubDet to sort..... " << det->components().front()->type(); 
  }
  
  for( uint32_t i = 0; i < comp.size(); i++ )
  {
    det->component(i)->setGeographicalID(i+1); // Every subdetector: Layer/Disk/Wheel Number
  }
}

