#ifndef RecoEgamma_ElectronIdentification_ElectronMVAEstimatorRun2Phys14NonTrig_H
#define RecoEgamma_ElectronIdentification_ElectronMVAEstimatorRun2Phys14NonTrig_H

#include "RecoEgamma/EgammaTools/interface/AnyMVAEstimatorRun2Base.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"

#include <vector>
#include <string>
#include <TROOT.h>
#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"

class ElectronMVAEstimatorRun2Phys14NonTrig : public AnyMVAEstimatorRun2Base{
  
 public:

  // Define here the number and the meaning of the categories
  // for this specific MVA
  const int nCategories = 6;
  enum mvaCategories {
    UNDEFINED = -1,
    CAT_EB1_PT5to10  = 0,
    CAT_EB2_PT5to10  = 1,
    CAT_EE_PT5to10   = 2,
    CAT_EB1_PT10plus = 3,
    CAT_EB2_PT10plus = 4,
    CAT_EE_PT10plus  = 5
  };

  // Define the struct that contains all necessary for MVA variables
  struct AllVariables {
    Float_t kfhits;
    // Pure ECAL -> shower shapes
    Float_t see;
    Float_t spp;
    Float_t OneMinusE1x5E5x5;
    Float_t R9;
    Float_t etawidth;
    Float_t phiwidth;
    Float_t HoE;
    // Endcap only variables
    Float_t PreShowerOverRaw;
    //Pure tracking variables
    Float_t kfchi2;
    Float_t gsfchi2;
    // Energy matching
    Float_t fbrem;
    Float_t EoP;
    Float_t eleEoPout;
    Float_t IoEmIoP;
    // Geometrical matchings
    Float_t deta;
    Float_t dphi;
    Float_t detacalo;
    // Spectator variables  
    Float_t pt;
    Float_t isBarrel;
    Float_t isEndcap;
    Float_t SCeta;
  };
  
  // Constructor and destructor
  ElectronMVAEstimatorRun2Phys14NonTrig( std::vector<std::string> filenames );
  ~ElectronMVAEstimatorRun2Phys14NonTrig();

  // Calculation of the MVA value
  float mvaValue( const edm::Ptr<reco::Candidate>& particle);
 
  // Utility functions
  TMVA::Reader *createSingleReader(int iCategory, std::string filename);
  bool isEndcapCategory( int category );
  // Functions that should work on both pat and reco electrons
  // (use the fact that pat::Electron inherits from reco::GsfElectron)
  void fillMVAVariables(const edm::Ptr<reco::Candidate>& particle);
  int findCategory( const edm::Ptr<reco::Candidate>& particle);
  // The function below ensures that the variables passed to MVA are within reasonable bounds
  void constrainMVAVariables();
  
 private:
  // Data members
  std::vector<TMVA::Reader*> _tmvaReaders;

  // All variables needed by this MVA
  std::string _MethodName;
  AllVariables _allMVAVars;
  
};

#endif
