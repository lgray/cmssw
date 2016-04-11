#ifndef __SimCalorimetry_HGCCalSimProducers_HGCDigitizerTypes_h__
#define __SimCalorimetry_HGCCalSimProducers_HGCDigitizerTypes_h__

#include <unordered_map>
#include <Eigen/Dense>
#include <array>

#define EIGEN_NO_DEBUG

namespace hgc_digi {

  //15 time samples: 9 pre-samples, 1 in-time, 5 post-samples
  constexpr size_t nSamples = 15;
  constexpr size_t PULSE_SIZE = 6;
  
  typedef float HGCSimData_t;
  
  typedef Eigen::Matrix<float,nSamples,1> HGCSimHitData;  
  typedef Eigen::Matrix<float,PULSE_SIZE,1> HGCPulseVector;
  typedef Eigen::Matrix<float,PULSE_SIZE*2+nSamples,1> HGCWorkVector;

  struct HGCCellInfo {
    //1st array=energy, 2nd array=energy weighted time-of-flight
    std::array<HGCSimHitData,2> hit_info;
    int thickness;
  };
  
  typedef std::unordered_map<uint32_t, HGCCellInfo > HGCSimHitDataAccumulator; 

}
#endif
