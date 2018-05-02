#ifndef RecoMTD_Record_MTDRecoGeometryRecord_h
#define RecoMTD_Record_MTDRecoGeometryRecord_h

/** \class MuonRecoGeometryRecord
 *
 *  Record to hold mtd reconstruction geometries.
 *
 *  \author L. Gray - FNAL
 */

#include "FWCore/Framework/interface/EventSetupRecordImplementation.h"
#include "FWCore/Framework/interface/DependentRecordImplementation.h"
#include "Geometry/Records/interface/MTDGeometryRecord.h"

#include "boost/mpl/vector.hpp"


class MTDRecoGeometryRecord : public edm::eventsetup::DependentRecordImplementation<MTDRecoGeometryRecord,
  boost::mpl::vector<MTDGeometryRecord> > {};

#endif 

