#ifndef Geometry_MTDGeometryBuilder_MTDGeomBuilderFromGeometricTimingDet_H
#define Geometry_MTDGeometryBuilder_MTDGeomBuilderFromGeometricTimingDet_H

#include <string>
#include <vector>
#include <map>
#include "Geometry/MTDGeometryBuilder/interface/PlaneBuilderFromGeometricTimingDet.h"
#include "Geometry/MTDNumberingBuilder/interface/GeometricTimingDet.h"
#include "Geometry/CommonDetUnit/interface/GeomDetType.h"

class MTDGeometry;
class MTDTopology;
class PixelGeomDetType;
class StripGeomDetType;
class PMTDParameters;

class MTDGeomBuilderFromGeometricTimingDet {
public:

  MTDGeometry* build(const GeometricTimingDet* gd, 
		     const PMTDParameters & ptp, 
		     const MTDTopology* tTopo);

private:

  void buildPixel(std::vector<const GeometricTimingDet*> const &,
		  MTDGeometry*,GeomDetType::SubDetector det,
		  const PMTDParameters& ptp );
  void buildSilicon(std::vector<const GeometricTimingDet*> const &,
		    MTDGeometry*,GeomDetType::SubDetector det, const std::string& part);
  void buildGeomDet(MTDGeometry* );

  PlaneBuilderFromGeometricTimingDet::ResultType
  buildPlaneWithMaterial(const GeometricTimingDet* gd,double scaleFactor=1.) const;

  std::map<std::string,const PixelGeomDetType*> thePixelDetTypeMap;
  std::map<std::string,const StripGeomDetType*> theStripDetTypeMap;
  const MTDTopology* theTopo;
};

#endif
