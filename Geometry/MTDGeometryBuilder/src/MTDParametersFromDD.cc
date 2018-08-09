#include "Geometry/MTDGeometryBuilder/interface/MTDParametersFromDD.h"
#include "CondFormats/GeometryObjects/interface/PMTDParameters.h"
#include "DetectorDescription/Core/interface/DDCompactView.h"
#include "DetectorDescription/Core/interface/DDVectorGetter.h"
#include "DetectorDescription/Core/interface/DDutils.h"

MTDParametersFromDD::MTDParametersFromDD(const edm::ParameterSet& pset) {
  const edm::VParameterSet& items = 
    pset.getParameter<edm::VParameterSet>("vitems");
  parsFromPython = pset.getParameter<std::vector<int32_t> >("vpars");

  itemsFromPython.resize(items.size());
  for( unsigned i = 0; i < items.size(); ++i) {
    auto& item = itemsFromPython[i];
    item.id = i+1;
    item.vpars = items[i].getParameter<std::vector<int32_t> >("subdetPars");    
  }
}

bool
MTDParametersFromDD::build( const DDCompactView* cvp,
				PMTDParameters& ptp)
{
  if( itemsFromPython.empty() ) {
    for( int subdet = 1; subdet <= 6; ++subdet )
      {
	std::stringstream sstm;
	sstm << "Subdetector" << subdet;
	std::string name = sstm.str();
	
	if( DDVectorGetter::check( name ))
	  {
	    std::vector<int> subdetPars = dbl_to_int( DDVectorGetter::get( name ));
	    putOne( subdet, subdetPars, ptp );
	  }
      }
  } else {
    ptp.vitems = itemsFromPython;
  }

  if( parsFromPython.empty() ) {
    ptp.vpars = dbl_to_int( DDVectorGetter::get( "vPars" ));
  } else {
    ptp.vpars = parsFromPython;
  }

  return true;
}

void
MTDParametersFromDD::putOne( int subdet, std::vector<int> & vpars, PMTDParameters& ptp )
{
  PMTDParameters::Item item;
  item.id = subdet;
  item.vpars = vpars;
  ptp.vitems.emplace_back( item );
}
