#include "RecoTracker/FastTimeMatching/interface/FTLTracker.h"
#include "RecoTracker/FastTimeMatching/interface/ftldebug.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "RecoTracker/FastTimeMatching/interface/FTLBarrelGeomDet.h"

FTLTracker::FTLTracker(const FastTimeGeometry *barrel,
		       const FastTimeGeometry *endcap) :
  barrelGeom_(barrel),
  endcapGeom_(endcap)
{
  if (ftltracking::g_debuglevel > 0) std::cout << "Making the FTLTracker" << std::endl;
  //makeBarrel(FastTimeDetId::FastTimeBarrel, 1); // type , number of layers
  makeDisks(FastTimeDetId::FastTimeEndcap, 1);

  auto endcapSort = [](const FTLDiskGeomDet *a, const FTLDiskGeomDet *b) -> bool { return (*a) < (*b); };
  std::sort(disksPos_.begin(), disksPos_.end(), endcapSort);
  for( unsigned i = 0; i < disksPos_.size(); ++i ) 
    disksLookupPos_.emplace(disksPos_[i]->geographicalId(),i);
  std::sort(disksNeg_.begin(), disksNeg_.end(), endcapSort);
  for( unsigned i = 0; i < disksNeg_.size(); ++i ) 
    disksLookupNeg_.emplace(disksNeg_[i]->geographicalId(),i);

  makeCylinders(FastTimeDetId::FastTimeBarrel,1);

  auto barrelSort = [](const FTLBarrelDetLayer *a, const FTLBarrelDetLayer *b) -> bool { return (*a) < (*b); };
  std::sort(cylindersPos_.begin(), cylindersPos_.end(), barrelSort);
  for( unsigned i = 0; i < cylindersPos_.size(); ++i ) 
    cylindersLookupPos_.emplace(cylindersPos_[i]->geographicalId(),i);
  std::sort(cylindersNeg_.begin(), cylindersNeg_.end(), barrelSort);  
  for( unsigned i = 0; i < cylindersNeg_.size(); ++i ) 
    cylindersLookupNeg_.emplace(cylindersNeg_[i]->geographicalId(),i);
}

#include "DataFormats/GeometrySurface/interface/BoundingBox.h"
void FTLTracker::makeCylinders(int subdet, int layers) {
  std::unordered_multimap<uint32_t,std::pair<DetId,const GeomDet*> > sectorsPos,sectorsNeg;

  const std::vector<DetId>& ids = barrelGeom_->getValidDetIds();
  if (ftltracking::g_debuglevel > 0) std::cout << "on subdet " << subdet << " I got a total of " << ids.size() << " det ids " << std::endl;

  GlobalVector aX( 1,0,0 );
  GlobalVector aY( 0,1,0 );
  GlobalVector aZ( 0,0,1 );
  Plane::RotationType rot(aZ,-aY,aX); // plane from global vectors;

  for( const auto& i : ids ) {
    FastTimeDetId id(i);
    FastTimeDetId geo(id.type(),0,id.iphi(),id.zside());
    Surface::PositionType::BasicVectorType posSum(0,0,0);
    const auto& corners = barrelGeom_->getCorners(id);
    
    for( unsigned i = 0; i < 2; ++i ) {
      posSum += corners[i].basicVector();
      posSum += corners[i+4].basicVector();
    }
    Surface::PositionType meanPos( posSum/4.f );
    const float width = (corners[0] - corners[4]).mag();
    const float length = (corners[0] - corners[1]).mag();
        
    GeomDet* thedet = new FTLBarrelGeomDet(id,meanPos,rot,width,length);
    
    std::vector<GlobalPoint> gdcorners = BoundingBox().corners(thedet->specificSurface());
      
    /*
    for( unsigned i = 0; i < corners.size(); ++i ) {
      std::cout << i << " global calo: " << (corners[i] - meanPos) << " perp=" << corners[i].perp() << std::endl;
      auto localpt = thedet->specificSurface().toLocal(corners[i]);
      std::cout << i << " local calo : " << localpt << " perp= " << localpt.perp() << std::endl ;
      std::cout << i << " globalgd   : " << (gdcorners[i] - meanPos) << " perp= " << gdcorners[i].perp() << std::endl ;
      localpt = thedet->specificSurface().toLocal(gdcorners[i]);
      std::cout << i << " localdg    : " << localpt << " perp= " << localpt.perp() << std::endl ;
      std::cout << std::endl;
    }
    std::cout << std::endl << std::endl;
    */

    if( id.zside() > 0 )
      sectorsPos.emplace(geo,std::make_pair(id,thedet));
    else 
      sectorsNeg.emplace(geo,std::make_pair(id,thedet));
  }
  
  std::cout << sectorsPos.size() << ' ' << sectorsNeg.size() << std::endl;

  float radlen=0.65f, xi=radlen * 0.42e-3;
  addBarrelLayer(new FTLBarrelDetLayer(FastTimeDetId(FastTimeDetId::FastTimeBarrel,0,0,1),sectorsPos,radlen,xi));
  addBarrelLayer(new FTLBarrelDetLayer(FastTimeDetId(FastTimeDetId::FastTimeBarrel,0,0,-1),sectorsNeg,radlen,xi));
}

void FTLTracker::makeDisks(int subdet, int disks)
{    
  std::vector<float>  rmax(disks, 0), rmin(disks, 9e9);
  std::vector<double> zsumPos(disks), zsumNeg(disks);
  std::vector<int>    countPos(disks), countNeg(disks);
  const std::vector<DetId> & ids = endcapGeom_->getValidDetIds();
  if (ftltracking::g_debuglevel > 0) std::cout << "on subdet " << subdet << " I got a total of " << ids.size() << " det ids " << std::endl;
  for (auto & i : ids) {
    //const GlobalPoint & pos = endcapGeom_->getPosition(i); 
    const auto& corners = endcapGeom_->getCorners(i);
    float z = corners[0].z();
    float maxrho = 0.0;
    float minrho = std::numeric_limits<float>::max();
    for( unsigned i = 0; i < corners.size()/2; ++i ) {
      maxrho = std::max(corners[i].perp2(), maxrho);
      minrho = std::min(corners[i].perp2(), minrho);
    }
    maxrho = std::sqrt(maxrho);
    minrho = std::sqrt(minrho);
    int side = z > 0 ? +1 : -1;
    int layer = 0;
    if (subdet == 5) {
      if (std::abs(z) < 400 || std::abs(z) > 600 || (layer == 4 && maxrho > 240)) continue; // bad, not BH
    }
    (side > 0 ? zsumPos : zsumNeg)[layer] += z;
    (side > 0 ? countPos : countNeg)[layer]++;
    if (maxrho > rmax[layer]) rmax[layer] = maxrho;
    if (minrho < rmin[layer]) rmin[layer] = minrho;
  }
  for (int i = 0; i < disks; ++i) {
    float radlen=0.65f, xi=radlen * 0.42e-3; // see DataFormats/GeometrySurface/interface/MediumProperties.h    
    if (countPos[i]) {
      printf("Positive disk %2d at z = %+7.2f   %6.1f <= rho <= %6.1f\n", i+1, zsumPos[i]/countPos[i], rmin[i], rmax[i]);
      addEndcapLayer(new FTLDiskGeomDet(subdet, +1, i+1, zsumPos[i]/countPos[i], rmin[i], rmax[i], radlen, xi));
    }
    if (countNeg[i]) {
      printf("Negative disk %2d at z = %+7.2f   %6.1f <= rho <= %6.1f\n", i+1, zsumNeg[i]/countPos[i], rmin[i], rmax[i]);
      addEndcapLayer(new FTLDiskGeomDet(subdet, -1, i+1, zsumNeg[i]/countNeg[i], rmin[i], rmax[i], radlen, xi));
    }
  }
}


const FTLDiskGeomDet * FTLTracker::nextEndcapLayer(const FTLDiskGeomDet *from, PropagationDirection direction) const 
{
    const std::vector<FTLDiskGeomDet *> & vec = (from->zside() > 0 ? disksPos_ : disksNeg_);
    auto it = std::find(vec.begin(), vec.end(), from);
    if (it == vec.end()) throw cms::Exception("LogicError", "nextDisk called with invalid starting disk");
    if (direction == alongMomentum) {
        if (*it == vec.back()) return nullptr;
        return *(++it);
    } else {
        if (it == vec.begin()) return nullptr;
        return *(--it);
    }
}

const FTLDiskGeomDet * FTLTracker::idToEndcapDet(DetId id) const 
{
  FastTimeDetId temp(id);
  auto positr = disksLookupPos_.find(temp.geometryCell());
  auto negitr = disksLookupNeg_.find(temp.geometryCell());
  if( positr != disksLookupPos_.end() ) return disksPos_[positr->second];
  if( negitr != disksLookupNeg_.end() ) return disksNeg_[negitr->second];
  
  return nullptr;
}

const FTLBarrelDetLayer * FTLTracker::nextBarrelLayer(const FTLBarrelDetLayer *from, PropagationDirection direction) const 
{
    const auto& lookup = (from->zside() > 0 ? cylindersLookupPos_ : cylindersLookupNeg_);
    const auto& vec = (from->zside() > 0 ? cylindersPos_ : cylindersNeg_);
    auto layer = lookup.find(from->geographicalId());
    if (layer == lookup.end()) throw cms::Exception("LogicError", "nextDisk called with invalid starting disk");
    auto it = vec.begin() + layer->second;
    if (direction == alongMomentum) {
        if (*it == vec.back()) return nullptr;
        return *(++it);
    } else {
        if (it == vec.begin()) return nullptr;
        return *(--it);
    }
}

const FTLBarrelDetLayer * FTLTracker::idToBarrelDet(DetId id) const 
{
  FastTimeDetId temp(id);
  auto positr = cylindersLookupPos_.find(temp.geometryCell());
  auto negitr = cylindersLookupNeg_.find(temp.geometryCell());
  if( positr != cylindersLookupPos_.end() ) 
    return cylindersPos_[positr->second];
  if( negitr != cylindersLookupNeg_.end() ) 
    return cylindersNeg_[negitr->second];
  
  return nullptr;
}



