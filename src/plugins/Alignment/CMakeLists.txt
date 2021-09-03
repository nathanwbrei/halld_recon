

set(alignment_projects
        MilleFieldOn
        MilleFieldOff
        MilleKs
        FDCProjectionResiduals
        FDC_InternalAlignment
        )

foreach (project ${alignment_projects})
    file(GLOB ${project}_SOURCES ${project}/*.cc)
    set(target_name plugin_alignment_${project})
    add_library(${target_name} ${${project}_SOURCES})
    target_include_directories(${target_name} PRIVATE ../include ../../libraries ../../libraries/include ../../include ${ROOT_INCLUDE_DIRS} ${JANA_INCLUDE_DIR})
    target_link_libraries(${target_name} halld_recon)
endforeach ()
