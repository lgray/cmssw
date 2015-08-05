#include "DataFormats/PatCandidates/interface/VIDCutFlowResult.h"
#include "FWCore/Utilities/interface/Exception.h"

namespace {
  static const std::string empty_str("");
  static const unsigned char empty_hash[MD5_DIGEST_LENGTH] = { 0 };
}

namespace vid {

  CutFlowResult::CutFlowResult(const std::string& name,
                               const unsigned char hash[MD5_DIGEST_LENGTH],
                               const std::map<std::string,unsigned char>& n2idx,
                               const std::vector<double>& values,
                               unsigned bitmap,
                               unsigned mask) : 
    bitmap_(bitmap), 
    mask_(mask),
    values_(values),
    name_(name) {

    memcpy(hash_,hash,MD5_DIGEST_LENGTH*sizeof(unsigned char));

    for( const auto& val : n2idx ) {
      names_.push_back(val.first);
      indices_.push_back(val.second);
    }
  }

  const std::string CutFlowResult::cutFlowHash() const {
    char buf[2*MD5_DIGEST_LENGTH+1]; // one byte = two hex numbers
    for( unsigned i=0; i<MD5_DIGEST_LENGTH; ++i ){
      sprintf(buf+2*i, "%02x", hash_[i]);   
    }
    return std::string(buf);
  }

  const std::string& CutFlowResult::getNameAtIndex(const unsigned idx) const {
    unsigned internal_idx = 0;
    for( const auto& value : indices_ ) {
      if( value == idx ) return names_[internal_idx];
      ++internal_idx;
    }
    throw cms::Exception("IndexNotFound")
      << "index = " << idx << " has no corresponding cut name!";
    return empty_str;
  }

  bool CutFlowResult::getCutResultByIndex(const unsigned idx) const {
    if( idx >= indices_.size() ) {
        throw cms::Exception("OutOfBounds")
          << idx << " is out of bounds for this cut flow!";
    }
    return getCutBit(idx);
  }

  bool CutFlowResult::getCutResultByName(const std::string& name) const {
    auto found_name = std::lower_bound(names_.begin(),names_.end(),name);
    if( found_name == names_.end() || *found_name != name ) {
      throw cms::Exception("UnknownName")
        << "Cut name: " << name 
        << " is not known for this cutflow!";
    }
    return getCutBit(indices_[std::distance(names_.begin(),found_name)]);
  }

  bool CutFlowResult::isCutMasked(const unsigned idx) const {
    if( idx >= indices_.size() ) {
        throw cms::Exception("OutOfBounds")
          << idx << " is out of bounds for this cut flow!";
    }
    return getMaskBit(idx);
  }

  bool CutFlowResult::isCutMasked(const std::string& name) const {
    auto found_name = std::lower_bound(names_.begin(),names_.end(),name);
    if( found_name == names_.end() || *found_name != name ) {
      throw cms::Exception("UnknownName")
        << "Cut name: " << name 
        << " is not known for this cutflow!";
    }
    return getMaskBit(indices_[std::distance(names_.begin(),found_name)]);
  }

  double CutFlowResult::getValueCutUpon(const unsigned idx) const {
    if( idx >= indices_.size() ) {
        throw cms::Exception("OutOfBounds")
          << idx << " is out of bounds for this cut flow!";
    }
    return getCutValue(idx);    
  }
  
  double CutFlowResult::getValueCutUpon(const std::string& name) const {
    auto found_name = std::lower_bound(names_.begin(),names_.end(),name);
    if( found_name == names_.end() || *found_name != name ) {
      throw cms::Exception("UnknownName")
        << "Cut name: " << name 
        << " is not known for this cutflow!";
    }
    return getCutValue(indices_[std::distance(names_.begin(),found_name)]);
  }

  CutFlowResult CutFlowResult::
  getCutFlowResultMasking(const std::vector<unsigned>& idxs) const {
    unsigned bitmap = bitmap_;
    unsigned mask = mask_;
    for( const unsigned idx : idxs ) {
      if( idx >= indices_.size() ) {
        throw cms::Exception("OutOfBounds")
          << idx << " is out of bounds for this cut flow!";
      }
      mask = mask | 1 << idx;
    }
    bitmap = bitmap | mask;
    return CutFlowResult(name_,empty_hash,names_,indices_,values_,bitmap,mask);
  }

  CutFlowResult CutFlowResult::
  getCutFlowResultMasking(const std::vector<std::string>& names) const {
    unsigned bitmap = bitmap_;
    unsigned mask = mask_;
    for( const std::string& name : names ) {
      auto found_name = std::lower_bound(names_.begin(),names_.end(),name);
      if( found_name == names_.end() || *found_name != name ) {
        throw cms::Exception("UnknownName")
          << "Cut name: " << name 
          << " is not known for this cutflow!";
      }
      mask = mask | 1 << indices_[std::distance(names_.begin(),found_name)];
    }
    bitmap = bitmap | mask;
    return CutFlowResult(name_,empty_hash,names_,indices_,values_,bitmap,mask);
  }

  CutFlowResult CutFlowResult::
  getCutFlowResultMasking(const unsigned idx) const {
    unsigned bitmap = bitmap_;
    unsigned mask = mask_;
    if( idx >= indices_.size() ) {
      throw cms::Exception("OutOfBounds")
        << idx << " is out of bounds for this cut flow!";
    }
    mask = mask | 1 << idx;
    bitmap = bitmap | mask;
    return CutFlowResult(name_,empty_hash,names_,indices_,values_,bitmap,mask);
  }
  
  CutFlowResult CutFlowResult::
  getCutFlowResultMasking(const std::string& name) const {
    unsigned bitmap = bitmap_;
    unsigned mask = mask_;
    auto found_name = std::lower_bound(names_.begin(),names_.end(),name);
    if( found_name == names_.end() || *found_name != name ) {
      throw cms::Exception("UnknownName")
        << "Cut name: " << name 
        << " is not known for this cutflow!";
    }
    mask = mask | 1 << indices_[std::distance(names_.begin(),found_name)];
    bitmap = bitmap | mask;
    return CutFlowResult(name_,empty_hash,names_,indices_,values_,bitmap,mask);
  }
}
