#include "RecoTracker/FastTimeMatching/interface/FTLBarrelDetLayer.h"
#include "DataFormats/GeometrySurface/interface/MediumProperties.h"
#include "DataFormats/GeometrySurface/interface/BoundPlane.h"

#include "DataFormats/GeometrySurface/interface/RectangularPlaneBounds.h"

#include "TrackingTools/DetLayers/interface/RodPlaneBuilderFromDet.h"
#include "TrackingTools/DetLayers/interface/CylinderBuilderFromDet.h"

#include "DataFormats/GeometrySurface/interface/BoundingBox.h"

FTLBarrelDetLayer::FTLBarrelDetLayer(DetId id, const std::unordered_multimap<uint32_t,std::pair<DetId,const GeomDet*> >& sectors, float radlen, float xi) :  
  id_(id)  
{
  
  sectors_.resize(0);
  
  std::vector<const GeomDet*> sectorsTemp; // for making a cylinder
  RodPlaneBuilderFromDet planeBuilder;
  for(unsigned iphi=1; iphi < 721; ++iphi) {
    std::vector<const GeomDet*> dets;
    std::vector<std::pair<DetId,const GeomDet*> > detsForSector;
    auto range = sectors.equal_range(FastTimeDetId(id_.type(),0,iphi,id_.zside()));
    for( auto itr = range.first; itr != range.second; ++itr ) {
      dets.emplace_back(itr->second.second);
      detsForSector.emplace_back(itr->second.first,itr->second.second);
    }
    Plane::PlanePointer detPlane( planeBuilder(dets) );
    std::vector<GlobalPoint> gdcorners = BoundingBox().corners(*detPlane);
    sectors_.emplace_back( new FTLBarrelSectorGeomDet(FastTimeDetId(id_.type(),0,iphi,id_.zside()), detPlane, detsForSector,radlen,xi) );
    sectorsTemp.emplace_back(sectors_.back());
  }
  
  CylinderBuilderFromDet cylinderBuilder;
  cylinder_ = cylinderBuilder(sectorsTemp.begin(),sectorsTemp.end());

  std::cout << FastTimeDetId(id) << ' ' << cylinder_->radius() 
	    << " bounds: " << cylinder_->bounds().length() << ' '
	    << cylinder_->bounds().width() << ' ' 
	    << cylinder_->bounds().thickness() << std::endl;;
  
}

