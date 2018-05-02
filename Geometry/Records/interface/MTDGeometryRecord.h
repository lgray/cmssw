#ifndef RECORDS_MTDGEOMETRYRECORD_H
#define RECORDS_MTDGEOMETRYRECORD_H

#include "FWCore/Framework/interface/EventSetupRecordImplementation.h"
#include "FWCore/Framework/interface/DependentRecordImplementation.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "Geometry/Records/interface/PMTDRcd.h"
#include "CondFormats/AlignmentRecord/interface/GlobalPositionRcd.h"
#include "boost/mpl/vector.hpp"

class MTDGeometryRecord : 
   public edm::eventsetup::DependentRecordImplementation<
   MTDGeometryRecord,
     boost::mpl::vector<
     IdealGeometryRecord,
     GlobalPositionRcd               > > {};

#endif /* RECORDS_MTDGEOMETRYRECORD_H */

