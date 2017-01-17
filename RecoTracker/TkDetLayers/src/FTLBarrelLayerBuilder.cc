#include "FTLBarrelLayerBuilder.h"
#include "FTLBarrelSectorBuilder.h"

using namespace std;
using namespace edm;

FTLBarrelLayer* FTLBarrelLayerBuilder::build(const GeometricDet* aFTLBarrelLayer,
					     const TrackerGeometry* theGeomDetGeometry)
{
  // This builder is very similar to TOBLayer one. Most of the code should be put in a 
  // common place.

  LogTrace("TkDetLayers") << "FTLBarrelLayerBuilder::build";
  vector<const GeometricDet*>  theGeometricDets = aFTLBarrelLayer->components();
  LogDebug("TkDetLayers") << "FTLBarrelLayerBuilder with #Components: " << theGeometricDets.size() << std::endl;
  vector<const GeometricDet*>  theGeometricDetSectors;
  vector<const GeometricDet*>  theGeometricDetRings;

  for(vector<const GeometricDet*>::const_iterator it = theGeometricDets.begin();
      it!=theGeometricDets.end(); it++){

    if( (*it)->type() == GeometricDet::ladder) {
      theGeometricDetSectors.push_back(*it);
    } else if( (*it)->type() == GeometricDet::panel) {
      theGeometricDetRings.push_back(*it);
    } else {
      LogDebug("TkDetLayers") << "FTLBarrelLayerBuilder with no Sectors and no Rings! ";
    }
  }

  LogDebug("TkDetLayers") << "FTLBarrelLayerBuilder with #Sectors: " << theGeometricDetSectors.size() << std::endl;

  FTLBarrelSectorBuilder myFTLBarrelSectorBuilder;

  vector<const FTLBarrelSector*> theInnerSectors;
  vector<const FTLBarrelSector*> theOuterSectors;

  // properly calculate the meanR value to separate rod in inner/outer.

  double meanR = 0;
  for (unsigned int index=0; index!=theGeometricDetSectors.size(); index++)   meanR+=theGeometricDetSectors[index]->positionBounds().perp();
  if (theGeometricDetSectors.size()!=0)
    meanR/=(double) theGeometricDetSectors.size();
  
  for(unsigned int index=0; index!=theGeometricDetSectors.size(); index++){    
    if(theGeometricDetSectors[index]->positionBounds().perp() < meanR)
      theInnerSectors.push_back(myFTLBarrelSectorBuilder.build(theGeometricDetSectors[index],
								  theGeomDetGeometry)    );       

    if(theGeometricDetSectors[index]->positionBounds().perp() > meanR)
      theOuterSectors.push_back(myFTLBarrelSectorBuilder.build(theGeometricDetSectors[index],
								  theGeomDetGeometry)    );       

  }
  
  return new FTLBarrelLayer(theInnerSectors,theOuterSectors);
}

