#ifndef RecoTracker_FastTimeMatching_FTLBarrelGeomDet_h
#define RecoTracker_FastTimeMatching_FTLBarrelGeomDet_h

/// Class corresponding to one sensor of the FTL barrel

#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "DataFormats/ForwardDetId/interface/FastTimeDetId.h"

class FTLBarrelGeomDet : public GeomDet {
 public:
  FTLBarrelGeomDet(DetId id, const GlobalPoint& pos, const Plane::RotationType&,float width, float length);
	
};

#endif
