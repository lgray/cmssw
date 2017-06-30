import FWCore.ParameterSet.Config as cms

trackTimeValueMapRecycler = cms.EDProducer(
    'TrackTimeValueMapRecycler',
    trackSrc = cms.InputTag('generalTracks'),
    trackTimeSrc = cms.InputTag(''),
    trackTimeResoSrc = cms.InputTag(''),
    pileupSummaryInfo = cms.InputTag('addPileupInfo'),
    resolutionModels = cms.VPSet( cms.PSet( modelName = cms.string('ConfigurableFlatResolutionModel'),
                                            resolutionInNs = cms.double(0.030) ) ),
    etaMin = cms.double(-1.0),
    etaMax = cms.double(3.0),
    ptMin = cms.double(0.7),
    pMin = cms.double(0.7),
    etaMaxForPtThreshold = cms.double(1.5),
    )
