#ifndef RecoParticleFlow_PFTimeAssigner_PFLinker_h
#define RecoParticleFlow_PFTimeAssigner_PFLinker_h

/** \class PFTimeAssigner
 *  Takes PFCandidates that do not have timing information, then calculates it if able.
 *
 * Creates new PFCandidates with timing information filled in.
 *  \author L. Gray (FNAL)
 */

#include <iostream>
#include <string>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"

#include <string>

class PFTimeAssigner : public edm::global::EDProducer<> {
 public:

  enum electron_assign { ELEC_NO_TIME=0, ELEC_TRACK_TIME=1, ELEC_CLUS_TIME=2 };
  enum pimu_assign { PIMU_NO_TIME=0, PIMU_TRACK_TIME=1 };
  enum gammah0_assign { GAMMAH0_NO_TIME=0, GAMMAH0_CLUS_TIME=2 };
  
  explicit PFTimeAssigner(const edm::ParameterSet&);

  virtual ~PFTimeAssigner() {}

  virtual void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const override;
  
 private:
 
  /// Input PFCandidates
  const edm::EDGetTokenT<reco::PFCandidateCollection> pfCandidates_;

  // track times
  const edm::EDGetTokenT<edm::ValueMap<float> > tracksTime_, tracksTimeReso_;
  const edm::EDGetTokenT<edm::ValueMap<float> > gsfTracksTime_, gsfTracksTimeReso_;
  
};

DEFINE_FWK_MODULE(PFTimeAssigner);

PFTimeAssigner::PFTimeAssigner(const edm::ParameterSet& conf): 
  pfCandidates_( consumes<reco::PFCandidateCollection>(conf.getParameter<edm::InputTag>("pfCandidateSrc") ) ),
  tracksTime_( consumes<edm::ValueMap<float> >( conf.getParameter<edm::InputTag>("trackTimeSrc") ) ),
  tracksTimeReso_( consumes<edm::ValueMap<float> >( conf.getParameter<edm::InputTag>("trackTimeResoSrc") ) ),
  gsfTracksTime_( consumes<edm::ValueMap<float> >( conf.getParameter<edm::InputTag>("gsfTrackTimeSrc") ) ),
  gsfTracksTimeReso_( consumes<edm::ValueMap<float> >( conf.getParameter<edm::InputTag>("gsfTrackTimeResoSrc") ) ) {
  produces<reco::PFCandidateCollection>();
}

void PFTimeAssigner::produce(edm::StreamID, edm::Event& ev, const edm::EventSetup& es) const {
  edm::Handle<reco::PFCandidateCollection> pfCandidatesH;
  ev.getByToken(pfCandidates_,pfCandidatesH);
  const auto& pfCandidates = *pfCandidatesH;
  
  edm::Handle<edm::ValueMap<float> > tracksTimeH;
  ev.getByToken(tracksTime_,tracksTimeH);
  const auto& tracksTime = *tracksTimeH;
  
  edm::Handle<edm::ValueMap<float> > tracksTimeResoH;
  ev.getByToken(tracksTimeReso_,tracksTimeResoH);
  const auto& tracksTimeReso = *tracksTimeResoH;

  edm::Handle<edm::ValueMap<float> > gsfTracksTimeH;
  ev.getByToken(gsfTracksTime_,gsfTracksTimeH);
  const auto& gsfTracksTime = *gsfTracksTimeH;
  
  edm::Handle<edm::ValueMap<float> > gsfTracksTimeResoH;
  ev.getByToken(gsfTracksTimeReso_,gsfTracksTimeResoH);
  const auto& gsfTracksTimeReso = *gsfTracksTimeResoH;

  auto out = std::make_unique<reco::PFCandidateCollection>();

  for( const auto& cand : pfCandidates ) {
    out->push_back(cand);
    auto& copy = out->back();
    switch( std::abs( copy.pdgId() ) ) {
    case 11:
      {
	auto ref = copy.gsfTrackRef();
	if( ref.isNonnull() ) { copy.setTime(gsfTracksTime[ref],gsfTracksTimeReso[ref]); }
      }
      break;
    case 13:
    case 211:
      {
	auto ref = copy.trackRef();
	if( ref.isNonnull() ) { copy.setTime(tracksTime[ref],tracksTimeReso[ref]); }
      }
      break;
      // no photons or neutral hadrons yet, photons should be OK already in HGCal
      // neutral hadrons will need do be recalculated at some point
    default:
      break;
    };
  }
  
  ev.put( std::move(out) );
}

#endif
