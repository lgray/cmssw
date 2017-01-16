#ifndef RecoTracker_FastTimeMatching_TrajectoryCleanerBySharedEndpoints_h
#define RecoTracker_FastTimeMatching_TrajectoryCleanerBySharedEndpoints_h

/// Trajectory cleaner that merges trajectories that share the last rec hit
//  Could become a real TrajectoryCleaner with some effort
//  Commented out code allow for requiring also the shared first hit, or he last two hits

#include <vector>
#include "TrackingTools/PatternTools/interface/Trajectory.h"
#include "TrackingTools/PatternTools/interface/TempTrajectory.h"

class TrajectoryCleanerBySharedEndpoints {
  public:
      TrajectoryCleanerBySharedEndpoints(float foundHitBonus, float lostHitPenalty) :
        theFoundHitBonus(foundHitBonus), theLostHitPenalty(lostHitPenalty) {}

      void clean(std::vector<Trajectory> &trajs) const ;
      void clean(std::vector<TempTrajectory> &trajs) const ;

  private:
      float theFoundHitBonus, theLostHitPenalty;

      template<typename Traj> void clean_(std::vector<Traj> &trajs) const ;

      template<typename Traj> bool cmp(const Traj &a, const Traj &b) const {
          return  (a.chiSquared() + a.lostHits()*theLostHitPenalty - a.foundHits()*theFoundHitBonus)  < 
                  (b.chiSquared() + b.lostHits()*theLostHitPenalty - b.foundHits()*theFoundHitBonus);
      }
};

#endif
