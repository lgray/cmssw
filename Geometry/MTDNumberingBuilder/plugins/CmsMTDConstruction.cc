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

  std::cout << "CmsMTDConstruction :: " << fv.logicalPart().name().fullname().substr(0,11) << std::endl;

  if ( theCmsMTDStringToEnum.type(fv.logicalPart().name().fullname().substr(0,11)) ==  GeometricTimingDet::BTLModule || 
       theCmsMTDStringToEnum.type(fv.logicalPart().name().fullname().substr(0,11)) ==  GeometricTimingDet::ETLModule    ) {
  
    bool dodets = fv.firstChild(); 
    while (dodets) {
      buildSmallDetsforStack(fv,det,attribute);
      dodets = fv.nextSibling(); 
    }
    fv.parent();
  } else {
    std::cout << "woops got: " << fv.logicalPart().name().fullname().substr(0,11) << std::endl;
  }
  
  mother->addComponent(det);

}

void CmsMTDConstruction::buildSmallDetsforStack(DDFilteredView& fv,
        	                                GeometricTimingDet *mother,
                	                        const std::string& attribute){

  GeometricTimingDet * det  = new GeometricTimingDet(&fv, theCmsMTDStringToEnum.type(ExtractStringFromDDD::getString(attribute,&fv)));
  static const std::string isLower = "TrackerLowerDetectors";
  static const std::string isUpper = "TrackerUpperDetectors";

  std::cout << "CmsMTDConstruction::buildSmallDetsforStack : " << fv.logicalPart().name() << std::endl;

  if (ExtractStringFromDDD::getString(isLower,&fv) == "true"){
    uint32_t temp = 1;
    det->setGeographicalID(DetId(temp));
  } else if (ExtractStringFromDDD::getString(isUpper,&fv) == "true"){
    uint32_t temp = 2;
    det->setGeographicalID(DetId(temp));
  } else {
    edm::LogError("DetConstruction") << " module defined in a Stack but not upper either lower!? ";
  }
  mother->addComponent(det);
}
