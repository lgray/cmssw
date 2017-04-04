#include "Geometry/HGCalCommonData/interface/FastTimeParameters.h"

FastTimeParameters::FastTimeParameters():
  nZBarrel_(0),
  nPhiBarrel_(0),
  nEtaEndcap_(0),
  nPhiEndcap_(0) { }

FastTimeParameters::~FastTimeParameters() { }

#include "FWCore/Utilities/interface/typelookup.h"

TYPELOOKUP_DATA_REG(FastTimeParameters);
