#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

#include <TH1F.h>
#include <TROOT.h>
#include <TFile.h>
#include <TSystem.h>

#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

#include "DataFormats/FWLite/interface/InputSource.h"
#include "DataFormats/FWLite/interface/OutputFiles.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "PhysicsTools/FWLite/interface/TFileService.h"

#include "RecoEgamma/ElectronIdentification/interface/VersionedGsfElectronSelector.h"

int main(int argc, char* argv[]) 
{
  // ----------------------------------------------------------------------
  // First Part: 
  //
  //  * enable FWLite 
  //  * enable the IDs you want to use
  //  * open the input file
  // ----------------------------------------------------------------------

  // load framework libraries
  gSystem->Load( "libFWCoreFWLite" );
  AutoLibraryLoader::enable();

  // only allow one argument for this simple example which should be the
  // the python cfg file
  if ( argc < 2 ) {
    std::cout << "Usage : " << argv[0] << " [parameters.py]" << std::endl;
    return 0;
  }
  if( !edm::readPSetsFrom(argv[1])->existsAs<edm::ParameterSet>("process") ){
    std::cout << " ERROR: ParametersSet 'process' is missing in your configuration file" << std::endl; exit(0);
  }
  // get the python configuration
  const edm::ParameterSet& process = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");
  fwlite::InputSource inputHandler_(process); fwlite::OutputFiles outputHandler_(process);
  
  // now get each parameter
  const edm::ParameterSet& ana = process.getParameter<edm::ParameterSet>("electronAnalyzer");
  edm::InputTag electrons_( ana.getParameter<edm::InputTag>("electrons") );
  
  // setup an ID for use
  const edm::ParameterSet& my_ids = process.getParameterSet("my_vid_configuration");
  const edm::ParameterSet& loose_id_conf = my_ids.getParameterSet("loose");
  const edm::ParameterSet& medium_id_conf = my_ids.getParameterSet("medium");
  const edm::ParameterSet& tight_id_conf = my_ids.getParameterSet("tight");
    
  VersionedGsfElectronSelector loose_id(loose_id_conf);
  VersionedGsfElectronSelector medium_id(medium_id_conf);
  VersionedGsfElectronSelector tight_id(tight_id_conf);  

  // loop the events
  int ievt=0;  
  int maxEvents_( inputHandler_.maxEvents() );
  for(unsigned int iFile=0; iFile<inputHandler_.files().size(); ++iFile){
    // open input file (can be located on castor)
    TFile* inFile = TFile::Open(inputHandler_.files()[iFile].c_str());
    if( inFile ){
      // ----------------------------------------------------------------------
      // Second Part: 
      //
      //  * loop the events in the input file 
      //  * receive the collections of interest via fwlite::Handle
      //  * fill the histograms
      //  * after the loop close the input file
      // ----------------------------------------------------------------------      
      fwlite::Event ev(inFile);
      for(ev.toBegin(); !ev.atEnd(); ++ev, ++ievt){
        edm::EventBase const & event = ev;
        // break loop if maximal number of events is reached 
        if(maxEvents_>0 ? ievt+1>maxEvents_ : false) break;
        // simple event counter
        if(inputHandler_.reportAfter()!=0 ? (ievt>0 && ievt%inputHandler_.reportAfter()==0) : false) 
          std::cout << "  processing event: " << ievt << std::endl;

        // Handle to the muon collection
        edm::Handle<std::vector<pat::Electron> > electrons;
        event.getByLabel(electrons_, electrons);
        
        // loop muon collection and fill histograms
        for(unsigned i=0; i<electrons->size(); ++i){
          if( loose_id(electrons->at(i),event) ) {
            std::cout << "Electron at " << i << " passed loose ID!" << std::endl;
          }
          if( medium_id(electrons->at(i),event) ) {
            std::cout << "Electron at " << i << " passed medium ID!" << std::endl;
          }
          if( tight_id(electrons->at(i),event) ) {
            std::cout << "Electron at " << i << " passed tight ID!" << std::endl;
          }
        }
      }  
      // close input file
      inFile->Close();
    }
    // break loop if maximal number of events is reached:
    // this has to be done twice to stop the file loop as well
    if(maxEvents_>0 ? ievt+1>maxEvents_ : false) break;
  }
  return 0;
}
