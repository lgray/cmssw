import FWCore.ParameterSet.Config as cms

pfNoPileUp = cms.EDProducer(
    "TPPFCandidatesOnPFCandidates",
    enable =  cms.bool( True ),
    verbose = cms.untracked.bool( False ),
    name = cms.untracked.string("pileUpOnPFCandidates"),
    topCollection = cms.InputTag("pfPileUp"),
    bottomCollection = cms.InputTag("particleFlowTmpPtrs"),
)

from Configuration.Eras.Modifier_phase2_timing_layer_cff import phase2_timing_layer
phase2_timing_layer.toModify(pfNoPileUp, bottomCollection = cms.InputTag("particleFlowPtrs") )
