#include "CondDBCmsMTDConstruction.h"
#include "DetectorDescription/Core/interface/DDFilteredView.h"
#include "DetectorDescription/Core/interface/DDCompactView.h"
#include "CondFormats/GeometryObjects/interface/PGeometricTimingDet.h"
#include "Geometry/MTDNumberingBuilder/interface/GeometricTimingDet.h"
#include "Geometry/MTDNumberingBuilder/plugins/ExtractStringFromDDD.h"
#include "Geometry/MTDNumberingBuilder/plugins/CmsMTDBuilder.h"
#include "Geometry/MTDNumberingBuilder/plugins/CmsMTDDetIdBuilder.h"

using namespace cms;

CondDBCmsMTDConstruction::CondDBCmsMTDConstruction() { }

const GeometricTimingDet* CondDBCmsMTDConstruction::construct(const PGeometricTimingDet& pgd) {
  //std::cout << "In CondDBCmsMTDConstruction::construct with pgd.pgeomdets_.size() == " << pgd.pgeomdets_.size() << std::endl;  
  GeometricTimingDet* mtd  = new GeometricTimingDet(pgd.pgeomdets_[0],GeometricTimingDet::MTD);

  size_t detMax =  pgd.pgeomdets_.size();
  size_t tri = 1;
  std::vector<GeometricTimingDet*> hier;
  int lev=1;
  GeometricTimingDet* subdet = mtd;
  hier.emplace_back(subdet);
    while ( tri < detMax && pgd.pgeomdets_[tri]._level == 1 ) {
      subdet = new GeometricTimingDet(pgd.pgeomdets_[tri], GeometricTimingDet::GTDEnumType(pgd.pgeomdets_[tri]._type));
      //std::cout << lev << " type " << pgd.pgeomdets_[tri]._type << " " << subdet->geographicalId() << std::endl;
      ++tri;
      hier.back()->addComponent(subdet);
      hier.emplace_back(subdet);
      ++lev;
      while ( tri < detMax && pgd.pgeomdets_[tri]._level == 2 ) {
	subdet = new GeometricTimingDet(pgd.pgeomdets_[tri], GeometricTimingDet::GTDEnumType(pgd.pgeomdets_[tri]._type));
	//std::cout << lev << "\ttype " << pgd.pgeomdets_[tri]._type << " " << subdet->geographicalId() << std::endl;
	++tri;
	hier.back()->addComponent(subdet);
	hier.emplace_back(subdet);
	++lev;
	while ( tri < detMax && pgd.pgeomdets_[tri]._level == 3 ) {
	  subdet = new GeometricTimingDet(pgd.pgeomdets_[tri], GeometricTimingDet::GTDEnumType(pgd.pgeomdets_[tri]._type));
	  //std::cout << lev << "\t\ttype " << pgd.pgeomdets_[tri]._type << " " << subdet->geographicalId() << std::endl;
	  ++tri;
	  hier.back()->addComponent(subdet);
	  hier.emplace_back(subdet);
	  ++lev;
	  while ( tri < detMax && pgd.pgeomdets_[tri]._level == 4 ) {
	    subdet = new GeometricTimingDet(pgd.pgeomdets_[tri], GeometricTimingDet::GTDEnumType(pgd.pgeomdets_[tri]._type));
	    //std::cout << lev << "\t\t\ttype " << pgd.pgeomdets_[tri]._type << " " << subdet->geographicalId() << std::endl;
	    ++tri;
	    hier.back()->addComponent(subdet);
	    hier.emplace_back(subdet);
	    ++lev;
	    while ( tri < detMax && pgd.pgeomdets_[tri]._level == 5 ) {
	      subdet = new GeometricTimingDet(pgd.pgeomdets_[tri], GeometricTimingDet::GTDEnumType(pgd.pgeomdets_[tri]._type));
	      //std::cout << lev << "\t\t\t\ttype " << pgd.pgeomdets_[tri]._type << " " << subdet->geographicalId() << std::endl;
	      ++tri;
	      hier.back()->addComponent(subdet);
	      hier.emplace_back(subdet);
	      ++lev;
	      while ( tri < detMax && pgd.pgeomdets_[tri]._level == 6 ) {
		subdet = new GeometricTimingDet(pgd.pgeomdets_[tri], GeometricTimingDet::GTDEnumType(pgd.pgeomdets_[tri]._type));
		//std::cout << lev << "\t\t\t\t\ttype " << pgd.pgeomdets_[tri]._type << " " << subdet->geographicalId() << std::endl;
		++tri;
		hier.back()->addComponent(subdet);
	      }
	      --lev;
	      hier.pop_back();
	    }
	    --lev;
	    hier.pop_back();
	  }
	  --lev;
	  hier.pop_back();
	}
	--lev;
	hier.pop_back();
      }
      --lev;
      hier.pop_back();
    }
//     std::cout << "Before \"deep components\" test I want to see if I can iterate to 6 layers by myself..." << std::endl;
//     std::vector<const GeometricTimingDet*> l0 = tracker->components();
//     std::vector<const GeometricTimingDet*>::const_iterator i0 = l0.begin();
//     std::vector<const GeometricTimingDet*>::const_iterator e0 = l0.end();
//     int count=0; // count only the leaves.
//     for ( ; i0 != e0 ; ++i0) {
//       std::cout << lev << " type " << (*i0)->type() << " " << int((*i0)->geographicalId()) << std::endl; 
//       std::vector<const GeometricTimingDet*> l1 = (*i0)->components();
//       if ( l1.size() == 0 )  ++count;
//       std::vector<const GeometricTimingDet*>::const_iterator i1 = l1.begin();
//       std::vector<const GeometricTimingDet*>::const_iterator e1 = l1.end();
//       ++lev;
//       for ( ; i1 != e1 ; ++i1) {
// 	std::cout << lev << "\ttype " << (*i1)->type() << " " << int((*i1)->geographicalId()) << std::endl; 
// 	std::vector<const GeometricTimingDet*> l2 = (*i1)->components();
// 	if ( l2.size() == 0 )  ++count;
// 	std::vector<const GeometricTimingDet*>::const_iterator i2 = l1.begin();
// 	std::vector<const GeometricTimingDet*>::const_iterator e2 = l1.end();
// 	++lev;
// 	for ( ; i2 != e2 ; ++i2) {
// 	  std::cout << lev << "\t\ttype " << (*i2)->type() << " " << int((*i2)->geographicalId()) << std::endl; 
// 	  std::vector<const GeometricTimingDet*> l3 = (*i2)->components();
// 	  if ( l3.size() == 0 )  ++count;
// 	  std::vector<const GeometricTimingDet*>::const_iterator i3 = l3.begin();
// 	  std::vector<const GeometricTimingDet*>::const_iterator e3 = l3.end();
// 	  ++lev;
// 	  for ( ; i3 != e3 ; ++i3) {
// 	    std::cout << lev << "\t\t\ttype " << (*i3)->type() << " " << int((*i3)->geographicalId()) << std::endl; 
// 	    std::vector<const GeometricTimingDet*> l4 = (*i3)->components();
// 	    if ( l4.size() == 0 )  ++count;
// 	    std::vector<const GeometricTimingDet*>::const_iterator i4 = l4.begin();
// 	    std::vector<const GeometricTimingDet*>::const_iterator e4 = l4.end();
// 	    ++lev;
// 	    for ( ; i4 != e4 ; ++i4) {
// 	      std::cout << lev << "\t\t\t\ttype " << (*i4)->type() << " " << int((*i4)->geographicalId()) << std::endl; 
// 	      std::vector<const GeometricTimingDet*> l5 = (*i4)->components();
// 	      if ( l5.size() == 0 )  ++count;
// 	      std::vector<const GeometricTimingDet*>::const_iterator i5 = l5.begin();
// 	      std::vector<const GeometricTimingDet*>::const_iterator e5 = l5.end();
// 	      ++lev;
// 	      for ( ; i5 != e5 ; ++i5) {
// 		std::cout << lev << "\t\t\t\t\ttype " << (*i5)->type() << " " << int((*i5)->geographicalId()) << std::endl; 
// 		++count;
// 		//       std::vector<const GeometricTimingDet*> l6 = (*i0)->components();
// 		//       std::vector<const GeometricTimingDet*>::const_iterator i6 = l6.begin();
// 		//       std::vector<const GeometricTimingDet*>::const_iterator e6 = l6.end();
// 		//     for ( ; i6 != e6 ; ++i6) {
// 		//       std::cout << lev << " type " << (*i6)->type() << " " << int((*i6)->geographicalId()) << std::endl; 
// 		//       std::vector<const GeometricTimingDet*> l1 = (*i0)->components();
// 		//       std::vector<const GeometricTimingDet*>::const_iterator i1 = l1.begin();
// 		//       std::vector<const GeometricTimingDet*>::const_iterator e1 = l1.end();
// 		//     }
// 	      }
// 	      --lev;
// 	    }
// 	    --lev;
// 	  }
// 	  --lev;
// 	}
// 	--lev;
//       }
//       --lev;
//     }
//     std::cout << "done... count = " << count << std::endl;
//     std::cout << "about to try to see what the \"deep components\" are" << std::endl;
//    std::cout << "done with the \"deep components\" check, there are: " << tracker->deepComponents().size() << std::endl;
  return mtd;
}

