
set (calibrations_projects
        BCAL_SiPM_saturation
        FCAL_Pi0HFA
        BCAL_attenlength_gainratio
        BCAL_gainmatrix
        BCAL_point_time
        BCAL_point_calib
        BCAL_saturation
        BCAL_TDC_Timing
        CDC_amp
        HLDetectorTiming
        TAGH_timewalk
        CDC_TimeToDistance
        PS_timing
        PSC_TW
        PS_E_calib
        st_tw_corr_auto
        ST_Propagation_Time
        ST_Tresolution
        TAGM_TW
        TOF_calib
        FCALLEDTree
        FCAL_LED_shifts
        FCAL_Pi0TOF
        FCAL_TimingOffsets_Primex
        )

set (optional_calibrations_projects
        FCALgains
        FCALpedestals
        FCALpulsepeak
        FCAL_TimingOffsets
)

set (presumably_abandoned_calibrations_projects
        BCAL_ADC_4ns
        BCAL_TimeCalibration
)


foreach (project ${calibrations_projects})
file(GLOB ${project}_SOURCES ${project}/*.cc)
set(target_name plugin_calibrations_${project})
add_library(${target_name} ${${project}_SOURCES})
    target_include_directories(${target_name} PRIVATE ../include ../../libraries ../../libraries/include ../../include ${ROOT_INCLUDE_DIRS} ${JANA_INCLUDE_DIR})
    target_link_libraries(${target_name} halld_recon)
endforeach ()
