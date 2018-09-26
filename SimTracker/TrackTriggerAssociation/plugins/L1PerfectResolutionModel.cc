#include "SimTracker/TrackTriggerAssociation/interface/L1ResolutionModel.h"

class L1PerfectResolutionModel : public L1ResolutionModel {
public:
  L1PerfectResolutionModel( const edm::ParameterSet& conf ) : L1ResolutionModel( conf ) {}
  
  float getTimeResolution(const TTTrack<Ref_Phase2TrackerDigi_>&) const override { return 1e-6; }
  
};

DEFINE_EDM_PLUGIN(L1ResolutionModelFactory, 
                  L1PerfectResolutionModel,
                  "L1PerfectResolutionModel");
