#ifndef Geometry_MTDNumberingBuilder_GeometricTimingDetExtra_H
#define Geometry_MTDNumberingBuilder_GeometricTimingDetExtra_H

//#include "CondFormats/GeometryObjects/interface/PGeometricTimingDetExtra.h"
#include "Geometry/MTDNumberingBuilder/interface/GeometricTimingDet.h"
#include "DetectorDescription/Core/interface/DDExpandedView.h"
/* #include "DetectorDescription/Core/interface/DDRotationMatrix.h" */
/* #include "DetectorDescription/Core/interface/DDTranslation.h" */
/* #include "DetectorDescription/Core/interface/DDSolidShapes.h" */
/* #include "DataFormats/GeometrySurface/interface/Surface.h" */
/* #include "DataFormats/GeometrySurface/interface/Bounds.h" */
#include "DataFormats/DetId/interface/DetId.h"

#include <vector>
#include "FWCore/ParameterSet/interface/types.h"

#include <ext/pool_allocator.h>
// waiting for template-alias
//#define PoolAlloc  __gnu_cxx::__pool_alloc
/**
 * Composite class GeometricTimingDetExtra. A composite can contain other composites, and so on;
 * You can understand what you are looking at via enum.
 */

class GeometricTimingDetExtra {
 public:
  typedef DDExpandedView::NavRange NavRange;
#ifdef PoolAlloc
  typedef std::vector< DDExpandedNode, PoolAlloc<DDExpandedNode> > GeoHistory;
#endif
#ifndef PoolAlloc
  typedef std::vector<DDExpandedNode> GeoHistory;
#endif
  /**
   * Constructors to be used when looping over DDD
   */
  GeometricTimingDetExtra( GeometricTimingDet const *gd ) : _mygd(gd) { }; // this better be "copied into" or it will never have any valid numbers/info.
    
    GeometricTimingDetExtra( GeometricTimingDet const *gd, DetId id, GeoHistory& gh,  double vol, double dens, double wgt, double cpy, const std::string& mat, const std::string& name, bool dd=false );

  /**
   *
   */
  ~GeometricTimingDetExtra();
  
  /*
    GeometricTimingDetExtra(const GeometricTimingDetExtra &);
  
  GeometricTimingDetExtra & operator=( const GeometricTimingDetExtra & );
  */
  /**
   * get and set associated GeometricTimingDet 
   * DOES NO CHECKING!
   */
  GeometricTimingDet const * geometricDet() const { return _mygd; } 
  //  void setGeometricTimingDet( GeometricTimingDet* gd )  { _mygd=gd; }
  
  /**
   * set or add or clear components
   */
  void setGeographicalId(DetId id) {
    _geographicalId = id; 
    //std::cout <<"setGeographicalId " << int(id) << std::endl;
  }
  DetId geographicalId() const { return _geographicalId; }
  //rr
  /** parents() retuns the geometrical history
   * mec: only works if this is built from DD and not from reco DB.
   */
  GeoHistory const &  parents() const {
    //std::cout<<"parents"<<std::endl;
    return _parents;
  }
  //rr  
  int copyno() const {
    //std::cout<<"copyno"<<std::endl;
    return _copy;
  }
  double volume() const {
    //std::cout<<"volume"<<std::endl;
    return _volume;
  }
  double density() const {
    //std::cout<<"density"<<std::endl;
    return _density;
  }
  double weight() const {
    //std::cout<<"weight"<<std::endl;
    return _weight;
  }
  std::string const &  material() const {
    //std::cout<<"material"<<std::endl;
    return _material;
  }
  
  /**
   * what it says... used the DD in memory model to build the geometry... or not.
   */
  bool wasBuiltFromDD() const {
    //std::cout<<"wasBuildFromDD"<<std::endl;
    return _fromDD;
  }

  std::string const& name() const { return _name; }
  
 private:

  /** Data members **/

  GeometricTimingDet const* _mygd;  
  DetId _geographicalId;
  GeoHistory _parents;
  double _volume;
  double _density;
  double _weight;
  int    _copy;
  std::string _material;
  std::string _name;
  bool _fromDD; // may not need this, keep an eye on it.
};

#undef PoolAlloc
#endif
