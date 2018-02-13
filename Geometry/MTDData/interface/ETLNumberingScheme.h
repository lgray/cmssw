#ifndef ETLNumberingScheme_h
#define ETLNumberingScheme_h

#include "Geometry/MTDData/interface/MTDNumberingScheme.h"

class ETLNumberingScheme : public MTDNumberingScheme {
 public:
  ETLNumberingScheme();
  ~ETLNumberingScheme();
  uint32_t getUnitID(const MTDBaseNumber& baseNumber) const override ;
};

#endif
