import FWCore.ParameterSet.Config as cms


pfPileUp = cms.EDProducer(
    "PFPileUp",
    PFCandidates = cms.InputTag("particleFlowTmpPtrs"),
    Vertices = cms.InputTag("offlinePrimaryVertices"),
    # pile-up identification now enabled by default. To be studied for jets
    Enable = cms.bool(True),
    verbose = cms.untracked.bool(False),
    checkClosestZVertex = cms.bool(True)
    )

from Configuration.Eras.Modifier_phase2_timing_miniaod_cff import phase2_timing_miniaod
phase2_timing_miniaod.toModify(pfPileUp, PFCandidates = cms.InputTag("particleFlowPtrs") )
