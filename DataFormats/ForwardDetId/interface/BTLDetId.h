#ifndef DataFormats_BTLDetId_BTLDetId_h
#define DataFormats_BTLDetId_BTLDetId_h

#include "DataFormats/ForwardDetId/interface/MTDDetId.h"
#include <ostream>

/** 
    @class BTLDetId
    @brief Detector identifier class for the Barrel Timing Layer.
*/

class BTLDetId : public MTDDetId {
  
 private:
  
  static const uint32_t kBTLmoduleOffset           = 8;
  static const uint32_t kBTLmoduleMask             = 0x7F;
  static const uint32_t kBTLmodTypeOffset          = 6;
  static const uint32_t kBTLmodTypeMask            = 0x3;
  static const uint32_t kBTLCrystalOffset          = 0;
  static const uint32_t kBTLCrystalMask            = 0x3F;
  
 public:
  
  // ---------- Constructors, enumerated types ----------
  
  /** Construct a null id */
 BTLDetId()  : MTDDetId() {;}
  
  /** Construct from a raw value */
 BTLDetId( const uint32_t& raw_id ) : MTDDetId( raw_id ) {;}
  
  /** Construct from generic DetId */
 BTLDetId( const DetId& det_id )  : MTDDetId( det_id.rawId() ) {;}
  
  /** Construct and fill only the det and sub-det fields. */
 BTLDetId( uint32_t zside, 
           uint32_t rod, 
           uint32_t module, 
           uint32_t crytyp, 
           uint32_t crystal ) : MTDDetId( DetId::Forward, ForwardSubdetector::FastTime ) {
    id_ |= ( MTDType::BTL& kMTDtypeMask ) << kMTDtypeOffset |
      ( zside& kZsideMask ) << kZsideOffset |
      ( rod& kRodRingMask ) << kRodRingOffset |
      ( module& kBTLmoduleMask ) << kBTLmoduleOffset |
      ( crytyp& kBTLmodTypeMask ) << kBTLmodTypeOffset |
      ( crystal& kBTLCrystalMask ) << kBTLCrystalOffset ; 
}

// ---------- Common methods ----------

/** Returns BTL module number. */
inline int btlModule() const { return (id_>>kBTLmoduleOffset)&kBTLmoduleMask; }

/** Returns BTL crystal type number. */
inline int btlmodType() const { return (id_>>kBTLmodTypeOffset)&kBTLmodTypeMask; }

/** Returns BTL crystal number. */
inline int btlCrystal() const { return (id_>>kBTLCrystalOffset)&kBTLCrystalMask; }

};

std::ostream& operator<< ( std::ostream&, const BTLDetId& );

#endif // DataFormats_BTLDetId_BTLDetId_h

