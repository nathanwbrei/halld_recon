#DYLD_LIBRARY_PATH=/Users/nbrei/gluex_top/evio/4.4.6/Darwin-x86_64/lib


cmake_minimum_required(VERSION 3.15)
project(halld_recon)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(ROOT REQUIRED COMPONENTS Geom)
find_package(JANA REQUIRED)

# Only add ET if ETROOT is set
#etroot = os.getenv('ETROOT')
#if(etroot != None) :

find_package(evio)
if (EVIO_FOUND)
    list(APPEND CMAKE_CXX_FLAGS -DHAVE_EVIO=1)
    find_package(expat REQUIRED)
endif()

set(HDDS_INCLUDE_DIR /Users/nbrei/src/hdds/build/src)
set(HDDS_LIBRARY /Users/nbrei/src/hdds/build/lib/libhdds.so)

file(GLOB DAQ_SOURCES ${PROJECT_SOURCE_DIR}/src/libraries/DAQ/*.cc)
file(GLOB DAQ_HEADERS ${PROJECT_SOURCE_DIR}/src/libraries/DAQ/*.h)

set(HALLD_RECON_SOURCES

        src/libraries/BCAL/DBCALCluster.h
        src/libraries/BCAL/DBCALCluster.cc
        src/libraries/BCAL/DBCALTDCHit.h
        src/libraries/BCAL/DBCALTDCDigiHit.h
        src/libraries/BCAL/DBCALGeometry.h
        src/libraries/BCAL/DBCALGeometry.cc     # Uses japp
        src/libraries/BCAL/DBCALIncidentParticle.h
        src/libraries/BCAL/DBCALSiPMSpectrum.h
        src/libraries/BCAL/DBCALUnifiedHit.h
        src/libraries/BCAL/DBCALTruthCell.h
        src/libraries/BCAL/DBCALClump.h
        src/libraries/BCAL/DBCALClump.cc
        src/libraries/BCAL/DBCALPoint.h
        src/libraries/BCAL/DBCALPoint.cc
        src/libraries/BCAL/DBCALSiPMHit.h
        src/libraries/BCAL/DBCALDigiHit.h
        src/libraries/BCAL/DBCALShower.h
        src/libraries/BCAL/DBCALHit.h
        src/libraries/BCAL/DBCALTruthShower.h
        src/libraries/BCAL/DBCALTDCHit_factory.h
        src/libraries/BCAL/DBCALTDCHit_factory.cc
        src/libraries/BCAL/DBCALUnifiedHit_factory.h
        src/libraries/BCAL/DBCALUnifiedHit_factory.cc
        src/libraries/BCAL/DBCALShower_factory.h
        src/libraries/BCAL/DBCALShower_factory_KLOE.h
        src/libraries/BCAL/DBCALShower_factory_KLOE.cc
        src/libraries/BCAL/DBCALShower_factory_JLAB.h
        src/libraries/BCAL/DBCALShower_factory_JLAB.cc
#        src/libraries/BCAL/DBCALShower_factory_IU.cc  # ROOT rwlock
        src/libraries/BCAL/DBCALShower_factory_IU.h
        src/libraries/BCAL/DBCALShower_factory_CURVATURE.cc
        src/libraries/BCAL/DBCALShower_factory_CURVATURE.h
        src/libraries/BCAL/DBCALClump_factory.cc
        src/libraries/BCAL/DBCALClump_factory.h
#        src/libraries/BCAL/DBCALCluster_factory.cc   # DTrackFitter
#        src/libraries/BCAL/DBCALCluster_factory.h
        src/libraries/BCAL/DBCALCluster_factory_SINGLE.cc
        src/libraries/BCAL/DBCALCluster_factory_SINGLE.h
        src/libraries/BCAL/DBCALGeometry_factory.h    # Construction logic in ctor
        src/libraries/BCAL/DBCALPoint_factory.cc
        src/libraries/BCAL/DBCALPoint_factory.h
        src/libraries/BCAL/DBCALHit_factory.cc
        src/libraries/BCAL/DBCALHit_factory.h

        src/libraries/CDC/DCDCTrackHit.h
        src/libraries/CDC/DCDCHit.h
        src/libraries/CDC/DCDCDigiHit.h

        src/libraries/CCAL/DCCALShower.h
#        src/libraries/CCAL/DCCALShower_factory.h
#        src/libraries/CCAL/DCCALShower_factory.cc
        src/libraries/CCAL/DCCALDigiHit.h
        src/libraries/CCAL/DCCALRefDigiHit.h
        src/libraries/CCAL/DCCALGeometry.h
        src/libraries/CCAL/DCCALGeometry.cc
        src/libraries/CCAL/DCCALTruthShower.h
        src/libraries/CCAL/DCCALHit.h

        src/libraries/CERE/DCereHit.h

        src/libraries/DANA/DANARootErrorHandler.cc
        src/libraries/DANA/DANARootErrorHandler.h
        src/libraries/DANA/DGeometryManager.cc
        src/libraries/DANA/DGeometryManager.h
#        src/libraries/DANA/DApplication.cc
        src/libraries/DANA/DApplication.h
#        src/libraries/DANA/DFactoryGenerator.cc
        src/libraries/DANA/DFactoryGenerator.h
        src/libraries/DANA/DObjectID.h
        src/libraries/DANA/DStatusBits.h
        src/libraries/DANA/JExceptionDataFormat.h
        src/libraries/DANA/ReadWriteLock.cc
        src/libraries/DANA/ReadWriteLock.h

#        src/libraries/DAQ/JEventSource_EVIO.cc
        src/libraries/DAQ/JEventSource_EVIO.h
#        src/libraries/DAQ/JEventSourceGenerator_EVIO.cc
        src/libraries/DAQ/JEventSourceGenerator_EVIO.h
#        src/libraries/DAQ/JEventSource_EVIO.cc

        src/libraries/DAQ/DF1TDCConfig.h
        src/libraries/DAQ/Df250EmulatorAlgorithm_v3.h  # JEventLoop
        src/libraries/DAQ/DF1TDCHit.h
        src/libraries/DAQ/Df125EmulatorAlgorithm.h
        src/libraries/DAQ/Df125Config.h
        src/libraries/DAQ/Df125PulseIntegral.h
        src/libraries/DAQ/DCAEN1290TDCConfig.h
        src/libraries/DAQ/Df250Scaler.h
        src/libraries/DAQ/Df250TriggerTime.h
        src/libraries/DAQ/Df125BORConfig.h
        src/libraries/DAQ/Df125FDCPulse.h
        src/libraries/DAQ/Df250BORConfig.h
        src/libraries/DAQ/DCAEN1290TDCHit.h
        src/libraries/DAQ/DTSscalers.h
        src/libraries/DAQ/DCODAROCInfo.h
        src/libraries/DAQ/DGEMSRSWindowRawData.h
        src/libraries/DAQ/DDIRCTDCHit.h
        src/libraries/DAQ/DCODAControlEvent.h
        src/libraries/DAQ/Df250Config.h
        src/libraries/DAQ/Df250WindowRawData.h
        src/libraries/DAQ/DF1TDCTriggerTime.h
        src/libraries/DAQ/DCODAEventInfo.h
        src/libraries/DAQ/Df125PulsePedestal.h
        src/libraries/DAQ/Df250AsyncPedestal.h
        src/libraries/DAQ/DTSGBORConfig.h
        src/libraries/DAQ/Df125PulseRawData.h
        src/libraries/DAQ/Df250StreamingRawData.h
        src/libraries/DAQ/Df125PulseTime.h
        src/libraries/DAQ/Df125CDCPulse.h
        src/libraries/DAQ/DEPICSvalue.h
        src/libraries/DAQ/Df250PulseTime.h
        src/libraries/DAQ/DCAEN1290TDCBORConfig.h
        src/libraries/DAQ/DL1Info.h
        src/libraries/DAQ/DF1TDCBORConfig.h
        src/libraries/DAQ/Df250EmulatorAlgorithm_v1.h
        src/libraries/DAQ/Df250EmulatorAlgorithm.h
        src/libraries/DAQ/Df125TriggerTime.h
        src/libraries/DAQ/DEventTag.h
        src/libraries/DAQ/Df250PulseIntegral.h
        src/libraries/DAQ/DBeamCurrent.h
        src/libraries/DAQ/DDIRCADCHit.h
        src/libraries/DAQ/Df250PulseRawData.h
        src/libraries/DAQ/DDIRCTriggerTime.h
        src/libraries/DAQ/Df250PulseData.h
        src/libraries/DAQ/DDAQConfig.h
        src/libraries/DAQ/Df250PulsePedestal.h
        src/libraries/DAQ/Df250WindowSum.h
        src/libraries/DAQ/Df250EmulatorAlgorithm_v2.h   # JEventLoop
        src/libraries/DAQ/DParsedEvent.h                # JEventLoop
        src/libraries/DAQ/Df125WindowRawData.h
        src/libraries/DAQ/DDAQAddress.h
        src/libraries/DAQ/HDEVIO.h
        src/libraries/DAQ/HDEVIO.cc

        src/libraries/DIRC/DDIRCLut.h
        src/libraries/DIRC/DDIRCLut.cc     # Init requires event
        src/libraries/DIRC/DDIRCLutReader.h
        src/libraries/DIRC/DDIRCLutReader.cc
        src/libraries/DIRC/DDIRCLEDRef.h
        src/libraries/DIRC/DDIRCTruthPmtHit.h
        src/libraries/DIRC/DDIRCTDCDigiHit.h
        src/libraries/DIRC/DDIRCPmtHit.h
        src/libraries/DIRC/DDIRCGeometry.h
        src/libraries/DIRC/DDIRCGeometry.cc
        src/libraries/DIRC/DDIRCTruthBarHit.h

        src/libraries/EVENTSTORE/DESSkimData.h

        src/libraries/FCAL/DFCALHit.h
        src/libraries/FCAL/DFCALDigiHit.h
        src/libraries/FCAL/DFCALGeometry.h
        src/libraries/FCAL/DFCALGeometry.cc
        src/libraries/FCAL/DFCALShower.h
        src/libraries/FCAL/DFCALShower.cc
        src/libraries/FCAL/DFCALTruthShower.h
        src/libraries/FCAL/DFCALCluster.h
        src/libraries/FCAL/DFCALCluster.cc

        src/libraries/FDC/DFDCCathodeDigiHit.h
        src/libraries/FDC/DFDCWireDigiHit.h
        src/libraries/FDC/DFDCSegment.h
        src/libraries/FDC/DFDCGeometry.h
        src/libraries/FDC/DFDCGeometry.cc
        src/libraries/FDC/DFDCIntersection.h
        src/libraries/FDC/DFDCHit.h
        src/libraries/FDC/DFDCCathodeCluster.h
        src/libraries/FDC/DFDCPseudo.h

        src/libraries/FMWPC/DFMWPCTruthHit.h
        src/libraries/FMWPC/DFMWPCHit.h


        src/libraries/HDGEOMETRY/DGeometry.cc
        src/libraries/HDGEOMETRY/DGeometry.h
        src/libraries/HDGEOMETRY/DLorentzDeflections.cc
        src/libraries/HDGEOMETRY/DLorentzDeflections.h
        src/libraries/HDGEOMETRY/DLorentzMapCalibDB.cc
        src/libraries/HDGEOMETRY/DLorentzMapCalibDB.h
        src/libraries/HDGEOMETRY/DMagneticFieldMap.h
        src/libraries/HDGEOMETRY/DMagneticFieldMapCalibDB.cc
        src/libraries/HDGEOMETRY/DMagneticFieldMapCalibDB.h
        src/libraries/HDGEOMETRY/DMagneticFieldMapConst.cc
        src/libraries/HDGEOMETRY/DMagneticFieldMapConst.h
        src/libraries/HDGEOMETRY/DMagneticFieldMapFineMesh.cc
        src/libraries/HDGEOMETRY/DMagneticFieldMapFineMesh.h
        src/libraries/HDGEOMETRY/DMagneticFieldMapNoField.cc
        src/libraries/HDGEOMETRY/DMagneticFieldMapNoField.h
        src/libraries/HDGEOMETRY/DMagneticFieldMapParameterized.cc
        src/libraries/HDGEOMETRY/DMagneticFieldMapParameterized.h
        src/libraries/HDGEOMETRY/DMagneticFieldMapPS.h
        src/libraries/HDGEOMETRY/DMagneticFieldMapPS2DMap.cc
        src/libraries/HDGEOMETRY/DMagneticFieldMapPS2DMap.h
        src/libraries/HDGEOMETRY/DMagneticFieldMapPSConst.cc
        src/libraries/HDGEOMETRY/DMagneticFieldMapPSConst.h
        src/libraries/HDGEOMETRY/DMagneticFieldMapSpoiled.cc
        src/libraries/HDGEOMETRY/DMagneticFieldMapSpoiled.h
        src/libraries/HDGEOMETRY/DMaterial.cc
        src/libraries/HDGEOMETRY/DMaterial.h
        src/libraries/HDGEOMETRY/DMaterialMap.cc
        src/libraries/HDGEOMETRY/DMaterialMap.h
        src/libraries/HDGEOMETRY/DRootGeom.cc
        src/libraries/HDGEOMETRY/DRootGeom.h

        src/libraries/KINFITTER/DKinFitChain.h
        src/libraries/KINFITTER/DKinFitChainStep.h
        src/libraries/KINFITTER/DKinFitConstraint.h
        src/libraries/KINFITTER/DKinFitConstraint_Mass.h
        src/libraries/KINFITTER/DKinFitConstraint_P4.h
        src/libraries/KINFITTER/DKinFitConstraint_Spacetime.h
        src/libraries/KINFITTER/DKinFitConstraint_Vertex.h
        src/libraries/KINFITTER/DKinFitParticle.h
        src/libraries/KINFITTER/DKinFitter.cc
        src/libraries/KINFITTER/DKinFitter.h
        src/libraries/KINFITTER/DKinFitUtils.cc
        src/libraries/KINFITTER/DKinFitUtils.h

        src/libraries/PAIR_SPECTROMETER/DPSDigiHit.h
        src/libraries/PAIR_SPECTROMETER/DPSGeometry.h
        src/libraries/PAIR_SPECTROMETER/DPSGeometry.cc
        src/libraries/PAIR_SPECTROMETER/DPSCDigiHit.h
        src/libraries/PAIR_SPECTROMETER/DPSCTDCDigiHit.h
        src/libraries/PAIR_SPECTROMETER/DPSHit.h
        src/libraries/PAIR_SPECTROMETER/DPSTruthHit.h
        src/libraries/PAIR_SPECTROMETER/DPSCTruthHit.h
        src/libraries/PAIR_SPECTROMETER/DPSPair.h
        src/libraries/PAIR_SPECTROMETER/DLumi.h  # Uses JEventLoop, DTAGHGeometry, DTAGMGeometry
        src/libraries/PAIR_SPECTROMETER/DPSCHit.h
        src/libraries/PAIR_SPECTROMETER/DPSCPair.h

        src/libraries/PID/DChargedTrack.h
        src/libraries/PID/DChargedTrackHypothesis.h
        src/libraries/PID/DBeamPhoton.h
        src/libraries/PID/DVertex.h
        src/libraries/PID/DKinematicData.h
        src/libraries/PID/DKinematicPools.cc  # Commented out ChargedTrackHypothesis, needs DTrackFitter
        src/libraries/PID/DMCReaction.h
        src/libraries/PID/DNeutralShower.h
        src/libraries/PID/DNeutralParticleHypothesis.h
        src/libraries/PID/DDetectorMatches.h
        src/libraries/PID/DParticleID.h
        src/libraries/PID/DParticleID.cc
        src/libraries/PID/DNeutralParticle.h
        src/libraries/PID/DEventRFBunch.h

        src/libraries/RF/DRFTDCDigiTime.h
        src/libraries/RF/DRFTime.h
        src/libraries/RF/DRFDigiTime.h

        src/libraries/START_COUNTER/DSCDigiHit.h
        src/libraries/START_COUNTER/DSCTruthHit.h
        src/libraries/START_COUNTER/DSCHit.h
        src/libraries/START_COUNTER/DSCTDCDigiHit.h
        src/libraries/START_COUNTER/DSCHit_factory.h
        src/libraries/START_COUNTER/DSCHit_factory.cc

        src/libraries/TAC/DTACHit.h
        src/libraries/TAC/DTACTDCDigiHit.h
        src/libraries/TAC/DTACDigiHit.h

        src/libraries/TAGGER/DTAGHHit.h
        src/libraries/TAGGER/DTAGHGeometry.h   # Uses JEventLoop to get calibrations
        src/libraries/TAGGER/DTAGHDigiHit.h
        src/libraries/TAGGER/DTAGMHit.h
        src/libraries/TAGGER/DTAGMDigiHit.h
        src/libraries/TAGGER/DTAGMGeometry.h   # Uses JEventLoop to get calibrations
        src/libraries/TAGGER/DTAGMTDCDigiHit.h
        src/libraries/TAGGER/DTAGHTDCDigiHit.h

        src/libraries/TOF/DTOFHit.h
        src/libraries/TOF/DTOFHitMC.h
        src/libraries/TOF/DTOFTruth.h
        src/libraries/TOF/DTOFPaddleHit.h
        src/libraries/TOF/DTOFDigiHit.h
        src/libraries/TOF/DTOFGeometry.h
        src/libraries/TOF/DTOFGeometry.cc
        src/libraries/TOF/DTOFTDCDigiHit.h
        src/libraries/TOF/DTOFPoint.h
        src/libraries/TOF/DTOFPoint_factory.cc
        src/libraries/TOF/DTOFPoint_factory.h

        src/libraries/TPOL/DTPOLTruthHit.h
        src/libraries/TPOL/DTPOLHit.h
        src/libraries/TPOL/DTPOLSectorDigiHit.h
        src/libraries/TPOL/DTPOLRingDigiHit.h

        src/libraries/TRACKING/DTrackWireBased.h
        src/libraries/TRACKING/DTrackFinder.h
        src/libraries/TRACKING/DTrackFinder.cc
        src/libraries/TRACKING/DTrackCandidate.h
        src/libraries/TRACKING/DMCTrackHit.h
        src/libraries/TRACKING/DHoughFind.h
        src/libraries/TRACKING/DMCThrown.h
        src/libraries/TRACKING/DTrackTimeBased.h
        src/libraries/TRACKING/DTrackHit.h
        src/libraries/TRACKING/DMCTrajectoryPoint.h
        src/libraries/TRACKING/DTrackFitter.h        # Put some of this in a factory?!?
        src/libraries/TRACKING/DTrackFitter.cc
        src/libraries/TRACKING/DTrackHitSelector.h
        src/libraries/TRACKING/DTrackHitSelector.cc
        src/libraries/TRACKING/DTrackWireBased_factory.cc
        src/libraries/TRACKING/DTrackWireBased_factory.h
        src/libraries/TRACKING/DTrackTimeBased_factory.cc
        src/libraries/TRACKING/DTrackTimeBased_factory.h
        src/libraries/TRACKING/DReferenceTrajectory.h
        src/libraries/TRACKING/DReferenceTrajectory.cc
        src/libraries/TRACKING/DMagneticFieldStepper.cc
        src/libraries/TRACKING/DMagneticFieldStepper.h
        src/libraries/TRACKING/grkuta.cc
        src/libraries/TRACKING/DMatrixPools.cc

        src/libraries/TRD/DGEMPoint.h
        src/libraries/TRD/DTRDStripCluster.h
        src/libraries/TRD/DGEMHit.h
        src/libraries/TRD/DTRDDigiHit.h
        src/libraries/TRD/DGEMDigiWindowRawData.h
        src/libraries/TRD/DGEMStripCluster.h
        src/libraries/TRD/DTRDHit.h
        src/libraries/TRD/DTRDPoint.h

        src/libraries/TRIGGER/DTrigger.h
        src/libraries/TRIGGER/DL3Trigger.h
        src/libraries/TRIGGER/DMCTrigger.h
        src/libraries/TRIGGER/DL1MCTrigger.h
        src/libraries/TRIGGER/DL1Trigger.h

        src/libraries/TTAB/DTranslationTable.h
        src/libraries/TTAB/DTranslationTable.cc
        src/libraries/TTAB/DTranslationTable_factory.h
        src/libraries/TTAB/DTTabUtilities.cc
        src/libraries/TTAB/DTTabUtilities.h
        src/libraries/TTAB/DTTabUtilities_factory.cc
        src/libraries/TTAB/DTTabUtilities_factory.h
        src/libraries/TTAB/TTab_init.cc

        )

add_executable(halld_recon
        src/programs/Analysis/hd_port/hd_port.cc
#        src/programs/Analysis/hd_dump/hd_dump.cc
#        src/programs/Analysis/hd_dump/MyProcessor.cc
#        src/programs/Analysis/hd_dump/MyProcessor.h
        ${HALLD_RECON_SOURCES}
        )


target_include_directories(halld_recon PRIVATE src/libraries src/libraries/include src/include
        ${CCDB_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${JANA_INCLUDE_DIR} ${EVIO_INCLUDE_DIR} ${HDDS_INCLUDE_DIR})

message(WARN ${EVIO_LIBS})
target_link_libraries(halld_recon ${JANA_LIBRARY} ${EVIO_LIBS} ${EXPAT_LIBRARY} ${ROOT_LIBRARIES} ${HDDS_LIBRARY})


