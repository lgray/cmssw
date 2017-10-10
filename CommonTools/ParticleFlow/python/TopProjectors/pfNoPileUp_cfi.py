import FWCore.ParameterSet.Config as cms

pfNoPileUp = cms.EDProducer(
    "TPPFCandidatesOnPFCandidates",
    enable =  cms.bool( True ),
    verbose = cms.untracked.bool( False ),
    name = cms.untracked.string("pileUpOnPFCandidates"),
    topCollection = cms.InputTag("pfPileUp"),
    bottomCollection = cms.InputTag("particleFlowTmpPtrs"),
)

from Configuration.Eras.Modifier_phase2_timing_miniaod_cff import phase2_timing_miniaod
phase2_timing_miniaod.toModify(pfNoPileUp, bottomCollection = cms.InputTag("particleFlowPtrs") )
