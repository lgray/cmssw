
#include "GEDGsfElectronCoreProducer.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectronCore.h"
#include "DataFormats/ParticleFlowReco/interface/GsfPFRecTrack.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/EgammaReco/interface/ElectronSeedFwd.h"
#include "DataFormats/EgammaReco/interface/ElectronSeed.h"
#include "DataFormats/Common/interface/ValueMap.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateEGammaExtra.h"

#include <map>

using namespace reco ;

void GEDGsfElectronCoreProducer::fillDescriptions( edm::ConfigurationDescriptions & descriptions )
{
  edm::ParameterSetDescription desc ;
  GsfElectronCoreBaseProducer::fillDescription(desc) ;
  desc.add<edm::InputTag>("GEDEMUnbiased",edm::InputTag("GEDPFCandidates")) ;

  descriptions.add("produceEcalDrivenGsfElectronCores",desc) ;
}

GEDGsfElectronCoreProducer::GEDGsfElectronCoreProducer( const edm::ParameterSet & config )
 : GsfElectronCoreBaseProducer(config)
{
  gedEMUnbiasedTag_ = config.getParameter<edm::InputTag>("GEDEMUnbiased") ;
}

void GEDGsfElectronCoreProducer::produce( edm::Event & event, const edm::EventSetup & setup )
 {
  // base input
  GsfElectronCoreBaseProducer::initEvent(event,setup) ;

  event.getByLabel(gedEMUnbiasedTag_,gedEMUnbiasedH_);

  // output
  std::auto_ptr<GsfElectronCoreCollection> electrons(new GsfElectronCoreCollection) ;

  const PFCandidateCollection * pfCandidateCollection = gedEMUnbiasedH_.product();
  for ( unsigned int i=0 ; i<pfCandidateCollection->size() ; ++i )
           produceElectronCore((*pfCandidateCollection)[i],electrons.get()) ;
    
  event.put(electrons) ;
 }

void GEDGsfElectronCoreProducer::produceElectronCore( const reco::PFCandidate & pfCandidate, reco::GsfElectronCoreCollection * electrons )
 {
  const GsfTrackRef gsfTrackRef = pfCandidate.gsfTrackRef();
  if(gsfTrackRef.isNull()) 
	return;

  reco::PFCandidateEGammaExtraRef extraRef = pfCandidate.egammaExtraRef();
  if(extraRef.isNull()) 
	return;

  GsfElectronCore * eleCore = new GsfElectronCore(gsfTrackRef) ;

  GsfElectronCoreBaseProducer::fillElectronCore(eleCore) ;

  SuperClusterRef scRef = extraRef->superClusterRef();
  SuperClusterRef parentScRef = extraRef->parentSuperClusterRef();  

  if (!scRef.isNull() || !parentScRef.isNull())
  {
       eleCore->setSuperCluster(scRef) ;
       eleCore->setPflowSuperCluster(parentScRef) ;
       electrons->push_back(*eleCore) ;
   }
   else
   { edm::LogWarning("GEDGsfElectronCoreProducer")<<"Both superClusterRef and parentSuperClusterRef of pfCandidate.egammaExtraRef() are Null" ; }
  
  delete eleCore ;
 }

GEDGsfElectronCoreProducer::~GEDGsfElectronCoreProducer()
 {}

