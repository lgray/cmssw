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
from RecoVertex.AdaptiveVertexFinder.inclusiveVertexing_cff import *
#particle flow recycling
from RecoParticleFlow.PFProducer.pfTimeAssigner_cfi import *
#redo taus
#from RecoTauTag.Configuration.RecoPFTauTag_cff import *
#from RecoTauTag.Configuration.HPSPFTaus_cff import*

particleFlow = pfTimeAssigner.clone()

_patTask_timing = cms.Task(
    trackTimeValueMapProducer,
    unsortedOfflinePrimaryVertices,
    trackWithVertexRefSelectorBeforeSorting,
    trackRefsForJetsBeforeSorting,    
    offlinePrimaryVertices,
    offlinePrimaryVerticesWithBS,
    inclusiveVertexFinder,
    inclusiveCandidateVertexFinder,
    inclusiveCandidateVertexFinderCvsL,
    candidateVertexMerger,
    candidateVertexMergerCvsL,
    candidateVertexArbitrator,
    candidateVertexArbitratorCvsL,
    inclusiveCandidateSecondaryVertices,
    inclusiveCandidateSecondaryVerticesCvsL,
    particleFlow,
    patTask.copy()
)

timing_reproc.toModify(offlinePrimaryVertices, jets = cms.InputTag('ak4CaloJets') )
timing_reproc.toModify(offlinePrimaryVerticesWithBS, jets = cms.InputTag('ak4CaloJets') )
timing_reproc.toReplaceWith(patTask, _patTask_timing)
timing_reproc.toReplaceWith(trackTimeValueMapProducer, trackTimeValueMapRecycler)
timing_reproc.toReplaceWith(unsortedOfflinePrimaryVertices, unsortedOfflinePrimaryVertices4D)

#def _timingReprocAddTaus( processObject ) :
    
#timing_reproc_addtaus_ = timing_reproc.makeProcessModifier(_timingReprocAddTaus)
