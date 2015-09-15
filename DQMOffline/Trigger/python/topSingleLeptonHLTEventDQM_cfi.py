import FWCore.ParameterSet.Config as cms

topSingleLeptonHLTOfflineDQM = cms.EDAnalyzer("TopSingleLeptonHLTOfflineDQM",
  ## ------------------------------------------------------
  ## SETUP
  ##
  ## configuration of the MonitoringEnsemble(s)
  ## [mandatory] : optional PSets may be omitted
  ##
  setup = cms.PSet(
    ## sub-directory to write the monitor histograms to
    ## [mandatory] : should not be changed w/o explicit 
    ## communication to TopCom!
    directory = cms.string("HLT/TopHLTOffline/Top/SemiLeptonic/"),
    ## [mandatory]
    sources = cms.PSet(
      muons = cms.InputTag("muons"),
      elecs = cms.InputTag("gedGsfElectrons"),
      jets  = cms.InputTag("ak4PFJetsCHS"),
      mets  = cms.VInputTag("met", "tcMet", "pfMet"),
      pvs   = cms.InputTag("offlinePrimaryVertices")
    ),
    ## [optional] : when omitted all monitoring plots for primary vertices
    ## will be filled w/o extras
    pvExtras = cms.PSet(
      ## when omitted electron plots will be filled w/o additional pre-
      ## selection of the primary vertex candidates                                                                                            
      select = cms.string("std::abs(obj.x())<1. && std::abs(obj.y())<1. && std::abs(obj.z())<20. && obj.tracksSize()>3 && !obj.isFake()")
    ),
    ## [optional] : when omitted all monitoring plots for electrons
    ## will be filled w/o extras
    elecExtras = cms.PSet(
      ## when omitted electron plots will be filled w/o cut on electronId
      #electronId = cms.PSet( src = cms.InputTag("mvaTrigV0"), pattern = cms.int32(1) ),
      ## when omitted electron plots will be filled w/o additional pre-
      ## selection of the electron candidates                                                                                            
      select = cms.string("obj.pt()>30 && std::abs(obj.eta())<2.5"),
      ## when omitted isolated electron multiplicity plot will be equi-
      ## valent to inclusive electron multiplicity plot 
      isolation = cms.string("(obj.dr03TkSumPt()+obj.dr04EcalRecHitSumEt()+obj.dr04HcalTowerSumEt())/obj.pt()<0.1"),
    ),
    ## [optional] : when omitted all monitoring plots for muons
    ## will be filled w/o extras
    muonExtras = cms.PSet(
      ## when omitted muon plots will be filled w/o additional pre-
      ## selection of the muon candidates                                                                                            
      select = cms.string("obj.pt()>26 && std::abs(obj.eta())<2.1 && obj.isPFMuon() && obj.isGlobalMuon() && obj.globalTrack()->normalizedChi2()<10 && obj.innerTrack()->hitPattern().trackerLayersWithMeasurement()>5 && obj.innerTrack()->hitPattern().numberOfValidPixelHits()>0 && obj.numberOfMatches()>1"),
      ## when omitted isolated muon multiplicity plot will be equi-
      ## valent to inclusive muon multiplicity plot                                                    
      isolation = cms.string("(obj.pfIsolationR04().sumChargedHadronPt+obj.pfIsolationR04().sumPhotonEt+obj.pfIsolationR04().sumNeutralHadronEt)/obj.pt()<0.2"),
    ),
    ## [optional] : when omitted all monitoring plots for jets will
    ## be filled from uncorrected jets
    jetExtras = cms.PSet(
      ## when omitted monitor plots for pt will be filled from uncorrected
      ## jets                                            
      #jetCorrector = cms.string("ak4PFCHSL2L3"),
      ## when omitted no extra selection will be applied on jets before
      ## filling the monitor histograms; if jetCorrector is present the
      ## selection will be applied to corrected jets
      select = cms.string("obj.pt()>20 && std::abs(obj.eta())<2.5"),
    ),
    ## [optional] : when omitted no mass window will be applied
    ## for the W mass befor filling the event monitoring plots
    massExtras = cms.PSet(
      lowerEdge = cms.double( 70.),
      upperEdge = cms.double(110.)
    ),
    ## [optional] : when omitted the monitoring plots for triggering
    ## will be empty
    #triggerExtras = cms.PSet(
    #  src   = cms.InputTag("TriggerResults","","HLT"),
    #  paths = cms.vstring(['HLT_Mu3:HLT_QuadJet15U',
    #                       'HLT_Mu5:HLT_QuadJet15U',
    #                       'HLT_Mu7:HLT_QuadJet15U',
    #                       'HLT_Mu9:HLT_QuadJet15U'])
    #)                                            
  ),                                  
  ## ------------------------------------------------------
  ## PRESELECTION
  ##
  ## setup of the event preselection, which will not
  ## be monitored
  ## [mandatory] : but may be empty
  ##
  preselection = cms.PSet(
   trigger = cms.PSet(
      src    = cms.InputTag("TriggerResults","","HLT"),
      select = cms.vstring(['HLT_Ele23_WPLoose_Gsf_TriCentralPFJet50_40_30_v', 'HLT_Ele23_WPLoose_Gsf_CentralPFJet30_BTagCVS07_v', 'HLT_Ele27_WPLoose_Gsf_WHbbBoost_v', 'HLT_Ele27_WPLoose_Gsf_v', 'HLT_Ele27_WPLoose_Gsf_CentralPFJet30_BTagCSV07_v' 'HLT_Ele27_WPLoose_Gsf_TriCentralPFJet50_40_30_v', 'HLT_Ele27_eta2p1_WPLoose_Gsf_TriCentralPFJet30_v', 'HLT_Ele27_eta2p1_WPLoose_Gsf_TriCentralPFJet50_40_30_v', 'HLT_Ele27_eta2p1_WPLoose_Gsf_v', 'HLT_Ele27_eta2p1_WPLoose_Gsf_CentralPFJet30_BTagCSV07_v', 'HLT_Ele32_eta2p1_WPLoose_Gsf_TriCentralPFJet30_v', 'HLT_Ele32_eta2p1_WPLoose_Gsf_TriCentralPFJet50_40_30_v', 'HLT_Ele32_eta2p1_WPLoose_Gsf_v', 'HLT_Ele32_eta2p1_WPLoose_Gsf_CentralPFJet30_BTagCSV07_v', 'HLT_IsoMu18_CentralPFJet30_BTagCSV07_v', 'HLT_IsoMu18_TriCentralPFJet50_40_30_v', 'HLT_IsoMu22_v', 'HLT_IsoMu18_v', 'HLT_IsoMu22_TriCentralPFJet50_40_30_v', 'HLT_IsoMu22_CentralPFJet30_BTagCSV07_v', 'HLT_IsoMu20_eta2p1_TriCentralPFJet30_v', 'HLT_IsoMu20_eta2p1_TriCentralPFJet50_40_30_v', 'HLT_IsoMu20_eta2p1_CentralPFJet30_BTagCSV07_v', 'HLT_IsoMu20_eta2p1_v', 'HLT_IsoMu24_eta2p1_TriCentralPFJet30_v', 'HLT_IsoMu24_eta2p1_TriCentralPFJet50_40_30_v', 'HLT_IsoMu24_eta2p1_CentralPFJet30_BTagCSV07_v'])
    ),
    ## [optional] : when omitted no preselection is applied
    vertex = cms.PSet(
      src    = cms.InputTag("offlinePrimaryVertices"),
      select = cms.string('std::abs(obj.x())<1. && std::abs(obj.y())<1. && std::abs(obj.z())<20. && obj.tracksSize()>3 && !obj.isFake()')
    )                                       
  ),  
  ## ------------------------------------------------------    
  ## SELECTION
  ##
  ## monitor histrograms are filled after each selection
  ## step, the selection is applied in the order defined
  ## by this vector
  ## [mandatory] : may be empty or contain an arbitrary
  ## number of PSets
  ##    
  selection = cms.VPSet(
    cms.PSet(
      label  = cms.string("Hlt:step0"),
      src    = cms.InputTag(""),
      select = cms.string(""),
      min    = cms.int32(0),
      max    = cms.int32(0),
    ),
    cms.PSet(
      label  = cms.string("jets/pf:step1"),
      src  = cms.InputTag("ak4PFJetsCHS"),
      select = cms.string("obj.pt()>20 && std::abs(obj.eta())<2.5"),
      min = cms.int32(4),
    ),
  )
)

topSingleMuonHLTOfflineDQM = cms.EDAnalyzer("TopSingleLeptonHLTOfflineDQM",
  ## ------------------------------------------------------
  ## SETUP
  ##
  ## configuration of the MonitoringEnsemble(s)
  ## [mandatory] : optional PSets may be omitted
  ##
  setup = cms.PSet(
    ## sub-directory to write the monitor histograms to
    ## [mandatory] : should not be changed w/o explicit
    ## communication to TopCom!
    directory = cms.string("HLT/TopHLTOffline/Top/SemiMuonic/"),
    ## [mandatory]
    sources = cms.PSet(
      muons = cms.InputTag("muons"),
      elecs = cms.InputTag("gedGsfElectrons"),
      jets  = cms.InputTag("ak4PFJetsCHS"),
      mets  = cms.VInputTag("met", "tcMet", "pfMet"),
      pvs   = cms.InputTag("offlinePrimaryVertices")

    ),
    ## [optional] : when omitted all monitoring plots for primary vertices
    ## will be filled w/o extras
    pvExtras = cms.PSet(
      ## when omitted electron plots will be filled w/o additional pre-
      ## selection of the primary vertex candidates                                                                                            
      select = cms.string("std::abs(obj.x())<1. && std::abs(obj.y())<1. && std::abs(obj.z())<20. && obj.tracksSize()>3 && !obj.isFake()")
    ),
    ## [optional] : when omitted all monitoring plots for muons
    ## will be filled w/o extras                                           
    muonExtras = cms.PSet(
      ## when omitted muon plots will be filled w/o additional pre-
      ## selection of the muon candidates                                                
      select = cms.string("obj.pt()>26 && std::abs(obj.eta())<2.1 && obj.isPFMuon() && obj.isGlobalMuon() && obj.globalTrack()->normalizedChi2()<10 && obj.innerTrack()->hitPattern().trackerLayersWithMeasurement()>5 && obj.innerTrack()->hitPattern().numberOfValidPixelHits()>0 && obj.numberOfMatches()>1"),
      ## when omitted isolated muon multiplicity plot will be equi-
      ## valent to inclusive muon multiplicity plot                                                    
      isolation = cms.string("(obj.pfIsolationR04().sumChargedHadronPt+obj.pfIsolationR04().sumPhotonEt+obj.pfIsolationR04().sumNeutralHadronEt)/obj.pt()<0.2"),
    ),
    ## [optional] : when omitted all monitoring plots for jets
    ## will be filled w/o extras
    jetExtras = cms.PSet(
      ## when omitted monitor plots for pt will be filled from uncorrected
      ## jets
      #jetCorrector = cms.string("ak4PFCHSL2L3"),
      ## when omitted no extra selection will be applied on jets before
      ## filling the monitor histograms; if jetCorrector is present the
      ## selection will be applied to corrected jets                                                
      select = cms.string("obj.pt()>20 && std::abs(obj.eta())<2.5"),
      ## when omitted monitor histograms for b-tagging will not be filled
      jetBTaggers  = cms.PSet(
         trackCountingEff = cms.PSet(
           label = cms.InputTag("jetProbabilityBJetTags" ),
           workingPoint = cms.double(0.275)
         ),
         trackCountingPur = cms.PSet(
           label = cms.InputTag("trackCountingHighPurBJetTags" ),
           workingPoint = cms.double(3.41)
         ),
         secondaryVertex  = cms.PSet(
           label = cms.InputTag("combinedSecondaryVertexBJetTags"),
           workingPoint = cms.double(0.679)
         )
       ),
    ),
    ## [optional] : when omitted no mass window will be applied
    ## for the W mass before filling the event monitoring plots
    massExtras = cms.PSet(
      lowerEdge = cms.double( 70.),
      upperEdge = cms.double(110.)
    ),
    ## [optional] : when omitted the monitoring plots for triggering
    ## will be empty
    triggerExtras = cms.PSet(
      src   = cms.InputTag("TriggerResults","","HLT"),
      paths = cms.vstring(['HLT_IsoMu18_CentralPFJet30_BTagCSV07_v', 'HLT_IsoMu18_TriCentralPFJet50_40_30_v', 'HLT_IsoMu22_v', 'HLT_IsoMu18_v', 'HLT_IsoMu22_TriCentralPFJet50_40_30_v', 'HLT_IsoMu22_CentralPFJet30_BTagCSV07_v', 'HLT_IsoMu20_eta2p1_TriCentralPFJet30_v', 'HLT_IsoMu20_eta2p1_TriCentralPFJet50_40_30_v', 'HLT_IsoMu20_eta2p1_CentralPFJet30_BTagCSV07_v', 'HLT_IsoMu20_eta2p1_v', 'HLT_IsoMu24_eta2p1_TriCentralPFJet30_v', 'HLT_IsoMu24_eta2p1_TriCentralPFJet50_40_30_v', 'HLT_IsoMu24_eta2p1_CentralPFJet30_BTagCSV07_v'])
    )
  ),
  ## ------------------------------------------------------
  ## PRESELECTION
  ##
  ## setup of the event preselection, which will not
  ## be monitored
  ## [mandatory] : but may be empty
  ##
  preselection = cms.PSet(
   trigger = cms.PSet(
      src    = cms.InputTag("TriggerResults","","HLT"),
      select = cms.vstring(['HLT_IsoMu18_CentralPFJet30_BTagCSV07_v', 'HLT_IsoMu18_TriCentralPFJet50_40_30_v', 'HLT_IsoMu22_v', 'HLT_IsoMu18_v', 'HLT_IsoMu22_TriCentralPFJet50_40_30_v', 'HLT_IsoMu22_CentralPFJet30_BTagCSV07_v', 'HLT_IsoMu20_eta2p1_TriCentralPFJet30_v', 'HLT_IsoMu20_eta2p1_TriCentralPFJet50_40_30_v', 'HLT_IsoMu20_eta2p1_CentralPFJet30_BTagCSV07_v', 'HLT_IsoMu20_eta2p1_v', 'HLT_IsoMu24_eta2p1_TriCentralPFJet30_v', 'HLT_IsoMu24_eta2p1_TriCentralPFJet50_40_30_v', 'HLT_IsoMu24_eta2p1_CentralPFJet30_BTagCSV07_v'])
    ),
    ## [optional] : when omitted no preselection is applied
    vertex = cms.PSet(
      src    = cms.InputTag("offlinePrimaryVertices"),
      select = cms.string('std::abs(obj.x())<1. && std::abs(obj.y())<1. && std::abs(obj.z())<20. && obj.tracksSize()>3 && !obj.isFake()')
    )                                       
  ), 
  ## ------------------------------------------------------
  ## SELECTION
  ##
  ## monitor histrograms are filled after each selection
  ## step, the selection is applied in the order defined
  ## by this vector
  ## [mandatory] : may be empty or contain an arbitrary
  ## number of PSets
  selection = cms.VPSet(
    cms.PSet(
      label  = cms.string("Hlt:step0"),
      src    = cms.InputTag(""),
      select = cms.string(""),
      min    = cms.int32(0),
      max    = cms.int32(0),
    ),
    cms.PSet(
      label  = cms.string("muons:step1"),
      src    = cms.InputTag("muons"),
      select = cms.string("obj.pt()>26 && std::abs(obj.eta())<2.1 && obj.isPFMuon() && obj.isGlobalMuon() && obj.globalTrack()->normalizedChi2()<10 && obj.innerTrack()->hitPattern().trackerLayersWithMeasurement()>5 && obj.innerTrack()->hitPattern().numberOfValidPixelHits()>0 && obj.numberOfMatches()>1 && (obj.pfIsolationR04().sumChargedHadronPt+obj.pfIsolationR04().sumPhotonEt+obj.pfIsolationR04().sumNeutralHadronEt)/obj.pt()<0.12"),
      min    = cms.int32(1),
      max    = cms.int32(1),
    ),
#    cms.PSet(
#      label  = cms.string("jets/pf:step2"),
#      src  = cms.InputTag("ak4PFJetsCHS"),
#      jetCorrector = cms.string("ak4PFCHSL2L3"),
#      select = cms.string("obj.pt()>20 && std::abs(obj.eta())<2.5"),
#      min = cms.int32(1),                                                
#    ), 
#    cms.PSet(
#      label  = cms.string("jets/pf:step3"),
#      src  = cms.InputTag("ak4PFJetsCHS"),
#      jetCorrector = cms.string("ak4PFCHSL2L3"),
#      select = cms.string("obj.pt()>20 && std::abs(obj.eta())<2.5"),
#      min = cms.int32(2),                                                
#    ), 
#    cms.PSet(
#      label  = cms.string("jets/pf:step4"),
#      src  = cms.InputTag("ak4PFJetsCHS"),
#      jetCorrector = cms.string("ak4PFCHSL2L3"),
#      select = cms.string("obj.pt()>20 && std::abs(obj.eta())<2.5"),
#      min = cms.int32(3),                                                
#    ), 
    cms.PSet(
#      label  = cms.string("jets/pf:step5"),
      label  = cms.string("jets/pf:step2"),
      src  = cms.InputTag("ak4PFJetsCHS"),
      #jetCorrector = cms.string("ak4PFCHSL2L3"),
      select = cms.string("obj.pt()>20 && std::abs(obj.eta())<2.5"),
      min = cms.int32(4),                                                
    ),
  )
)

topSingleElectronHLTOfflineDQM = cms.EDAnalyzer("TopSingleLeptonHLTOfflineDQM",
  ## ------------------------------------------------------
  ## SETUP
  ##
  ## configuration of the MonitoringEnsemble(s)
  ## [mandatory] : optional PSets may be omitted
  ##
  setup = cms.PSet(
    ## sub-directory to write the monitor histograms to
    ## [mandatory] : should not be changed w/o explicit
    ## communication to TopCom!
    directory = cms.string("HLT/TopHLTOffline/Top/SemiElectronic/"),
    ## [mandatory]
    sources = cms.PSet(
      muons = cms.InputTag("muons"),
      elecs = cms.InputTag("gedGsfElectrons"),
      jets  = cms.InputTag("ak4PFJetsCHS"),
      mets  = cms.VInputTag("met", "tcMet", "pfMet"),
      pvs   = cms.InputTag("offlinePrimaryVertices")

    ),
    ## [optional] : when omitted all monitoring plots for primary vertices
    ## will be filled w/o extras
    pvExtras = cms.PSet(
      ## when omitted electron plots will be filled w/o additional pre-
      ## selection of the primary vertex candidates                                                                                            
      select = cms.string("std::abs(obj.x())<1. && std::abs(obj.y())<1. && std::abs(obj.z())<20. && obj.tracksSize()>3 && !obj.isFake()")
    ),
    ## [optional] : when omitted all monitoring plots for electrons
    ## will be filled w/o extras
    elecExtras = cms.PSet(
      ## when omitted electron plots will be filled w/o cut on electronId
      #electronId = cms.PSet( src = cms.InputTag("mvaTrigV0"), pattern = cms.int32(1) ),
      ## when omitted electron plots will be filled w/o additional pre-
      ## selection of the electron candidates
      select = cms.string("obj.pt()>30 && std::abs(obj.eta())<2.5"),
      ## when omitted isolated electron multiplicity plot will be equi-
      ## valent to inclusive electron multiplicity plot 
      isolation = cms.string("(obj.dr03TkSumPt()+obj.dr04EcalRecHitSumEt()+obj.dr04HcalTowerSumEt())/obj.pt()<0.1"),
    ),
    ## [optional] : when omitted all monitoring plots for jets
    ## will be filled w/o extras
    jetExtras = cms.PSet(
      ## when omitted monitor plots for pt will be filled from uncorrected
      ## jets
      #jetCorrector = cms.string("ak4PFCHSL2L3"),
      ## when omitted no extra selection will be applied on jets before
      ## filling the monitor histograms; if jetCorrector is present the
      ## selection will be applied to corrected jets 
      select = cms.string("obj.pt()>20 && std::abs(obj.eta())<2.5"),
      ## when omitted monitor histograms for b-tagging will not be filled
      jetBTaggers  = cms.PSet(
         trackCountingEff = cms.PSet(
           label = cms.InputTag("jetProbabilityBJetTags" ),
           workingPoint = cms.double(0.275)
         ),
         trackCountingPur = cms.PSet(
           label = cms.InputTag("trackCountingHighPurBJetTags" ),
           workingPoint = cms.double(3.41)
         ),
         secondaryVertex  = cms.PSet(
           label = cms.InputTag("combinedSecondaryVertexBJetTags"),
           workingPoint = cms.double(0.679)
         )
       ),
    ),
    ## [optional] : when omitted no mass window will be applied
    ## for the W mass before filling the event monitoring plots
    massExtras = cms.PSet(
      lowerEdge = cms.double( 70.),
      upperEdge = cms.double(110.)
    ),
    ## [optional] : when omitted the monitoring plots for triggering
    ## will be empty
    triggerExtras = cms.PSet(
      src   = cms.InputTag("TriggerResults","","HLT"),
      paths = cms.vstring(['HLT_Ele23_WPLoose_Gsf_TriCentralPFJet50_40_30_v', 'HLT_Ele23_WPLoose_Gsf_CentralPFJet30_BTagCVS07_v', 'HLT_Ele27_WPLoose_Gsf_WHbbBoost_v', 'HLT_Ele27_WPLoose_Gsf_v', 'HLT_Ele27_WPLoose_Gsf_CentralPFJet30_BTagCSV07_v' 'HLT_Ele27_WPLoose_Gsf_TriCentralPFJet50_40_30_v', 'HLT_Ele27_eta2p1_WPLoose_Gsf_TriCentralPFJet30_v', 'HLT_Ele27_eta2p1_WPLoose_Gsf_TriCentralPFJet50_40_30_v', 'HLT_Ele27_eta2p1_WPLoose_Gsf_v', 'HLT_Ele27_eta2p1_WPLoose_Gsf_CentralPFJet30_BTagCSV07_v', 'HLT_Ele32_eta2p1_WPLoose_Gsf_TriCentralPFJet30_v', 'HLT_Ele32_eta2p1_WPLoose_Gsf_TriCentralPFJet50_40_30_v', 'HLT_Ele32_eta2p1_WPLoose_Gsf_v', 'HLT_Ele32_eta2p1_WPLoose_Gsf_CentralPFJet30_BTagCSV07_v'])
    )
  ),
  ## ------------------------------------------------------
  ## PRESELECTION
  ##
  ## setup of the event preselection, which will not
  ## be monitored
  ## [mandatory] : but may be empty
  ##
  preselection = cms.PSet(
   trigger = cms.PSet(
      src    = cms.InputTag("TriggerResults","","HLT"),
      select = cms.vstring(['HLT_Ele23_WPLoose_Gsf_TriCentralPFJet50_40_30_v', 'HLT_Ele23_WPLoose_Gsf_CentralPFJet30_BTagCVS07_v', 'HLT_Ele27_WPLoose_Gsf_WHbbBoost_v', 'HLT_Ele27_WPLoose_Gsf_v', 'HLT_Ele27_WPLoose_Gsf_CentralPFJet30_BTagCSV07_v' 'HLT_Ele27_WPLoose_Gsf_TriCentralPFJet50_40_30_v', 'HLT_Ele27_eta2p1_WPLoose_Gsf_TriCentralPFJet30_v', 'HLT_Ele27_eta2p1_WPLoose_Gsf_TriCentralPFJet50_40_30_v', 'HLT_Ele27_eta2p1_WPLoose_Gsf_v', 'HLT_Ele27_eta2p1_WPLoose_Gsf_CentralPFJet30_BTagCSV07_v', 'HLT_Ele32_eta2p1_WPLoose_Gsf_TriCentralPFJet30_v', 'HLT_Ele32_eta2p1_WPLoose_Gsf_TriCentralPFJet50_40_30_v', 'HLT_Ele32_eta2p1_WPLoose_Gsf_v', 'HLT_Ele32_eta2p1_WPLoose_Gsf_CentralPFJet30_BTagCSV07_v'])
    ),
    ## [optional] : when omitted no preselection is applied
    vertex = cms.PSet(
      src    = cms.InputTag("offlinePrimaryVertices"),
      select = cms.string('std::abs(obj.x())<1. && std::abs(obj.y())<1. && std::abs(obj.z())<20. && obj.tracksSize()>3 && !obj.isFake()')
    )                                       
  ), 
  ## ------------------------------------------------------
  ## SELECTION
  ##
  ## monitor histrograms are filled after each selection
  ## step, the selection is applied in the order defined
  ## by this vector
  ## [mandatory] : may be empty or contain an arbitrary
  ## number of PSets
  selection = cms.VPSet(
    cms.PSet(
      label  = cms.string("Hlt:step0"),
      src    = cms.InputTag(""),
      select = cms.string(""),
      min    = cms.int32(0),
      max    = cms.int32(0),
    ),
    cms.PSet(
      label = cms.string("elecs:step1"),
      src   = cms.InputTag("gedGsfElectrons"),
      #electronId = cms.PSet( src = cms.InputTag("mvaTrigV0"), pattern = cms.int32(1) ),
      select = cms.string("obj.pt()>30 && std::abs(obj.eta())<2.5 && (obj.dr03TkSumPt()+obj.dr04EcalRecHitSumEt()+obj.dr04HcalTowerSumEt())/obj.pt()<0.1"),
      min = cms.int32(1),
      max = cms.int32(1),
    ),
#    cms.PSet(
#      label  = cms.string("jets/pf:step2"),
#      src  = cms.InputTag("ak4PFJetsCHS"),
#      jetCorrector = cms.string("ak4PFCHSL2L3"),
#      select = cms.string("obj.pt()>20 && std::abs(obj.eta())<2.5"),
#      min = cms.int32(1),                                                
#    ), 
#    cms.PSet(
#      label  = cms.string("jets/pf:step3"),
#      src  = cms.InputTag("ak4PFJetsCHS"),
#      jetCorrector = cms.string("ak4PFCHSL2L3"),
#      select = cms.string("obj.pt()>20 && std::abs(obj.eta())<2.5"),
#      min = cms.int32(2),                                                
#    ), 
#    cms.PSet(
#      label  = cms.string("jets/pf:step4"),
#      src  = cms.InputTag("ak4PFJetsCHS"),
#      jetCorrector = cms.string("ak4PFCHSL2L3"),
#      select = cms.string("obj.pt()>20 && std::abs(obj.eta())<2.5"),
#      min = cms.int32(3),                                                
#    ), 
    cms.PSet(
#      label  = cms.string("jets/pf:step5"),
      label  = cms.string("jets/pf:step2"),
      src  = cms.InputTag("ak4PFJetsCHS"),
      #jetCorrector = cms.string("ak4PFCHSL2L3"),
      select = cms.string("obj.pt()>20 && std::abs(obj.eta())<2.5"),
      min = cms.int32(4),                                                
    ),
  )
)
