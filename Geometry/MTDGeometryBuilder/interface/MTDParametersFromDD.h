#ifndef MTDGeometryBuilder_MTDParametersFromDD_h
#define MTDGeometryBuilder_MTDParametersFromDD_h

#include <vector>

class DDCompactView;
class PMTDParameters;

class MTDParametersFromDD {
 public:
  MTDParametersFromDD() {}
  virtual ~MTDParametersFromDD() {}

  bool build( const DDCompactView*,
	      PMTDParameters& );
 private:
  void putOne( int, std::vector<int> &, PMTDParameters& );
};

#endif
