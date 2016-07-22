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

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/Math/interface/deltaR.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"

#include "TTree.h"
#include "TRandom.h"

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
    edm::InputTag vtxTag_;
    edm::InputTag ebclusTag_;
    edm::InputTag ebtimeTag_;
    edm::InputTag genPartTag_;
    
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
    float chIsoAll_;
    float chIsoZCut_;
    float chIsoZTCut_;
    float ecalPFIsoAll_;
    float ecalPFIsoTCut_;
    bool genMatched_;
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
  vtxTag_("offlinePrimaryVertices4D",""),
  ebclusTag_("particleFlowClusterECAL"),
  ebtimeTag_("ecalBarrelClusterFastTimer","PerfectResolutionModel"),
  genPartTag_("genParticles")
{
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
  tree_->Branch("chIsoAll",&chIsoAll_);
  tree_->Branch("chIsoZCut",&chIsoZCut_);
  tree_->Branch("chIsoZTCut",&chIsoZTCut_);
  tree_->Branch("ecalPFIsoAll",&ecalPFIsoAll_);
  tree_->Branch("ecalPFIsoTCut",&ecalPFIsoTCut_);
  tree_->Branch("genMatched",&genMatched_);
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

  edm::Handle<std::vector<std::vector<float> > > ebtime;
  iEvent.getByLabel(ebtimeTag_,ebtime);
  
  edm::Handle<reco::GenParticleCollection> genparts;
  iEvent.getByLabel(genPartTag_,genparts);
  
  const reco::Vertex &vtx = vtxs->front();

  
  for (const reco::Muon &muon : *muons) {
    
    if (muon.pt()<5. || std::abs(muon.eta())>1.4442) continue;
    
    if (muon.track().isNull()) continue;
    
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
    chIsoAll_ = 0.;
    chIsoZCut_ = 0.;
    chIsoZTCut_ = 0.;
    
    double ecalisoall = 0.;
    double ecalisotcut = 0.;
    
    genMatched_ = false;
    genPt_ = -99.;
    genEta_ = -99.;
    genPhi_ = -99.;
    
    double mindr = std::numeric_limits<double>::max();
    for (const reco::GenParticle &p : *genparts) {
      if (p.status()!=3) continue;
      if (std::abs(p.pdgId())!=13) continue;
      
      double dr = reco::deltaR(muon,p);
      if (dr<0.2 && dr<mindr) {
        mindr = dr;
        genMatched_ = true;
        genPt_ = p.pt();
        genEta_ = p.eta();
        genPhi_ = p.phi();
      }
    }
    
    unsigned int iclus = 0;
    for (const reco::PFCluster &clus : *ebclus) {
      double pt = clus.energy()/cosh(clus.eta());
      double t = (*ebtime)[0][iclus];

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
