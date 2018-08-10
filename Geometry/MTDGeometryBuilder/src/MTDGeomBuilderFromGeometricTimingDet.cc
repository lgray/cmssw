#include "Geometry/MTDGeometryBuilder/interface/MTDGeomBuilderFromGeometricTimingDet.h"
#include "Geometry/MTDGeometryBuilder/interface/MTDGeometry.h"
#include "Geometry/MTDGeometryBuilder/interface/PlaneBuilderForGluedDet.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/CommonDetUnit/interface/GluedGeomDet.h"
#include "Geometry/MTDGeometryBuilder/interface/StackGeomDet.h"
#include "Geometry/MTDGeometryBuilder/interface/PixelGeomDetType.h"
#include "Geometry/MTDGeometryBuilder/interface/PixelGeomDetUnit.h"
#include "Geometry/MTDGeometryBuilder/interface/StripGeomDetType.h"
#include "Geometry/MTDGeometryBuilder/interface/StripGeomDetUnit.h"
#include "Geometry/MTDGeometryBuilder/interface/PixelTopologyBuilder.h"
#include "Geometry/MTDGeometryBuilder/interface/StripTopologyBuilder.h"
#include "CondFormats/GeometryObjects/interface/PTrackerParameters.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "CondFormats/GeometryObjects/interface/PMTDParameters.h"
#include "Geometry/MTDNumberingBuilder/interface/MTDTopology.h"
#include "DataFormats/GeometrySurface/interface/MediumProperties.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"

#include <cfloat>
#include <cassert>
using std::vector;
using std::string;

namespace {
  void verifyDUinTG(MTDGeometry const & tg) {
    int off=0; int end=0;
    for ( int i=1; i!=2; i++) {
      auto det = i - 1;
      off = tg.offsetDU(det);
      end = tg.endsetDU(det); assert(end>=off); // allow empty subdetectors. Needed for upgrade
      for (int j=off; j!=end; ++j) {
	assert(tg.detUnits()[j]->geographicalId().subdetId()==i);
	assert(tg.detUnits()[j]->index()==j);
      }
    }
  }
}

MTDGeometry*
MTDGeomBuilderFromGeometricTimingDet::build( const GeometricTimingDet* gd, const PMTDParameters& ptp, const MTDTopology* tTopo )
{  
  thePixelDetTypeMap.clear();
  theStripDetTypeMap.clear();
   
  MTDGeometry* tracker = new MTDGeometry(gd);
  std::vector<const GeometricTimingDet*> comp;
  gd->deepComponents(comp);
 
  std::cout << "MTDGeomBuidlerFromGeometricTimingDet: " << comp.size() << std::endl;

  if(tTopo)  theTopo = tTopo;

  //define a vector which associate to the detid subdetector index -1 (from 0 to 5) the GeometridDet enumerator to be able to know which type of subdetector it is
  
  std::vector<GeometricTimingDet::GTDEnumType> gdsubdetmap(2,GeometricTimingDet::unknown); // hardcoded "2" should not be a surprise... 
  GeometricTimingDet::ConstGeometricTimingDetContainer subdetgd = gd->components();
  
  //LogDebug("SubDetectorGeometricTimingDetType") 
  std::cout << "MTDGeomBuidlerFromGeometricTimingDet: " << "GeometricTimingDet enumerator values of the subdetectors" << std::endl;
  for(unsigned int i=0;i<subdetgd.size();++i) {
    MTDDetId mtdid(subdetgd[i]->geographicalId());
    assert(mtdid.mtdSubDetector()>0 && mtdid.mtdSubDetector()<3);
    gdsubdetmap[mtdid.mtdSubDetector()-1]= subdetgd[i]->type();
    //LogTrace("SubDetectorGeometricTimingDetType") 
    std::cout << "SubDetectorGeometricTimingDetType " << "subdet " << i 
	      << " type " << subdetgd[i]->type()
	      << " detid " << std::hex <<  subdetgd[i]->geographicalId().rawId() << std::dec
	      << " subdetid " <<  subdetgd[i]->geographicalId().subdetId() << std::endl;
  }

  std::cout << "MTDGeomBuidlerFromGeometricTimingDet: " << "GeometricTimingDet done building MTDGeometry" << std::endl;
  std::cout << "MTDGeomBuidlerFromGeometricTimingDet: " << "Comp size: " << comp.size() << std::endl;
  
  std::vector<const GeometricTimingDet*> dets[2];
  std::vector<const GeometricTimingDet*> & btl = dets[0]; btl.reserve(comp.size());
  std::vector<const GeometricTimingDet*> & etl = dets[1]; etl.reserve(comp.size());
 
  for(auto & i : comp) {
    MTDDetId mtdid(i->geographicalId());
    dets[mtdid.mtdSubDetector()-1].emplace_back(i);
  }
  
  std::cout << "MTDGeomBuidlerFromGeometricTimingDet: " << "GeometricTimingDet done building dets list" << std::endl;

  //loop on all the six elements of dets and firstly check if they are from pixel-like detector and call buildPixel, then loop again and check if they are strip and call buildSilicon. "unknown" can be filled either way but the vector of GeometricTimingDet must be empty !!
  // this order is VERY IMPORTANT!!!!! For the moment I (AndreaV) understand that some pieces of code rely on pixel-like being before strip-like 
  
  // now building the Pixel-like subdetectors
  for(unsigned int i=0;i<2;++i) {
    if(gdsubdetmap[i] == GeometricTimingDet::BTL) 
      buildPixel(dets[i],tracker,
		 GeomDetEnumerators::SubDetector::TimingBarrel,
		 ptp); 
    if(gdsubdetmap[i] == GeometricTimingDet::ETL) 
      buildPixel(dets[i],tracker,
		 GeomDetEnumerators::SubDetector::TimingEndcap,
		 ptp);    
  }  

  std::cout << "MTDGeomBuidlerFromGeometricTimingDet: " << "GeometricTimingDet done building pixels" << std::endl;

  // and finally the "empty" subdetectors (maybe it is not needed)
  for(unsigned int i=0;i<2;++i) {
    if(gdsubdetmap[i] == GeometricTimingDet::unknown) {
      if(!dets[i].empty()) throw cms::Exception("NotEmptyUnknownSubDet") << "Subdetector " << i+1 << " is unknown but it is not empty: " << dets[i].size();
      buildSilicon(dets[i],tracker,GeomDetEnumerators::tkDetEnum[i+1], "barrel"); // "barrel" is used but it is irrelevant
    }
  }

  std::cout << "MTDGeomBuidlerFromGeometricTimingDet: " << "GeometricTimingDet done building silicon" << std::endl;

  buildGeomDet(tracker);//"GeomDet"

  std::cout << "MTDGeomBuidlerFromGeometricTimingDet: " << "GeometricTimingDet done building GeomDet" << std::endl;

  verifyDUinTG(*tracker);

  std::cout << "MTDGeomBuidlerFromGeometricTimingDet: " << "GeometricTimingDet done verifying" << std::endl;


  return tracker;
}

void MTDGeomBuilderFromGeometricTimingDet::buildPixel(std::vector<const GeometricTimingDet*>  const & gdv, 
						      MTDGeometry* tracker,
						      GeomDetType::SubDetector det,
						      const PMTDParameters& ptp) // in y direction, cols. BIG_PIX_PER_ROC_Y = 0 for SLHC
{
  //LogDebug("BuildingGeomDetUnits") 
  std::cout << "BuildingGeomDetUnits: " 
	    << " Pixel type. Size of vector: " << gdv.size() 
	    << " GeomDetType subdetector: " << det 
	    << " logical subdetector: " << GeomDetEnumerators::subDetGeom[det]
	    << " big pix per ROC x: " << 0<< " y: " << 0
	    << " is upgrade: " << true << std::endl;
  
  // this is a hack while we put things into the DDD
  int ROCrows(0),ROCcols(0),ROCSx(0),ROCSy(0);
  switch(det) {
  case GeomDetType::SubDetector::TimingBarrel:
    ROCrows = ptp.vitems[0].vpars[8];
    ROCcols = ptp.vitems[0].vpars[9];
    ROCSx   = ptp.vitems[0].vpars[10];
    ROCSy   = ptp.vitems[0].vpars[11];
    break;
  case GeomDetType::SubDetector::TimingEndcap:
    ROCrows = ptp.vitems[1].vpars[8];
    ROCcols = ptp.vitems[1].vpars[9];
    ROCSx   = ptp.vitems[1].vpars[10];
    ROCSy   = ptp.vitems[1].vpars[11];
    break;
    break;
  default:
    throw cms::Exception("UnknownDet") 
      << "MTDGeomBuilderFromGeometricTimingDet got a weird detector: " << det;
  }
  
  switch(det) {
  case GeomDetEnumerators::TimingBarrel:
    tracker->setOffsetDU(0);
    break;
  case GeomDetEnumerators::TimingEndcap:
    tracker->setOffsetDU(1);
    break;
  default:
    throw cms::Exception("MTDGeomBuilderFromGeometricTimingDet") << det << " is not a timing detector!";
  }

  for(auto i : gdv){

    std::string const & detName = i->name().fullname();
    if (thePixelDetTypeMap.find(detName) == thePixelDetTypeMap.end()) {
      std::unique_ptr<const Bounds> bounds(i->bounds());
      
      PixelTopology* t = 
	  PixelTopologyBuilder().build(&*bounds,
				       true,
				       ROCrows,
				       ROCcols,
				       0,0, // these are BIG_PIX_XXXXX
				       ROCSx, ROCSy);
      
      thePixelDetTypeMap[detName] = new PixelGeomDetType(t,detName,det);
      tracker->addType(thePixelDetTypeMap[detName]);
    }

    PlaneBuilderFromGeometricTimingDet::ResultType plane = buildPlaneWithMaterial(i);
    GeomDetUnit* temp =  new PixelGeomDetUnit(&(*plane),thePixelDetTypeMap[detName],i->geographicalID());

    tracker->addDetUnit(temp);
    tracker->addDetUnitId(i->geographicalID());
  }
  switch(det) {
  case GeomDetEnumerators::TimingBarrel:
    tracker->setEndsetDU(0);
    break;
  case GeomDetEnumerators::TimingEndcap:
    tracker->setEndsetDU(1);
    break;
  default:
    throw cms::Exception("MTDGeomBuilderFromGeometricTimingDet") << det << " is not a timing detector!";
  }
}

void MTDGeomBuilderFromGeometricTimingDet::buildSilicon(std::vector<const GeometricTimingDet*>  const & gdv, 
						      MTDGeometry* tracker,
						      GeomDetType::SubDetector det,
						      const std::string& part)
{ 
  LogDebug("BuildingGeomDetUnits") 
    << " Strip type. Size of vector: " << gdv.size() 
    << " GeomDetType subdetector: " << det 
    << " logical subdetector: " << GeomDetEnumerators::subDetGeom[det]
    << " part " << part;
  
  switch(det) {
  case GeomDetEnumerators::TimingBarrel:
    tracker->setOffsetDU(0);
    break;
  case GeomDetEnumerators::TimingEndcap:
    tracker->setOffsetDU(0);
    break;
  default:
    throw cms::Exception("MTDGeomBuilderFromGeometricTimingDet") << det << " is not a timing detector!";
  }

  for(auto i : gdv){

    std::string const & detName = i->name().fullname();
    if (theStripDetTypeMap.find(detName) == theStripDetTypeMap.end()) {
       std::unique_ptr<const Bounds> bounds(i->bounds());
       StripTopology* t =
	   StripTopologyBuilder().build(&*bounds,
				       i->siliconAPVNum(),
				       part);
      theStripDetTypeMap[detName] = new  StripGeomDetType( t,detName,det,
						   i->stereo());
      tracker->addType(theStripDetTypeMap[detName]);
    }
     
    const double scale  = 1.0; //(theTopo->partnerDetId(i->geographicalID())) ? 0.5 : 1.0 ;	

    PlaneBuilderFromGeometricTimingDet::ResultType plane = buildPlaneWithMaterial(i,scale);  
    GeomDetUnit* temp = new StripGeomDetUnit(&(*plane), theStripDetTypeMap[detName],i->geographicalID());
    
    tracker->addDetUnit(temp);
    tracker->addDetUnitId(i->geographicalID());
  }  
  tracker->setEndsetDU(GeomDetEnumerators::subDetGeom[det]);

}


void MTDGeomBuilderFromGeometricTimingDet::buildGeomDet(MTDGeometry* tracker){

  //PlaneBuilderForGluedDet gluedplaneBuilder;
  auto const & gdu = tracker->detUnits();
  auto const & gduId = tracker->detUnitIds();

  for(u_int32_t i=0;i<gdu.size();i++){

    tracker->addDet(gdu[i]);
    tracker->addDetId(gduId[i]);
    string gduTypeName = gdu[i]->type().name();

    /*
    //this step is time consuming >> TO FIX with a MAP?
    if( (gduTypeName.find("Ster")!=std::string::npos || 
         gduTypeName.find("Lower")!=std::string::npos) && 
        (theTopo->glued(gduId[i])!=0 || theTopo->stack(gduId[i])!=0 )) {
	
      int partner_pos=-1;
      for(u_int32_t jj=0;jj<gduId.size();jj++){
  	  if(theTopo->partnerDetId(gduId[i]) == gduId[jj]) {
  	    partner_pos=jj;
  	    break;
  	  }
      }
      if(partner_pos==-1){
	  throw cms::Exception("Configuration") <<"Module Type is Stereo or Lower but no partner detector found \n"
					        <<"There is a problem on Tracker geometry configuration\n";
      }
      
      const GeomDetUnit* dus = gdu[i];
      const GeomDetUnit* dum = gdu[partner_pos];
      std::vector<const GeomDetUnit *> composed(2);
      composed[0]=dum;
      composed[1]=dus;
      DetId composedDetId;
      if(gduTypeName.find("Ster")!=std::string::npos){

        PlaneBuilderForGluedDet::ResultType plane = gluedplaneBuilder.plane(composed);
        composedDetId = theTopo->glued(gduId[i]);
        GluedGeomDet* gluedDet = new GluedGeomDet(&(*plane),dum,dus,composedDetId);
        tracker->addDet((GeomDet*) gluedDet);
        tracker->addDetId(composedDetId);

      } else if (gduTypeName.find("Lower")!=std::string::npos){

        //FIXME::ERICA: the plane builder is built in the middle...
        PlaneBuilderForGluedDet::ResultType plane = gluedplaneBuilder.plane(composed);
        composedDetId = theTopo->stack(gduId[i]);
        StackGeomDet* stackDet = new StackGeomDet(&(*plane),dum,dus,composedDetId);
        tracker->addDet((GeomDet*) stackDet);
        tracker->addDetId(composedDetId);

      } 

    }
  */
  }
}

PlaneBuilderFromGeometricTimingDet::ResultType
MTDGeomBuilderFromGeometricTimingDet::buildPlaneWithMaterial(const GeometricTimingDet* gd,
							   double scale) const
{
  PlaneBuilderFromGeometricTimingDet planeBuilder;
  PlaneBuilderFromGeometricTimingDet::ResultType plane = planeBuilder.plane(gd);  
  //
  // set medium properties (if defined)
  //
  plane->setMediumProperties(MediumProperties(gd->radLength()*scale,gd->xi()*scale));

  return plane;
}
