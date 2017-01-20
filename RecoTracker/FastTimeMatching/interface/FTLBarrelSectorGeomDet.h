#ifndef RecoTracker_FastTimeMatching_FTLBarrelSectorGeomDet_h
#define RecoTracker_FastTimeMatching_FTLBarrelSectorGeomDet_h

/// Class corresponding to one layer of FTL barrel

#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "DataFormats/ForwardDetId/interface/FastTimeDetId.h"
#include <unordered_map>

// this represents a single "slat" in phi of one side of the timing barrel
// i.e. the total z-length of one iphi sector

class FTLBarrelSectorGeomDet : public GeomDet {
 public:
  FTLBarrelSectorGeomDet(DetId id, Plane::PlanePointer detPlane, const std::vector<std::pair<DetId, const GeomDet*> >& dets, float radlen, float xi) ;

  ~FTLBarrelSectorGeomDet() {
    for( auto ptr : components_ ) delete ptr;
  }
  
  int layer() const { return 0; }
  int ftlType() const { return id_.type(); }
  int zside() const { return id_.zside(); }
  int iphi() const { return id_.iphi(); }
  
  bool operator<(const FTLBarrelSectorGeomDet &other) const { 
    if( id_.type() == other.ftlType() && id_.zside() == other.zside() ) return id_.iphi() < other.iphi();
    else if( id_.type() == other.ftlType() ) return id_.zside() < other.zside();
    return id_.type() < other.ftlType();    
  }
  
  virtual std::vector<const GeomDet*> components() const override { return components_; }
  virtual const GeomDet* component(DetId id) const override {
    auto itr = indexMap_.find(id);
    if( itr != indexMap_.end() ) return components_[itr->second];
    return nullptr;
  }

 protected:
  const FastTimeDetId id_;
  std::vector<const GeomDet*> components_;
  std::unordered_map<uint32_t,unsigned> indexMap_;
};

#endif
