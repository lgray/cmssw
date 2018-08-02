#include "Geometry/MTDGeometryBuilder/interface/MTDParametersFromDD.h"
#include "CondFormats/GeometryObjects/interface/PMTDParameters.h"
#include "DetectorDescription/Core/interface/DDCompactView.h"
#include "DetectorDescription/Core/interface/DDVectorGetter.h"
#include "DetectorDescription/Core/interface/DDutils.h"

bool
MTDParametersFromDD::build( const DDCompactView* cvp,
				PMTDParameters& ptp)
{
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

  ptp.vpars = dbl_to_int( DDVectorGetter::get( "vPars" ));

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
