#ifndef _SimpleTrackingtrack_
#define _SimpleTrackingtrack_

#include "SimpleTracking/SimpleTracking/interface/Hit.h"
#include "SimpleTracking/SimpleTracking/interface/Matrix.h"

#include <vector>
#include <map>

struct TrackState //  possible to add same accessors as track? 
{
public:
  
  TrackState() : valid(true) {}
  TrackState(int charge, const SVector3& pos, const SVector3& mom, const SMatrixSym66& err) :
    parameters(SVector6(pos.At(0),pos.At(1),pos.At(2),mom.At(0),mom.At(1),mom.At(2))),
    errors(err), charge(charge), valid(true) {}
  SVector3 position() const {return SVector3(parameters[0],parameters[1],parameters[2]);}
  SVector6 parameters;
  SMatrixSym66 errors;
  short charge;
  bool valid;

  // track state position
  float x()      const {return parameters.At(0);}
  float y()      const {return parameters.At(1);}
  float z()      const {return parameters.At(2);}
  float posR()   const {return getHypot(x(),y());}
  float posPhi() const {return getPhi  (x(),y());}
  float posEta() const {return getEta  (posR(),z());}

  // track state position errors
  float exx()    const {return std::sqrt(errors.At(0,0));}
  float eyy()    const {return std::sqrt(errors.At(1,1));}
  float ezz()    const {return std::sqrt(errors.At(2,2));}
  float exy()    const {return std::sqrt(errors.At(0,1));}
  float exz()    const {return std::sqrt(errors.At(0,2));}
  float eyz()    const {return std::sqrt(errors.At(1,2));}

  float eposR()   const {return std::sqrt(getRadErr2(x(),y(),errors.At(0,0),errors.At(1,1),errors.At(0,1)));}
  float eposPhi() const {return std::sqrt(getPhiErr2(x(),y(),errors.At(0,0),errors.At(1,1),errors.At(0,1)));}
  float eposEta() const {return std::sqrt(getEtaErr2(x(),y(),z(),errors.At(0,0),errors.At(1,1),errors.At(2,2),
						     errors.At(0,1),errors.At(0,2),errors.At(1,2)));}

  // track state momentum
  float invpT()  const {return parameters.At(3);}
  float momPhi() const {return parameters.At(4);}
  float theta()  const {return parameters.At(5);}
  float pT()     const {return std::abs(1.f/parameters.At(3));}
  float px()     const {return pT()*std::cos(parameters.At(4));}
  float py()     const {return pT()*std::sin(parameters.At(4));}
  float pz()     const {return pT()/std::tan(parameters.At(5));}
  float momEta() const {return getEta (theta());}
  float p()      const {return pT()/std::sin(parameters.At(5));}

  float einvpT()  const {return std::sqrt(errors.At(3,3));}
  float emomPhi() const {return std::sqrt(errors.At(4,4));}
  float etheta()  const {return std::sqrt(errors.At(5,5));}
  float epT()     const {return std::sqrt(errors.At(3,3))/(parameters.At(3)*parameters.At(3));}
  float emomEta() const {return std::sqrt(errors.At(5,5))/std::sin(parameters.At(5));}
  float epxpx()   const {return std::sqrt(getPxPxErr2(invpT(),momPhi(),errors.At(3,3),errors.At(4,4)));}
  float epypy()   const {return std::sqrt(getPyPyErr2(invpT(),momPhi(),errors.At(3,3),errors.At(4,4)));}
  float epzpz()   const {return std::sqrt(getPyPyErr2(invpT(),theta(),errors.At(3,3),errors.At(5,5)));}

  void convertFromCartesianToCCS();
  void convertFromCCSToCartesian();
  SMatrix66 jacobianCCSToCartesian(float invpt,float phi,float theta) const;
  SMatrix66 jacobianCartesianToCCS(float px,float py,float pz) const;
};


class Track
{
public:
  
  Track() {}

  
  Track(const TrackState& state, float chi2, int label, int nHits, const HitOnTrack* hits) :
    state_(state),
    chi2_ (chi2),
    label_(label)
  {
    for (int h = 0; h < nHits; ++h)
    {
      addHitIdx(hits[h].index, hits[h].layer, 0.0f);
    }
  }

  Track(int charge, const SVector3& position, const SVector3& momentum, const SMatrixSym66& errors, float chi2) :
    state_(charge, position, momentum, errors), chi2_(chi2) {}

  
  ~Track(){}

  const SVector6&     parameters() const {return state_.parameters;}
  const SMatrixSym66& errors()     const {return state_.errors;}
  const TrackState&   state()      const {return state_;}

  const float* posArray() const {return state_.parameters.Array();}
  const float* errArray() const {return state_.errors.Array();}

  // Non-const versions needed for CopyOut of Matriplex.
  SVector6&     parameters_nc() {return state_.parameters;}
  SMatrixSym66& errors_nc()     {return state_.errors;}
  TrackState&   state_nc()      {return state_;}

  SVector3 position() const {return SVector3(state_.parameters[0],state_.parameters[1],state_.parameters[2]);}
  SVector3 momentum() const {return SVector3(state_.parameters[3],state_.parameters[4],state_.parameters[5]);}

  
  int      charge() const {return state_.charge;}
  
  float    chi2()   const {return chi2_;}
  
  int      label()  const {return label_;}

  float x()      const { return state_.parameters[0]; }
  float y()      const { return state_.parameters[1]; }
  float z()      const { return state_.parameters[2]; }
  float posR()   const { return getHypot(state_.parameters[0],state_.parameters[1]); }
  float posPhi() const { return getPhi(state_.parameters[0],state_.parameters[1]); }
  float posEta() const { return getEta(state_.parameters[0],state_.parameters[1],state_.parameters[2]); }

  float px()     const { return state_.px();}
  float py()     const { return state_.py();}
  float pz()     const { return state_.pz();}
  float pT()     const { return state_.pT();}
  float invpT()  const { return state_.invpT();}
  float p()      const { return state_.p(); }
  float momPhi() const { return state_.momPhi(); }
  float momEta() const { return state_.momEta(); }
  float theta()  const { return state_.theta(); }

  // track state momentum errors
  float epT()     const { return state_.epT();}
  float emomPhi() const { return state_.emomPhi();}
  float emomEta() const { return state_.emomEta();}

  //this function is very inefficient, use only for debug and validation!
  const HitVec hitsVector(const std::vector<HitVec>& globalHitVec) const 
  {
    HitVec hitsVec;
    for (int ihit = 0; ihit < Config::nMaxTrkHits; ++ihit) {
      const HitOnTrack &hot = hitsOnTrk_[ihit];
      if (hot.index >= 0) {
        hitsVec.push_back( globalHitVec[hot.layer][hot.index] );
      }
    }
    return hitsVec;
  }

  void addHitIdx(int hitIdx, int hitLyr, float chi2)
  {
    if (lastHitIdx_ < Config::nMaxTrkHits - 1)
    {
      hitsOnTrk_[++lastHitIdx_] = { hitIdx, hitLyr };
      if (hitIdx >= 0) { ++nFoundHits_; chi2_+=chi2; }
    }
    else
    {
      // printf("WARNING Track::addHitIdx hit-on-track limit reached for label=%d\n", label_);
      // print("Track", -1, *this, true);

      if (hitIdx >= 0)
      {
        ++nFoundHits_;
        chi2_ += chi2;
        hitsOnTrk_[lastHitIdx_] = { hitIdx, hitLyr };
      }
      else if (hitIdx == -2)
      {
        hitsOnTrk_[lastHitIdx_] = { hitIdx, hitLyr };
      }

    }
  }

  void addHitIdx(const HitOnTrack &hot, float chi2)
  {
    addHitIdx(hot.index, hot.layer, chi2);
  }

  HitOnTrack getHitOnTrack(int posHitIdx) const { return hitsOnTrk_[posHitIdx]; }

   int getHitIdx(int posHitIdx) const { return hitsOnTrk_[posHitIdx].index; }
   int getHitLyr(int posHitIdx) const { return hitsOnTrk_[posHitIdx].layer; }

   HitOnTrack getLastHitOnTrack() const { return hitsOnTrk_[lastHitIdx_]; }
   int        getLastHitIdx()     const { return hitsOnTrk_[lastHitIdx_].index;  }
   int        getLastHitLyr()     const { return hitsOnTrk_[lastHitIdx_].layer;  }

  int getLastFoundHitPos() const
  {
    int hi = lastHitIdx_;
    while (hitsOnTrk_[hi].index < 0) --hi;
    return hi;
  }

  HitOnTrack getLastFoundHitOnTrack() const { return hitsOnTrk_[getLastFoundHitPos()]; }
  int        getLastFoundHitIdx()     const { return hitsOnTrk_[getLastFoundHitPos()].index; }
  int        getLastFoundHitLyr()     const { return hitsOnTrk_[getLastFoundHitPos()].layer; }

  const HitOnTrack* getHitsOnTrackArray() const { return hitsOnTrk_; }
  const HitOnTrack* BeginHitsOnTrack()    const { return hitsOnTrk_; }
  const HitOnTrack* EndHitsOnTrack()      const { return & hitsOnTrk_[lastHitIdx_ + 1]; }

  HitOnTrack* BeginHitsOnTrack_nc() { return hitsOnTrk_; }

  void sortHitsByLayer();

  void fillEmptyLayers() {
    for (int h = lastHitIdx_ + 1; h < Config::nMaxTrkHits; h++) {
      setHitIdxLyr(h, -1, -1);
    }
  }
  
  void setHitIdx(int posHitIdx, int newIdx) {
    hitsOnTrk_[posHitIdx].index = newIdx;
  }
  
  void setHitIdxLyr(int posHitIdx, int newIdx, int newLyr) {
    hitsOnTrk_[posHitIdx] = { newIdx, newLyr };
  }

  void setNFoundHits() {
    nFoundHits_=0;
    for (int i = 0; i <= lastHitIdx_; i++) {
      if (hitsOnTrk_[i].index >= 0) nFoundHits_++;
    }
  }
  
  void setNFoundHits(int nHits) { nFoundHits_ = nHits; }
  void setNTotalHits(int nHits) { lastHitIdx_ = nHits - 1; }

  void resetHits() { lastHitIdx_ = -1; nFoundHits_ =  0; }

  int  nFoundHits() const { return nFoundHits_; }
  int  nTotalHits() const { return lastHitIdx_+1; }

  int nStoredFoundHits() const
  {
    int n = 0;
    for (int i = 0; i <= lastHitIdx_; ++i)
      if (hitsOnTrk_[i].index >= 0) ++n;
    return n;
  }

  int nUniqueLayers() const
  {
    int lyr_cnt  =  0;
    int prev_lyr = -1;
    for (int ihit = 0; ihit <= lastHitIdx_ ; ++ihit)
    {
      int h_lyr = hitsOnTrk_[ihit].layer;
      if (h_lyr >= 0 && hitsOnTrk_[ihit].index >= 0 && h_lyr != prev_lyr)
      {
        ++lyr_cnt;
        prev_lyr = h_lyr;
      }
    }
    return lyr_cnt;
  }

  const std::vector<int> foundLayers() const
  {
    std::vector<int> layers;
    for (int ihit = 0; ihit <= lastHitIdx_; ++ihit) {
      if (hitsOnTrk_[ihit].index >= 0) {
        layers.push_back( hitsOnTrk_[ihit].layer );
      }
    }
    return layers;
  }

   void setCharge(int chg)  { state_.charge = chg; }
   void setChi2(float chi2) { chi2_ = chi2; }
   void setLabel(int lbl)   { label_ = lbl; }

   void setState(const TrackState& newState) { state_ = newState; }

   Track clone() const { return Track(state_,chi2_,label_,nTotalHits(),hitsOnTrk_); }

private:

  TrackState    state_;
  float         chi2_       =  0.;
  short int     lastHitIdx_ = -1;
  short int     nFoundHits_ =  0;
  //Status        status_;
  int           label_      = -1;
  HitOnTrack    hitsOnTrk_[Config::nMaxTrkHits];
};

inline bool sortByHitsChi2(const Track & cand1, const Track & cand2)
{
  if (cand1.nFoundHits()==cand2.nFoundHits()) return cand1.chi2()<cand2.chi2();
  return cand1.nFoundHits()>cand2.nFoundHits();
}

void print(const TrackState& s);
void print(std::string label, int itrack, const Track& trk, bool print_hits=false);
void print(std::string label, const TrackState& s);

#endif
