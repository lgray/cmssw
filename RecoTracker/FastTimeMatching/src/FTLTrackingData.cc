#include "RecoTracker/FastTimeMatching/interface/FTLTrackingData.h"
#include "RecoTracker/FastTimeMatching/interface/ftldebug.h"
#include <cstdio>

void
FTLTrackingData::addData(const edm::Handle<FTLTrackingData::TColl> &data, int subdet) 
{
    if (ftltracking::g_debuglevel > 0) printf("Adding data for subdet %d, %lu total hits\n", subdet, data->size());
    for (int zside = -1; zside <= +1; zside += 2) {
        for (unsigned int idisk = 0, ndisk = tracker_->numberOfDisks(zside); idisk < ndisk; ++idisk) {
            const  FTLDiskGeomDet *disk = tracker_->disk(zside, idisk);
            if (disk->subdet() != subdet) continue;
            data_[disk] = Disk(data, subdet, zside, disk->layer(), cpe_);
            printf("Added DiskData @%p for %1d/%+1d/%2d with %u hits\n", disk, subdet, zside, disk->layer(), data_[disk].size());
        }
    }
}

void
FTLTrackingData::addClusters(const edm::Handle<reco::CaloClusterCollection> &data) 
{
    if (ftltracking::g_debuglevel > 0) printf("Adding clusters, %lu total\n", data->size());
    for (int zside = -1; zside <= +1; zside += 2) {
        for (unsigned int idisk = 0, ndisk = tracker_->numberOfDisks(zside); idisk < ndisk; ++idisk) {
            const  FTLDiskGeomDet *disk = tracker_->disk(zside, idisk);
            data_[disk].addClusters(data, disk->subdet(), zside, disk->layer());
            //printf("Added clusters for %1d/%+1d/%2d with %u hits\n", disk->subdet(), zside, disk->layer(), data_[disk].nclusters());
        }
    }
}

void
FTLTrackingData::throwBadDisk_(const FTLDiskGeomDet *disk) const
{
    printf("Missing disk %p with  %1d/%+1d/%2d\n", disk, disk->subdet(), disk->zside(), disk->layer());
    for (const auto &p : data_) {
        printf("  available: %p with  %1d/%+1d/%2d ?  %d \n", p.first, p.first->subdet(), p.first->zside(), p.first->layer(), p.first == disk);
    }
    throw cms::Exception("LogicError", "lookin for a non-existent disk\n");
}
