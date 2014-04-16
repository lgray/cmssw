#ifndef RecoParticleFlow_PFClusterProducer_PFEcalRecHitQTests_h
#define RecoParticleFlow_PFClusterProducer_PFEcalRecHitQTests_h

#include "RecoParticleFlow/PFClusterProducer/interface/PFRecHitQTestBase.h"



//
//  Quality test that checks threshold
//
class PFRecHitQTestThreshold : public PFRecHitQTestBase {
 public:
  PFRecHitQTestThreshold() {

  }

  PFRecHitQTestThreshold(const edm::ParameterSet& iConfig):
    PFRecHitQTestBase(iConfig)
    {
      threshold_ = iConfig.getParameter<double>("threshold");

    }

    void beginEvent(const edm::Event& event,const edm::EventSetup& iSetup) {
    }

    bool test(reco::PFRecHit& hit,const EcalRecHit& rh,bool& clean) {
      return pass(hit);
    }
    bool test(reco::PFRecHit& hit,const HBHERecHit& rh,bool& clean) {
      return pass(hit);
    }

    bool test(reco::PFRecHit& hit,const HFRecHit& rh,bool& clean) {
      return pass(hit);
    }
    bool test(reco::PFRecHit& hit,const HORecHit& rh,bool& clean) {
      return pass(hit);
    }

    bool test(reco::PFRecHit& hit,const CaloTower& rh,bool& clean) {
      return pass(hit);
    }

 protected:
  double threshold_;

  bool pass(const reco::PFRecHit& hit) {
    if (hit.energy()>threshold_) return true;

    return false;
  }
};




//
//  Quality test that checks kHCAL Severity
//
class PFRecHitQTestHCALChannel : public PFRecHitQTestBase {
 public:
  PFRecHitQTestHCALChannel() {

  }

  PFRecHitQTestHCALChannel(const edm::ParameterSet& iConfig):
    PFRecHitQTestBase(iConfig)
    {
      thresholds_ = iConfig.getParameter<std::vector<int> >("maxSeverities");
      flags_ = iConfig.getParameter<std::vector<int> >("flags");
    }

    void beginEvent(const edm::Event& event,const edm::EventSetup& iSetup) {
      edm::ESHandle<HcalTopology> topo;
      iSetup.get<HcalRecNumberingRecord>().get( topo );
      edm::ESHandle<HcalChannelQuality> hcalChStatus;    
      iSetup.get<HcalChannelQualityRcd>().get( hcalChStatus );
      theHcalChStatus_ = hcalChStatus.product();
      if (!theHcalChStatus_->topo()) theHcalChStatus_->setTopo(topo.product());
      edm::ESHandle<HcalSeverityLevelComputer> hcalSevLvlComputerHndl;
      iSetup.get<HcalSeverityLevelComputerRcd>().get(hcalSevLvlComputerHndl);
      hcalSevLvlComputer_  =  hcalSevLvlComputerHndl.product();
    }

    bool test(reco::PFRecHit& hit,const EcalRecHit& rh,bool& clean) {
      return true;
    }
    bool test(reco::PFRecHit& hit,const HBHERecHit& rh,bool& clean) {
      return test(rh.detid(),rh.flags(),clean);
    }

    bool test(reco::PFRecHit& hit,const HFRecHit& rh,bool& clean) {
      return test(rh.detid(),rh.flags(),clean);
    }
    bool test(reco::PFRecHit& hit,const HORecHit& rh,bool& clean) {
      return test(rh.detid(),rh.flags(),clean);
    }

    bool test(reco::PFRecHit& hit,const CaloTower& rh,bool& clean) {
      return true;

    }

 protected:
    std::vector<int> thresholds_;
    std::vector<int> flags_;
  const HcalChannelQuality* theHcalChStatus_;
  const HcalSeverityLevelComputer* hcalSevLvlComputer_;

  bool test(unsigned DETID,int flags,bool& clean) {
    const HcalDetId& detid = (HcalDetId)DETID;
    const HcalChannelStatus* theStatus = theHcalChStatus_->getValues(detid);
    unsigned theStatusValue = theStatus->getValue();
    // Now get severity of problems for the given detID, based on the rechit flag word and the channel quality status value
    for (unsigned int i=0;i<thresholds_.size();++i) {
      int hitSeverity =0;
      if(flags_[i]<0) {
	hitSeverity=hcalSevLvlComputer_->getSeverityLevel(detid, flags,theStatusValue);
      }
      else {
	hitSeverity=hcalSevLvlComputer_->getSeverityLevel(detid, flags & flags_[i],theStatusValue);
      }
      
      if (hitSeverity>thresholds_[i]) {
	clean=true;
	return false;
      }
    }  
    return true;
  }

};

//
//  Quality test that applies threshold and timing as a function of depth 
//
class PFRecHitQTestHCALThresholdTimeVsDepth : public PFRecHitQTestBase {
 public:
  PFRecHitQTestHCALThresholdTimeVsDepth() {
    
  }

  PFRecHitQTestHCALThresholdTimeVsDepth(const edm::ParameterSet& iConfig):
    PFRecHitQTestBase(iConfig)
    {
      std::vector<edm::ParameterSet> psets = iConfig.getParameter<std::vector<edm::ParameterSet> >("cuts");
      for (unsigned int i=0;i<psets.size();++i) {
	depths_.push_back(psets[i].getParameter<int>("depth"));
	minTimes_.push_back(psets[i].getParameter<double>("minTime"));
	maxTimes_.push_back(psets[i].getParameter<double>("maxTime"));
	thresholds_.push_back(psets[i].getParameter<double>("threshold"));
      }
    }

    void beginEvent(const edm::Event& event,const edm::EventSetup& iSetup) {
    }

    bool test(reco::PFRecHit& hit,const EcalRecHit& rh,bool& clean) {
      return true;
    }
    bool test(reco::PFRecHit& hit,const HBHERecHit& rh,bool& clean) {
      return test(rh.detid(),rh.energy(),rh.time(),clean);
    }

    bool test(reco::PFRecHit& hit,const HFRecHit& rh,bool& clean) {
      return test(rh.detid(),rh.energy(),rh.time(),clean);
    }
    bool test(reco::PFRecHit& hit,const HORecHit& rh,bool& clean) {
      return test(rh.detid(),rh.energy(),rh.time(),clean);
    }

    bool test(reco::PFRecHit& hit,const CaloTower& rh,bool& clean) {
      return true;
    }

 protected:
    std::vector<int> depths_;
    std::vector<double> minTimes_;
    std::vector<double> maxTimes_;
    std::vector<double> thresholds_;

    bool test(unsigned DETID,double energy,double time,bool& clean) {
      HcalDetId detid(DETID);
      for (unsigned int i=0;i<depths_.size();++i) {
	if (detid.depth() == depths_[i]) {
	  if (time <minTimes_[i] || time >maxTimes_[i] || energy<thresholds_[i])
	    {
	      clean=true;
	      return false;
	    }
	  break;
	}
      }
      return true;
    }
};






//
//  Quality test that checks HO threshold applying different threshold in rings
//
class PFRecHitQTestHOThreshold : public PFRecHitQTestBase {
 public:
  PFRecHitQTestHOThreshold() {

  }

  PFRecHitQTestHOThreshold(const edm::ParameterSet& iConfig):
    PFRecHitQTestBase(iConfig)
    {
      threshold0_ = iConfig.getParameter<double>("threshold_ring0");
      threshold12_ = iConfig.getParameter<double>("threshold_ring12");
    }

    void beginEvent(const edm::Event& event,const edm::EventSetup& iSetup) {
    }

    bool test(reco::PFRecHit& hit,const EcalRecHit& rh,bool& clean) {
      return true;
    }
    bool test(reco::PFRecHit& hit,const HBHERecHit& rh,bool& clean) {
      return true;
    }

    bool test(reco::PFRecHit& hit,const HFRecHit& rh,bool& clean) {
      return true;
    }
    bool test(reco::PFRecHit& hit,const HORecHit& rh,bool& clean) {
      HcalDetId detid(rh.detid());
      if (abs(detid.ieta())<=4 && hit.energy()>threshold0_)
	return true;
      if (abs(detid.ieta())>4 && hit.energy()>threshold12_)
	return true;
      
      return false;
    }

    bool test(reco::PFRecHit& hit,const CaloTower& rh,bool& clean) {
      return true;
    }

 protected:
  double threshold0_;
  double threshold12_;

};

//
//  Quality test that checks ecal quality cuts
//
class PFRecHitQTestECAL : public PFRecHitQTestBase {
 public:
  PFRecHitQTestECAL() {

  }

  PFRecHitQTestECAL(const edm::ParameterSet& iConfig):
    PFRecHitQTestBase(iConfig)
    {
      thresholdCleaning_   = iConfig.getParameter<double>("cleaningThreshold");
      timingCleaning_      = iConfig.getParameter<bool>("timingCleaning");
      topologicalCleaning_ = iConfig.getParameter<bool>("topologicalCleaning");
      skipTTRecoveredHits_ = iConfig.getParameter<bool>("skipTTRecoveredHits");

    }

    void beginEvent(const edm::Event& event,const edm::EventSetup& iSetup) {
    }

    bool test(reco::PFRecHit& hit,const EcalRecHit& rh,bool& clean) {
      if (skipTTRecoveredHits_ && rh.checkFlag(EcalRecHit::kTowerRecovered))
	{
	  clean=true;
	  return false;
	}
      if (  timingCleaning_ && rh.energy() > thresholdCleaning_ && 
	    rh.checkFlag(EcalRecHit::kOutOfTime) ) {
	  clean=true;
	  return false;
      }
      
      if    ( topologicalCleaning_ && 
	      ( rh.checkFlag(EcalRecHit::kWeird) || 
		rh.checkFlag(EcalRecHit::kDiWeird))) {
	clean=true;
	return false;
      }

      return true;
    }

    bool test(reco::PFRecHit& hit,const HBHERecHit& rh,bool& clean) {
      return true;
    }

    bool test(reco::PFRecHit& hit,const HFRecHit& rh,bool& clean) {
      return true;

    }

    bool test(reco::PFRecHit& hit,const HORecHit& rh,bool& clean) {
      return true;
    }

    bool test(reco::PFRecHit& hit,const CaloTower& rh,bool& clean) {
      return true;

    }


 protected:
  double thresholdCleaning_;
  bool timingCleaning_;
  bool topologicalCleaning_;
  bool skipTTRecoveredHits_;

};





//
//  Quality test that calibrates tower 29 of HCAL
//
class PFRecHitQTestHCALCalib29 : public PFRecHitQTestBase {
 public:
  PFRecHitQTestHCALCalib29() {

  }

  PFRecHitQTestHCALCalib29(const edm::ParameterSet& iConfig):
    PFRecHitQTestBase(iConfig)
    {
      calibFactor_ =iConfig.getParameter<double>("calibFactor");
    }

    void beginEvent(const edm::Event& event,const edm::EventSetup& iSetup) {
    }

    bool test(reco::PFRecHit& hit,const EcalRecHit& rh,bool& clean) {
      return true;
    }
    bool test(reco::PFRecHit& hit,const HBHERecHit& rh,bool& clean) {
      HcalDetId detId(hit.detId());
      if (abs(detId.ieta())==29)
	hit.setEnergy(hit.energy()*calibFactor_);
      return true;

    }

    bool test(reco::PFRecHit& hit,const HFRecHit& rh,bool& clean) {
      return true;

    }
    bool test(reco::PFRecHit& hit,const HORecHit& rh,bool& clean) {
      return true;
    }

    bool test(reco::PFRecHit& hit,const CaloTower& rh,bool& clean) {
      CaloTowerDetId detId(hit.detId());
      if (detId.ietaAbs()==29)
	hit.setEnergy(hit.energy()*calibFactor_);
      return true;
	  
    }

 protected:
    float calibFactor_;
};



#endif
