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
  std::uniform_real_distribution<> positionReso(-0.5*positionResolution*std::sqrt(12),0.5*positionResolution*std::sqrt(12));
  std::normal_distribution<> timingReso(0.,timingResolution);
  
  //define seeding layers
  const std::unordered_set<unsigned> seedingLayers = {0,1,2,3};
  
  const int genCharge = -1;
  SVector3 genPos(0,0,0), genMom(std::sqrt(0.5),std::sqrt(0.5),0.1); // momentum to generate from, momentum units are GeV
  SMatrixSym66 genErrors;// = ROOT::Math::SMatrixIdentity();
  TrackState genTrack(genCharge,genPos,genMom,genErrors);
  
  
  std::array<bool, nLayers> layerHasMeasurement;
  std::array<Hit, nLayers> measurements;

  // generate a track
  for( unsigned iTrk = 0; iTrk < nTracks; ++iTrk ) {
    layerHasMeasurement.fill(false);
    std::cout << "generating track: " << iTrk+1 << " with " << nLayers << " layers!" << std::endl;
    float total_r = 0.f;
    TrackState genState = genTrack;
    for( unsigned iLay = 0; iLay < 1; ++iLay ) {
      std::cout << "layer: " << iLay+1;
      total_r += distanceBetweenLayers[iLay];
      if( eff(gen) > layerEfficiency) { std::cout << std::endl; continue; } // apply efficiency
      layerHasMeasurement[iLay] = true;
      std::cout << " has a measurement!" << std::endl;
      PropagationFlags pflags;      
      genState = propagateHelixToR(genState,total_r, pflags);
      std::cout << genState.valid << ' ' << genState.parameters << std::endl;
      if( genState.valid ) {
	// calculate the local -> global rotation matrix 
	// (here from a tangent plane at radius r)
	const float r = getHypot(genState.parameters[0],genState.parameters[1]);
	SMatrix33 rot;
	rot[0][0] = -(genState.parameters[1])/(r);
	rot[0][1] = 0;
	rot[0][2] =  (genState.parameters[0])/(r);
	rot[1][0] = rot[0][2];
	rot[1][1] = 0;
	rot[1][2] = -rot[0][0];
	rot[2][0] = 0;
	rot[2][1] = 1;
	rot[2][2] = 0;
	const SMatrix33 rotT = ROOT::Math::Transpose(rot);

	// now construct the local errors and transform to global
	SMatrixSym33 localErr;
	localErr[0][0] = positionResolution*positionResolution;
	localErr[1][1] = positionResolution*positionResolution;
	SMatrixSym33 globalErr = ROOT::Math::SimilarityT(rotT,localErr);

	std::cout << "local:\n" << localErr << std::endl << "global:\n" << globalErr << std::endl; 
	SVector3 pos_glo3(genState.x(),genState.y(),genState.z());
	SVector3 pos_loc3 = rotT*pos_glo3;
	std::cout << pos_glo3 << ' ' << pos_loc3 << std::endl;
	//smear hit coordinate in local x/y plane
	pos_loc3[0] = pos_loc3[0] + positionReso(gen);
	pos_loc3[1] = pos_loc3[1] + positionReso(gen);
	pos_glo3 = rot*pos_loc3;
	std::cout << pos_glo3 << ' ' << pos_loc3 << std::endl;

	Hit temp(pos_glo3,globalErr);	
	measurements[iLay] = std::move(temp);
      } else {
	layerHasMeasurement[iLay] = false;
      }
    }
  }

  std::cout << "done generating hits" << std::endl;

  return 0;

  // simplified seeding
  unsigned max_seed_layer = 0;
  SVector3 r0(0,0,0), p0(std::sqrt(0.5),std::sqrt(0.5),0.1); // the seed guess 
  SMatrixSym66 seedErrors = ROOT::Math::SMatrixIdentity();
  TrackState seedTrack(genCharge,r0,p0,seedErrors);
  for( unsigned iTrk = 0; iTrk < nTracks; ++iTrk ) {
    float total_r = 0.f;
    TrackState seedState = seedTrack;
    for( unsigned iLay = 0; iLay < nLayers; ++iLay ) {
      total_r += distanceBetweenLayers[iLay];      
      if( seedingLayers.count(iLay) && layerHasMeasurement[iLay] ) {
	if( iLay > max_seed_layer ) max_seed_layer = iLay;
	PropagationFlags pflags;      
	seedState = propagateHelixToR(seedState,total_r, pflags);
	seedState = updateParameters(seedState,measurements[iLay].measurementState());
      }
    }
    seedTrack = seedState;
  }
  
  // trackfinding
  TrackState kfTrack = seedTrack;
  for( unsigned iTrk = 0; iTrk < nTracks; ++iTrk ) {
    float total_r = 0.f;
    TrackState kfState = kfTrack;
    for( unsigned iLay = 0; iLay <= max_seed_layer; ++iLay ) {
      total_r += distanceBetweenLayers[iLay]; 
    }
    for( unsigned iLay = max_seed_layer; iLay < nLayers; ++iLay ) {
      total_r += distanceBetweenLayers[iLay];      
      if( layerHasMeasurement[iLay] ) {
	PropagationFlags pflags;      
	kfState = propagateHelixToR(kfState,total_r, pflags);
	kfState = updateParameters(kfState,measurements[iLay].measurementState());
      }
    }
  }
  
  return 0;
}
