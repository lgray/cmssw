import FWCore.ParameterSet.Config as cms

egamma_modifications = cms.VPSet(
    cms.PSet( modifierName  = cms.string('EGFull5x5ShowerShapeModifierFromValueMaps'),
              photon_config = cms.PSet( photonSrc     = cms.InputTag('slimmedPhotons',processName=cms.InputTag.skipCurrentProcess()),
                                        sigmaIetaIeta = cms.InputTag('photonIDValueMapProducer:phoFull5x5SigmaIEtaIEta'),
                                        e5x5          = cms.InputTag('photonIDValueMapProducer:phoFull5x5E5x5')
                                        ) 
              ),
    cms.PSet( modifierName    = cms.string('EGExtraInfoModifierFromFloatValueMaps'),
              electron_config = cms.PSet( electronSrc = cms.InputTag('slimmedElectrons',processName=cms.InputTag.skipCurrentProcess())
                                          ),
              photon_config   = cms.PSet( photonSrc   = cms.InputTag('slimmedPhotons',processName=cms.InputTag.skipCurrentProcess()),
                                          phoFull5x5SigmaIEtaIPhi = cms.InputTag('photonIDValueMapProducer:phoFull5x5SigmaIEtaIPhi'),
                                          phoFull5x5E1x3          = cms.InputTag('photonIDValueMapProducer:phoFull5x5E1x3'),
                                          phoFull5x5E2x2          = cms.InputTag('photonIDValueMapProducer:phoFull5x5E2x2'),
                                          phoFull5x5E2x5Max       = cms.InputTag('photonIDValueMapProducer:phoFull5x5E2x5Max'),
                                          phoESEffSigmaRR         = cms.InputTag('photonIDValueMapProducer:phoESEffSigmaRR'),
                                          phoChargedIsolation     = cms.InputTag('photonIDValueMapProducer:phoChargedIsolation'),
                                          phoNeutralHadronIsolation = cms.InputTag('photonIDValueMapProducer:phoNeutralHadronIsolation'),
                                          phoPhotonIsolation      = cms.InputTag('photonIDValueMapProducer:phoPhotonIsolation'),
                                          phoWorstChargedIsolation = cms.InputTag('photonIDValueMapProducer:phoWorstChargedIsolation')
                                          )
              ),
    cms.PSet( modifierName    = cms.string('EGExtraInfoModifierFromIntValueMaps'),
              electron_config = cms.PSet( electronSrc = cms.InputTag('slimmedElectrons',processName=cms.InputTag.skipCurrentProcess())
                                          ),
              photon_config   = cms.PSet( photonSrc   = cms.InputTag('slimmedPhotons',processName=cms.InputTag.skipCurrentProcess())
                                          )
              )
)

#patch in new regressions

from RecoEgamma.EgammaTools.regressionModifier_cfi import *

egamma_modifications.append( regressionModifier.clone() )

setattr(egamma_modifications[-1].electron_config,
        'electronSrc',
        cms.InputTag('slimmedElectrons',processName=cms.InputTag.skipCurrentProcess()))

setattr(egamma_modifications[-1].photon_config,
        'photonSrc',
        cms.InputTag('slimmedPhotons',processName=cms.InputTag.skipCurrentProcess()))

egamma_modifications[-1].vertexCollection = cms.InputTag('offlineSlimmedPrimaryVertices')

