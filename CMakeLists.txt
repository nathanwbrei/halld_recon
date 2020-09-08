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

#        src/libraries/TTAB/DTranslationTable.cc
#        src/libraries/TTAB/DTranslationTable.h
#        src/libraries/TTAB/DTranslationTable_factory.h
#        src/libraries/TTAB/DTTabUtilities.cc
#        src/libraries/TTAB/DTTabUtilities.h
#        src/libraries/TTAB/DTTabUtilities_factory.cc
#        src/libraries/TTAB/DTTabUtilities_factory.h
#        src/libraries/TTAB/TTab_init.cc

        src/libraries/DAQ/JEventSource_EVIO.cc
        src/libraries/DAQ/JEventSource_EVIO.h
#        src/libraries/DAQ/JEventSourceGenerator_EVIO.cc
#        src/libraries/DAQ/JEventSourceGenerator_EVIO.h
    )

add_executable(halld_recon
#        src/programs/Analysis/hd_port/hd_port.cc
        src/programs/Analysis/hd_dump/hd_dump.cc
        src/programs/Analysis/hd_dump/MyProcessor.cc
        src/programs/Analysis/hd_dump/MyProcessor.h
        ${HALLD_RECON_SOURCES}
        )


target_include_directories(halld_recon PRIVATE src/libraries src/libraries/include src/include
        ${CCDB_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${JANA_INCLUDE_DIR} ${EVIO_INCLUDE_DIR})

message(WARN ${EVIO_LIBS})
target_link_libraries(halld_recon ${JANA_LIBRARY} ${EVIO_LIBS} ${EXPAT_LIBRARY})
#target_link_libraries(halld_recon ${JANA_LIBRARY})


