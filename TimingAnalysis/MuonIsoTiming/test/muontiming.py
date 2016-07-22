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

# Input source
process.source = cms.Source("PoolSource",
    secondaryFileNames = cms.untracked.vstring(),
    fileNames = cms.untracked.vstring('/store/mc/TP2023HGCALDR/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola/GEN-SIM-RECO/62028p1_PU50NoOOT_FastTimeHLLHC_PH2_1K_FB_V6-v1/10000/00D9C228-B12E-E611-942D-0025901AC182.root', 
        '/store/mc/TP2023HGCALDR/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola/GEN-SIM-RECO/62028p1_PU50NoOOT_FastTimeHLLHC_PH2_1K_FB_V6-v1/10000/00F13043-8730-E611-8E97-008CFAF51448.root', 
        '/store/mc/TP2023HGCALDR/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola/GEN-SIM-RECO/62028p1_PU50NoOOT_FastTimeHLLHC_PH2_1K_FB_V6-v1/10000/02DA0A45-682E-E611-A51A-002590D9D8C4.root', 
        '/store/mc/TP2023HGCALDR/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola/GEN-SIM-RECO/62028p1_PU50NoOOT_FastTimeHLLHC_PH2_1K_FB_V6-v1/10000/065F28BE-522E-E611-B7A4-008CFAF508F0.root', 
        '/store/mc/TP2023HGCALDR/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola/GEN-SIM-RECO/62028p1_PU50NoOOT_FastTimeHLLHC_PH2_1K_FB_V6-v1/10000/06DE69B2-472F-E611-AC4D-0242AC110002.root', 
        '/store/mc/TP2023HGCALDR/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola/GEN-SIM-RECO/62028p1_PU50NoOOT_FastTimeHLLHC_PH2_1K_FB_V6-v1/10000/0AD939E4-FB2E-E611-B4F0-002590FD5A4C.root', 
        '/store/mc/TP2023HGCALDR/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola/GEN-SIM-RECO/62028p1_PU50NoOOT_FastTimeHLLHC_PH2_1K_FB_V6-v1/10000/0AF778D3-412E-E611-8204-008CFA50291C.root', 
        '/store/mc/TP2023HGCALDR/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola/GEN-SIM-RECO/62028p1_PU50NoOOT_FastTimeHLLHC_PH2_1K_FB_V6-v1/10000/0C0F4D14-172F-E611-AE28-008CFAF06DC8.root', 
        '/store/mc/TP2023HGCALDR/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola/GEN-SIM-RECO/62028p1_PU50NoOOT_FastTimeHLLHC_PH2_1K_FB_V6-v1/10000/0C550CC9-D52E-E611-907F-000F532734A0.root', 
        '/store/mc/TP2023HGCALDR/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola/GEN-SIM-RECO/62028p1_PU50NoOOT_FastTimeHLLHC_PH2_1K_FB_V6-v1/10000/0CF1C128-6A2F-E611-B518-001E675A5262.root')
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
process.GlobalTag = GlobalTag(process.GlobalTag, 'PH2_1K_FB_V6::All', '')


process.TFileService = cms.Service("TFileService", 
    fileName = cms.string("muontimingzmm50.root"),
    closeFileFast = cms.untracked.bool(True)
)

process.muonIsoTiming = cms.EDAnalyzer('MuonIsoTiming'
)

process.path = cms.Path(process.muonIsoTiming)

# Path and EndPath definitions

# Schedule definition
process.schedule = cms.Schedule(process.path)

