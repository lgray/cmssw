#ifndef ETLDetLayerGeometryBuilder_h
#define ETLDetLayerGeometryBuilder_h

/** \class ETLDetLayerGeometryBuilder
 *
 *  Build the ETL DetLayers.
 *
 *  \author L. Gray - FNAL
 */

#include <Geometry/MTDGeometry/interface/ETLGeometry.h>
#include <vector>

class DetLayer;
class MTDRingForwardDoubleLayer;
class MTDDetRing;

class ETLDetLayerGeometryBuilder {
 public:

  /// return.first=forward (+Z), return.second=backward (-Z)
  /// both vectors are sorted inside-out
  static std::pair<std::vector<DetLayer*>, std::vector<DetLayer*> > buildLayers(const ETLGeometry& geo);
 private:
  // Disable constructor - only static access is allowed.
  ETLDetLayerGeometryBuilder(){}

  static MTDRingForwardDoubleLayer* buildLayer(int endcap,
                                               int station,
                                               std::vector<int>& rings,
                                               const ETLGeometry& geo);
  
  static MTDDetRing * makeDetRing(std::vector<const GeomDet*> & geomDets);
  static bool isFront(int station, int ring, int chamber);
};
#endif

