#include "RecoEcal/EgammaCoreTools/interface/BaselinePFSCRegression.h"

void BaselinePFSCRegression::update(const edm::EventSetup& es) {
  const CaloTopologyRecord& topofrom_es = es.get<CaloTopologyRecord>();
  if( !topo_record ||
      topofrom_es.cacheIdentifier() != topo_record->cacheIdentifier() ) {
    topo_record = &topofrom_es;
    topo_record->get(calotopo);
  }
}

void BaselinePFSCRegression::set(const reco::SuperCluster& sc,
				 std::vector<float>& vars     ) const {
  
}

void BaselinePFSCRegression::setEvent(const edm::Event& ev) {
}
