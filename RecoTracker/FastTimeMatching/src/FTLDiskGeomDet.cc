#include "RecoTracker/FastTimeMatching/interface/FTLDiskGeomDet.h"
#include "DataFormats/GeometrySurface/interface/MediumProperties.h"
#include "DataFormats/GeometrySurface/interface/BoundDisk.h"
#include "DataFormats/GeometrySurface/interface/BoundCylinder.h"

FTLDiskGeomDet::FTLDiskGeomDet(int type, int zside, int layer, float z, float rmin, float rmax, float radlen, float xi) :
            GeomDet( Disk::build(Disk::PositionType(0,0,z), Disk::RotationType(), SimpleDiskBounds(rmin, rmax, -20, 20)).get() ),
            type_(type), zside_(zside), layer_(layer) 
{
  setDetId(FastTimeDetId(type,0,0,zside));
  
  if (radlen > 0) {
    (const_cast<Plane &>(surface())).setMediumProperties(MediumProperties(radlen,xi));
  }
}

