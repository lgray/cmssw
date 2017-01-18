#ifndef TkDetLayers_FTLBarrelSector_h
#define TkDetLayers_FTLBarrelSector_h


#include "TrackingTools/DetLayers/interface/GeometricSearchDet.h"
#include "TrackingTools/DetLayers/interface/DetRod.h"
#include "Utilities/BinningTools/interface/GenericBinFinderInZ.h"
#include "SubLayerCrossings.h"


/** A concrete implementation for FTL Barrel Sector
 *  
 */

#pragma GCC visibility push(hidden)
class FTLBarrelSector final : public DetRod {
 public:
  typedef GenericBinFinderInZ<float,GeomDet>   BinFinderType;

  FTLBarrelSector(std::vector<const GeomDet*>& innerDets,
		  std::vector<const GeomDet*>& outerDets) __attribute__ ((cold));
  ~FTLBarrelSector() __attribute__ ((cold));
  
  // GeometricSearchDet interface
  
  virtual const std::vector<const GeomDet*>& basicComponents() const {return theDets;}

  virtual const std::vector<const GeometricSearchDet*>& components() const __attribute__ ((cold));

  
  virtual std::pair<bool, TrajectoryStateOnSurface>
  compatible( const TrajectoryStateOnSurface& ts, const Propagator&, 
	      const MeasurementEstimator&) const  __attribute__ ((cold));

  void groupedCompatibleDetsV( const TrajectoryStateOnSurface& tsos,
			       const Propagator& prop,
			       const MeasurementEstimator& est,
			       std::vector<DetGroup> & result) const __attribute__ ((hot));
  
 
 private:
  // private methods for the implementation of groupedCompatibleDets()

  SubLayerCrossings computeCrossings( const TrajectoryStateOnSurface& tsos,
				      PropagationDirection propDir) const __attribute__ ((hot));
  
  bool addClosest( const TrajectoryStateOnSurface& tsos,
		   const Propagator& prop,
		   const MeasurementEstimator& est,
		   const SubLayerCrossing& crossing,
		   std::vector<DetGroup>& result) const __attribute__ ((hot));

  float computeWindowSize( const GeomDet* det, 
			   const TrajectoryStateOnSurface& tsos, 
			   const MeasurementEstimator& est) const __attribute__ ((hot));


  void searchNeighbors( const TrajectoryStateOnSurface& tsos,
			const Propagator& prop,
			const MeasurementEstimator& est,
			const SubLayerCrossing& crossing,
			float window, 
			std::vector<DetGroup>& result,
			bool checkClosest) const __attribute__ ((hot));

  const std::vector<const GeomDet*>& subSector( int ind) const {
    return (ind==0 ? theInnerDets : theOuterDets);
  }
  
 private:
  std::vector<const GeomDet*> theDets;
  std::vector<const GeomDet*> theInnerDets;
  std::vector<const GeomDet*> theOuterDets;
  
  ReferenceCountingPointer<Plane> theInnerPlane;
  ReferenceCountingPointer<Plane> theOuterPlane;

  BinFinderType theInnerBinFinder;
  BinFinderType theOuterBinFinder;

};


#pragma GCC visibility pop
#endif 
