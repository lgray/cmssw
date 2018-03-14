#include "SimpleTracking/SimpleTracking/interface/Propagation.h"

constexpr double tolerance = 0.0001;
constexpr double c_cm_ns = 29.9792; // c in cm/ns

struct HelixState
{
  HelixState(TrackState& s, const bool useParamBfield = false) :
    state(s)
  {
    setCoords(s.parameters);
    setHelixPar(s,useParamBfield);
  }

  void setCoords(const SVector8& par) {
    x = par.At(0);
    y = par.At(1);
    z = par.At(2);
    t = par.At(3);
    px = std::cos(par.At(5))*std::abs(1.f/par.At(4)); //par.At(3); 
    py = std::sin(par.At(5))*std::abs(1.f/par.At(4)); //par.At(4);
    pz = std::abs(1.f/par.At(4))/std::tan(par.At(6)); //par.At(5);
    betagamma = par.At(7);
    beta = betagamma/std::sqrt(1+betagamma*betagamma);
    r0 = getHypot(x,y);
  }

  void setHelixPar(const TrackState& s, const bool useParamBfield = false) {
    charge = s.charge;

    pt = getHypot(px,py);
    pt2 = pt*pt;
    pt3 = pt*pt2;
    pt4 = pt*pt3;

    //p=0.3Br => r=p/(0.3*B)
    k = charge*100./(-Config::sol*(useParamBfield?Config::BfieldFromZR(z,r0):Config::Bfield));
    curvature = pt * k; //in cm
    ctgTheta  = pz / pt;

    //variables to be updated at each iterations
    //derivatives initialized to value for first iteration, i.e. distance = r-r0in
    dTDdx = r0 > 0. ? -x/r0 : 0.;
    dTDdy = r0 > 0. ? -y/r0 : 0.;
    dTDdpx = 0.;
    dTDdpy = 0.;
  }

  void updateHelix(float distance, bool updateDeriv, bool debug = false);
  void propagateErrors(const HelixState& in, float totalDistance, bool debug = false);

  float x, y, z, t, px, py, pz, betagamma, beta;
  float k, pt, pt2, pt3, pt4, r0, curvature, ctgTheta;
  float dTDdx, dTDdy, dTDdpx, dTDdpy;
  int charge;
  TrackState& state;
};

void HelixState::updateHelix(float distance, bool updateDeriv, bool debug)
{
  // NOTE: Distance is sort-of radial distance (cord length, not curve).

  const float angPath = distance/curvature;


  dprint("angPath=" << angPath);
  const float cosAP = cos(angPath);
  const float sinAP = sin(angPath);

  //helix propagation formulas
  //http://www.phys.ufl.edu/~avery/fitting/fitting4.pdf
  SVector8& par = state.parameters;
  par.At(0) = x + k*(px*sinAP-py*(1-cosAP));
  par.At(1) = y + k*(py*sinAP+px*(1-cosAP));
  par.At(2) = z + distance*ctgTheta;
  // this is ~the path length from previous layer
  par.At(3) = t + std::hypot(distance,distance*ctgTheta)/(c_cm_ns*beta);
  SVector3 local_p;

  std::cout << "betagamma = "<< par.At(7) << " +/- " << state.ebetagamma() 
	    << " beta = " << state.beta() << " +/- " << state.ebeta()<< std::endl;

  local_p[0] = px*cosAP-py*sinAP;
  local_p[1] = py*cosAP+px*sinAP;
  local_p[2] = pz;

  par.At(4) = charge*1.0/std::hypot(local_p[0],local_p[1]);
  par.At(5) = std::atan2(local_p[0],local_p[1]);
  par.At(6) = std::atan2(std::hypot(local_p[0],local_p[1]),local_p[2]);

  dprint("x + " << k*(px*sinAP-py*(1-cosAP)) << std::endl
      << "y + " << k*(py*sinAP+px*(1-cosAP)) << std::endl
      << "z + " << distance*ctgTheta << std::endl
      << "t + " << std::hypot(distance,distance*ctgTheta)/(c_cm_ns*beta) << std::endl
      <<  "px: " << px*cosAP-py*sinAP
      << " py: " << py*cosAP+px*sinAP
      << " pz: " << pz
      << " betagamma: " << betagamma);
  
  if (updateDeriv)
  {
    // don't need timing information here, all the magic happens in the error prop
    //update derivatives on total distance for next step, where totalDistance+=r-r0
    //now r0 depends on px and py
    const float r0inv = 1./r0;
    dprint("r0=" << r0 << " r0inv=" << r0inv << " pt=" << pt);
    //update derivative on D
    const float dAPdx = -x/(r0*curvature);
    const float dAPdy = -y/(r0*curvature);
    const float dAPdpx = -angPath*px/pt2;
    const float dAPdpy = -angPath*py/pt2;

    const float dxdx = 1 + k*dAPdx*(px*cosAP - py*sinAP);
    const float dxdy = k*dAPdy*(px*cosAP - py*sinAP);
    const float dydx = k*dAPdx*(py*cosAP + px*sinAP);
    const float dydy = 1 + k*dAPdy*(py*cosAP + px*sinAP);

    const float dxdpx = k*(sinAP + px*cosAP*dAPdpx - py*sinAP*dAPdpx);
    const float dxdpy = k*(px*cosAP*dAPdpy - 1. + cosAP - py*sinAP*dAPdpy);
    const float dydpx = k*(py*cosAP*dAPdpx + 1. - cosAP + px*sinAP*dAPdpx);
    const float dydpy = k*(sinAP + py*cosAP*dAPdpy + px*sinAP*dAPdpy);

    dTDdx -= r0inv*(x*dxdx + y*dydx);
    dTDdy -= r0inv*(x*dxdy + y*dydy);
    dTDdpx -= r0inv*(x*dxdpx + y*dydpx);
    dTDdpy -= r0inv*(x*dxdpy + y*dydpy);
  }

  dprint(par.At(0) << " " << par.At(1) << " " << par.At(2) << "; r = " << std::hypot(par.At(0), par.At(1)) << std::endl
      << par.At(4) << " " << par.At(5) << " " << par.At(6));
}

void HelixState::propagateErrors(const HelixState& in, float totalDistance, bool debug)
{
  // NOTE: Distance is sort-of radial distance (cord length, not curve).

  const float TD = totalDistance;
  const float TP = totalDistance/curvature;
  const float C  = curvature;

#ifdef DEBUG
  SVector8& par = state.parameters;
  dprint("TD=" << TD << " TP=" << TP << " arrived at r=" << sqrt(par.At(0)*par.At(0)+par.At(1)*par.At(1)));
#endif

  const float dCdpx  = k*in.px/pt;
  const float dCdpy  = k*in.py/pt;

  const float dTPdx  =  dTDdx/C;
  const float dTPdy  =  dTDdy/C;
  const float dTPdpx = (dTDdpx*C - TD*dCdpx)/(C*C);
  const float dTPdpy = (dTDdpy*C - TD*dCdpy)/(C*C);

  const float cosTP  = cos(TP);
  const float sinTP  = sin(TP);
  
  //jacobian
  SMatrix88 errorProp = ROOT::Math::SMatrixIdentity(); //what is not explicitly set below is 1 (0) on (off) diagonal

  errorProp(0,0) = 1 + k*dTPdx*(in.px*cosTP - in.py*sinTP);                   //dxdx;
  errorProp(0,1) = k*dTPdy*(in.px*cosTP - in.py*sinTP);                       //dxdy;
  errorProp(0,4) = k*(sinTP + in.px*cosTP*dTPdpx - in.py*sinTP*dTPdpx);       //dxdpx;
  errorProp(0,5) = k*(in.px*cosTP*dTPdpy - 1. + cosTP - in.py*sinTP*dTPdpy);  //dxdpy;

  errorProp(1,0) = k*dTPdx*(in.py*cosTP + in.px*sinTP);                       //dydx;
  errorProp(1,1) = 1 + k*dTPdy*(in.py*cosTP + in.px*sinTP);                   //dydy;
  errorProp(1,4) = k*(in.py*cosTP*dTPdpx + 1. - cosTP + in.px*sinTP*dTPdpx);  //dydpx;
  errorProp(1,5) = k*(sinTP + in.py*cosTP*dTPdpy + in.px*sinTP*dTPdpy);       //dydpy;

  //par.At(2) = z + distance*ctgTheta;

  errorProp(2,0) = dTDdx*ctgTheta;                                            //dzdx;
  errorProp(2,1) = dTDdy*ctgTheta;                                            //dzdy;
  errorProp(2,4) = dTDdpx*ctgTheta - TD*in.pz*in.px/pt3;                      //dzdpx;
  errorProp(2,5) = dTDdpy*ctgTheta - TD*in.pz*in.py/pt3;                      //dzdpy;
  errorProp(2,6) = TD/pt;                                                     //dzdpz;

  //par.At(3) = t + std::hypot(distance,distance*ctgTheta)/(c_cm_ns*beta);

  errorProp(3,0) = dTDdx*std::sqrt(1+ctgTheta*ctgTheta)/(c_cm_ns*beta);       //dtdx;
  errorProp(3,1) = dTDdy*std::sqrt(1+ctgTheta*ctgTheta)/(c_cm_ns*beta);       //dtdy;
  errorProp(3,4) = ( dTDdpx*std::sqrt(1+ctgTheta*ctgTheta)/(c_cm_ns*beta) 
		     - 2*TD/(c_cm_ns*beta)*in.pz*in.pz*in.px/(pt4*std::sqrt(1+ctgTheta*ctgTheta)) ); //dtdpx;
  errorProp(3,5) = ( dTDdpy*std::sqrt(1+ctgTheta*ctgTheta)/(c_cm_ns*beta) 
		     - 2*TD/(c_cm_ns*beta)*in.pz*in.pz*in.py/(pt4*std::sqrt(1+ctgTheta*ctgTheta)) ); //dtdpy                     //dtdpy;
  errorProp(3,6) = TD*pz/(c_cm_ns*beta*pt2*std::sqrt(1+ctgTheta*ctgTheta));                                                     //dtdpz;

  errorProp(4,0) = -dTPdx*(in.px*sinTP + in.py*cosTP);                        //dpxdx;
  errorProp(4,1) = -dTPdy*(in.px*sinTP + in.py*cosTP);                        //dpxdy;
  errorProp(4,4) = cosTP - dTPdpx*(in.px*sinTP + in.py*cosTP);                //dpxdpx;
  errorProp(4,5) = -sinTP - dTPdpy*(in.px*sinTP + in.py*cosTP);               //dpxdpy;

  errorProp(5,0) = -dTPdx*(in.py*sinTP - in.px*cosTP);                        //dpydx;
  errorProp(5,1) = -dTPdy*(in.py*sinTP - in.px*cosTP);                        //dpydy;
  errorProp(5,4) = +sinTP - dTPdpx*(in.py*sinTP - in.px*cosTP);               //dpydpx;
  errorProp(5,5) = +cosTP - dTPdpy*(in.py*sinTP - in.px*cosTP);               //dpydpy;

  state.errors=ROOT::Math::Similarity(errorProp,state.errors);

  dprint("errorProp");
  dcall(dumpMatrix(errorProp));
  dprint("result.errors");
  dcall(dumpMatrix(state.errors));
}

//==============================================================================

// helix propagation in steps along helix trajectory. 
// each step travels for a path lenght equal to delta r between the current position and the target radius. 
// for track with pT>=1 GeV this converges to the correct path lenght in <5 iterations
// derivatives need to be updated at each iteration
TrackState propagateHelixToR(TrackState inputState, float r, const PropagationFlags pflags)
{
  bool debug = false;

  const HelixState hsin(inputState, pflags.use_param_b_field);
  TrackState result(inputState);
  HelixState hsout(result, pflags.use_param_b_field);

#ifdef CHECKSTATEVALID
  if (!hsout.state.valid) {
    return hsout.state;
  }
#endif

  dprint("attempt propagation from r=" << hsin.r0 << " to r=" << r << std::endl
              << "x=" << hsin.x << " y=" << hsin.y << " px=" << hsin.px
              << " py=" << hsin.py << " pz=" << hsin.pz << " q=" << inputState.charge);


  if (std::abs(r-hsout.r0) < tolerance) {
    dprint("at target radius, returning input");
    return hsout.state;
  }

  dprint("curvature=" << hsin.curvature);

  float totalDistance = 0;

  for (unsigned int i=0;i<Config::Niter;++i) {

    dprint("propagation iteration #" << i);

    const float distance = r-hsout.r0;
    totalDistance+=distance;

    dprint("r0=" << hsout.r0 << " pt=" << hsout.pt << std::endl
                 << "distance=" << distance);
 
    bool updateDeriv = i+1!=Config::Niter && hsout.r0>0.;
    hsout.updateHelix(distance, updateDeriv, debug);
    hsout.setCoords(hsout.state.parameters);

    if (std::abs(r-hsout.r0) < tolerance) {
      dprint("distance = " << r-hsout.r0 << " at iteration=" <<  i);
      break;
    }
    if ( i == (Config::Niter-1) && std::abs(r-hsout.r0) > tolerance) {
#ifdef DEBUG
      if (debug) { // common condition when fit fails to converge
        dmutex_guard;
        std::cerr << __FILE__ << ":" << __LINE__ 
                  << ": failed to converge in propagateHelixToR() after " << (i+1) << " iterations, r = "
                  << r 
                  << ", hsout.r = " << hsout.r0
                  << std::endl;
      }
#endif
      hsout.state.valid = false;
    }
  }

  hsout.propagateErrors(hsin, totalDistance, debug);
  return hsout.state;
}

TrackState propagateHelixToZ(TrackState inputState, float zout, const PropagationFlags pflags)
{
  TrackState result = inputState;

  const float z = inputState.z();
  const float ipT = inputState.invpT();
  const float phi   = inputState.momPhi();
  const float theta = inputState.theta();

  const float cosT = std::cos(theta);
  const float sinT = std::sin(theta);

  const float cosP = std::cos(phi);
  const float sinP = std::sin(phi);

  const float k = inputState.charge*100.f/(-Config::sol*(pflags.use_param_b_field ? Config::BfieldFromZR(z,inputState.posR()) : Config::Bfield));

  const float s = (zout - z)/cosT;
  const float angPath = s*sinT*ipT/k;

  float cosA, sinA;
  if (Config::useTrigApprox) {
    sincos4(angPath, sinA, cosA);
  } else {
    cosA=std::cos(angPath);
    sinA=std::sin(angPath);
  }

  result.parameters[0] = inputState.x() + k*(inputState.px()*sinA-inputState.py()*(1.f-cosA));
  result.parameters[1] = inputState.y() + k*(inputState.py()*sinA+inputState.px()*(1.f-cosA));
  result.parameters[2] = zout;
  result.parameters[5] = phi+angPath;

  SMatrix88 jac = ROOT::Math::SMatrixIdentity();
  jac.At(0,2) = cosP*sinT*(sinP*cosA*sin(cosP*sinA) - cosA)/cosT;
  jac.At(0,4) = cosP*sinT*(zout - z)*cosA*( 1.f - sinP*sin(cosP*sinA) )/(cosT*ipT) - k*(cosP*sinA - sinP*(1.f-cos(cosP*sinA)))/(ipT*ipT);
  jac.At(0,5) = (k/ipT)*( -sinP*sinA + sinP*sinP*sinA*sin(cosP*sinA) - cosP*(1.f - cos(cosP*sinA) ) );
  jac.At(0,6) = cosP*(zout - z)*cosA*( 1.f - sinP*sin(cosP*sinA) )/(cosT*cosT);
  jac.At(1,2) = cosA*sinT*(cosP*cosP*sin(cosP*sinA) - sinP)/cosT;
  jac.At(1,4) = sinT*(zout - z)*cosA*( cosP*cosP*sin(cosP*sinA) + sinP )/(cosT*ipT) - k*(sinP*sinA + cosP*(1.f-cos(cosP*sinA)))/(ipT*ipT);
  jac.At(1,5) = (k/ipT)*( -sinP*(1.f - cos(cosP*sinA)) - sinP*cosP*sinA*sin(cosP*sinA) + cosP*sinA );
  jac.At(1,6) = (zout - z)*cosA*( cosP*cosP*sin(cosP*sinA) + sinP )/(cosT*cosT);
  jac.At(2,2) = 0.f;
  jac.At(5,2) = -ipT*sinT/(cosT*k);
  jac.At(5,4) = sinT*(zout - z)/(cosT*k);
  jac.At(5,6) = ipT*(zout - z)/(cosT*cosT*k);
  result.errors=ROOT::Math::Similarity(jac,result.errors);

  return result;
}
