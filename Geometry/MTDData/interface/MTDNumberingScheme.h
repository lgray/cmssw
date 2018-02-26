#ifndef MTDNumberingScheme_h
#define MTDNumberingScheme_h

#include "Geometry/MTDData/interface/MTDBaseNumber.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <boost/cstdint.hpp>

class MTDNumberingScheme {
 public:
  MTDNumberingScheme();
  virtual ~MTDNumberingScheme();
  virtual uint32_t getUnitID(const MTDBaseNumber& baseNumber) const = 0;
};

#endif
