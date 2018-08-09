import FWCore.ParameterSet.Config as cms

from Geometry.MTDGeometryBuilder.mtdParametersBase_cfi import mtdParametersBase

mtdParameters = mtdParametersBase.clone()

mtdParameters.vpars = cms.vint32(4,4,4,24)
mtdParameters.vitems = cms.VPSet( 
    cms.PSet(  #BTL
        subdetPars = cms.vint32()
        ), 
    cms.PSet(  #ETL
        subdetPars = cms.vint32()
        )
    )
