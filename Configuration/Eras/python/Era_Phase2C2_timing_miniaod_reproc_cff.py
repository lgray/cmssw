import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Era_Phase2C2_timing_cff import Phase2C2_timing
from Configuration.Eras.Modifier_BarrelTDR_timing_reproc_miniaod_cff import BarrelTDR_timing_reproc_miniaod

Phase2C2_timing_miniaod_reproc = cms.ModifierChain(Phase2C2_timing, BarrelTDR_timing_reproc_miniaod)

