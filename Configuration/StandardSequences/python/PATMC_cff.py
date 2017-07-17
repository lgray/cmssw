import FWCore.ParameterSet.Config as cms

from PhysicsTools.PatAlgos.producersLayer1.patCandidates_cff import *
from PhysicsTools.PatAlgos.selectionLayer1.selectedPatCandidates_cff import *
from PhysicsTools.PatAlgos.slimming.slimming_cff import *
from RecoLuminosity.LumiProducer.bunchSpacingProducer_cfi import *

patTask = cms.Task(
    patCandidatesTask,
    selectedPatCandidatesTask,
    slimmingTask,
    bunchSpacingProducer
)

miniAOD=cms.Sequence()

#MiniAOD reprocessing with timing
from Configuration.Eras.Modifier_BarrelTDR_timing_reproc_miniaod_cff import BarrelTDR_timing_reproc_miniaod as timing_reproc

#vertexing setup
from RecoVertex.Configuration.RecoVertex_cff import *
from SimTracker.TrackAssociation.trackTimeValueMapRecycler_cfi import *
#particle flow recycling
from RecoParticleFlow.PFProducer.pfTimeAssigner import *
particleFlow = pfTimeAssigner.clone()


_patTask_timing = cms.Task(
    trackTimeValueMapProducer,
    unsortedOfflinePrimaryVertices,
    trackWithVertexRefSelectorBeforeSorting,
    trackRefsForJetsBeforeSorting,    
    offlinePrimaryVertices,
    offlinePrimaryVerticesWithBS,
    generalV0Candidates,
    particleFlow,
    patTask.copy()
    )

timing_reproc.toModify(offlinePrimaryVertices, jets = cms.InputTag('ak4CaloJets') )
timing_reproc.toModify(offlinePrimaryVerticesWithBS, jets = cms.InputTag('ak4CaloJets') )
timing_reproc.toReplaceWith(patTask, _patTask_timing)
timing_reproc.toReplaceWith(trackTimeValueMapProducer, trackTimeValueMapRecycler)
