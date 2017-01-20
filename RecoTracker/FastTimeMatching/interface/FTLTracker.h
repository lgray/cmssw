#ifndef RecoTracker_FastTimeMatching_FTLTracker_h
#define RecoTracker_FastTimeMatching_FTLTracker_h

/// Class holding all the layers of the FTL Tracker
/// With some more effort it could inherit from TrackingGeometry etc
// - holds (and owns) all the disks
// - implements navigation and lookup by id

#include "RecoTracker/FastTimeMatching/interface/FTLDiskGeomDet.h"
#include "RecoTracker/FastTimeMatching/interface/FTLBarrelDetLayer.h"
#include "Geometry/HGCalGeometry/interface/FastTimeGeometry.h"
#include "DataFormats/TrajectorySeed/interface/PropagationDirection.h"

#include <unordered_map>

class FTLTracker {
    public:
  FTLTracker(const FastTimeGeometry *barrel, const FastTimeGeometry *endcap) ;
  ~FTLTracker() { 
    for (auto* disk : disksPos_) delete disk;
    for (auto* disk : disksNeg_) delete disk;
    for (auto* layer : cylindersPos_ ) delete layer;
    for (auto* layer : cylindersNeg_ ) delete layer;
  }
  
  // disable copy constructor and assignment
  FTLTracker(const FTLTracker &other) = delete;
  FTLTracker & operator=(const FTLTracker &other) = delete;
  
  // endcap specific
  unsigned int numberOfEndcapLayers(int zside) const { return (zside > 0 ? disksPos_ : disksNeg_).size(); }
  const FTLDiskGeomDet * endcapLayer(int zside, int disk) const { return (zside > 0 ? disksPos_ : disksNeg_).at(disk); }
  const FTLDiskGeomDet * firstEndcapLayer(int zside) const { return (zside > 0 ? disksPos_ : disksNeg_).front(); }
  const FTLDiskGeomDet * lastEndcapLayer(int zside) const { return (zside > 0 ? disksPos_ : disksNeg_).back(); }
  const FTLDiskGeomDet * firstEndcapLayer(int zside, PropagationDirection direction) const { return direction == alongMomentum ? firstEndcapLayer(zside) : lastEndcapLayer(zside); }
  const FTLDiskGeomDet * lastEndcapLayer(int zside, PropagationDirection direction) const { return direction == alongMomentum ? lastEndcapLayer(zside) : firstEndcapLayer(zside); }
  
  const FTLDiskGeomDet * nextEndcapLayer(const FTLDiskGeomDet *from, PropagationDirection direction=alongMomentum) const ;
  const FTLDiskGeomDet * idToEndcapDet(DetId id) const ;
  
  //barrel specific
  unsigned int numberOfBarrelLayers(int zside) const { return (zside > 0 ? cylindersPos_ : cylindersNeg_).size(); }
  const FTLBarrelDetLayer * barrelLayer(int zside, int layer) const { return (zside > 0 ? cylindersPos_ : cylindersNeg_).at(layer); }
  const FTLBarrelDetLayer * firstBarrelLayer(int zside) const { return (zside > 0 ? cylindersPos_ : cylindersNeg_).front(); }
  const FTLBarrelDetLayer * lastBarrelLayer(int zside) const { return (zside > 0 ? cylindersPos_ : cylindersNeg_).back(); }
  const FTLBarrelDetLayer * firstBarrelLayer(int zside, PropagationDirection direction) const { return direction == alongMomentum ? firstBarrelLayer(zside) : lastBarrelLayer(zside); }
  const FTLBarrelDetLayer * lastBarrelLayer(int zside, PropagationDirection direction) const { return direction == alongMomentum ? lastBarrelLayer(zside) : firstBarrelLayer(zside); }

  const FTLBarrelDetLayer * nextBarrelLayer(const FTLBarrelDetLayer *from, PropagationDirection direction=alongMomentum) const ;
  const FTLBarrelDetLayer * idToBarrelDet(DetId id) const ;
  
 protected:
  const FastTimeGeometry *barrelGeom_, *endcapGeom_;
  // store hash maps of geographical det id to corresponding detector element
  // original vectors are sorted by layer number
  std::vector<FTLDiskGeomDet*> disksPos_, disksNeg_;
  std::unordered_map<uint32_t,unsigned> disksLookupPos_, disksLookupNeg_;
  // each cylinder is composed of 720 iphi sectors
  std::vector<FTLBarrelDetLayer*> cylindersPos_, cylindersNeg_;
  std::unordered_map<uint32_t,unsigned> cylindersLookupPos_, cylindersLookupNeg_;
  
  void makeDisks(int subdet, int disks) ;
  void makeCylinders(int subdet, int layers) ;
  void addEndcapLayer(FTLDiskGeomDet *layer) { 
    (layer->zside() > 0 ? disksPos_ : disksNeg_).emplace_back(layer);
  }

  void addBarrelLayer(FTLBarrelDetLayer *layer) { 
    (layer->zside() > 0 ? cylindersPos_ : cylindersNeg_).emplace_back(layer);
  }
  
};

#endif
