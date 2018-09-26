#include "SimTracker/TrackTriggerAssociation/interface/L1ResolutionModel.h"

class L1ConfigurableFlatResolutionModel : public L1ResolutionModel {
public:
  L1ConfigurableFlatResolutionModel( const edm::ParameterSet& conf ) : 
    L1ResolutionModel( conf ),
    reso_( conf.getParameter<double>("resolutionInNs") ) {
  }
  
  float getTimeResolution(const TTTrack<Ref_Phase2TrackerDigi_>&) const override { return reso_; }
  
private:
  const float reso_;
};

DEFINE_EDM_PLUGIN(L1ResolutionModelFactory, 
                  L1ConfigurableFlatResolutionModel,
                  "L1ConfigurableFlatResolutionModel");
