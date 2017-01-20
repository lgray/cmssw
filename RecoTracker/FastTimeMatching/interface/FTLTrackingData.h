#ifndef RecoTracker_FastTimeMatching_FTLTrackingData_h
#define RecoTracker_FastTimeMatching_FTLTrackingData_h

#include "RecoTracker/FastTimeMatching/interface/FTLTrackingLayerData.h"
#include "RecoTracker/FastTimeMatching/interface/FTLBarrelSectorGeomDet.h"
#include "RecoTracker/FastTimeMatching/interface/FTLTracker.h"
#include <unordered_map>

/// Class that holds the rechits of FTL Layer, and does the lookup
//  Almost like a MeasurementTrackerEvent in the tracker
//
//  note: the class may be expensive both to copy and to move


class FTLTrackingData {
 public:
  typedef FTLTrackingLayerData Layer;
  typedef Layer::TColl TColl;
  
 FTLTrackingData(const FTLTracker &tracker, 
		 const FTLTrackingBasicCPE *cpeBarrel, 
		 const FTLTrackingBasicCPE *cpeEndcap) : 
  tracker_(&tracker), cpeBarrel_(cpeBarrel), cpeEndcap_(cpeEndcap) {}
  
  void addData(const edm::Handle<TColl>& data, int type) ;
  void addData(const edm::Handle<TColl>& data, int type, int iphi);
  void addClusters(const edm::Handle<reco::CaloClusterCollection>& data) ;
  const Layer & layerData(const FTLDiskGeomDet *disk) const { 
    auto it = dataEndcap_.find(disk);
    if (it == dataEndcap_.end()) throwBadDisk_(disk);
    return it->second; 
  }
  
  const Layer & layerData(const FTLBarrelSectorGeomDet *sector) const { 
    auto it = dataBarrel_.find(sector);
    if (it == dataBarrel_.end()) throwBadSector_(sector);
    return it->second; 
  }
  
 private:
  const FTLTracker *tracker_;
  const FTLTrackingBasicCPE *cpeBarrel_, *cpeEndcap_;
  // FIXME better type later
  std::unordered_map<const FTLBarrelSectorGeomDet *, Layer> dataBarrel_;
  std::unordered_map<const FTLDiskGeomDet *, Layer> dataEndcap_; 
  
  void throwBadDisk_(const FTLDiskGeomDet *disk) const ;
  void throwBadSector_(const FTLBarrelSectorGeomDet *sector) const ;
};


#endif
