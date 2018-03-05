// this is a test program that makes random helices and fits them

#include <iostream>
#include <unordered_set>
#include <cmath>
#include <random>

constexpr unsigned nTracks = 1;
constexpr unsigned nLayers = 16;
// position units are cm
constexpr double distanceBetweenLayers[nLayers] = {1.0, 1.0, 1.0, 1.0,
						   5.0, 5.0, 5.0, 5.0,
						   5.0, 5.0, 10.0, 10.0,
						   10.0, 10.0, 10.0, 10.0};
constexpr double positionResolution = 50.0e-4/std::sqrt(12.0);
constexpr double timingResolution = 30e-3; // nanoseconds;
constexpr double layerEfficiency = 0.99;
constexpr double m_pion = 0.139570; // GeV
constexpr double m_kaon = 0.493677; // GeV
constexpr double m_proton = 0.938272; // GeV


#include "SimpleTracking/SimpleTracking/interface/Hit.h"
#include "SimpleTracking/SimpleTracking/interface/Track.h"
#include "SimpleTracking/SimpleTracking/interface/KalmanUtils.h"
#include "SimpleTracking/SimpleTracking/interface/Propagation.h"
#include "SimpleTracking/SimpleTracking/interface/Matrix.h"


int main() {
  
  //random number generators and distributions
  std::random_device rd;  //Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
  std::uniform_real_distribution<> eff(0.0, 1.0);
  std::uniform_real_distribution<> positionReso(-0.5*positionResolution,0.5*positionResolution);
  std::normal_distribution<> timingReso(0.,timingResolution);
  
  //define seeding layers
  const std::unordered_set<unsigned> seedingLayers = {0,1,2,3};


  const int genCharge = -1;
  SVector3 genPos(0,0,0), genMom(std::sqrt(0.5),std::sqrt(0.5),0.1); // momentum to generate from, momentum units are GeV
  SMatrixSym66 genErrors;// = ROOT::Math::SMatrixIdentity();
  TrackState genTrack(genCharge,genPos,genMom,genErrors);
  
  SVector3 rkf, pkf; // from the kalman filter

  std::array<bool, nLayers> layerHasMeasurement;
  std::array<SVector3, nLayers> measurements;

  // generate a track
  for( unsigned iTrk = 0; iTrk < nTracks; ++iTrk ) {
    layerHasMeasurement.fill(false);
    std::cout << "generating track: " << iTrk+1 << " with " << nLayers << " layers!" << std::endl;
    float total_r = 0.f;
    TrackState genState = genTrack;
    for( unsigned iLay = 0; iLay < nLayers; ++iLay ) {
      std::cout << "layer: " << iLay+1;
      total_r += distanceBetweenLayers[iLay];
      if( eff(gen) > layerEfficiency) { std::cout << std::endl; continue; } // apply efficiency
      layerHasMeasurement[iLay] = true;
      std::cout << " has a measurement!" << std::endl;
      PropagationFlags pflags;      
      genState = propagateHelixToR(genState,total_r, pflags);
      std::cout << genState.valid << ' ' << genState.parameters << std::endl;
    }
  }

  // simplified seeding
  SVector3 r0, p0(1.0,1.0,0.15); // the seed guess 
  SMatrixSym66 seedErrors;// = ROOT::Math::SMatrixIdentity();
  TrackState seedTrack(genCharge,r0,p0,seedErrors);
  for( unsigned iLay = 0; iLay < nLayers; ++iLay ) {
    if( seedingLayers.count(iLay) ) {
    } else {
    }
  }

  // trackfinding

  return 0;
}
