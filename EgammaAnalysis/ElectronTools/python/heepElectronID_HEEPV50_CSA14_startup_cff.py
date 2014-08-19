import FWCore.ParameterSet.Config as cms

from PhysicsTools.SelectorUtils.centralIDRegistry import central_id_registry

ebMax = 1.4442
eeMin = 1.566
ebCutOff=1.479
heepElectronID_HEEPV50_CSA14_startup = cms.PSet(
    idName = cms.string("heepElectronID-HEEPV50-CSA14-startup"),
    cutFlow = cms.VPSet(
        cms.PSet( cutName = cms.string("MinPtCut"),
                  minPt = cms.double(35.0),
                  needsAdditionalProducts = cms.bool(False),
                  isIgnored = cms.bool(False)                ),
        cms.PSet( cutName = cms.string("GsfEleSCEtaMultiRangeCut"),
                  useAbsEta = cms.bool(True),
                  allowedEtaRanges = cms.VPSet( 
                                  cms.PSet( minEta = cms.double(0.0), 
                                            maxEta = cms.double(ebMax) ),
                                  cms.PSet( minEta = cms.double(eeMin), 
                                            maxEta = cms.double(2.5) )
                                  ),
                  needsAdditionalProducts = cms.bool(False),
                  isIgnored = cms.bool(False)),
        cms.PSet( cutName = cms.string('GsfEleDEtaInLinearCut'),
                  constTermEB = cms.double(0.0),
                  constTermEE = cms.double(0.0),
                  slopeTermEB = cms.double(0.0),
                  slopeTermEE = cms.double(0.0),
                  minValueEB = cms.double(0.02),
                  minValueEE = cms.double(0.02),
                  needsAdditionalProducts = cms.bool(False),
                  isIgnored = cms.bool(False)),
        cms.PSet( cutName = cms.string('GsfEleDPhiInCut'),
                  dPhiInCutValueEB = cms.double(0.15),
                  dPhiInCutValueEE = cms.double(0.15),
                  barrelCutOff = cms.double(ebCutOff),
                  needsAdditionalProducts = cms.bool(False),
                  isIgnored = cms.bool(False)),
        cms.PSet( cutName = cms.string('GsfEleFull5x5SigmaIEtaIEtaCut'),
                  full5x5SigmaIEtaIEtaCutValueEB = cms.double(9999),
                  full5x5SigmaIEtaIEtaCutValueEE = cms.double(0.03),
                  full5x5SigmaIEtaIEtaMap = cms.InputTag("electronIDValueMapProducer","eleFull5x5SigmaIEtaIEta"),
                  barrelCutOff = cms.double(ebCutOff),
                  needsAdditionalProducts = cms.bool(True),
                  isIgnored = cms.bool(False)),
        cms.PSet( cutName = cms.string('GsfEleFull5x5E2x5OverE5x5Cut'),
                  minE1x5OverE5x5EB = cms.double(0.83),
                  minE1x5OverE5x5EE = cms.double(-1),
                  minE2x5OverE5x5EB = cms.double(0.94),
                  minE2x5OverE5x5EE = cms.double(-1),
                  needsAdditionalProducts = cms.bool(True),
                  e1x5 = cms.InputTag("electronIDValueMapProducer","eleFull5x5E1x5"),
                  e2x5 = cms.InputTag("electronIDValueMapProducer","eleFull5x5E2x5"),
                  e5x5 = cms.InputTag("electronIDValueMapProducer","eleFull5x5E5x5"),
                  isIgnored = cms.bool(False)),
        cms.PSet( cutName = cms.string('GsfEleHadronicOverEMLinearCut'),
                  slopeTermEB = cms.double(0.05),
                  slopeTermEE = cms.double(0.05),
                  slopeStartEB = cms.double(0),
                  slopeStartEE = cms.double(0),
                  constTermEB = cms.double(2),
                  constTermEE = cms.double(12.5),
                  needsAdditionalProducts = cms.bool(False),
                  isIgnored = cms.bool(False)),
        cms.PSet( cutName = cms.string('GsfEleTrkPtIsoCut'),
                  slopeTermEB = cms.double(0),
                  slopeTermEE = cms.double(0),
                  slopeStartEB = cms.double(0),
                  slopeStartEE = cms.double(0),
                  constTermEB = cms.double(5),
                  constTermEE = cms.double(5),
                  needsAdditionalProducts = cms.bool(False),
                  isIgnored = cms.bool(False)),
        cms.PSet( cutName = cms.string('GsfEleEmHadD1IsoRhoCut'),
                  slopeTermEB = cms.double(0.03),
                  slopeTermEE = cms.double(0.03),
                  slopeStartEB = cms.double(0),
                  slopeStartEE = cms.double(50),
                  constTermEB = cms.double(2),
                  constTermEE = cms.double(2.5),
                  rhoConstant = cms.double(0.28),
                  rho = cms.InputTag("fixedGridRhoFastjetAll"),
                  needsAdditionalProducts = cms.bool(True),
                  isIgnored = cms.bool(False)),
    
        cms.PSet( cutName = cms.string('GsfEleDxyCut'),
                  dxyCutValueEB = cms.double(0.02),
                  dxyCutValueEE = cms.double(0.05),
                  vertexSrc = cms.InputTag("offlinePrimaryVertices"),
                  barrelCutOff = cms.double(ebCutOff),
                  needsAdditionalProducts = cms.bool(True),
                  isIgnored = cms.bool(False)),
        cms.PSet( cutName = cms.string('GsfEleMissingHitsCut'),
                  maxMissingHitsEB = cms.uint32(1),
                  maxMissingHitsEE = cms.uint32(1),
                  barrelCutOff = cms.double(ebCutOff),
                  needsAdditionalProducts = cms.bool(False),
                  isIgnored = cms.bool(False)),
        cms.PSet( cutName = cms.string('GsfEleDzCut'),
                  dzCutValueEB = cms.double(0.1),
                  dzCutValueEE = cms.double(0.1),
                  vertexSrc = cms.InputTag("offlinePrimaryVertices"),
                  barrelCutOff = cms.double(ebCutOff),
                  needsAdditionalProducts = cms.bool(True),
                  isIgnored = cms.bool(True))                 
    )
)




central_id_registry.register(heepElectronID_HEEPV50_CSA14_startup.idName,
                             '404faed16f797c67d5dedffaeab26c2f')
