#include "RecoTracker/FastTimeMatching/interface/FTLBarrelSectorGeomDet.h"
#include "DataFormats/GeometrySurface/interface/MediumProperties.h"
#include "DataFormats/GeometrySurface/interface/BoundPlane.h"

#include "DataFormats/GeometrySurface/interface/RectangularPlaneBounds.h"

FTLBarrelSectorGeomDet::FTLBarrelSectorGeomDet(DetId id, Plane::PlanePointer detPlane, const std::vector<std::pair<DetId,const GeomDet*> >& components, float radlen, float xi) :
  GeomDet( detPlane ),
  id_(id)
{
  setDetId(FastTimeDetId(id_.type(),0,id_.iphi(),id_.zside()));

  for( unsigned i = 0; i < components.size(); ++i ) {
    indexMap_.emplace(components[i].first,i);
    components_.emplace_back(components[i].second);
  }

  if (radlen > 0) {
    (const_cast<Plane &>(surface())).setMediumProperties(MediumProperties(radlen,xi));
  }

}

