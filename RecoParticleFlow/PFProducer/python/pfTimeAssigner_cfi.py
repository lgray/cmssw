import FWCore.ParameterSet.Config as cms

pfTimeAssigner = cms.EDProducer(
    'PFTimeAssigner',
    pfCandidateSrc = cms.InputTag('particleFlow',processName=cms.InputTag.skipCurrentProcess()),
    trackTimeSrc = cms.InputTag('trackTimeValueMapProducer:generalTracksPerfectResolutionModel'),
    trackTimeResoSrc = cms.InputTag('trackTimeValueMapProducer:generalTracksPerfectResolutionModelResolution'),
    gsfTrackTimeSrc = cms.InputTag('trackTimeValueMapProducer:gsfTracksPerfectResolutionModel'),
    gsfTrackTimeResoSrc = cms.InputTag('trackTimeValueMapProducer:gsfTracksPerfectResolutionModelResolution')
    )


