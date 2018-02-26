#ifndef BTLNumberingScheme_h
#define BTLNumberingScheme_h

#include "Geometry/MTDData/interface/MTDNumberingScheme.h"

class BTLNumberingScheme : public MTDNumberingScheme {
 public:
  BTLNumberingScheme();
  ~BTLNumberingScheme();
  uint32_t getUnitID(const MTDBaseNumber& baseNumber) const override ;
};

#endif
