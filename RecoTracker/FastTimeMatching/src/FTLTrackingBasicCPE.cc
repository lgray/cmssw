#include "RecoTracker/FastTimeMatching/interface/FTLTrackingBasicCPE.h"

FTLTrackingBasicCPE::FTLTrackingBasicCPE(const FastTimeGeometry* geom, float fixedError, float fixedErrorBH)  : 
    geom_(geom),     
    fixedError_(fixedError), fixedError2_(fixedError*fixedError),
    fixedErrorBH_(fixedErrorBH), fixedErrorBH2_(fixedErrorBH*fixedErrorBH) 
{
  constexpr float oneOverTwelve = 1.0/12.0;

  for( const auto id : geom_->getValidDetIds() ) {
    const auto corners = std::move(geom_->getCorners(id));
    float maxdist2 = 0.f; 
    for( unsigned i = 0 ; i < 2; ++i ) {
      maxdist2 = std::max( 0.5f*(corners[i] - corners[i+2]).perp2(), maxdist2 );      
    }
    auto itr = detIdErr2_.emplace(id,oneOverTwelve*maxdist2);
    detIdErr_.emplace(id,std::sqrt(itr.first->second));
  }

}

