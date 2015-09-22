import FWCore.ParameterSet.Config as cms

process = cms.PSet()

process.fwliteInput = cms.PSet(
    fileNames   = cms.vstring(['root://cms-xrd-global.cern.ch//store/data/Run2015D/DoubleEG/MINIAOD/PromptReco-v3/000/256/677/00000/9C593CF1-865F-E511-9198-02163E0135A8.root']),  ## mandatory
    maxEvents   = cms.int32(-1),
    outputEvery = cms.uint32(10)
    )

process.fwliteOutput = cms.PSet(
    fileName  = cms.string('fwlite_vid_example.root') ## mandatory
    )

process.electronAnalyzer = cms.PSet(
    electrons = cms.InputTag("slimmedElectrons")
    )

#an example for cut based IDs
from RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Spring15_25ns_V1_cff import *

if hasattr(cutBasedElectronID_Spring15_25ns_V1_standalone_loose,'isPOGApproved'):
    del cutBasedElectronID_Spring15_25ns_V1_standalone_loose.isPOGApproved
if hasattr(cutBasedElectronID_Spring15_25ns_V1_standalone_medium,'isPOGApproved'):
    del cutBasedElectronID_Spring15_25ns_V1_standalone_medium.isPOGApproved
if hasattr(cutBasedElectronID_Spring15_25ns_V1_standalone_tight,'isPOGApproved'):
    del cutBasedElectronID_Spring15_25ns_V1_standalone_tight.isPOGApproved


process.my_vid_configuration = cms.PSet(
    loose = cutBasedElectronID_Spring15_25ns_V1_standalone_loose,
    medium = cutBasedElectronID_Spring15_25ns_V1_standalone_medium,
    tight = cutBasedElectronID_Spring15_25ns_V1_standalone_tight
)
