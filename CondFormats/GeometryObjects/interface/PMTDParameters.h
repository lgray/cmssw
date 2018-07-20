#ifndef CondFormats_GeometryObjects_PMTDParameters_h
#define CondFormats_GeometryObjects_PMTDParameters_h

#include "CondFormats/Serialization/interface/Serializable.h"

class PMTDParameters
{
 public:
  PMTDParameters( void ) { } 
  ~PMTDParameters( void ) { }

  struct Item
  {
    int id;
    std::vector<int> vpars;
    
    COND_SERIALIZABLE;
  };

  std::vector<Item> vitems;
  std::vector<int> vpars;
  
  COND_SERIALIZABLE;
};

#endif
