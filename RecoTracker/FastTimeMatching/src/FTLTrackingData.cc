#include "RecoTracker/FastTimeMatching/interface/FTLTrackingData.h"
#include "RecoTracker/FastTimeMatching/interface/ftldebug.h"
#include <cstdio>

void
FTLTrackingData::addData(const edm::Handle<FTLTrackingData::TColl> &data, int type) 
{
  if (ftltracking::g_debuglevel > 0) printf("Adding data for subdet %d, %lu total hits\n", type, data->size());
  for (int zside = -1; zside <= +1; zside += 2) {
    for (unsigned int idisk = 0, ndisk = tracker_->numberOfEndcapLayers(zside); idisk < ndisk; ++idisk) {
      const  FTLDiskGeomDet *disk = tracker_->endcapLayer(zside, idisk);
      if (disk->ftlType() != type) continue;
      dataEndcap_[disk] = Layer(data, type, zside, disk->layer(), cpeEndcap_);
      printf("Added LayerData @%p for Disk  : %1d/%+1d/%2d with %u hits\n", disk, type, zside, disk->layer(), dataEndcap_[disk].size());
    }
  }    
}

void
FTLTrackingData::addData(const edm::Handle<FTLTrackingData::TColl> &data, int type, int iphi) 
{
  if (ftltracking::g_debuglevel > 0) printf("Adding data for subdet %d/%d, %lu total hits\n", type,iphi, data->size());
  for (int zside = -1; zside <= +1; zside += 2) {
    for (unsigned int ilayer = 0, nlayer = tracker_->numberOfBarrelLayers(zside); ilayer < nlayer; ++ilayer) {
      const  FTLBarrelSectorGeomDet *sector = tracker_->barrelLayer(zside, ilayer)->sectors()[iphi];
      if (sector->ftlType() != type && sector->iphi() != iphi) continue;
      dataBarrel_[sector] = Layer(data, type, zside, iphi, sector->layer(), cpeBarrel_);
      printf("Added LayerData @%p for Sector: %1d/%+1d/%3d/%2d with %u hits\n", sector, type, zside, iphi, sector->layer(), dataBarrel_[sector].size());
    }
  }
}

void
FTLTrackingData::addClusters(const edm::Handle<reco::CaloClusterCollection> &data) 
{
  if (ftltracking::g_debuglevel > 0) printf("Adding clusters, %lu total\n", data->size());
  for (int zside = -1; zside <= +1; zside += 2) {
    for (unsigned int idisk = 0, ndisk = tracker_->numberOfEndcapLayers(zside); idisk < ndisk; ++idisk) {
      const  FTLDiskGeomDet *disk = tracker_->endcapLayer(zside, idisk);
      dataEndcap_[disk].addClusters(data, disk->ftlType(), zside, disk->layer());
      //printf("Added clusters for %1d/%+1d/%2d with %u hits\n", disk->subdet(), zside, disk->layer(), data_[disk].nclusters());
    }
  }
}

void
FTLTrackingData::throwBadDisk_(const FTLDiskGeomDet *disk) const
{
  printf("Missing disk %p with  %1d/%+1d/%2d\n", disk, disk->ftlType(), disk->zside(), disk->layer());
  for (const auto &p : dataEndcap_) {
    printf("  available: %p with  %1d/%+1d/%2d ?  %d \n", p.first, p.first->ftlType(), p.first->zside(), p.first->layer(), p.first == disk);
  }
  throw cms::Exception("LogicError", "looking for a non-existent disk\n");
}

void
FTLTrackingData::throwBadSector_(const FTLBarrelSectorGeomDet *sector) const
{
  printf("Missing sector %p with  %1d/%+1d/%3d/%2d\n", sector, sector->ftlType(), sector->zside(), sector->iphi(), sector->layer());
  for (const auto &p : dataBarrel_) {
    printf("  available: %p with  %1d/%+1d/%3d/%2d ?  %d \n", p.first, p.first->ftlType(), p.first->zside(), p.first->iphi(), p.first->layer(), p.first == sector);
  }
  throw cms::Exception("LogicError", "looking for a non-existent sector\n");
}
