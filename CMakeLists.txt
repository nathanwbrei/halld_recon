#DYLD_LIBRARY_PATH=/Users/nbrei/gluex_top/evio/4.4.6/Darwin-x86_64/lib


cmake_minimum_required(VERSION 3.15)
project(halld_recon)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(ROOT REQUIRED)
find_package(JANA REQUIRED)

# Only add ET if ETROOT is set
#etroot = os.getenv('ETROOT')
#if(etroot != None) :

find_package(evio)
if (EVIO_FOUND)
    list(APPEND CMAKE_CXX_FLAGS -DHAVE_EVIO=1)
    find_package(expat REQUIRED)
endif()


file(GLOB DAQ_SOURCES ${PROJECT_SOURCE_DIR}/src/libraries/DAQ/*.cc)
file(GLOB DAQ_HEADERS ${PROJECT_SOURCE_DIR}/src/libraries/DAQ/*.h)

set(HALLD_RECON_SOURCES

#        src/libraries/TRACKING/DMCThrown.h

#        src/libraries/CDC/DCDCTrackHit.h   # Uses TVersionCheck
        src/libraries/CDC/DCDCHit.h
        src/libraries/CDC/DCDCDigiHit.h

#        src/libraries/CCAL/DCCALShower.h    # Uses TVersionCheck
        src/libraries/CCAL/DCCALDigiHit.h
        src/libraries/CCAL/DCCALRefDigiHit.h
#        src/libraries/CCAL/DCCALGeometry.h  # Uses TVersionCheck
#        src/libraries/CCAL/DCCALTruthShower.h    # Uses oid
        src/libraries/CCAL/DCCALHit.h

        src/libraries/CERE/DCereHit.h

#        src/libraries/DAQ/JEventSource_EVIO.cc
#        src/libraries/DAQ/JEventSource_EVIO.h
#        src/libraries/DAQ/JEventSourceGenerator_EVIO.cc
#        src/libraries/DAQ/JEventSourceGenerator_EVIO.h

#        src/libraries/DIRC/DDIRCLut.h     # Needs a lot of work
        src/libraries/DIRC/DDIRCLEDRef.h
        src/libraries/DIRC/DDIRCTruthPmtHit.h
        src/libraries/DIRC/DDIRCTDCDigiHit.h
        src/libraries/DIRC/DDIRCPmtHit.h
        src/libraries/DIRC/DDIRCGeometry.h
        src/libraries/DIRC/DDIRCTruthBarHit.h

        src/libraries/FCAL/DFCALHit.h
        src/libraries/FCAL/DFCALDigiHit.h
#        src/libraries/FCAL/DFCALGeometry.h  # Uses TVersionCheck
#        src/libraries/FCAL/DFCALShower.h    # Uses oid via Cluster
#        src/libraries/FCAL/DFCALTruthShower.h  # Uses oid
#        src/libraries/FCAL/DFCALCluster.h     # Uses oid
#        src/libraries/FCAL/DFCALCluster.cc    # Uses oid

        src/libraries/FDC/DFDCCathodeDigiHit.h
        src/libraries/FDC/DFDCWireDigiHit.h
#        src/libraries/FDC/DFDCSegment.h
#        src/libraries/FDC/DFDCGeometry.h    # Uses DGeometry (with circular ref)
#        src/libraries/FDC/DFDCIntersection.h  # Pulls in TVersionCheck somehow
        src/libraries/FDC/DFDCHit.h
        src/libraries/FDC/DFDCCathodeCluster.h
#        src/libraries/FDC/DFDCPseudo.h   # Can't find cint.h

        src/libraries/FMWPC/DFMWPCTruthHit.h
        src/libraries/FMWPC/DFMWPCHit.h

        src/libraries/RF/DRFTDCDigiTime.h
        src/libraries/RF/DRFTime.h
        src/libraries/RF/DRFDigiTime.h

        src/libraries/START_COUNTER/DSCDigiHit.h
        src/libraries/START_COUNTER/DSCTruthHit.h
        src/libraries/START_COUNTER/DSCHit.h
        src/libraries/START_COUNTER/DSCTDCDigiHit.h

        src/libraries/TAC/DTACHit.h
        src/libraries/TAC/DTACTDCDigiHit.h
        src/libraries/TAC/DTACDigiHit.h

        src/libraries/TAGGER/DTAGHHit.h
        #src/libraries/TAGGER/DTAGHGeometry.h   # Uses JEventLoop to get calibrations
        src/libraries/TAGGER/DTAGHDigiHit.h
        src/libraries/TAGGER/DTAGMHit.h
        src/libraries/TAGGER/DTAGMDigiHit.h
        #src/libraries/TAGGER/DTAGMGeometry.h   # Uses JEventLoop to get calibrations
        src/libraries/TAGGER/DTAGMTDCDigiHit.h
        src/libraries/TAGGER/DTAGHTDCDigiHit.h

        src/libraries/TOF/DTOFHit.h
        src/libraries/TOF/DTOFHitMC.h
        src/libraries/TOF/DTOFTruth.h
        src/libraries/TOF/DTOFPaddleHit.h
        src/libraries/TOF/DTOFDigiHit.h
        # src/libraries/TOF/DTOFGeometry.h   # Uses DGeometry
        src/libraries/TOF/DTOFTDCDigiHit.h
        # src/libraries/TOF/DTOFPoint.h    # Uses TVersionCheck

        src/libraries/TPOL/DTPOLTruthHit.h
        src/libraries/TPOL/DTPOLHit.h
        src/libraries/TPOL/DTPOLSectorDigiHit.h
        src/libraries/TPOL/DTPOLRingDigiHit.h

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
#        src/libraries/TTAB/DTranslationTable.cc          # Pulls in JEventSource_EVIO
#        src/libraries/TTAB/DTranslationTable_factory.h
        src/libraries/TTAB/DTTabUtilities.cc
        src/libraries/TTAB/DTTabUtilities.h
#        src/libraries/TTAB/DTTabUtilities_factory.cc
#        src/libraries/TTAB/DTTabUtilities_factory.h
#        src/libraries/TTAB/TTab_init.cc

        )

add_executable(halld_recon
        src/programs/Analysis/hd_port/hd_port.cc
#        src/programs/Analysis/hd_dump/hd_dump.cc
#        src/programs/Analysis/hd_dump/MyProcessor.cc
#        src/programs/Analysis/hd_dump/MyProcessor.h
        ${HALLD_RECON_SOURCES}
        )


target_include_directories(halld_recon PRIVATE src/libraries src/libraries/include src/include
        ${CCDB_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${JANA_INCLUDE_DIR} ${EVIO_INCLUDE_DIR})

message(WARN ${EVIO_LIBS})
target_link_libraries(halld_recon ${JANA_LIBRARY} ${EVIO_LIBS} ${EXPAT_LIBRARY})
#target_link_libraries(halld_recon ${JANA_LIBRARY})


