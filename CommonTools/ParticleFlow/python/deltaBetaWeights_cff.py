import FWCore.ParameterSet.Config as cms
from CommonTools.ParticleFlow.deltaBetaWeights_cfi import *

pfDeltaBetaWeighting = cms.Sequence(pfWeightedPhotons,pfWeightedNeutralHadrons)
