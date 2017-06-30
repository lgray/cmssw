// This producer assigns vertex times (with a specified resolution) to tracks.
// The times are produced as valuemaps associated to tracks, so the track dataformat doesn't
// need to be modified.
// This particular recycler class is meant to be used on the perfect times and applies a new smearing to those.

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/View.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "SimTracker/TrackerHitAssociation/interface/TrackerHitAssociator.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertex.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertexContainer.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "SimDataFormats/Associations/interface/TrackToTrackingParticleAssociator.h"

#include <memory>

#include "SimTracker/TrackAssociation/interface/ResolutionModel.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "FWCore/Utilities/interface/isFinite.h"
#include "CLHEP/Random/RandGauss.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/RandomNumberGenerator.h"

#include "FWCore/Utilities/interface/isFinite.h"
#include "FWCore/Utilities/interface/transform.h"

class TrackTimeValueMapRecycler : public edm::global::EDProducer<> {
public:    
  TrackTimeValueMapRecycler(const edm::ParameterSet&);
  ~TrackTimeValueMapRecycler() { }
  
  virtual void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const override;
  
private:
  // inputs
  const edm::EDGetTokenT<edm::View<reco::Track> > tracks_;
  const std::string tracksName_;
  const edm::EDGetTokenT<edm::ValueMap<float> > tracksTime_, tracksTimeReso_;
  const edm::EDGetTokenT<std::vector<PileupSummaryInfo> > pileupSummaryInfo_;  
  // time resolution that indicates a fake track
  // note that this only applies to 91X series samples
  const float fakeTimeResolution_;
  // eta bounds
  const float etaMin_, etaMax_, ptMin_, pMin_, etaMaxForPtThreshold_;
  // options
  std::vector<std::unique_ptr<const ResolutionModel> > resolutions_;  
};

DEFINE_FWK_MODULE(TrackTimeValueMapRecycler);

namespace {
  constexpr float fakeBeamSpotTimeWidth = 0.300f; // ns
  static const std::string resolution("Resolution");

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

TrackTimeValueMapRecycler::TrackTimeValueMapRecycler(const edm::ParameterSet& conf) :
  tracks_(consumes<edm::View<reco::Track> >( conf.getParameter<edm::InputTag>("trackSrc") ) ),
  tracksName_(conf.getParameter<edm::InputTag>("trackSrc").label()),
  tracksTime_(consumes<edm::ValueMap<float> >( conf.getParameter<edm::InputTag>("trackTimeSrc") ) ),
  tracksTimeReso_(consumes<edm::ValueMap<float> >( conf.getParameter<edm::InputTag>("trackTimeResoSrc") )  ),
  pileupSummaryInfo_(consumes<std::vector<PileupSummaryInfo> >( conf.getParameter<edm::InputTag>("pileupSummaryInfo") ) ),
  fakeTimeResolution_( conf.getParameter<double>("fakeTrackTimeReso") - 0.001f),
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
    ResolutionModel* resomod = ResolutionModelFactory::get()->create(name,reso);
    resolutions_.emplace_back( resomod );  

    // times and time resolutions for general tracks
    produces<edm::ValueMap<float> >(tracksName_+name);
    produces<edm::ValueMap<float> >(tracksName_+name+resolution);
  }
  // get RNG engine
  edm::Service<edm::RandomNumberGenerator> rng;
  if (!rng.isAvailable()){
    throw cms::Exception("Configuration")
      << "TrackTimeValueMapRecycler::TrackTimeValueMapRecycler() - RandomNumberGeneratorService is not present in configuration file.\n"
      << "Add the service in the configuration file or remove the modules that require it.";
  }

}

void TrackTimeValueMapRecycler::produce(edm::StreamID sid, edm::Event& evt, const edm::EventSetup& es) const {
  // get RNG engine
  edm::Service<edm::RandomNumberGenerator> rng;  
  auto rng_engine = &(rng->getEngine(sid));
  
  std::vector<float> generalTrackTimes;
 
  //get track collections
  edm::Handle<edm::View<reco::Track> > TrackCollectionH;
  evt.getByToken(tracks_, TrackCollectionH);
  const edm::View<reco::Track>& TrackCollection = *TrackCollectionH;

  //get track times and resolutions
  edm::Handle<edm::ValueMap<float> > TrackTimesH;
  evt.getByToken(tracksTime_, TrackTimesH);
  const edm::ValueMap<float>& TrackTimes = *TrackTimesH;

  edm::Handle<edm::ValueMap<float> > TrackTimeResosH;
  evt.getByToken(tracksTimeReso_, TrackTimeResosH);
  const edm::ValueMap<float>& TrackTimeResos = *TrackTimeResosH;

  //get pileup summary
  edm::Handle<std::vector<PileupSummaryInfo> > pileupSummaryH;
  evt.getByToken(pileupSummaryInfo_, pileupSummaryH);
  
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
  
  const double meanSimTime = sumSimTime/double(nsim);
  const double varSimTime = sumSimTimeSq/double(nsim) - meanSimTime*meanSimTime;
  const double rmsSimTime = std::sqrt(std::max(0.1*0.1,varSimTime));
    
  for( unsigned itk = 0; itk < TrackCollection.size(); ++itk ) {
    const auto tkref = TrackCollection.refAt(itk);
    const float time = TrackTimes[tkref];
    const float timeReso = TrackTimeResos[tkref];
    
    if( timeReso < fakeTimeResolution_ ) {
      generalTrackTimes.push_back(time);
    } else {
      float rndtime = CLHEP::RandGauss::shoot(rng_engine, meanSimTime, rmsSimTime);
      generalTrackTimes.push_back(rndtime);      
    }
  }
  
  for( const auto& reso : resolutions_ ) {
    const std::string& name = reso->name();
    std::vector<float> times, resos;
    
    times.reserve(TrackCollection.size());
    resos.reserve(TrackCollection.size());

    for( unsigned i = 0; i < TrackCollection.size(); ++i ) {
      const reco::Track& tk = TrackCollection[i];
      const float absEta = std::abs(tk.eta());
      bool inAcceptance = absEta < etaMax_ && absEta >= etaMin_ && tk.p()>pMin_ && (absEta>etaMaxForPtThreshold_ || tk.pt()>ptMin_);
      if (inAcceptance) {
        const float resolution = reso->getTimeResolution(tk);
        times.push_back( CLHEP::RandGauss::shoot(rng_engine, generalTrackTimes[i], resolution) );
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
