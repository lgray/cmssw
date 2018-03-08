#ifndef _SimpleTrackingkalmanutils_
#define _SimpleTrackingkalmanutils_

#include "SimpleTracking/SimpleTracking/interface/Track.h"
#include "SimpleTracking/SimpleTracking/interface/Matrix.h"

float computeChi2(const TrackState& propagatedState, const MeasurementState& measurementState);

//see e.g. http://inspirehep.net/record/259509?ln=en
TrackState updateParameters(const TrackState& propagatedState, const MeasurementState& measurementState);

TrackState updateParametersWithTime(const TrackState& propagatedState, const MeasurementState& measurementState);

TrackState updateParametersEndcap(const TrackState& propagatedState, const MeasurementState& measurementState);
float computeChi2Endcap(const TrackState& propagatedState, const MeasurementState& measurementState);

#endif
