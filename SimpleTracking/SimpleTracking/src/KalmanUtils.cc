#include "SimpleTracking/SimpleTracking/interface/KalmanUtils.h"

#define DEBUG

static const SMatrix38 projMatrix  = ROOT::Math::SMatrixIdentity();
static const SMatrix83 projMatrixT = ROOT::Math::Transpose(projMatrix);

static const SMatrix48 projMatrixTime  = ROOT::Math::SMatrixIdentity();
static const SMatrix84 projMatrixTimeT = ROOT::Math::Transpose(projMatrixTime);


TrackState updateParameters(const TrackState& propagatedState, const MeasurementState& measurementState)
{
  float r = getHypot(measurementState.pos_[0],measurementState.pos_[1]);
  SMatrix33 rot;
  rot[0][0] = -(measurementState.pos_[1]+propagatedState.parameters[1])/(2*r);
  rot[0][1] = 0;
  rot[0][2] =  (measurementState.pos_[0]+propagatedState.parameters[0])/(2*r);
  rot[1][0] = rot[0][2];
  rot[1][1] = 0;
  rot[1][2] = -rot[0][0];
  rot[2][0] = 0;
  rot[2][1] = 1;
  rot[2][2] = 0;
  const SMatrix33 rotT = ROOT::Math::Transpose(rot);
  const SVector3 res_glo = measurementState.parameters()-propagatedState.parameters.Sub<SVector3>(0);
  const SVector3 res_loc3 = rotT * res_glo;
  const SVector3 res(res_loc3[0],res_loc3[1],0);
  const SMatrixSym33 resErr_glo = measurementState.errors() + propagatedState.errors.Sub<SMatrixSym33>(0,0);
  //the matrix to invert has to be 2x2
  int invFail(0);
  const SMatrixSym22 resErr22 = ROOT::Math::SimilarityT(rot,resErr_glo).Sub<SMatrixSym22>(0,0);
  const SMatrixSym22 resErrInv22 = resErr22.InverseFast(invFail);
  if (0 != invFail) {
    dprint(__FILE__ << ":" << __LINE__ << ": FAILED INVERSION");
    return propagatedState;
  }
  //now go back to 3x3
  SMatrixSym33 resErrInv;
  resErrInv[0][0] = resErrInv22[0][0];
  resErrInv[1][1] = resErrInv22[1][1];
  resErrInv[1][0] = resErrInv22[1][0];

  SVector8 pred_ccs = propagatedState.parameters;
  pred_ccs[4] = 1./propagatedState.pT();
  pred_ccs[5] = propagatedState.momPhi();
  pred_ccs[6] = propagatedState.theta();
  SMatrix88 jac_ccs = ROOT::Math::SMatrixIdentity();
  jac_ccs[4][4] = -propagatedState.px()/pow(propagatedState.pT(),3);
  jac_ccs[4][5] = -propagatedState.py()/pow(propagatedState.pT(),3);
  jac_ccs[5][4] = -propagatedState.py()/pow(propagatedState.pT(),2);
  jac_ccs[5][5] =  propagatedState.px()/pow(propagatedState.pT(),2);
  jac_ccs[6][4] =  propagatedState.px()*propagatedState.pz()/(propagatedState.pT()*pow(propagatedState.p(),2));
  jac_ccs[6][5] =  propagatedState.py()*propagatedState.pz()/(propagatedState.pT()*pow(propagatedState.p(),2));
  jac_ccs[6][6] = -propagatedState.pT()/pow(propagatedState.p(),2);
  SMatrixSym88 pred_err_ccs = ROOT::Math::Similarity(jac_ccs,propagatedState.errors);
  SVector8 up_pars_ccs =  pred_ccs + pred_err_ccs*projMatrixT*rot*resErrInv*res;

  SMatrixSym88 I88 = ROOT::Math::SMatrixIdentity();
  SMatrix38 H = rotT*projMatrix;
  SMatrix83 K = pred_err_ccs*ROOT::Math::Transpose(H)*resErrInv;
  SMatrixSym33 locErrMeas = ROOT::Math::SimilarityT(rot,measurementState.errors());
  locErrMeas[2][0] = 0;
  locErrMeas[2][1] = 0;
  locErrMeas[2][2] = 0;
  locErrMeas[1][2] = 0;
  locErrMeas[0][2] = 0;
  SMatrixSym88 up_errs_ccs = ROOT::Math::Similarity(I88-K*H,pred_err_ccs) + ROOT::Math::Similarity(K,locErrMeas);
  SMatrix88 jac_back_ccs = ROOT::Math::SMatrixIdentity();
  jac_back_ccs[4][4] = -cos(up_pars_ccs[5])/pow(up_pars_ccs[4],2);
  jac_back_ccs[4][5] = -sin(up_pars_ccs[5])/up_pars_ccs[4];
  jac_back_ccs[5][4] = -sin(up_pars_ccs[5])/pow(up_pars_ccs[4],2);
  jac_back_ccs[5][5] =  cos(up_pars_ccs[5])/up_pars_ccs[4];
  jac_back_ccs[6][4] = -cos(up_pars_ccs[6])/(sin(up_pars_ccs[6])*pow(up_pars_ccs[4],2));
  jac_back_ccs[6][6] = -1./(pow(sin(up_pars_ccs[6]),2)*up_pars_ccs[4]);

  TrackState result;
  result.parameters = up_pars_ccs;
  //result.parameters[4] = cos(up_pars_ccs[5])/up_pars_ccs[4];
  //result.parameters[5] = sin(up_pars_ccs[5])/up_pars_ccs[4];
  //result.parameters[6] = cos(up_pars_ccs[6])/(sin(up_pars_ccs[6])*up_pars_ccs[4]);
  result.errors = up_errs_ccs; //ROOT::Math::Similarity(jac_back_ccs,up_errs_ccs);
  result.charge = propagatedState.charge;
  result.valid = propagatedState.valid;

  if (0 != invFail) {
    dprint(__FILE__ << ":" << __LINE__ << ": FAILED INVERSION");
    return propagatedState;
  }

#ifdef DEBUG
  {
    //dmutex_guard;
    std::cout << "\n updateParameters \n" << std::endl << "propErr" << std::endl;
    dumpMatrix(propagatedState.errors);
    std::cout << "residual: " << res[0] << " " << res[1] << std::endl
              << "resErr22" << std::endl;
    dumpMatrix(resErr22);
    std::cout << "resErrInv22" << std::endl;
    dumpMatrix(resErrInv22);
    std::cout << "jac_ccs" << std::endl;
    dumpMatrix(jac_ccs);
    std::cout << "pred_err_ccs" << std::endl;
    dumpMatrix(pred_err_ccs);
    std::cout << "K" << std::endl;
    dumpMatrix(K);
    std::cout << "H" << std::endl;
    dumpMatrix(H);
    std::cout << "locErrMeas" << std::endl;
    dumpMatrix(locErrMeas);
    std::cout << "updatedPars" << std::endl;
    std::cout << result.parameters << std::endl;
    std::cout << "updatedErrs" << std::endl;
    dumpMatrix(result.errors);
    std::cout << std::endl;
  }
#endif

  return result;
}

TrackState updateParametersWithTime(const TrackState& propagatedState, const MeasurementState& measurementState)
{
  float r = getHypot(measurementState.pos_[0],measurementState.pos_[1]);
  SMatrix44 rot; // rotation only in 3D space
  rot[0][0] = -(measurementState.pos_[1]+propagatedState.parameters[1])/(2*r);
  rot[0][1] = 0;
  rot[0][2] =  (measurementState.pos_[0]+propagatedState.parameters[0])/(2*r);
  rot[1][0] = rot[0][2];
  rot[1][1] = 0;
  rot[1][2] = -rot[0][0];
  rot[2][0] = 0;
  rot[2][1] = 1;
  rot[2][2] = 0;
  rot[3][3] = 1;

  std::cout << "4D rotation:\n" << rot << std::endl;
  
  const SMatrix44 rotT = ROOT::Math::Transpose(rot); 
  const SVector4 res_glo = measurementState.parametersWithTime()-propagatedState.parameters.Sub<SVector4>(0);
  const SVector4 res_loc4 = rotT * res_glo;
  const SVector4 res(res_loc4[0],res_loc4[1],0,res_loc4[3]);
  const SMatrixSym44 resErr_glo = measurementState.errorsWithTime() + propagatedState.errors.Sub<SMatrixSym44>(0,0);
  //the matrix to invert has to be 3x3
  int invFail(0);
  SMatrixSym33 resErr33;  
  SMatrixSym44 resErr_loc = ROOT::Math::SimilarityT(rot,resErr_glo);
  resErr33[0][0] = resErr_loc[0][0];
  resErr33[0][1] = resErr_loc[0][1];
  resErr33[1][1] = resErr_loc[1][1];
  resErr33[2][2] = resErr_loc[3][3];
  std::cout << "resErr33:\n" << resErr33 << std::endl;
  const SMatrixSym33 resErrInv33 = resErr33.InverseFast(invFail);
  if (0 != invFail) {
    dprint(__FILE__ << ":" << __LINE__ << ": FAILED INVERSION");
    return propagatedState;
  }
  //now go back to 4x4
  SMatrixSym44 resErrInv;
  resErrInv[0][0] = resErrInv33[0][0];
  resErrInv[1][1] = resErrInv33[1][1];
  resErrInv[1][0] = resErrInv33[1][0];
  resErrInv[3][3] = resErrInv33[2][2];
    
  SVector8 pred_ccs = propagatedState.parameters;
  pred_ccs[4] = 1./propagatedState.pT();
  pred_ccs[5] = propagatedState.momPhi();
  pred_ccs[6] = propagatedState.theta();
  pred_ccs[7] = propagatedState.betagamma();
  SMatrix88 jac_ccs = ROOT::Math::SMatrixIdentity();
  jac_ccs[4][4] = -propagatedState.px()/pow(propagatedState.pT(),3);
  jac_ccs[4][5] = -propagatedState.py()/pow(propagatedState.pT(),3);
  jac_ccs[5][4] = -propagatedState.py()/pow(propagatedState.pT(),2);
  jac_ccs[5][5] =  propagatedState.px()/pow(propagatedState.pT(),2);
  jac_ccs[6][4] =  propagatedState.px()*propagatedState.pz()/(propagatedState.pT()*pow(propagatedState.p(),2));
  jac_ccs[6][5] =  propagatedState.py()*propagatedState.pz()/(propagatedState.pT()*pow(propagatedState.p(),2));
  jac_ccs[6][6] = -propagatedState.pT()/pow(propagatedState.p(),2);
  // time / betagamma
  const float mass = propagatedState.p()/propagatedState.betagamma();
  std::cout << " mass = " << mass << std::endl;
  jac_ccs[7][4] = propagatedState.px()/(propagatedState.p()*mass);
  jac_ccs[7][5] = propagatedState.py()/(propagatedState.p()*mass);
  jac_ccs[7][6] = propagatedState.pz()/(propagatedState.p()*mass);

  
  SMatrixSym88 pred_err_ccs = ROOT::Math::Similarity(jac_ccs,propagatedState.errors);
  SVector8 up_pars_ccs =  pred_ccs + pred_err_ccs*projMatrixTimeT*rot*resErrInv*res;

  SMatrixSym88 I88 = ROOT::Math::SMatrixIdentity();
  SMatrix48 H = rotT*projMatrixTime;
  SMatrix84 K = pred_err_ccs*ROOT::Math::Transpose(H)*resErrInv;
  SMatrixSym44 locErrMeas = ROOT::Math::SimilarityT(rot,measurementState.errorsWithTime());
  locErrMeas[3][0] = 0;
  locErrMeas[3][1] = 0;
  locErrMeas[3][2] = 0;
  locErrMeas[2][0] = 0;
  locErrMeas[2][1] = 0;
  locErrMeas[2][2] = 0;
  locErrMeas[1][2] = 0;
  locErrMeas[0][2] = 0;
  SMatrixSym88 up_errs_ccs = ROOT::Math::Similarity(I88-K*H,pred_err_ccs) + ROOT::Math::Similarity(K,locErrMeas);
  /*
  SMatrix88 jac_back_ccs = ROOT::Math::SMatrixIdentity();
  jac_back_ccs[4][4] = -cos(up_pars_ccs[4])/pow(up_pars_ccs[3],2);
  jac_back_ccs[4][5] = -sin(up_pars_ccs[4])/up_pars_ccs[3];
  jac_back_ccs[5][4] = -sin(up_pars_ccs[4])/pow(up_pars_ccs[3],2);
  jac_back_ccs[5][5] =  cos(up_pars_ccs[4])/up_pars_ccs[3];
  jac_back_ccs[6][4] = -cos(up_pars_ccs[5])/(sin(up_pars_ccs[5])*pow(up_pars_ccs[3],2));
  jac_back_ccs[6][6] = -1./(pow(sin(up_pars_ccs[5]),2)*up_pars_ccs[3]);
  */
  
  TrackState result;
  result.parameters = up_pars_ccs;
  //result.parameters[3] = cos(up_pars_ccs[4])/up_pars_ccs[3];
  //result.parameters[4] = sin(up_pars_ccs[4])/up_pars_ccs[3];
  //result.parameters[5] = cos(up_pars_ccs[5])/(sin(up_pars_ccs[5])*up_pars_ccs[3]);
  result.errors = up_errs_ccs;//ROOT::Math::Similarity(jac_back_ccs,up_errs_ccs);
  result.charge = propagatedState.charge;
  result.valid = propagatedState.valid;

  if (0 != invFail) {
    dprint(__FILE__ << ":" << __LINE__ << ": FAILED INVERSION");
    return propagatedState;
  }

#ifdef DEBUG
  {
    //dmutex_guard;
    std::cout << "\n updateParameters \n" << std::endl << "propErr" << std::endl;
    dumpMatrix(propagatedState.errors);
    std::cout << "residual: " << res[0] << " " << res[1] << " " << res[3]<< std::endl
              << "resErr33" << std::endl;
    dumpMatrix(resErr33);
    std::cout << "resErrInv33" << std::endl;
    dumpMatrix(resErrInv33);
    std::cout << "jac_ccs" << std::endl;
    dumpMatrix(jac_ccs);
    std::cout << "pred_err_ccs" << std::endl;
    dumpMatrix(pred_err_ccs);
    std::cout << "K" << std::endl;
    dumpMatrix(K);
    std::cout << "H" << std::endl;
    dumpMatrix(H);
    std::cout << "locErrMeas" << std::endl;
    dumpMatrix(locErrMeas);
    std::cout << "updatedPars" << std::endl;
    std::cout << result.parameters << std::endl;
    std::cout << "updatedErrs" << std::endl;
    dumpMatrix(result.errors);
    std::cout << std::endl;
  }
#endif
  return result;
}

float computeChi2(const TrackState& propagatedState, const MeasurementState& measurementState)
{
  float r = getHypot(measurementState.pos_[0],measurementState.pos_[1]);
  //rotate to the tangent plane to the cylinder of radius r at the hit position
  SMatrix33 rot;
  rot[0][0] = -(measurementState.pos_[1]+propagatedState.parameters[1])/(2*r);
  rot[0][1] = 0;
  rot[0][2] =  (measurementState.pos_[0]+propagatedState.parameters[0])/(2*r);
  rot[1][0] = rot[0][2];
  rot[1][1] = 0;
  rot[1][2] = -rot[0][0];
  rot[2][0] = 0;
  rot[2][1] = 1;
  rot[2][2] = 0;
  const SMatrix33 rotT = ROOT::Math::Transpose(rot);
  const SVector3 res_glo = measurementState.parameters()-propagatedState.parameters.Sub<SVector3>(0);
  const SVector3 res_loc3 = rotT * res_glo;
  //the matrix to invert has to be 2x2
  const SVector2 res(res_loc3[0],res_loc3[1]);
  const SMatrixSym33 resErr_glo = measurementState.errors() + propagatedState.errors.Sub<SMatrixSym33>(0,0);
  const SMatrixSym22 resErr = ROOT::Math::SimilarityT(rot,resErr_glo).Sub<SMatrixSym22>(0,0);
  int invFail(0);
  SMatrixSym22 resErrInv = resErr.InverseFast(invFail);
  if (0 != invFail) {
    dprint(__FILE__ << ":" << __LINE__ << ": FAILED INVERSION");
    return 9999.;
  }
  return ROOT::Math::Similarity(res,resErrInv);
}

TrackState updateParametersEndcap(const TrackState& propagatedState, const MeasurementState& measurementState)
{
  TrackState result = propagatedState;

  const SMatrixSym88& propErr = propagatedState.errors;
  const SMatrixSym33 herr = measurementState.errors();
  const SVector3& hpos = measurementState.parameters();

  SMatrixSym22 resErr = herr.Sub<SMatrixSym22>(0,0)+propErr.Sub<SMatrixSym22>(0,0);

  int invFail(0);
  SMatrixSym22 resErrInv22 = resErr.InverseFast(invFail);
  if (0 != invFail) {
    dprint(__FILE__ << ":" << __LINE__ << ": FAILED INVERSION");
    return propagatedState;
  }

  SVector2 res2 = SVector2(hpos.At(0)-propagatedState.x(),
			   hpos.At(1)-propagatedState.y());
    
  SMatrix82 projMatrixT_zp;
  projMatrixT_zp(0,0) = 1.;
  projMatrixT_zp(1,1) = 1.;

  SMatrix82 K = propErr*projMatrixT_zp*resErrInv22;

  result.parameters = propagatedState.parameters + K*res2;

  result.errors = propErr - ROOT::Math::SimilarityT(propErr,ROOT::Math::Similarity(projMatrixT_zp,resErrInv22));

  return result;
}

float computeChi2Endcap(const TrackState& propagatedState, const MeasurementState& measurementState)
{

  const SMatrixSym88& propErr = propagatedState.errors;
  const SMatrixSym33 herr = measurementState.errors();
  const SVector3& hpos = measurementState.parameters();

  SMatrixSym22 resErr = herr.Sub<SMatrixSym22>(0,0)+propErr.Sub<SMatrixSym22>(0,0);

  int invFail(0);
  SMatrixSym22 resErrInv = resErr.InverseFast(invFail);
  if (0 != invFail) {
    dprint(__FILE__ << ":" << __LINE__ << ": FAILED INVERSION");
    return 9999.;
  }
  const SVector2 res(hpos.At(0)-propagatedState.x(), hpos.At(1)-propagatedState.y());
  return ROOT::Math::Similarity(res,resErrInv);
}
