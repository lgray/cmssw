# Auto generated configuration file
# using: 
# Revision: 1.20 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: TimingAnalysis/MuonIsoTiming/python/muonIsoTiming_cfi.py -s NONE --conditions PH2_1K_FB_V6::All --no_output --no_exec -n -1 --filein das:/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola/TP2023HGCALDR-62028p1_PU50NoOOT_FastTimeHLLHC_PH2_1K_FB_V6-v1/GEN-SIM-RECO --python_filename muontiming.py
import FWCore.ParameterSet.Config as cms

process = cms.Process('NONE')

# import of standard configurations
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

the_reco_file = open('reco_files_140.txt','r')
reco_input_files = [line for line in the_reco_file]
the_reco_file.close()

# Input source
process.source = cms.Source("PoolSource",
    secondaryFileNames = cms.untracked.vstring(),
    fileNames = cms.untracked.vstring(reco_input_files)
)

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.20 $'),
    annotation = cms.untracked.string('TimingAnalysis/MuonIsoTiming/python/muonIsoTiming_cfi.py nevts:-1'),
    name = cms.untracked.string('Applications')
)

# Output definition

# Additional output definition

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase2_realistic', '')


process.TFileService = cms.Service("TFileService", 
    fileName = cms.string("muontimingzmm140.root"),
    closeFileFast = cms.untracked.bool(True)
)

process.muonIsoTiming = cms.EDAnalyzer('MuonIsoTiming'
)

process.path = cms.Path(process.muonIsoTiming)

# Path and EndPath definitions

# Schedule definition
process.schedule = cms.Schedule(process.path)

