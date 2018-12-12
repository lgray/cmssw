// Modify the pixel packing to make 100micron pixels possible. d.k. 2/02
//
#include "DataFormats/ForwardDetId/interface/MTDChannelIdentifier.h"

// Initialization of static data members - DEFINES DIGI PACKING !
const MTDChannelIdentifier::Packing MTDChannelIdentifier::thePacking( 6, 5, 12, 9); // row, col, time, adc
