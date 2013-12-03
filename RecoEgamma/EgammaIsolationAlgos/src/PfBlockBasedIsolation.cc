#include "RecoEgamma/EgammaIsolationAlgos/interface/PfBlockBasedIsolation.h"
#include <cmath>
#include "DataFormats/Math/interface/deltaR.h"



#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/Common/interface/RefToPtr.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockFwd.h"

void PfBlockBasedIsolation::setup(const edm::ParameterSet& conf) {


}



//--------------------------------------------------------------------------------------------------

PfBlockBasedIsolation::PfBlockBasedIsolation() {
  // Default Constructor.
}

PfBlockBasedIsolation::PfBlockBasedIsolation(const reco::Photon) {
  // Constructor for Photons

}

PfBlockBasedIsolation::PfBlockBasedIsolation(const reco::GsfElectron) {
  // Constructor for gsfElectrons


}



//--------------------------------------------------------------------------------------------------
PfBlockBasedIsolation::~PfBlockBasedIsolation()
{

}



 std::vector<std::pair<reco::PFCandidateRef, bool> >   PfBlockBasedIsolation::calculate(math::XYZTLorentzVectorD p4, const reco::PFCandidateRef pfEGCand, const edm::Handle<reco::PFCandidateCollection> pfCandidateHandle) {

  std::vector<std::pair<reco::PFCandidateRef, bool> > myPairVec;

  math::XYZVector candidateMomentum(p4.px(),p4.py(),p4.py());
  math::XYZVector candidateDirection=candidateMomentum.Unit();
  std::cout << " PfBlockBasedIsolation::calculate photon momentum direction " << candidateDirection << " pfEGCand SC energy " <<   pfEGCand->superClusterRef()->energy() << std::endl; 
  const reco::PFCandidate::ElementsInBlocks& theElementsInpfEGcand = (*pfEGCand).elementsInBlocks();
  reco::PFCandidate::ElementsInBlocks::const_iterator ieg = theElementsInpfEGcand.begin();
  const reco::PFBlockRef egblock = ieg->first;



  std::pair<reco::PFCandidateRef, bool> aPair;
  unsigned nObj = pfCandidateHandle->size();
  for(unsigned int lCand=0; lCand < nObj; lCand++) {

    reco::PFCandidateRef pfCandRef(reco::PFCandidateRef(pfCandidateHandle,lCand));
    aPair.first=pfCandRef;
    aPair.second=false;

    float dR = deltaR(candidateDirection.Eta(), candidateDirection.Phi(),  pfCandRef->eta(),   pfCandRef->phi());   
    std::cout << " PfBlockBasedIsolation::calculate candidate type " << pfCandRef->particleId() << " dR  " << dR << std::endl;       
    if ( dR>0.5) continue;
    const reco::PFCandidate::ElementsInBlocks& theElementsInPFcand = pfCandRef->elementsInBlocks();

    bool elementFound=false;
    for (reco::PFCandidate::ElementsInBlocks::const_iterator ipf = theElementsInPFcand.begin(); ipf<theElementsInPFcand.end(); ++ipf) {

 
 
     if ( ipf->first == egblock && !elementFound ) {
	std::cout << " this is the same block as the unbiassed cand  and this is an element " << ipf->second<< std::endl;
	  for (ieg = theElementsInpfEGcand.begin(); ieg<theElementsInpfEGcand.end(); ++ieg) {
	    if ( ipf->second == ieg->second && !elementFound  ) {
	      elementFound=true;
              aPair.second=true;	      
	      std::cout << " EG candidate has at leaast one same element " <<  ieg->second << " elementFound " << elementFound <<  std::endl;  
	    }
	  }
	
	
	
      }
    }
    myPairVec.push_back(aPair);    
    for ( std::vector<std::pair<reco::PFCandidateRef, bool> >::const_iterator iPair=myPairVec.begin(); iPair<myPairVec.end(); iPair++) {
      std::cout << " Pair " << (iPair->first)->particleId() << " " <<  iPair->second << std::endl; 
    }

  }
  


  return myPairVec;



 }




