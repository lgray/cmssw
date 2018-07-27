#include "Geometry/MTDNumberingBuilder/plugins/CmsMTDConstruction.h"
#include "Geometry/MTDNumberingBuilder/plugins/ExtractStringFromDDD.h"
#include "DataFormats/DetId/interface/DetId.h"

void CmsMTDConstruction::buildComponent(DDFilteredView& fv, 
					GeometricTimingDet *mother, 
					std::string attribute){
  
  //
  // at this level I check whether it is a merged detector or not
  //

  GeometricTimingDet * det  = new GeometricTimingDet(&fv,theCmsMTDStringToEnum.type(fv.logicalPart().name()));

  const std::string part_name = fv.logicalPart().name().fullname().substr(0,11);
  
  if ( theCmsMTDStringToEnum.type(part_name) ==  GeometricTimingDet::BTLModule ) {
    bool dodets = fv.firstChild(); 
    while (dodets) {
      buildBTLModule(fv,det,attribute);
      dodets = fv.nextSibling(); 
    }
    fv.parent();
  } else if ( theCmsMTDStringToEnum.type(part_name) ==  GeometricTimingDet::ETLModule ) {  
    bool dodets = fv.firstChild(); 
    while (dodets) {
      buildETLModule(fv,det,attribute);
      dodets = fv.nextSibling(); 
    }
    fv.parent();
  } else {
    throw cms::Exception("MTDConstruction") << "woops got: " << part_name << std::endl;
  }
  
  mother->addComponent(det);
}

void CmsMTDConstruction::buildBTLModule(DDFilteredView& fv,
					GeometricTimingDet *mother,
					const std::string& attribute){

  GeometricTimingDet * det  = new GeometricTimingDet(&fv, theCmsMTDStringToEnum.type(ExtractStringFromDDD::getString(attribute,&fv)));
  
  const auto& copyNumbers = fv.copyNumbers();
  auto module_number = copyNumbers[copyNumbers.size()-2];

  constexpr char positive[] = "PositiveZ";
  constexpr char negative[] = "NegativeZ";

  const std::string modname = fv.logicalPart().name().fullname();
  
  if( modname.find(positive) != std::string::npos ) {
    det->setGeographicalID(DetId(1));
  } else if ( modname.find(negative) != std::string::npos ) {
    det->setGeographicalID(DetId(2));
  } else {
    throw cms::Exception("CmsMTDConstruction::buildBTLModule") 
      << "BTL Module " << module_number << " is neither positive nor negative in Z!";
  }
  
  mother->addComponent(det);
}

void CmsMTDConstruction::buildETLModule(DDFilteredView& fv,
					GeometricTimingDet *mother,
					const std::string& attribute){

  GeometricTimingDet * det  = new GeometricTimingDet(&fv, theCmsMTDStringToEnum.type(ExtractStringFromDDD::getString(attribute,&fv)));
  
  const auto& copyNumbers = fv.copyNumbers();
  auto module_number = copyNumbers[copyNumbers.size()-2];
  
  // label geographic detid is front or back (even though it is one module per entry here)
  det->setGeographicalID(DetId(module_number%2+1)); 
  
  mother->addComponent(det);
}
