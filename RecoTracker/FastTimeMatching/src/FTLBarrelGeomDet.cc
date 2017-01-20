#include "RecoTracker/FastTimeMatching/interface/FTLBarrelGeomDet.h"
#include "DataFormats/GeometrySurface/interface/MediumProperties.h"
#include "DataFormats/GeometrySurface/interface/BoundPlane.h"
#include "DataFormats/GeometrySurface/interface/RectangularPlaneBounds.h"

FTLBarrelGeomDet::FTLBarrelGeomDet(DetId id, const GlobalPoint& pos, const Plane::RotationType& rot, float width, float length) :
  GeomDet( Plane::build(pos, rot, new RectangularPlaneBounds(0.5f*width,0.5f*length,0.5f)).get() )
{
  setDetId(id);
}

