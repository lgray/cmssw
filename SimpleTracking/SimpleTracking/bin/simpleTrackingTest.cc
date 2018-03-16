// this is a test program that makes random helices and fits them

#include <iostream>
#include <unordered_set>
#include <cmath>
#include <random>

constexpr unsigned nTracks = 1;
constexpr unsigned nLayers = 16;
// position units are cm
constexpr double distanceBetweenLayers[nLayers] = {1.0, 1.0, 2.0, 2.0,
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
  const double m_gen = m_pion;
  SVector3 genPos(0,0,0), genMom(1.0/std::sqrt(2),1.0/std::sqrt(2),0.1);
  SVector4 genPos4(0,0,0,0);
  SVector3 genMomRPT(genCharge*1.0/std::hypot(genMom[0],genMom[1]),
		     std::atan2(genMom[0],genMom[1]),
		     std::atan2(std::hypot(genMom[0],genMom[1]),genMom[2])); // momentum to generate from, momentum units are GeV

  std::cout << "pT = " << std::abs(1.0/genMomRPT[0]) << " pZ = " << std::abs(1.f/genMomRPT[0])/std::tan(genMomRPT[2]) << std::endl;

  SMatrixSym88 genErrors;// = ROOT::Math::SMatrixIdentity();
  TrackState genTrack(genCharge,genPos4,genMomRPT,m_gen,genErrors);
  
  
  std::array<bool, nLayers> layerHasMeasurement;
  std::array<Hit, nLayers> measurements;

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
      std::cout << "gen pT: " << genState.pT() << std::endl;
      std::cout << genState.valid << ' ' << genState.parameters << std::endl;
      if( genState.valid ) {
	// calculate the local -> global rotation matrix 
	// (here from a tangent plane at radius r)
	const float r = getHypot(genState.parameters[0],genState.parameters[1]);
	SMatrix44 rot; // SO+(1,3) rotation
	rot[0][0] = -(genState.parameters[1])/(r);
	rot[0][1] = 0;
	rot[0][2] =  (genState.parameters[0])/(r);
	rot[1][0] = rot[0][2];
	rot[1][1] = 0;
	rot[1][2] = -rot[0][0];
	rot[2][0] = 0;
	rot[2][1] = 1;
	rot[2][2] = 0;
	rot[3][3] = 1;
	const SMatrix44 rotT = ROOT::Math::Transpose(rot);

	// now construct the local errors and transform to global
	SMatrixSym44 localErr;
	localErr[0][0] = positionResolution*positionResolution;
	localErr[1][1] = positionResolution*positionResolution;
	localErr[3][3] = timingResolution*timingResolution;
	SMatrixSym44 globalErr = ROOT::Math::SimilarityT(rotT,localErr);

	std::cout << "local:\n" << localErr << std::endl << "global:\n" << globalErr << std::endl; 
	SVector4 pos_glo4(genState.x(),genState.y(),genState.z(),genState.t());
	SVector4 pos_loc4 = rotT*pos_glo4;
	std::cout << pos_glo4 << ' ' << pos_loc4 << std::endl;
	//smear hit coordinate in local x/y plane
	pos_loc4[0] = pos_loc4[0] + positionReso(gen);
	pos_loc4[1] = pos_loc4[1] + positionReso(gen);
	pos_loc4[3] = pos_loc4[3] + timingReso(gen);
	pos_glo4 = rot*pos_loc4;
	std::cout << pos_glo4 << ' ' << pos_loc4 << std::endl;

	Hit temp(pos_glo4,globalErr);	
	measurements[iLay] = std::move(temp);
      } else {
	layerHasMeasurement[iLay] = false;
      }
    }
  }

  std::cout << "done generating hits" << std::endl;
  
  //return 0;

  // simplified seeding
  unsigned max_seed_layer = 0;
  SVector3 r0(0.0,0,0.0), p0(1.0*genMomRPT); // the seed guess 
  SVector4 r04(0,0,0,0);
  SMatrixSym88 seedErrors = ROOT::Math::SMatrixIdentity();
  //seedErrors *= 10;
  TrackState seedTrack(genCharge,r04,p0,m_gen,seedErrors);
  for( unsigned iTrk = 0; iTrk < nTracks; ++iTrk ) {
    float total_r = 0.f;
    TrackState seedState = seedTrack;
    for( unsigned iLay = 0; iLay < nLayers; ++iLay ) {
      total_r += distanceBetweenLayers[iLay];      
      if( seedingLayers.count(iLay) && layerHasMeasurement[iLay] ) {
	if( iLay > max_seed_layer ) max_seed_layer = iLay;
	PropagationFlags pflags;      
	seedState = propagateHelixToR(seedState,total_r, pflags);
	//updateParametersWithTime(seedState,measurements[iLay].measurementState());
	seedState = updateParametersWithTime(seedState,measurements[iLay].measurementState());

	std::cout << measurements[iLay].measurementState().parametersWithTime() << std::endl;
	std::cout << "new pT = " << seedState.pT() << " +/- " << seedState.epT() << std::endl
		  << "new pZ = " << seedState.pz() << " +/- " << std::sqrt(seedState.epzpz()) << std::endl
		  << "new beta = " << seedState.beta() << " +/- " << seedState.ebeta() << std::endl;
      }
    }
    seedTrack = seedState;
  }
  
  return 0;

  // trackfinding
  TrackState kfTrack = seedTrack;
  for( unsigned iTrk = 0; iTrk < nTracks; ++iTrk ) {
    float total_r = 0.f;
    TrackState kfState = kfTrack;
    for( unsigned iLay =0 ; iLay <= max_seed_layer ; ++iLay ) {
      total_r += distanceBetweenLayers[iLay]; 
    }
    for( unsigned iLay = max_seed_layer+1; iLay < nLayers; ++iLay ) {
      total_r += distanceBetweenLayers[iLay];      
      if( layerHasMeasurement[iLay] ) {
	PropagationFlags pflags;      
	kfState = propagateHelixToR(kfState,total_r, pflags);
	kfState = updateParameters(kfState,measurements[iLay].measurementState());

	std::cout << measurements[iLay].measurementState().parametersWithTime() << std::endl;
	std::cout << "new pT = " << kfState.pT() << " +/- " << kfState.epT() << std::endl
		  << "new pZ = " << kfState.pz() << " +/- " << std::sqrt(kfState.epzpz()) << std::endl;
      }
    }
  }

  
  
  return 0;
}
