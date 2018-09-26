#ifndef __SimTracker_TrackTriggerAssociation_L1ResolutionModel_h__
#define __SimTracker_TrackTriggerAssociation_L1ResolutionModel_h__

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/L1TrackTrigger/interface/TTTypes.h"
#include "DataFormats/L1TrackTrigger/interface/TTTrack.h"

#include <string>
#include <iostream>

class L1ResolutionModel {
 public:
 L1ResolutionModel(const edm::ParameterSet& conf):    
  _modelName(conf.getParameter<std::string>("modelName")) { 
  }
  virtual ~L1ResolutionModel() { }
  // get rid of things we should never use...
  L1ResolutionModel(const L1ResolutionModel&) = delete;
  L1ResolutionModel& operator=(const L1ResolutionModel&) = delete;

  virtual float getTimeResolution(const TTTrack<Ref_Phase2TrackerDigi_>&)     const { return -1.f; }
  
  const std::string& name() const { return _modelName; }
  
 private:
  const std::string _modelName;  
};

#include "FWCore/PluginManager/interface/PluginFactory.h"
typedef edmplugin::PluginFactory< L1ResolutionModel* (const edm::ParameterSet&) > L1ResolutionModelFactory;

#endif
