import FWCore.ParameterSet.Config as cms

ttTrackTimeValueMapProducer = cms.EDProducer(
    'L1TrackTimeValueMapProducer',
    tkTriggerTrackSrc = cms.InputTag('TTTracksFromTracklet'),
    tkTriggerTrackTruthSrc = cms.InputTag('TTTrackAssociatorFromPixelDigis'),
    pileupSummaryInfo = cms.InputTag('addPileupInfo'),
    resolutionModels = cms.VPSet( cms.PSet( modelName = cms.string('L1ConfigurableFlatResolutionModel'),
                                            resolutionInNs = cms.double(0.050) ),
                                  cms.PSet( modelName = cms.string('L1PerfectResolutionModel') ) ),
    etaMin = cms.double(-1.0),
    etaMax = cms.double(3.0),
    ptMin = cms.double(0.7),
    pMin = cms.double(0.7),
    etaMaxForPtThreshold = cms.double(1.5),
    )
