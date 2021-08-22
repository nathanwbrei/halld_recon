


set (analysis_projects
        ReactionFilter
        b1pi_hists
        ReactionEfficiency
        DAQTree
        DAQTreeBCAL
        monitoring_hists
        p2pi_hists
        p3pi_hists
        p4pi_hists
        p2k_hists
        p2gamma_hists
        ppi0gamma_hists
        p2pi0_hists
        fcal_charged
        TPOL_tree
        mcthrown_tree
        trackeff_hists
        imaging
        pid_dirc
        truth_dirc
        lut_dirc
        dirc_hists
        dirc_reactions
        dirc_tree
        TRD_hists
        B3pi_eff_missgamma
        B3pi_eff_misspip
        B3pi_eff_misspim
        B3pi_eff_missprot
        mcthrown_hists
        compton
        # Everything below is optional but hey, it compiles
        bcal_calib
        acceptance_hists
        fcal_hists
        ccal_hits
        ccal_display
        fcal_led
        F250_mode10_pedestal
)

set (optional_but_broken_analysis_projects
        phys_tree
        pedestals
        bcal_calib_cosmic_cdc
        cdc_hists
        dc_alignment
        p2pi_trees
        Z2pi_trees
        )

set (presumably_abandoned_analysis_projects
        bcal_hists
        bcal_shower
        bcalfcaltof_res_tree
        candidate_tree
        cdc_covariance_hists
        DAQ_WRD2PI
        DCdEdxStudy_tree
        eloss_tree
        eta_ntuple
        event_size
        F250_mode8_pedestal
        fdc_covariance_hists
        fdc_hists
        invariant_mass_hists
        level1_trigger
        mc_tree
        photoneff_hists
        pidstudies_tree
        pulls_tree
        radlen_hists
        rho_p_hists
        track_hists
        trackeff_hists2
        trk_profile
        trkfitterLSLM
        trkres_tree
        )


foreach (project ${analysis_projects})
    file(GLOB ${project}_SOURCES ${project}/*.cc)
    set(target_name plugin_analysis_${project})
    add_library(${target_name} ${${project}_SOURCES})
    target_include_directories(${target_name} PRIVATE ../include ../../libraries ../../libraries/include ../../include ${ROOT_INCLUDE_DIRS} ${JANA_INCLUDE_DIR})
    target_link_libraries(${target_name} halld_recon)
endforeach ()