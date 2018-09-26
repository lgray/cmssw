// This producer assigns vertex times (with a specified resolution) to tracks.
// The times are produced as valuemaps associated to tracks, so the track dataformat doesn't
// need to be modified.

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Math/interface/deltaPhi.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertex.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertexContainer.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"

// track trigger stuff
#include "DataFormats/L1TrackTrigger/interface/TTTypes.h"
#include "DataFormats/L1TrackTrigger/interface/TTTrack.h"
#include "SimTracker/TrackTriggerAssociation/interface/TTStubAssociationMap.h"
#include "SimTracker/TrackTriggerAssociation/interface/TTTrackAssociationMap.h"
#include "SimTracker/TrackTriggerAssociation/plugins/TTStubAssociator.h"
#include "SimTracker/TrackTriggerAssociation/plugins/TTClusterAssociator.h"
#include "SimTracker/TrackTriggerAssociation/plugins/TTTrackAssociator.h"

#include <random>
#include <memory>

#include "SimTracker/TrackTriggerAssociation/interface/L1ResolutionModel.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "FWCore/Utilities/interface/isFinite.h"
#include "FWCore/Utilities/interface/transform.h"

class L1TrackTimeValueMapProducer : public edm::global::EDProducer<> {
public:    
  L1TrackTimeValueMapProducer(const edm::ParameterSet&);
  ~L1TrackTimeValueMapProducer() override { }
  
  void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const override;
  
private:
  // inputs
  edm::EDGetTokenT<edm::View<TTTrack<Ref_Phase2TrackerDigi_> > > tracks_;
  const std::string tracksName_;
  edm::EDGetTokenT<TTTrackAssociationMap<Ref_Phase2TrackerDigi_> > ttTrackTruth_;  
  const edm::EDGetTokenT<std::vector<PileupSummaryInfo> > pileupSummaryInfo_;  
  // eta bounds
  const float etaMin_, etaMax_, ptMin_, pMin_, etaMaxForPtThreshold_;
  // options
  std::vector<std::unique_ptr<const L1ResolutionModel> > resolutions_;
  // functions
  float extractTrackVertexTime(const TrackingParticle&, const reco::TransientTrack&) const;
  reco::Track buildTrackFromL1Track(const TTTrack<Ref_Phase2TrackerDigi_>& tt) const;
};

DEFINE_FWK_MODULE(L1TrackTimeValueMapProducer);

namespace {
  constexpr float m_pion = 139.57061e-3;
  const std::string resolution("Resolution");

  template<typename ParticleType, typename T>
  void writeValueMap(edm::Event &iEvent,
                     const edm::Handle<edm::View<ParticleType> > & handle,
                     const std::vector<T> & values,
                     const std::string    & label) {
    std::unique_ptr<edm::ValueMap<T> > valMap(new edm::ValueMap<T>());
    typename edm::ValueMap<T>::Filler filler(*valMap);
    filler.insert(handle, values.begin(), values.end());
    filler.fill();
    iEvent.put(std::move(valMap), label);
  }
}

L1TrackTimeValueMapProducer::L1TrackTimeValueMapProducer(const edm::ParameterSet& conf) :
  tracks_(consumes<edm::View<TTTrack<Ref_Phase2TrackerDigi_> > >( conf.getParameter<edm::InputTag>("tkTriggerTrackSrc") ) ),
  tracksName_(conf.getParameter<edm::InputTag>("tkTriggerTrackSrc").label()),
  ttTrackTruth_(consumes<TTTrackAssociationMap<Ref_Phase2TrackerDigi_> >(conf.getParameter<edm::InputTag>("tkTriggerTrackTruth") ) ),
  pileupSummaryInfo_(consumes<std::vector<PileupSummaryInfo> >( conf.getParameter<edm::InputTag>("pileupSummaryInfo") ) ),
  etaMin_( conf.getParameter<double>("etaMin") ),
  etaMax_( conf.getParameter<double>("etaMax") ),
  ptMin_( conf.getParameter<double>("ptMin") ),
  pMin_( conf.getParameter<double>("pMin") ),
  etaMaxForPtThreshold_( conf.getParameter<double>("etaMaxForPtThreshold") )
{
  // setup resolution models
  const std::vector<edm::ParameterSet>& resos = conf.getParameterSetVector("resolutionModels");
  for( const auto& reso : resos ) {
    const std::string& name = reso.getParameter<std::string>("modelName");
    L1ResolutionModel* resomod = L1ResolutionModelFactory::get()->create(name,reso);
    resolutions_.emplace_back( resomod );  

    // times and time resolutions for general tracks
    produces<edm::ValueMap<float> >(tracksName_+name);
    produces<edm::ValueMap<float> >(tracksName_+name+resolution);
  }
}

void L1TrackTimeValueMapProducer::produce(edm::StreamID sid, edm::Event& evt, const edm::EventSetup& es) const {
  // get sim track associators
  edm::Handle<TTTrackAssociationMap<Ref_Phase2TrackerDigi_> > TTTrackAssociationMapHandle;
  evt.getByToken(ttTrackTruth_, TTTrackAssociationMapHandle);
  const TTTrackAssociationMap<Ref_Phase2TrackerDigi_>& TTTrackAssociation = *TTTrackAssociationMapHandle;

  std::vector<float> ttTrackTimes;
 
  //get track collections
  edm::Handle<edm::View<TTTrack<Ref_Phase2TrackerDigi_> > > TrackCollectionH;
  evt.getByToken(tracks_, TrackCollectionH);
  const edm::View<TTTrack<Ref_Phase2TrackerDigi_> >& TrackCollection = *TrackCollectionH;
  
  edm::Handle<std::vector<PileupSummaryInfo> > pileupSummaryH;
  evt.getByToken(pileupSummaryInfo_, pileupSummaryH);
  
  //transient track builder
  edm::ESHandle<TransientTrackBuilder> theB;
  es.get<TransientTrackRecord>().get("TransientTrackBuilder",theB);
  
  double sumSimTime = 0.;
  double sumSimTimeSq = 0.;
  int nsim = 0;
  for (const PileupSummaryInfo &puinfo : *pileupSummaryH) {
    if (puinfo.getBunchCrossing() == 0) {
      for (const float &time : puinfo.getPU_times()) {
        double simtime = time;
        sumSimTime += simtime;
        sumSimTimeSq += simtime*simtime;
        ++nsim;
      }
      break;
    }
  }
  
  double meanSimTime = sumSimTime/double(nsim);
  double varSimTime = sumSimTimeSq/double(nsim) - meanSimTime*meanSimTime;
  double rmsSimTime = std::sqrt(std::max(0.1*0.1,varSimTime));
  std::normal_distribution<float> gausSimTime(meanSimTime, rmsSimTime);

  // get event-based seed for RNG
  unsigned int runNum_uint = static_cast <unsigned int> (evt.id().run());
  unsigned int lumiNum_uint = static_cast <unsigned int> (evt.id().luminosityBlock());
  unsigned int evNum_uint = static_cast <unsigned int> (evt.id().event());
  unsigned int tkChi2_uint = uint32_t(TrackCollection.empty() ? 0 : TrackCollection.ptrAt(0)->getChi2()/0.01);
  std::uint32_t seed = tkChi2_uint + (lumiNum_uint<<10) + (runNum_uint<<20) + evNum_uint;
  std::mt19937 rng(seed);

  auto TTTrackToTrackingParticleMap = TTTrackAssociation.getTTTrackToTrackingParticleMap(); 

  for( unsigned itk = 0; itk < TrackCollection.size(); ++itk ) {
    const auto tkref = TrackCollection.ptrAt(itk);
    const auto the_tp = TTTrackToTrackingParticleMap.find(tkref);
    
    if (the_tp != TTTrackToTrackingParticleMap.end()) {
      reco::TransientTrack tt = theB->build(buildTrackFromL1Track(*tkref));
      float time = extractTrackVertexTime(*(the_tp->second),tt);
      ttTrackTimes.push_back(time);
    }
    else {
      float rndtime = gausSimTime(rng);
      ttTrackTimes.push_back(rndtime);
      LogDebug("TooManyTracks") << "TTTrack not matched to any tracking particles!" << std::endl;
    }
  }
  
  for( const auto& reso : resolutions_ ) {
    const std::string& name = reso->name();
    std::vector<float> times, resos;
    
    times.reserve(TrackCollection.size());
    resos.reserve(TrackCollection.size());

    for( unsigned i = 0; i < TrackCollection.size(); ++i ) {
      const auto& tk = TrackCollection[i];
      const auto mom = tk.getMomentum();
      const float absEta = std::abs(mom.eta());
      bool inAcceptance = absEta < etaMax_ && absEta >= etaMin_ && mom.mag()>pMin_ && (absEta>etaMaxForPtThreshold_ || mom.perp()>ptMin_);
      if (inAcceptance) {
        const float resolution = reso->getTimeResolution(tk);
        std::normal_distribution<float> gausGeneralTime(ttTrackTimes[i], resolution);
        times.push_back( gausGeneralTime(rng) );
        resos.push_back( resolution );
      }
      else {
        times.push_back(0.0f);
        resos.push_back(-1.);
      }
    }

    writeValueMap( evt, TrackCollectionH, times, tracksName_+name );
    writeValueMap( evt, TrackCollectionH, resos, tracksName_+name+resolution );
  }
}

float L1TrackTimeValueMapProducer::
extractTrackVertexTime( const TrackingParticle &tp, const reco::TransientTrack &tt ) const {
  int pdgid = tp.pdgId();
  const auto& tvertex = tp.parentVertex();
  math::XYZTLorentzVectorD result = tvertex->position();
  
  // account for secondary vertices...
  if( tvertex->nSourceTracks() && tvertex->sourceTracks()[0]->pdgId()==pdgid ) {
    auto pvertex = tvertex->sourceTracks()[0]->parentVertex();
    result = pvertex->position();
    while( pvertex->nSourceTracks() && pvertex->sourceTracks()[0]->pdgId()==pdgid ) {
      pvertex = pvertex->sourceTracks()[0]->parentVertex();
      result = pvertex->position();
    }
  }
  
  float time = result.T()*CLHEP::second;
  //correct for time of flight from track reference position
  GlobalPoint result_pos(result.x(),result.y(),result.z());
  const auto &tkstate = tt.trajectoryStateClosestToPoint(result_pos);
  float tkphi = tkstate.momentum().phi();
  float tkz = tkstate.position().z();
  float dphi = reco::deltaPhi(tkphi,tt.track().phi());
  float dz = tkz - tt.track().vz();
  
  float radius = 100.*tt.track().pt()/(0.3*tt.field()->inTesla(GlobalPoint(0,0,0)).z());
  float pathlengthrphi = tt.track().charge()*dphi*radius;
  
  float pathlength = std::sqrt(pathlengthrphi*pathlengthrphi + dz*dz);
  float p = tt.track().p();
  
  float speed = std::sqrt(1./(1.+m_pion/p))*CLHEP::c_light/CLHEP::cm;  //speed in cm/ns
  float dt = pathlength/speed;

  return time-dt;
}

reco::Track 
L1TrackTimeValueMapProducer::buildTrackFromL1Track(const TTTrack<Ref_Phase2TrackerDigi_>& tt) const {
  const unsigned nPars = ( tt.getRInv(5) == 0.0 ? 4 : 5 );
  //setup track inputs
  GlobalPoint  ref = tt.getPOCA(nPars);
  reco::TrackBase::Point reftk(ref.x(),ref.y(),ref.z());
  GlobalVector mom = tt.getMomentum(nPars);  
  reco::TrackBase::Vector momtk(mom.x(),mom.y(),mom.z());
  double chi2 = tt.getChi2();
  double ndof = 2*tt.getStubRefs().size() - nPars;
  int charge = tt.getRInv(nPars) < 0 ? -1 : 1;
  
  return reco::Track(chi2,ndof,reftk,momtk,charge,reco::TrackBase::CovarianceMatrix());
}
