#ifndef RecoTracker_FastTimeMatching_FTLBarrelDetLayer_h
#define RecoTracker_FastTimeMatching_FTLBarrelDetLayer_h

/// Class corresponding to one layer of FTL barrel

#include "TrackingTools/DetLayers/interface/BarrelDetLayer.h"
#include "DataFormats/ForwardDetId/interface/FastTimeDetId.h"
#include "RecoTracker/FastTimeMatching/interface/FTLBarrelSectorGeomDet.h"

#include <unordered_map>

// this represents a cylinder of one side of the timing barrel
// i.e. the total z-length of one iphi sector

class FTLBarrelDetLayer {
 public:
  FTLBarrelDetLayer(DetId id, const std::unordered_multimap<uint32_t,std::pair<DetId,const GeomDet*> >& sectors, float radlen, float xi);
  
  ~FTLBarrelDetLayer() {
    for( auto* ptr : sectors_ ) delete ptr;
  }
  
  int ftlType() const { return id_.type(); }
  int zside() const { return id_.zside(); }
  int layer() const { return 0; }

  DetId geographicalId() const { return id_; }
  
  bool operator<(const FTLBarrelDetLayer &other) const { 
    if( id_.type() == other.ftlType() ) return id_.zside() < other.zside();
    return id_.type() < other.ftlType();
  }
  
  const std::vector<const FTLBarrelSectorGeomDet*>& sectors() const { return sectors_; }
  
  const Cylinder* surface() const { return cylinder_.get(); }
  
 protected:
  const FastTimeDetId id_;
  std::vector<const FTLBarrelSectorGeomDet*> sectors_;
  Cylinder::CylinderPointer cylinder_;
};

#endif
