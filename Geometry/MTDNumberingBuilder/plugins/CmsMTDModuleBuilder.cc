#include "Geometry/MTDNumberingBuilder/plugins/CmsMTDModuleBuilder.h"
#include "DetectorDescription/Core/interface/DDFilteredView.h"
#include "Geometry/MTDNumberingBuilder/interface/GeometricTimingDet.h"
#include "Geometry/MTDNumberingBuilder/plugins/ExtractStringFromDDD.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "Geometry/MTDNumberingBuilder/plugins/CmsMTDConstruction.h"
#include <vector>

void CmsMTDModuleBuilder::buildComponent(DDFilteredView& fv, GeometricTimingDet* g, std::string side){

  //if( fv.logicalPart().name().fullname().find(side) == std::string::npos ) return;

  CmsMTDConstruction theCmsMTDConstruction;
  theCmsMTDConstruction.buildComponent(fv,g,side);  
}


#include "DataFormats/ForwardDetId/interface/BTLDetId.h"
void CmsMTDModuleBuilder::sortNS(DDFilteredView& fv, GeometricTimingDet* det){
  GeometricTimingDet::ConstGeometricTimingDetContainer & comp = det->components();

  //sorting for PhaseI & PhaseII pixel ladder modules
  //sorting also for PhaseII outer tracker rod modules
  std::stable_sort(comp.begin(),comp.end(),LessZ());
 
  /*
  for(uint32_t i=0; i<comp.size();i++){
    det->component(i)->setGeographicalID(BTLDetId(0,0,0,i+1,0));
  }
  */

  if (comp.empty() ){
   edm::LogError("CmsMTDModuleBuilder") << "Where are the OT Phase2/ pixel barrel modules modules?";
  } 

}




