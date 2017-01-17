#ifndef HGCalGeometry_FastTimeTrackingGeometry_h
#define HGCalGeometry_FastTimeTrackingGeometry_h

/** \class FastTimeTrackingGeometry
 *
 *  The model of the geometry of the endcap muon CSC detectors.
 *
 *  \author Tim Cox
 */

#include <DataFormats/DetId/interface/DetId.h>
#include <DataFormats/ForwardDetId/interface/FastTimeDetId.h>
#include <Geometry/CommonDetUnit/interface/TrackingGeometry.h>
#include <Geometry/HGCalGeometry/interface/FastTimeGeometry.h>
#include <vector>
#include <unordered_map>

class GeomDetType;

namespace std{
  template<>
  struct hash<DetId> {
    size_t operator()(const DetId& id) const {
      return std::hash<uint32_t>()(id.rawId());
    }
  };
}

class FastTimeTrackingGeometry : public TrackingGeometry {

  typedef std::unordered_map<DetId, GeomDet*> FTLDetMap;
  
 public:
  
  /// Default constructor
  FastTimeTrackingGeometry();

  /// Real constructor
  FastTimeTrackingGeometry( const FastTimeGeometry* geom );

  /// Destructor
  virtual ~FastTimeTrackingGeometry();

  //---- Base class' interface

  // Return a vector of all det types
  virtual const DetTypeContainer&  detTypes() const override;

  // Return a vector of all GeomDetUnit
  virtual const DetUnitContainer& detUnits() const override;

  // Return a vector of all GeomDet (including all GeomDetUnits)
  virtual const DetContainer& dets() const override;
  
  // Return a vector of all GeomDetUnit DetIds
  virtual const DetIdContainer&    detUnitIds() const override;

  // Return a vector of all GeomDet DetIds (including those of GeomDetUnits)
  virtual const DetIdContainer& detIds() const override;

  // Return the pointer to the GeomDetUnit corresponding to a given DetId
  virtual const GeomDetUnit* idToDetUnit(DetId) const override;

  // Return the pointer to the GeomDet corresponding to a given DetId
  virtual const GeomDet* idToDet(DetId) const override;
  
 private:

  /// Add a DetType
  void addDetType(GeomDetType* type);
  
  /// Add a GeomDet; not to be called by the builder.
  void addDet(GeomDet* det);
  
  // Map for efficient lookup by DetId 
  FTLDetMap         theMap;

  // These are used rarely; they could be computed at runtime 
  // to save memory.
  DetTypeContainer  theDetTypes;
  DetContainer      theDets;       // all dets (chambers and layers)
  DetUnitContainer  theDetUnits;   // all layers
  DetIdContainer    theDetIds;
  DetIdContainer    theDetUnitIds;

  // pointer back to the FastTime CaloGeometry (why....)
  const FastTimeGeometry* theGeom;

  // Parameters controlling modelling of geometry 
  
};

#endif

