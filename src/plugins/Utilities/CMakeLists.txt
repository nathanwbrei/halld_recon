
set (utilities_plugins
        danarest
        evio_writer
        evio-hddm
        2trackskim
        pi0bcalskim
        pi0fcalskim
        twogamma_fcal_skim
        run_summary
        track_skimmer
        trackeff_missing
        ps_skim
        trigger_skims
        bigevents_skim
        coherent_peak_skim
        exclusivepi0skim
        randomtrigger_skim
        pi0fcaltofskim
        single_neutral_skim
        compton_neutral_skim
        eta2g_primexd_skim
        eta6g_primexd_skim
        etapi0_primexd_skim
        cdcbcal_skim
        EventTagPi0
        es_test
        omega_skim
        cal_high_energy_skim
        syncskim

        # Optional

        danahddm
        dumpcandidates
        dumpthrowns
#        l3bdt # Ported, but broken elsewhere
        merge_rawevents
        DAQ
        TTab
        rawevent

        # Abandoned

#        cdc_emu
#        danaevio
#        eviodana
#        janaded
#        scanf250
#        trackanal
        )


foreach (project ${utilities_plugins})
    file(GLOB ${project}_SOURCES ${project}/*.cc)
    set(target_name plugin_utilities_${project})
    add_library(${target_name} ${${project}_SOURCES})
    target_include_directories(${target_name} PRIVATE . ../include ../../libraries ../../libraries/include ../../include ${ROOT_INCLUDE_DIRS} ${JANA_INCLUDE_DIR} ${EVIO_INCLUDE_DIR} ${CCDB_INCLUDE_DIRS})
    target_link_libraries(${target_name} halld_recon {EVIO_LIB} ${CCDB_LIB})
endforeach()

