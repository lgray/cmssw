#include "SimpleTracking/SimpleTracking/interface/Track.h"

//==============================================================================
// TrackState
//==============================================================================

void TrackState::convertFromCartesianToCCS() {
  //assume we are currently in cartesian coordinates and want to move to ccs
  const float px = parameters.At(4);
  const float py = parameters.At(5);
  const float pz = parameters.At(6);
  const float pt = std::sqrt(px*px+py*py);
  const float phi = getPhi(px,py);
  const float theta = getTheta(pt,pz);
  parameters.At(4) = 1.f/pt;
  parameters.At(5) = phi;
  parameters.At(6) = theta;
  SMatrix88 jac = jacobianCartesianToCCS(px,py,pz);
  errors = ROOT::Math::Similarity(jac,errors);
}

void TrackState::convertFromCCSToCartesian() {
  //assume we are currently in ccs coordinates and want to move to cartesian
  const float invpt = parameters.At(4);
  const float phi   = parameters.At(5);
  const float theta = parameters.At(6);
  const float pt = 1.f/invpt;
  float cosP = std::cos(phi);
  float sinP = std::sin(phi);
  float cosT = std::cos(theta);
  float sinT = std::sin(theta);
  parameters.At(4) = cosP*pt;
  parameters.At(5) = sinP*pt;
  parameters.At(6) = cosT*pt/sinT;
  SMatrix88 jac = jacobianCCSToCartesian(invpt, phi, theta);
  errors = ROOT::Math::Similarity(jac,errors);
}

SMatrix88 TrackState::jacobianCCSToCartesian(float invpt,float phi,float theta) const {
  //arguments are passed so that the function can be used both starting from ccs and from cartesian
  SMatrix88 jac = ROOT::Math::SMatrixIdentity();
  float cosP = std::cos(phi);
  float sinP = std::sin(phi);
  float cosT = std::cos(theta);
  float sinT = std::sin(theta);
  const float pt = 1.f/invpt;
  jac(4,4) = -cosP*pt*pt;
  jac(4,5) = -sinP*pt;
  jac(5,4) = -sinP*pt*pt;
  jac(5,5) =  cosP*pt;
  jac(6,4) = -cosT*pt*pt/sinT;
  jac(6,6) = -pt/(sinT*sinT);
  return jac;
}

SMatrix88 TrackState::jacobianCartesianToCCS(float px,float py,float pz) const {
  //arguments are passed so that the function can be used both starting from ccs and from cartesian
  SMatrix88 jac = ROOT::Math::SMatrixIdentity();
  const float pt = std::sqrt(px*px+py*py);
  const float p2 = px*px+py*py+pz*pz;
  jac(4,4) = -px/(pt*pt*pt);
  jac(4,5) = -py/(pt*pt*pt);
  jac(5,4) = -py/(pt*pt);
  jac(5,5) =  px/(pt*pt);
  jac(6,4) =  px*pz/(pt*p2);
  jac(6,5) =  py*pz/(pt*p2);
  jac(6,6) = -pt/p2;
  return jac;
}


//==============================================================================
// Track
//==============================================================================

void Track::sortHitsByLayer()
{
  std::sort(& hitsOnTrk_[0], & hitsOnTrk_[lastHitIdx_ + 1],
            [&](HitOnTrack h1, HitOnTrack h2) { return h1.layer < h2.layer; });
}

//==============================================================================

void print(const TrackState& s)
{
  std::cout << " x:  " << s.parameters[0]
            << " y:  " << s.parameters[1]
            << " z:  " << s.parameters[2] << std::endl
            << " px: " << s.parameters[3]
            << " py: " << s.parameters[4]
            << " pz: " << s.parameters[5] << std::endl
            << "valid: " << s.valid << " errors: " << std::endl;
  dumpMatrix(s.errors);
  std::cout << std::endl;
}

void print(std::string label, int itrack, const Track& trk, bool print_hits)
{
  std::cout << std::endl << label << ": " << itrack << " hits: " << trk.nFoundHits() << " State" << std::endl;
  print(trk.state());
  if (print_hits)
  {
    for (int i = 0; i < trk.nTotalHits(); ++i)
      printf("  %2d: lyr %2d idx %d\n", i, trk.getHitLyr(i), trk.getHitIdx(i));
  }
}

void print(std::string label, const TrackState& s)
{
  std::cout << label << std::endl;
  print(s);
}
