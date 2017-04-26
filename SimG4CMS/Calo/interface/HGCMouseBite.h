#ifndef SimG4CMS_HGCMouseBite_h
#define SimG4CMS_HGCMouseBite_h

#include "Geometry/HGCalCommonData/interface/HGCalDDDConstants.h"
#include "G4ThreeVector.hh"

#include <vector>

class HGCMouseBite {

public:    

  HGCMouseBite(const HGCalDDDConstants& hgc, const std::vector<double>& angle,
	       double maxLength, bool waferRotate); 
  virtual ~HGCMouseBite();
  bool                  exclude(G4ThreeVector& point, int zside, int wafer);

private:    

  const HGCalDDDConstants&      hgcons_;
  double                        cut_;
  bool                          rot_;
  std::vector<double>           projX_, projY_;
};

#endif // HGCMouseBite_h
