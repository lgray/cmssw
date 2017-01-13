import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process('TRY',eras.Phase2C2_timing_layer)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.Geometry.GeometryExtended2023D8Reco_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(5) )
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:/uscms/home/lagray/nobackup/muons_90X_2017-01-03-100/step3.root'),
    secondaryFileNames = cms.untracked.vstring()
)

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase2_realistic', '')
# customisation of the process.

process.load('RecoTracker.FastTimeMatching.ftlTracks_cff')

process.ftlTracks.debugLevel = 999

process.run = cms.Path(process.ftlTracks)

process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string("ftltracks.root"),
    outputCommands = cms.untracked.vstring("drop *", 
        "keep recoTracks_generalTracks_*_*",
        "keep *_FTLRecHit_*_*", 
        "keep *_particleFlowClusterHGCal_*_*", 
        "keep *_hgcalLayerClusters_*_*",
        "keep *_ftlTracks_*_*",
    ),
)
process.e = cms.EndPath(process.out)

import sys
args = sys.argv[2:] if sys.argv[0] == "cmsRun" else sys.argv[1:]
if args:
    process.source.fileNames = [ args[0] ]
