#include "RecoParticleFlow/PFClusterProducer/plugins/PFRecHitProducerNew.h"

PFRecHitProducerNew::PFRecHitProducerNew(const edm::ParameterSet& iConfig)
{

  produces<reco::PFRecHitCollection>();
  produces<reco::PFRecHitCollection>("Cleaned");

  edm::ConsumesCollector iC = consumesCollector();

  std::vector<edm::ParameterSet> creators = iConfig.getParameter<std::vector<edm::ParameterSet> >("producers");
  for (unsigned int i=0;i<creators.size();++i) {
      std::string name = creators.at(i).getParameter<std::string>("name");
      creators_.push_back(PFRecHitFactory::get()->create(name,creators.at(i),iC));
  }


  edm::ParameterSet navSet = iConfig.getParameter<edm::ParameterSet>("navigator");

  navigator_ = PFRecHitNavigationFactory::get()->create(navSet.getParameter<std::string>("name"),navSet);
    
}


PFRecHitProducerNew::~PFRecHitProducerNew()
{
 }


//
// member functions
//

// ------------ method called to produce the data  ------------
void
PFRecHitProducerNew::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   std::auto_ptr<reco::PFRecHitCollection> out(new reco::PFRecHitCollection );
   std::auto_ptr<reco::PFRecHitCollection> cleaned(new reco::PFRecHitCollection );

   navigator_->beginEvent(iSetup);

   for (unsigned int i=0;i<creators_.size();++i) {
     creators_.at(i)->importRecHits(out,cleaned,iEvent,iSetup);
   }

   for( unsigned int i=0;i<out->size();++i) {
     navigator_->associateNeighbours(out->at(i),out);
   }

   iEvent.put(out,"");
   iEvent.put(cleaned,"Cleaned");

}

// ------------ method called once each job just before starting event loop  ------------
void 
PFRecHitProducerNew::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
PFRecHitProducerNew::endJob() {
}


void
PFRecHitProducerNew::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

