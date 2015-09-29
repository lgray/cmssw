import FWCore.ParameterSet.Config as cms

from PhysicsTools.PatAlgos.slimming.modifyPrimaryPhysicsObjects_cff import *
from PhysicsTools.PatAlgos.slimming.MicroEventContent_cff import *

from RecoLuminosity.LumiProducer.bunchSpacingProducer_cfi import *

EIsequence = cms.Sequence( bunchSpacingProducer*modifyPrimaryPhysicsObjects )
