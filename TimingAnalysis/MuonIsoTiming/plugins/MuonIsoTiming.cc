// -*- C++ -*-
//
// Package:    MuonIsoTiming
// Class:      MuonIsoTiming
// 
/**\class MuonIsoTiming MuonIsoTiming.cc TimingAnalysis/MuonIsoTiming/plugins/MuonIsoTiming.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Josh Bendavid
//         Created:  Fri, 22 Jul 2016 00:03:02 GMT
// $Id$
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/EDConsumerBase.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

#include "DataFormats/ParticleFlowReco/interface/PFClusterFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/Math/interface/deltaR.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"

#include "DataFormats/MuonReco/interface/MuonSelectors.h"

#include "TTree.h"
#include "TRandom.h"

#include <unordered_map>

//
// class declaration
//

class MuonIsoTiming : public edm::EDAnalyzer {
   public:
      explicit MuonIsoTiming(const edm::ParameterSet&);
      ~MuonIsoTiming();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

      // ----------member data ---------------------------
      
//     edm::EDGetTokenT<reco::VertexCollection>               vtxToken_;
//     edm::EDGetTokenT<reco::MuonCollection>                   muonToken_;
//     edm::EDGetTokenT<reco::VertexCollection>                 vtxToken_;
//     edm::EDGetTokenT<reco::PFClusterCollection>              ebclusToken_;
//     edm::EDGetTokenT<std::vector<std::vector<float> > >      ebtimeToken_;
    
   
    edm::InputTag muonTag_;
    edm::InputTag tracksTag_;
    edm::InputTag timesTag_, timeResosTag_;
    edm::InputTag vtxTag_;
    edm::InputTag ebclusTag_;
    edm::InputTag ebtimeTag_;
    edm::InputTag genPartTag_;
    edm::InputTag genJetsTag_;
    
    TTree *tree_;
    
    float pt_;
    float eta_;
    float phi_;
    float px_;
    float py_;
    float pz_;
    float z_;
    float vtxX_;
    float vtxY_;
    float vtxZ_;
    float vtxT_;
    float chIsoZCut_;
    float chIsoZTCut_;
    float chIsoZTCut_3sigma_;
    float chIsoZTCut_4sigma_;
    float chIsoZTCut_5sigma_;
    float chIsoZTCut_6sigma_;
    float ecalPFIsoAll_;
    float ecalPFIsoTCut_;
    int vtxIndex_;
    bool isLooseMuon_;
    bool genMatched_;    
    bool genMatchedPrompt_;    
    bool genMatchedJet_;
    float genPt_;
    float genEta_;
    float genPhi_;
    std::vector<float> clusE_;
    std::vector<float> clusPt_;
    std::vector<float> clusEta_;
    std::vector<float> clusPhi_;
    std::vector<float> clusT_;
    std::vector<float> clusTsmeared_;
    
    
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
MuonIsoTiming::MuonIsoTiming(const edm::ParameterSet& iConfig) :
  muonTag_("muons",""),	
  tracksTag_("generalTracks",""),
  timesTag_("trackTimeValueMapProducer","generalTracksConfigurableFlatResolutionModel"), 
  timeResosTag_("trackTimeValueMapProducer","generalTracksConfigurableFlatResolutionModelResolution"),
  vtxTag_("offlinePrimaryVertices4D",""),
  ebclusTag_("particleFlowClusterECAL"),
  ebtimeTag_("ecalBarrelClusterFastTimer","PerfectResolutionModel"),
  genPartTag_("genParticles"),
  genJetsTag_("ak4GenJets")
{
  consumes<reco::MuonCollection>(muonTag_);
  consumes<edm::View<reco::Track> >(tracksTag_);
  consumes<edm::ValueMap<float> >(timesTag_);
  consumes<edm::ValueMap<float> >(timeResosTag_);
  consumes<std::vector<reco::Vertex> >(vtxTag_);
  consumes<reco::PFClusterCollection>(ebclusTag_);
  consumes<edm::ValueMap<float> >(ebtimeTag_);
  consumes<reco::GenParticleCollection>(genPartTag_);
  consumes<std::vector<reco::GenJet> >(genJetsTag_);
	   
  
   //now do what ever initialization is needed
  
  edm::Service<TFileService> fs;
  tree_ = fs->make<TTree>("TimingTree","");
  
  
  tree_->Branch("pt",&pt_);
  tree_->Branch("eta",&eta_);
  tree_->Branch("phi",&phi_);
  tree_->Branch("px",&px_);
  tree_->Branch("py",&py_);
  tree_->Branch("pz",&pz_);
  tree_->Branch("z",&z_);
  tree_->Branch("vtxX",&vtxX_);
  tree_->Branch("vtxY",&vtxY_);
  tree_->Branch("vtxZ",&vtxZ_);
  tree_->Branch("vtxT",&vtxT_);
  tree_->Branch("chIsoZCut",&chIsoZCut_);
  tree_->Branch("chIsoZTCut_3sigma",&chIsoZTCut_3sigma_);
  tree_->Branch("chIsoZTCut_4sigma",&chIsoZTCut_4sigma_);
  tree_->Branch("chIsoZTCut_5sigma",&chIsoZTCut_5sigma_);
  tree_->Branch("chIsoZTCut_6sigma",&chIsoZTCut_6sigma_);
  tree_->Branch("ecalPFIsoAll",&ecalPFIsoAll_);
  tree_->Branch("ecalPFIsoTCut",&ecalPFIsoTCut_);
  tree_->Branch("vtxIndex",&vtxIndex_);
  tree_->Branch("isLooseMuon",&isLooseMuon_);
  tree_->Branch("genMatched",&genMatched_);
  tree_->Branch("genMatchedPrompt",&genMatchedPrompt_);
  tree_->Branch("genMatchedJet",&genMatchedJet_);
  tree_->Branch("genPt",&genPt_);
  tree_->Branch("genEta",&genEta_);
  tree_->Branch("genPhi",&genPhi_);
  tree_->Branch("clusE",&clusE_);
  tree_->Branch("clusPt",&clusPt_);
  tree_->Branch("clusEta",&clusEta_);
  tree_->Branch("clusPhi",&clusPhi_);
  tree_->Branch("clusT",&clusT_);
  tree_->Branch("clusTsmeared",&clusTsmeared_);

}


MuonIsoTiming::~MuonIsoTiming()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
MuonIsoTiming::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  


//   edm::Handle<reco::MuonCollection> muons;
//   iEvent.getByToken(muonToken_,muons);
// 
//   edm::Handle<reco::VertexCollection> vtxs;
//   iEvent.getByToken(vtxToken_,vtxs);
//   
//   edm::Handle<reco::PFClusterCollection> ebclus;
//   iEvent.getByToken(ebclusToken_,ebclus);
// 
//   edm::Handle<std::vector<std::vector<float> > > ebtime;
//   iEvent.getByToken(ebtimeToken_,ebtime);
  
  edm::Handle<reco::MuonCollection> muons;
  iEvent.getByLabel(muonTag_,muons);
  
  unsigned int nmuons = 0;
  for (const reco::Muon &muon : *muons) {
    if (muon.pt()<5. || std::abs(muon.eta())>1.4442) continue;
    ++nmuons;
  }
  
  if (!nmuons) return;
  
  edm::Handle<reco::VertexCollection> vtxs;
  iEvent.getByLabel(vtxTag_,vtxs);
  
  edm::Handle<reco::PFClusterCollection> ebclus;
  iEvent.getByLabel(ebclusTag_,ebclus);

  edm::Handle<edm::ValueMap<float> > ebtime;
  iEvent.getByLabel(ebtimeTag_,ebtime);

  edm::Handle<edm::View<reco::Track> > tracks;
  iEvent.getByLabel(tracksTag_,tracks);

  edm::Handle<edm::ValueMap<float> > times;
  iEvent.getByLabel(timesTag_,times);

  edm::Handle<edm::ValueMap<float> > timeResos;
  iEvent.getByLabel(timeResosTag_,timeResos);

  edm::Handle<reco::GenParticleCollection> genparts;
  iEvent.getByLabel(genPartTag_,genparts);
  

  edm::Handle<std::vector<reco::GenJet> > genjets;
  iEvent.getByLabel(genJetsTag_,genjets);
    
  //make a map of vertices to track refs within cuts
  std::unordered_multimap<unsigned,reco::TrackBaseRef> vertices_to_tracks_z, vertices_to_tracks_zt3, vertices_to_tracks_zt4, vertices_to_tracks_zt5, vertices_to_tracks_zt6 ;
  for( unsigned i = 0; i < tracks->size(); ++i ) {
    auto ref = tracks->refAt(i);
    const float time = (*times)[ref];
    const float timeReso = (*timeResos)[ref] != 0.f ? (*timeResos)[ref] : 0.170f;
    for( unsigned ivtx = 0; ivtx < vtxs->size(); ++ivtx ) {
      const auto& thevtx = (*vtxs)[ivtx];
      const float dz = std::abs(ref->dz(thevtx.position()));
      const float dt = std::abs(time - thevtx.t());
      const bool useTime = (thevtx.t() != 0.);

      const float base_cut = std::sqrt(thevtx.tError()*thevtx.tError()
					    + timeReso*timeReso);

      const float time_cut3 = 3.f*base_cut;
      const float time_cut4 = 4.f*base_cut;
      const float time_cut5 = 5.f*base_cut;
      const float time_cut6 = 6.f*base_cut;

      const bool keepz = ( dz < 0.2f );
      const bool keept3 = (!useTime || dt < time_cut3);
      const bool keept4 = (!useTime || dt < time_cut4);
      const bool keept5 = (!useTime || dt < time_cut5);
      const bool keept6 = (!useTime || dt < time_cut6);
      
      if( keepz ) {
	vertices_to_tracks_z.emplace(ivtx,ref);
	if( keept6 ) {
	  vertices_to_tracks_zt6.emplace(ivtx,ref);
	}
	if( keept5 ) {
	  vertices_to_tracks_zt5.emplace(ivtx,ref);
	}
	if( keept4 ) {
	  vertices_to_tracks_zt4.emplace(ivtx,ref);
	}
	if( keept3 ) {
	  vertices_to_tracks_zt3.emplace(ivtx,ref);
	}	
      }      
    }
  }
  
  for (const reco::Muon &muon : *muons) {    

    if( muon.pt()<5. ) continue;
    
    if( muon.track().isNull() ) continue;
    
    isLooseMuon_ = muon::isLooseMuon(muon);

    int vtx_index = -1;

    // find the 4D vertex this muon is best associated to..
    float max_weight = 0.f;
    for( unsigned i = 0; i < vtxs->size(); ++i ) {
      const auto& vtx = (*vtxs)[i];      
      const float weight = vtx.trackWeight(muon.track());
      if( weight > max_weight ) {
	max_weight = weight;
	vtx_index = i;
      }
    }
    
    vtxIndex_ = vtx_index;

    // use highest ranked if muon doesn't belong to any vertex
    const reco::Vertex& vtx = (vtx_index == -1 ? (*vtxs)[0] : (*vtxs)[vtx_index]);
    const auto tracks_z  = vertices_to_tracks_z.equal_range(vtx_index);
    const auto tracks_zt3 = vertices_to_tracks_zt3.equal_range(vtx_index);
    const auto tracks_zt4 = vertices_to_tracks_zt4.equal_range(vtx_index);
    const auto tracks_zt5 = vertices_to_tracks_zt5.equal_range(vtx_index);
    const auto tracks_zt6 = vertices_to_tracks_zt6.equal_range(vtx_index);

    
//     printf("found muon, pt = %5f, eta = %5f\n",muon.pt(),muon.eta());
    
    clusE_.clear();
    clusPt_.clear();
    clusEta_.clear();
    clusPhi_.clear();
    clusT_.clear();
    clusTsmeared_.clear();
    
    pt_ = muon.pt();
    eta_ = muon.eta();
    phi_ = muon.phi();
    px_ = muon.px();
    py_ = muon.py();
    pz_ = muon.pz();
    z_ = muon.track()->dz(vtx.position()) + vtx.z();
    vtxX_ = vtx.x();
    vtxY_ = vtx.y();
    vtxZ_ = vtx.z();
    vtxT_ = vtx.t();
    chIsoZCut_ = 0.;
    chIsoZTCut_3sigma_ = 0.;
    chIsoZTCut_4sigma_ = 0.;
    chIsoZTCut_5sigma_ = 0.;
    chIsoZTCut_6sigma_ = 0.;
    
    
    for( auto it = tracks_z.first; it != tracks_z.second; ++it ) {
      auto ref = it->second.castTo<reco::TrackRef>();
      if( !ref->quality(reco::TrackBase::highPurity) ) continue;
      if( ref == muon.track() ) continue;
      if( reco::deltaR2(ref->eta(), ref->phi(), muon.eta(), muon.phi()) >= 0.3*0.3 ) continue;
      chIsoZCut_ += ref->pt();
    }

    for( auto it = tracks_zt3.first; it != tracks_zt3.second; ++it ) {
      auto ref = it->second.castTo<reco::TrackRef>();
      if( !ref->quality(reco::TrackBase::highPurity) ) continue;
      if( ref == muon.track() ) continue;
      if( reco::deltaR2(ref->eta(), ref->phi(), muon.eta(), muon.phi()) >= 0.3*0.3 ) continue;
      chIsoZTCut_3sigma_ += ref->pt();
    }

    for( auto it = tracks_zt4.first; it != tracks_zt4.second; ++it ) {
      auto ref = it->second.castTo<reco::TrackRef>();
      if( !ref->quality(reco::TrackBase::highPurity) ) continue;
      if( ref == muon.track() ) continue;
      if( reco::deltaR2(ref->eta(), ref->phi(), muon.eta(), muon.phi()) >= 0.3*0.3 ) continue;
      chIsoZTCut_4sigma_ += ref->pt();
    }
    
    for( auto it = tracks_zt5.first; it != tracks_zt5.second; ++it ) {
      auto ref = it->second.castTo<reco::TrackRef>();
      if( !ref->quality(reco::TrackBase::highPurity) ) continue;
      if( ref == muon.track() ) continue;
      if( reco::deltaR2(ref->eta(), ref->phi(), muon.eta(), muon.phi()) >= 0.3*0.3 ) continue;
      chIsoZTCut_5sigma_ += ref->pt();
    }

    for( auto it = tracks_zt6.first; it != tracks_zt6.second; ++it ) {
      auto ref = it->second.castTo<reco::TrackRef>();
      if( !ref->quality(reco::TrackBase::highPurity) ) continue;
      if( ref == muon.track() ) continue;
      if( reco::deltaR2(ref->eta(), ref->phi(), muon.eta(), muon.phi()) >= 0.3*0.3 ) continue;
      chIsoZTCut_6sigma_ += ref->pt();
    }


    double ecalisoall = 0.;
    double ecalisotcut = 0.;
    
    genMatched_ = false;
    genMatchedPrompt_ = false;
    genMatchedJet_ = false;
    genPt_ = -99.;
    genEta_ = -99.;
    genPhi_ = -99.;
    
    double mindr = std::numeric_limits<double>::max();
    for (const reco::GenParticle &p : *genparts) {
      if ( p.status() != 1 ) continue;
      if (std::abs(p.pdgId())!=13) continue;
      
      double dr = reco::deltaR(muon,p);
      if( dr<0.2 && dr<mindr ) {
        mindr = dr;	
        genMatched_ = true;
	genMatchedPrompt_ = p.isPromptFinalState();
        genPt_ = p.pt();
        genEta_ = p.eta();
        genPhi_ = p.phi();
      }      
    }

    mindr = std::numeric_limits<double>::max();
    for( const auto& jet : *genjets ) {
      if( jet.pt() < 15.0 ) continue;
      double dr = reco::deltaR(muon,jet);
      if( dr < 0.3 && dr < mindr ) {
	genMatchedJet_ = true;
      }
    }
    
    unsigned int iclus = 0;
    for (const reco::PFCluster &clus : *ebclus) {
      edm::Ref<std::vector<reco::PFCluster> > clusref(ebclus,iclus);
      double pt = clus.energy()/cosh(clus.eta());
      double t = 0;

      ++iclus;
      if (pt < 1.) continue;

      math::XYZVector clusdir(clus.x()-vtx.x(), clus.y()-vtx.y(), clus.z()-vtx.z());
      double dr = reco::deltaR(muon,clusdir);
      if (dr>0.4) continue;
      
      double tsmeared = t + gRandom->Gaus(0.,0.030);
      
      clusE_.push_back(clus.energy());
      clusPt_.push_back(pt);
      clusEta_.push_back(clus.eta());
      clusPhi_.push_back(clus.phi());
      clusT_.push_back(t);
      clusTsmeared_.push_back(tsmeared);
      
      ecalisoall += pt;
      
      double dt = std::abs(tsmeared - vtx.t());
      if (dt<0.045) {
        ecalisotcut += pt;
      }
    }
    
    ecalPFIsoAll_ = ecalisoall;
    ecalPFIsoTCut_ = ecalisotcut;
       
    tree_->Fill();
  }
  

// #ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
//    ESHandle<SetupData> pSetup;
//    iSetup.get<SetupRecord>().get(pSetup);
// #endif
}


// ------------ method called once each job just before starting event loop  ------------
void 
MuonIsoTiming::beginJob()
{
  
//   muonToken_ = consumes<reco::MuonCollection>(edm::InputTag("muons",""));
//   vtxToken_ = consumes<reco::VertexCollection>(edm::InputTag("offlinePrimaryVertices4D",""));
//   ebclusToken_ = consumes<reco::PFClusterCollection>(edm::InputTag("particleFlowClusterECAL"));
//   ebtimeToken_ = consumes<std::vector<std::vector<float> > >(edm::InputTag("ecalBarrelClusterFastTimer","PerfectResolutionModel"));
  
  


  
  

  

  
}

// ------------ method called once each job just after ending the event loop  ------------
void 
MuonIsoTiming::endJob() 
{
  
//   tree_->Write();
}

// ------------ method called when starting to processes a run  ------------
/*
void 
MuonIsoTiming::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a run  ------------
/*
void 
MuonIsoTiming::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
MuonIsoTiming::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
MuonIsoTiming::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
MuonIsoTiming::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(MuonIsoTiming);
