#ifndef RecoHGCAL_HGCALClusters_HGCalImagingAlgo_h
#define RecoHGCAL_HGCALClusters_HGCalImagingAlgo_h

#include "Geometry/HGCalGeometry/interface/HGCalGeometry.h"
#include "Geometry/CaloTopology/interface/HGCalTopology.h"
#include "Geometry/HcalTowerAlgo/interface/HcalGeometry.h"
#include "DataFormats/ForwardDetId/interface/HGCEEDetId.h"
#include "DataFormats/HGCRecHit/interface/HGCRecHitCollections.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/CaloGeometry/interface/TruncatedPyramid.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

#include "DataFormats/Math/interface/Point3D.h"
#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/ForwardDetId/interface/HGCalDetId.h"

//#include "RecoLocalCalo/HGCalRecHitDump/interface/RecHitTools.h"

// C/C++ headers
#include <string>
#include <vector>
#include <set>


#include "RecoLocalCalo/HGCalRecHitDump/interface/HGCalHelpers.h"
#include "KDTreeLinkerAlgoT.h"


template <typename T>
std::vector<size_t> sorted_indices(const std::vector<T> &v) {

  // initialize original index locations
  std::vector<size_t> idx(v.size());
  for (size_t i = 0; i != idx.size(); ++i) idx[i] = i;

  // sort indices based on comparing values in v
  sort(idx.begin(), idx.end(),
       [&v](size_t i1, size_t i2) {return v[i1] > v[i2];});

  return idx;
}

class HGCalImagingAlgo
{
  
 public:
  enum VerbosityLevel { pDEBUG = 0, pWARNING = 1, pINFO = 2, pERROR = 3 };

 HGCalImagingAlgo() : layer_select(-1), delta_c(0.), kappa(1.), ecut(0.),
                      cluster_offset(0),
		      geometry(0), 
                      algoId(reco::CaloCluster::undefined),
		      verbosity(pERROR),
		      points(2*(maxlayer+2)){
 }

  HGCalImagingAlgo(float delta_c_in, double kappa_in, double ecut_in,
		   const CaloGeometry *thegeometry_p,
		   //		   const CaloSubdetectorTopology *thetopology_p,
		   reco::CaloCluster::AlgoId algoId_in,
		   VerbosityLevel the_verbosity = pERROR,
		   int dbglayer = -1) :
    layer_select(dbglayer),
    delta_c(delta_c_in),
    kappa(kappa_in),
    ecut(ecut_in),
    cluster_offset(0),
    sigma2(1.0),
    geometry(thegeometry_p),
    //topology(*thetopology_p),
    algoId(algoId_in),
    verbosity(the_verbosity),
    points(2*(maxlayer+2)),
    minpos(2*(maxlayer+2),{ {0.0f,0.0f} }),
    maxpos(2*(maxlayer+1),{ {0.0f,0.0f} }){
    }

  HGCalImagingAlgo(float delta_c_in, double kappa_in, double ecut_in,
		   double showerSigma,
		   const CaloGeometry *thegeometry_p,
		   //		   const CaloSubdetectorTopology *thetopology_p,
		   reco::CaloCluster::AlgoId algoId_in,
		   VerbosityLevel the_verbosity = pERROR,
		   int dbglayer = -1) :
    layer_select(dbglayer),
    delta_c(delta_c_in),
    kappa(kappa_in),
    ecut(ecut_in),
    cluster_offset(0),
    sigma2(std::pow(showerSigma,2.0)),
    geometry(thegeometry_p),
    //topology(*thetopology_p),
    algoId(algoId_in),
    verbosity(the_verbosity),
    points(2*(maxlayer+2)),
    minpos(2*(maxlayer+2),{ {0.0f,0.0f} }),
    maxpos(2*(maxlayer+1),{ {0.0f,0.0f} }){
    }

  virtual ~HGCalImagingAlgo()
    {
    }

  void setVerbosity(VerbosityLevel the_verbosity)
    {
      verbosity = the_verbosity;
    }

  void populate(const HGCRecHitCollection &hits);

  // this is the method that will start the clusterisation (it is possible to invoke this method more than once - but make sure it is with
  // different hit collections (or else use reset)
  void makeClusters();
  // this is the method to get the cluster collection out
  std::vector<reco::BasicCluster> getClusters(bool);
  // needed to switch between EE and HE with the same algorithm object (to get a single cluster collection)
  void setGeometry(const CaloGeometry *thegeometry_p){geometry = thegeometry_p;}
  // use this if you want to reuse the same cluster object but don't want to accumulate clusters (hardly useful?)
  void reset(){
    current_v.clear();
    clusters_v.clear();
    cluster_offset = 0;
    // the kdnode-embedded hexel vectors need to be cleared across events or different iterations of the algorithm
    for( std::vector< std::vector<KDNode> >::iterator it = points.begin(); it != points.end(); it++)
      {
	    for( std::vector<KDNode>::iterator jt = it->begin(); jt != it->end(); jt++)
	      delete jt->data;
	    it->clear();
      }
  }
  /// point in the space
  typedef math::XYZPoint Point;

 private:

  //max number of layers
  static const unsigned int maxlayer = 52;
  const int layer_select; // for debugging

  // The two parameters used to identify clusters
  float delta_c;
  double kappa;

  // The hit energy cutoff
  double ecut;

  // The current offset into the temporary cluster structure
  unsigned int cluster_offset;

  // for energy sharing
  double sigma2; // transverse shower size

  // The vector of clusters
  std::vector<reco::BasicCluster> clusters_v;

  const CaloGeometry *geometry;
  
  // The algo id
  reco::CaloCluster::AlgoId algoId;

  // The verbosity level
  VerbosityLevel verbosity;

  
  struct Hexel {

    double x;
    double y;
    double z;
    bool isHalfCell;
    double weight;
    double fraction;
    DetId detid;
    double rho;
    double delta;
    int nearestHigher;
    bool isBorder;
    bool isHalo;
    int clusterIndex;
    const CaloGeometry *geometry;

    Hexel(const HGCRecHit &hit, DetId id_in, bool isHalf, const CaloGeometry *geometry_in) :
      x(0.),y(0.),z(0.),isHalfCell(isHalf),
      weight(0.), fraction(1.0), detid(id_in), rho(0.), delta(0.),
      nearestHigher(-1), isBorder(false), isHalo(false),
      clusterIndex(-1), geometry(geometry_in)
    {
      const GlobalPoint position( std::move( detid.det() == DetId::Hcal ?
					     hgcia::getPosition(geometry, HcalDetId(id_in)) :
					     hgcia::getPosition(geometry, HGCalDetId(id_in)) ) );
      /*
      const CaloCellGeometry::CornersVec corners( std::move( geometry->getCorners( detid.det() == DetId::Hcal ?
										   getCorners(geometry, HcalDetId(id)),
										   getCorners(geometry, HGCalDetId(id)) ) ) );
      */

      weight = hit.energy();
      x = position.x();
      y = position.y();
      z = position.z();

    }
    Hexel() :
      x(0.),y(0.),z(0.),isHalfCell(false),
      weight(0.), fraction(1.0), detid(), rho(0.), delta(0.),
      nearestHigher(-1), isBorder(false), isHalo(false),
      clusterIndex(-1),
      geometry(0)
    {}
    bool operator > (const Hexel& rhs) const {
      return (rho > rhs.rho);
    }
    int layer(){
      return hgcia::layerOffset(detid);
    }

  };

  typedef KDTreeLinkerAlgo<Hexel*,2> KDTree;
  typedef KDTreeNodeInfoT<Hexel*,2> KDNode;
  
  // A vector of vectors of KDNodes holding an Hexel in the clusters - to be used to build CaloClusters of DetIds
  std::vector< std::vector<KDNode> > current_v;

  std::vector<size_t> sort_by_delta(const std::vector<KDNode> &v){
    std::vector<size_t> idx(v.size());
    for (size_t i = 0; i != idx.size(); ++i) idx[i] = i;
    sort(idx.begin(), idx.end(),
	 [&v](size_t i1, size_t i2) {return v[i1].data->delta > v[i2].data->delta;});
    return idx;
  }

  std::vector<std::vector<KDNode> > points;
  // std::vector<std::vector<Hexel> > points; //a vector of vectors of hexels, one for each layer
  //@@EM todo: the number of layers should be obtained programmatically - the range is 1-n instead of 0-n-1...

  std::vector<std::array<float,2> > minpos;
  std::vector<std::array<float,2> > maxpos;

  //these functions should be in a helper class.
  double distance(const Hexel *pt1, const Hexel *pt2); //2-d distance on the layer (x-y)
  double calculateLocalDensity(std::vector<KDNode> &, KDTree &); //return max density
  double calculateDistanceToHigher(std::vector<KDNode> &, KDTree &);
  int findAndAssignClusters(std::vector<KDNode> &, KDTree &, double, KDTreeBox &);
  math::XYZPoint calculatePosition(std::vector<KDNode> &);

  // attempt to find subclusters within a given set of hexels
  std::vector<unsigned> findLocalMaximaInCluster(const std::vector<KDNode>&);
  math::XYZPoint calculatePositionWithFraction(const std::vector<KDNode>&, const std::vector<double>&);
  double calculateEnergyWithFraction(const std::vector<KDNode>&, const std::vector<double>&);
  // outputs
  void shareEnergy(const std::vector<KDNode>&,
		   const std::vector<unsigned>&,
		   std::vector<std::vector<double> >&);
 };

#endif
