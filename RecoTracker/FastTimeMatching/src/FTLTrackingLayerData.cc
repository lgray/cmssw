#include "RecoTracker/FastTimeMatching/interface/FTLTrackingLayerData.h"
#include "RecoTracker/FastTimeMatching/interface/ftldebug.h"

namespace {
        struct FTLBySideAndEta {
            bool operator()(const FTLRecHit &hit1, const FTLRecHit &hit2) const { return comp(FastTimeDetId(hit1.id()), FastTimeDetId(hit2.id())); }
            bool operator()(FastTimeDetId id1, const FTLRecHit &hit2) const { return comp(id1, FastTimeDetId(hit2.id())); }
            bool operator()(const FTLRecHit &hit1, FastTimeDetId id2) const { return comp(FastTimeDetId(hit1.id()),id2); }
            bool comp(FastTimeDetId id1, FastTimeDetId id2) const {
	      if( id1.zside() == id2.zside() ) {
		return id1.ieta() < id2.ieta();
	      }
	      return id1.zside() < id2.zside();
            }
        };
}

FTLTrackingLayerData::FTLTrackingLayerData(const edm::Handle<FTLTrackingLayerData::TColl> &data, int type, int zside, int layer, const FTLTrackingBasicCPE *cpe) :
    alldata_(&data), 
    cpe_(cpe),truthMap_(0)
{
    index_.clear();
    for( auto it = data->begin(); it != data->end(); ++it ) {
      FastTimeDetId temp(it->id());      
      if( temp.zside() == zside && 
	  temp.type() == type ) {	
	index_.emplace_back(cpe_->hint(*it),it);
      }
    }
    buildIndex_();
}

FTLTrackingLayerData::FTLTrackingLayerData(const edm::Handle<FTLTrackingLayerData::TColl> &data, int type, int zside, int iphi, int layer, const FTLTrackingBasicCPE *cpe) :
    alldata_(&data), 
    cpe_(cpe),truthMap_(0)
{
    index_.clear();
    for( auto it = data->begin(); it != data->end(); ++it ) {
      FastTimeDetId temp(it->id());      
      if( temp.zside() == zside && 
	  temp.type() == type &&
	  temp.iphi() == iphi ) {
	index_.emplace_back(cpe_->hint(*it),it);
      }
    }
    buildIndex_();
}

void FTLTrackingLayerData::addClusters(const edm::Handle<reco::CaloClusterCollection> &data, int type, int zside, int layer) 
{
    clusterData_ = & data;
    clusterIndex_.clear();
    for (reco::CaloClusterCollection::const_iterator it = data->begin(), ed = data->end(); it != ed; ++it) {
        FastTimeDetId firstid = it->hitsAndFractions().front().first;
	if (firstid.type() != type) continue;	
	if( firstid.zside() != zside ) {
	  continue;
	}        
        clusterIndex_.emplace_back(cpe_->hint(*it), it);
    }
}

std::vector<TrajectoryMeasurement> FTLTrackingLayerData::measurements(TrajectoryStateOnSurface &tsos, const MeasurementEstimator &mest) const 
{
    std::vector<TrajectoryMeasurement> ret;
    GlobalPoint lp = tsos.globalPosition();
    const LocalError & loce = tsos.localError().positionError();
    float window = 4*std::sqrt(std::max(loce.xx(), loce.yy()));
    for (const auto &pair : index_) {
        if (std::max(std::abs(lp.x()-pair.first.x), std::abs(lp.y()-pair.first.y)) < std::max(2.f,window + 3*pair.first.size)) {
            const value_type & obj = *pair.second;
            auto const & params = cpe_->localParameters(obj, tsos.surface());
            auto hitptr = std::make_shared<FTLTrackingRecHitFromHit>(obj.id(), 
                    ref_type(*alldata_, pair.second - (*alldata_)->begin()),  
                    params.first, params.second);
            auto est_pair = mest.estimate(tsos, *hitptr);
            float energy = obj.energy();
            LocalTrajectoryError lte = tsos.localError();
            if (est_pair.first) {
	      ret.emplace_back(tsos, hitptr, est_pair.second);
            }
            if (ftltracking::g_debuglevel > 2) {
                if (est_pair.second > 400) continue;
                //printf("\t\tstate at x = %+7.2f +- %4.2f   y = %+7.2f +- %4.2f   hit at x = %+7.2f  y = %+7.2f    energy %7.3f   dist = %5.1f chi2 = %8.1f   pass = %1d ",
                //    lp.x(), sqrt(loce.xx()), lp.y(), sqrt(loce.yy()), pair.first.x, pair.first.y, energy, hypot(lp.x()-pair.first.x,lp.y()-pair.first.y), est_pair.second, est_pair.first);
                printf("\t\tstate at x = %+7.2f   y = %+7.2f  dxy = %4.2f   hit %12d at x = %+7.2f  y = %+7.2f    energy %7.3f   dist = %5.1f chi2 = %8.1f   pass = %1d ",
                        lp.x(), lp.y(), sqrt(loce.xx()+loce.yy()), pair.second->id().rawId(), pair.first.x, pair.first.y, energy, hypot(lp.x()-pair.first.x,lp.y()-pair.first.y), est_pair.second, est_pair.first);
                if (truthMap_) {
                    auto range = truthMap_->equal_range(pair.second->id().rawId());
                    for (; range.first != range.second; ++range.first) {
                        const auto &pair = *range.first;
                        const auto &p = *pair.second.first;
                        printf("    from %s pdgId %+d eid %d/%d pt %.1f eta %+5.2f phi %+5.2f ",
                                (p.eventId().event()==0&&p.eventId().bunchCrossing()==0 ? "SIGNAL" : "pileup"),
                                p.pdgId(), p.eventId().event(), p.eventId().bunchCrossing(), p.pt(), p.eta(), p.phi());
                        if (pair.second.second < 0.999) printf("(frac %.2f) ", pair.second.second); 
                    }
                }
                printf("\n");
            }
        }
    }
    return ret;
}

std::vector<TrajectoryMeasurement> FTLTrackingLayerData::clusterizedMeasurements(TrajectoryStateOnSurface &tsos, const MeasurementEstimator &mest, float rCut) const
{
    std::vector<TrajectoryMeasurement> ret;
    GlobalPoint lp = tsos.globalPosition();
    const LocalError & loce = tsos.localError().positionError();
    float window = 4*std::sqrt(std::max(loce.xx(), loce.yy()));
    std::vector<std::pair<FTLTrackingBasicCPE::PositionHint,const_iterator>> selHits;
    std::vector<TrackingRecHit::ConstRecHitPointer> hitptrs;
    std::vector<std::pair<bool,float>> estpairs;
    std::vector<int> selids;
    unsigned int ihit = 0;
    for (const auto &pair : index_) {
        if (std::max(std::abs(lp.x()-pair.first.x), std::abs(lp.y()-pair.first.y)) < std::max(2.f,window + 3*pair.first.size)) {
            const value_type & obj = *pair.second;
            auto const & params = cpe_->localParameters(obj, tsos.surface());
            auto hitptr = std::make_shared<FTLTrackingRecHitFromHit>(obj.id(), 
                    ref_type(*alldata_, pair.second - (*alldata_)->begin()),  
                    params.first, params.second);
            auto est_pair = mest.estimate(tsos, *hitptr);
            float energy = obj.energy();
            LocalTrajectoryError lte = tsos.localError();
            if (est_pair.first) {
                selids.push_back(ihit);
                selHits.push_back(pair);
                hitptrs.push_back(hitptr);
                estpairs.push_back(est_pair);
            }
            if (ftltracking::g_debuglevel > 2) {
                if (est_pair.second > 400) continue;
                printf("\t\t%3d state at x = %+7.2f +- %4.2f   y = %+7.2f +- %4.2f   hit at x = %+7.2f  y = %+7.2f    energy %7.3f   dist = %5.1f chi2 = %8.1f   pass = %1d ", ++ihit,
                        lp.x(), sqrt(loce.xx()), lp.y(), sqrt(loce.yy()), pair.first.x, pair.first.y, energy, hypot(lp.x()-pair.first.x,lp.y()-pair.first.y), est_pair.second, est_pair.first);
                if (truthMap_) {
                    auto range = truthMap_->equal_range(pair.second->id().rawId());
                    for (; range.first != range.second; ++range.first) {
                        const auto &pair = *range.first;
                        const auto &p = *pair.second.first;
                        printf("    from %s pdgId %+d eid %d/%d pt %.1f eta %+5.2f phi %+5.2f ",
                                (p.eventId().event()==0&&p.eventId().bunchCrossing()==0 ? "SIGNAL" : "pileup"),
                                p.pdgId(), p.eventId().event(), p.eventId().bunchCrossing(), p.pt(), p.eta(), p.phi());
                        if (pair.second.second < 0.999) printf("(frac %.2f) ", pair.second.second); 
                    }
                }
                printf("\n");
            }
        }
    }
    std::vector<int> above, below;
    for (unsigned int i = 0, n = selHits.size(); i < n; ++i) {
        float myene = selHits[i].second->energy(); 
        above.clear(); below.clear();
        if (ftltracking::g_debuglevel > 2) printf("\t\t%3d energy %7.3f neighbours: ",selids[i], myene); 
        for (unsigned int j = 0; j < n; ++j) {
            if (i == j) continue;
            float dist = hypot(selHits[i].first.x-selHits[j].first.x, selHits[i].first.y-selHits[j].first.y);
            if (dist < rCut) {
                float hisene = selHits[j].second->energy();
                if (hisene > myene || (hisene == myene && i < j)) {
                    if (ftltracking::g_debuglevel > 2) printf("%d (d=%.1f, e=%.3f, above)    ", selids[j], dist, hisene);
                    above.push_back(j); 
                } else {
                    if (ftltracking::g_debuglevel > 2) printf("%d (d=%.1f, e=%.3f, below)    ", selids[j], dist, hisene);
                    below.push_back(j); 
                }
            }
        }
        if (above.empty()) {
            if (below.empty()) {
                if (ftltracking::g_debuglevel > 2) printf("<- isolated rechit.");
                ret.emplace_back(tsos, hitptrs[i], estpairs[i].second);
            } else {
                if (ftltracking::g_debuglevel > 2) printf("<- local maxima, will clusterize.\n");
                refvector_type hits;
                float esum = myene, esumx = myene * selHits[i].first.x, esumy = myene * selHits[i].first.y;
                hits.push_back( ref_type(*alldata_, selHits[i].second - (*alldata_)->begin()) );
                for (int j : below) {
                    float hisene = selHits[j].second->energy();
                    esum  += hisene;
                    esumx += hisene * selHits[j].first.x;
                    esumy += hisene * selHits[j].first.y;
                    hits.push_back( ref_type(*alldata_, selHits[j].second - (*alldata_)->begin()) );
                }
                auto chitptr = std::make_shared<FTLTrackingClusteringRecHit>(selHits[i].second->id(), 
                        hits, esum,  LocalPoint(esumx/esum, esumy/esum, 0), hitptrs[i]->localPositionError());
                auto cest_pair = mest.estimate(tsos, *chitptr);
                if (ftltracking::g_debuglevel > 2) {
                    printf("\t\t                                                    %3lu hits at x = %+7.2f  y = %+7.2f    energy %7.3f   dist = %5.1f chi2 = %8.1f   pass = %1d ", 
                            hits.size(), esumx/esum, esumy/esum, esum, hypot(lp.x()-esumx/esum,lp.y()-esumy/esum), cest_pair.second, cest_pair.first);
                }
                if (cest_pair.first) {
                    ret.emplace_back(tsos, chitptr, cest_pair.second);
                }
            }
        }
        if (ftltracking::g_debuglevel > 2) printf("\n");
    }
    return ret;
}

std::vector<TrajectoryMeasurement> FTLTrackingLayerData::clusterMeasurements(TrajectoryStateOnSurface &tsos, const MeasurementEstimator &mest) const 
{
    std::vector<TrajectoryMeasurement> ret;
    GlobalPoint lp = tsos.globalPosition();
    const LocalError & loce = tsos.localError().positionError();
    float window = 4*std::sqrt(std::max(loce.xx(), loce.yy()));
    for (const auto &pair : clusterIndex_) {
        if (std::max(std::abs(lp.x()-pair.first.x), std::abs(lp.y()-pair.first.y)) < std::max(2.f,window + 3*pair.first.size)) {
            const reco::CaloCluster & obj = *pair.second;
            auto const & params = cpe_->localParameters(obj, tsos.surface());
            auto hitptr = std::make_shared<FTLTrackingRecHitFromCluster>(obj.hitsAndFractions().front().first, 
                    reco::CaloClusterPtr(*clusterData_, pair.second - (*clusterData_)->begin()),  
                    params.first, params.second);
            auto est_pair = mest.estimate(tsos, *hitptr);
            float energy = obj.energy();
            LocalTrajectoryError lte = tsos.localError();
            if (est_pair.first) {
                ret.emplace_back(tsos, hitptr, est_pair.second);
            }
            if (ftltracking::g_debuglevel > 2) {
                if (est_pair.second > 400) continue;
                //printf("\t\tstate at x = %+7.2f +- %4.2f   y = %+7.2f +- %4.2f   hit at x = %+7.2f  y = %+7.2f    energy %7.3f   dist = %5.1f chi2 = %8.1f   pass = %1d ",
                //    lp.x(), sqrt(loce.xx()), lp.y(), sqrt(loce.yy()), pair.first.x, pair.first.y, energy, hypot(lp.x()-pair.first.x,lp.y()-pair.first.y), est_pair.second, est_pair.first);
                printf("\t\tstate at x = %+7.2f   y = %+7.2f  dxy = %4.2f   cluster %8d at x = %+7.2f  y = %+7.2f    energy %7.3f   dist = %5.1f chi2 = %8.1f   pass = %1d\n",
                        lp.x(), lp.y(), sqrt(loce.xx()+loce.yy()), int(hitptr->objRef().key()), pair.first.x, pair.first.y, energy, hypot(lp.x()-pair.first.x,lp.y()-pair.first.y), est_pair.second, est_pair.first);
                for (const auto & hitAndF : pair.second->hitsAndFractions()) {
                    if (hitAndF.second == 0) continue;
                    GlobalPoint hitgp = cpe_->getPosition(hitAndF.first);
                    printf("\t\t                                                 hit %12d at x = %+7.2f  y = %+7.2f    fract.   %5.3f   dist = %5.1f                            ",
                            hitAndF.first.rawId(), hitgp.x(), hitgp.y(), hitAndF.second, hypot(lp.x()-hitgp.x(),lp.y()-hitgp.y()));
                    if (truthMap_) {
                        auto range = truthMap_->equal_range(hitAndF.first.rawId());
                        for (; range.first != range.second; ++range.first) {
                            const auto &pair = *range.first;
                            const auto &p = *pair.second.first;
                            printf("    from %s pdgId %+d eid %d/%d pt %.1f eta %+5.2f phi %+5.2f ",
                                    (p.eventId().event()==0&&p.eventId().bunchCrossing()==0 ? "SIGNAL" : "pileup"),
                                    p.pdgId(), p.eventId().event(), p.eventId().bunchCrossing(), p.pt(), p.eta(), p.phi());
                            if (pair.second.second < 0.999) printf("(frac %.2f) ", pair.second.second); 
                        }
                    }
                    printf("\n");
                }
            }
        }
    }
    return ret;
}

