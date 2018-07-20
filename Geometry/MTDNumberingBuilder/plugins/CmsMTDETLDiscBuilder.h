#ifndef Geometry_MTDNumberingBuilder_CmsMTDETLDiscBuilder_H
#define Geometry_MTDNumberingBuilder_CmsMTDETLDiscBuilder_H

# include "Geometry/MTDNumberingBuilder/plugins/CmsMTDLevelBuilder.h"
# include "FWCore/ParameterSet/interface/types.h"
# include <string>

/**
 * Class which contructs Phase2 MTD ETL Discs.
 */
class CmsMTDETLDiscBuilder : public CmsMTDLevelBuilder
{
  
private:
  void sortNS( DDFilteredView& , GeometricTimingDet* ) override;
  void buildComponent( DDFilteredView& , GeometricTimingDet*, std::string ) override;
  
};

#endif
