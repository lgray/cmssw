import FWCore.ParameterSet.Config as cms

from RecoParticleFlow.PFClusterProducer.particleFlowClusterCleaners_cfi \
     import *

from RecoParticleFlow.PFClusterProducer.particleFlowClusterSeeders_cfi import *

from RecoParticleFlow.PFClusterProducer.particleFlowClusterizers_cfi import *

from RecoParticleFlow.PFClusterProducer.\
     particleFlowClusterPositionCalculators_cfi import *

from RecoParticleFlow.PFClusterProducer.\
     particleFlowClusterEnergyCorrectors_cfi import *

particleFlowClusterHFHAD = cms.EDProducer(
    "PFClusterProducer",
    recHitsSource = cms.InputTag("particleFlowRecHitHCAL:HFHAD"),
    recHitCleaners = cms.VPSet(spikeAndDoubleSpikeCleaner_HFHAD),
    seedFinder = localMaxSeeds_HF,
    initialClusteringStep = topoClusterizer_HF,
    pfClusterBuilder = pfClusterizer_HF
    )

