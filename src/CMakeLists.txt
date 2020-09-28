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

file(GLOB DAQ_SOURCES ${PROJECT_SOURCE_DIR}/libraries/DAQ/*.cc)
file(GLOB DAQ_HEADERS ${PROJECT_SOURCE_DIR}/libraries/DAQ/*.h)

set(HALLD_RECON_SOURCES

#        libraries/ANALYSIS/ANALYSIS_init.cc
#        libraries/ANALYSIS/DAnalysisAction.cc
#        libraries/ANALYSIS/DAnalysisAction.h
#        libraries/ANALYSIS/DAnalysisResults.h
#        libraries/Analysis/DAnalysisResults_factory.cc
#        libraries/Analysis/DAnalysisResults_factory.h
#        libraries/Analysis/DAnalysisUtilities.cc
#        libraries/Analysis/DAnalysisUtilities.h
#        libraries/Analysis/DAnalysisUtilities_factory.h
#        libraries/Analysis/DChargedTrack_factory_Combo.cc
#        libraries/Analysis/DChargedTrack_factory_Combo.h
#        libraries/Analysis/DCutActions.cc
#        libraries/Analysis/DCutActions.h
#        libraries/Analysis/DDetectorMatches_factory_Combo.cc
#        libraries/Analysis/DDetectorMatches_factory_Combo.h
#        libraries/Analysis/DEventWriterROOT.cc
#        libraries/Analysis/DEventWriterROOT.h
#        libraries/Analysis/DEventWriterROOT_factory.h
#        libraries/Analysis/DHistogramActions.h
#        libraries/Analysis/DHistogramActions_Independent.cc
#        libraries/Analysis/DHistogramActions_Independent.h
#        libraries/Analysis/DHistogramActions_Reaction.cc
#        libraries/Analysis/DHistogramActions_Reaction.h
#        libraries/Analysis/DHistogramActions_Thrown.cc
#        libraries/Analysis/DHistogramActions_Thrown.h
#        libraries/Analysis/DKinFitResults.h
#        libraries/Analysis/DKinFitUtils_GlueX.cc
#        libraries/Analysis/DKinFitUtils_GlueX.h
#        libraries/Analysis/DMCThrownMatching.h
#        libraries/Analysis/DMCThrownMatching_factory.cc
#        libraries/Analysis/DMCThrownMatching_factory.h
#        libraries/Analysis/DNeutralParticle_factory_Combo.cc
#        libraries/Analysis/DNeutralParticle_factory_Combo.h
#        libraries/Analysis/DParticleCombo.h
#        libraries/Analysis/DParticleComboCreator.cc
#        libraries/Analysis/DParticleComboCreator.h
#        libraries/Analysis/DParticleComboStep.h
#        libraries/Analysis/DReaction.cc
#        libraries/Analysis/DReaction.h
#        libraries/Analysis/DReaction_factory_Thrown.cc
#        libraries/Analysis/DReaction_factory_Thrown.h
#        libraries/Analysis/DReactionStep.cc
#        libraries/Analysis/DReactionStep.h
#        libraries/Analysis/DReactionStepVertexInfo.cc
#        libraries/Analysis/DReactionStepVertexInfo.h
#        libraries/Analysis/DReactionVertexInfo.h
#        libraries/Analysis/DReactionVertexInfo_factory.cc
#        libraries/Analysis/DReactionVertexInfo_factory.h
#        libraries/Analysis/DSourceCombo.h
#        libraries/Analysis/DSourceComboer.cc
#        libraries/Analysis/DSourceComboer.h
#        libraries/Analysis/DSourceComboP4Handler.cc
#        libraries/Analysis/DSourceComboP4Handler.h
#        libraries/Analysis/DSourceComboTimeHandler.cc
#        libraries/Analysis/DSourceComboTimeHandler.h
#        libraries/Analysis/DSourceComboVertexer.cc
#        libraries/Analysis/DSourceComboVertexer.h
#        libraries/Analysis/DTrackTimeBased_factory_Combo.cc
#        libraries/Analysis/DTrackTimeBased_factory_Combo.h
#        libraries/Analysis/DTreeInterface.cc
#        libraries/Analysis/DTreeInterface.h
#        libraries/Analysis/DTreeInterfaceObjects.h

        libraries/BCAL/BCAL_init.cc
        libraries/BCAL/DBCALClump.cc
        libraries/BCAL/DBCALClump.h
        libraries/BCAL/DBCALClump_factory.cc
        libraries/BCAL/DBCALClump_factory.h
        libraries/BCAL/DBCALCluster.cc
        libraries/BCAL/DBCALCluster.h
        libraries/BCAL/DBCALCluster_factory.cc
        libraries/BCAL/DBCALCluster_factory.h
        libraries/BCAL/DBCALCluster_factory_SINGLE.cc
        libraries/BCAL/DBCALCluster_factory_SINGLE.h
        libraries/BCAL/DBCALDigiHit.h
        libraries/BCAL/DBCALGeometry.cc
        libraries/BCAL/DBCALGeometry.h
        libraries/BCAL/DBCALGeometry_factory.h
        libraries/BCAL/DBCALHit.h
        libraries/BCAL/DBCALHit_factory.cc
        libraries/BCAL/DBCALHit_factory.h
        libraries/BCAL/DBCALIncidentParticle.h
        libraries/BCAL/DBCALPoint.cc
        libraries/BCAL/DBCALPoint.h
        libraries/BCAL/DBCALPoint_factory.cc
        libraries/BCAL/DBCALPoint_factory.h
        libraries/BCAL/DBCALShower.h
        libraries/BCAL/DBCALShower_factory.h
        libraries/BCAL/DBCALShower_factory_CURVATURE.cc
        libraries/BCAL/DBCALShower_factory_CURVATURE.h
        libraries/BCAL/DBCALShower_factory_IU.cc
        libraries/BCAL/DBCALShower_factory_IU.h
        libraries/BCAL/DBCALShower_factory_JLAB.cc
        libraries/BCAL/DBCALShower_factory_JLAB.h
        libraries/BCAL/DBCALShower_factory_KLOE.cc
        libraries/BCAL/DBCALShower_factory_KLOE.h
        libraries/BCAL/DBCALSiPMHit.h
        libraries/BCAL/DBCALSiPMSpectrum.h
        libraries/BCAL/DBCALTDCDigiHit.h
        libraries/BCAL/DBCALTDCHit.h
        libraries/BCAL/DBCALTDCHit_factory.cc
        libraries/BCAL/DBCALTDCHit_factory.h
        libraries/BCAL/DBCALTruthCell.h
        libraries/BCAL/DBCALTruthShower.h
        libraries/BCAL/DBCALUnifiedHit.h
        libraries/BCAL/DBCALUnifiedHit_factory.cc
        libraries/BCAL/DBCALUnifiedHit_factory.h

        libraries/CCAL/CCAL_init.cc
        libraries/CCAL/DCCALDigiHit.h
        libraries/CCAL/DCCALGeometry.cc
        libraries/CCAL/DCCALGeometry.h
        libraries/CCAL/DCCALGeometry_factory.h
        libraries/CCAL/DCCALHit.h
        libraries/CCAL/DCCALHit_factory.cc
        libraries/CCAL/DCCALHit_factory.h
        libraries/CCAL/DCCALRefDigiHit.h
        libraries/CCAL/DCCALShower.cc
        libraries/CCAL/DCCALShower.h
        libraries/CCAL/DCCALShower_factory.cc
        libraries/CCAL/DCCALShower_factory.h
        libraries/CCAL/DCCALTruthShower.h
        libraries/CCAL/DCCALTruthShower_factory.cc
        libraries/CCAL/DCCALTruthShower_factory.h

        libraries/CDC/CDC_init.cc
        libraries/CDC/DCDCDigiHit.h
        libraries/CDC/DCDCHit.h
        libraries/CDC/DCDCHit_factory.cc
        libraries/CDC/DCDCHit_factory.h
        libraries/CDC/DCDCHit_factory_Calib.cc
        libraries/CDC/DCDCHit_factory_Calib.h
        libraries/CDC/DCDCTrackHit.h
        libraries/CDC/DCDCTrackHit_factory.cc
        libraries/CDC/DCDCTrackHit_factory.h
        libraries/CDC/DCDCWire.h


        libraries/CERE/DCereHit.h
        libraries/CERE/CERE_init.cc

        libraries/DANA/DANARootErrorHandler.cc
        libraries/DANA/DANARootErrorHandler.h
        libraries/DANA/DGeometryManager.cc
        libraries/DANA/DGeometryManager.h
        libraries/DANA/DEvent.h
#        libraries/DANA/DApplication.cc
        libraries/DANA/DApplication.h
#        libraries/DANA/DFactoryGenerator.cc
        libraries/DANA/DFactoryGenerator.h
        libraries/DANA/DObjectID.h
        libraries/DANA/DStatusBits.h
        libraries/DANA/JExceptionDataFormat.h
        libraries/DANA/ReadWriteLock.cc
        libraries/DANA/ReadWriteLock.h

#        libraries/DAQ/JEventSource_EVIO.cc
        libraries/DAQ/JEventSource_EVIO.h
#        libraries/DAQ/JEventSourceGenerator_EVIO.cc
        libraries/DAQ/JEventSourceGenerator_EVIO.h
#        libraries/DAQ/JEventSource_EVIO.cc

        libraries/DAQ/DF1TDCConfig.h
        libraries/DAQ/Df250EmulatorAlgorithm_v3.h  # JEventLoop
        libraries/DAQ/DF1TDCHit.h
        libraries/DAQ/Df125EmulatorAlgorithm.h
        libraries/DAQ/Df125Config.h
        libraries/DAQ/Df125PulseIntegral.h
        libraries/DAQ/DCAEN1290TDCConfig.h
        libraries/DAQ/Df250Scaler.h
        libraries/DAQ/Df250TriggerTime.h
        libraries/DAQ/Df125BORConfig.h
        libraries/DAQ/Df125FDCPulse.h
        libraries/DAQ/Df250BORConfig.h
        libraries/DAQ/DCAEN1290TDCHit.h
        libraries/DAQ/DTSscalers.h
        libraries/DAQ/DCODAROCInfo.h
        libraries/DAQ/DGEMSRSWindowRawData.h
        libraries/DAQ/DDIRCTDCHit.h
        libraries/DAQ/DCODAControlEvent.h
        libraries/DAQ/Df250Config.h
        libraries/DAQ/Df250WindowRawData.h
        libraries/DAQ/DF1TDCTriggerTime.h
        libraries/DAQ/DCODAEventInfo.h
        libraries/DAQ/Df125PulsePedestal.h
        libraries/DAQ/Df250AsyncPedestal.h
        libraries/DAQ/DTSGBORConfig.h
        libraries/DAQ/Df125PulseRawData.h
        libraries/DAQ/Df250StreamingRawData.h
        libraries/DAQ/Df125PulseTime.h
        libraries/DAQ/Df125CDCPulse.h
        libraries/DAQ/DEPICSvalue.h
        libraries/DAQ/Df250PulseTime.h
        libraries/DAQ/DCAEN1290TDCBORConfig.h
        libraries/DAQ/DL1Info.h
        libraries/DAQ/DF1TDCBORConfig.h
        libraries/DAQ/Df250EmulatorAlgorithm_v1.h
        libraries/DAQ/Df250EmulatorAlgorithm.h
        libraries/DAQ/Df125TriggerTime.h
        libraries/DAQ/DEventTag.h
        libraries/DAQ/Df250PulseIntegral.h
        libraries/DAQ/DBeamCurrent.h
        libraries/DAQ/DDIRCADCHit.h
        libraries/DAQ/Df250PulseRawData.h
        libraries/DAQ/DDIRCTriggerTime.h
        libraries/DAQ/Df250PulseData.h
        libraries/DAQ/DDAQConfig.h
        libraries/DAQ/Df250PulsePedestal.h
        libraries/DAQ/Df250WindowSum.h
        libraries/DAQ/Df250EmulatorAlgorithm_v2.h
        libraries/DAQ/Df250EmulatorAlgorithm_v2.cc
        libraries/DAQ/DParsedEvent.h
        libraries/DAQ/Df125WindowRawData.h
        libraries/DAQ/DDAQAddress.h
        libraries/DAQ/HDEVIO.h
        libraries/DAQ/HDEVIO.cc

        libraries/DIRC/DDIRCGeometry.cc
        libraries/DIRC/DDIRCGeometry.h
        libraries/DIRC/DDIRCGeometry_factory.h
        libraries/DIRC/DDIRCLEDRef.h
        libraries/DIRC/DDIRCLEDRef_factory.cc
        libraries/DIRC/DDIRCLEDRef_factory.h
        libraries/DIRC/DDIRCLut.cc
        libraries/DIRC/DDIRCLut.h
        libraries/DIRC/DDIRCLut_factory.h
        libraries/DIRC/DDIRCLutReader.cc
        libraries/DIRC/DDIRCLutReader.h
        libraries/DIRC/DDIRCPmtHit.h
        libraries/DIRC/DDIRCPmtHit_factory.cc
        libraries/DIRC/DDIRCPmtHit_factory.h
        libraries/DIRC/DDIRCTDCDigiHit.h
        libraries/DIRC/DDIRCTruthBarHit.h
        libraries/DIRC/DDIRCTruthPmtHit.h
        libraries/DIRC/DIRC_init.cc


        libraries/EVENTSTORE/DESSkimData.h


        libraries/FCAL/DFCALCluster.cc
        libraries/FCAL/DFCALCluster.h
        libraries/FCAL/DFCALCluster_factory.cc
        libraries/FCAL/DFCALCluster_factory.h
        libraries/FCAL/DFCALDigiHit.h
        libraries/FCAL/DFCALGeometry.cc
        libraries/FCAL/DFCALGeometry.h
        libraries/FCAL/DFCALGeometry_factory.cc
        libraries/FCAL/DFCALGeometry_factory.h
        libraries/FCAL/DFCALHit.h
        libraries/FCAL/DFCALHit_factory.cc
        libraries/FCAL/DFCALHit_factory.h
        libraries/FCAL/DFCALShower.cc
        libraries/FCAL/DFCALShower.h
        libraries/FCAL/DFCALShower_factory.cc
        libraries/FCAL/DFCALShower_factory.h
        libraries/FCAL/DFCALTruthShower.h
        libraries/FCAL/FCAL_init.cc


        libraries/FDC/DFDCCathodeDigiHit.h
        libraries/FDC/DFDCWireDigiHit.h
        libraries/FDC/DFDCSegment.h
        libraries/FDC/DFDCGeometry.h
        libraries/FDC/DFDCGeometry.cc
        libraries/FDC/DFDCIntersection.h
        libraries/FDC/DFDCHit.h
        libraries/FDC/DFDCCathodeCluster.h
        libraries/FDC/DFDCPseudo.h
        libraries/FDC/DFDCSegment_factory.cc
        libraries/FDC/DFDCSegment_factory.h
        libraries/FDC/DFDCPseudo_factory.cc
        libraries/FDC/DFDCPseudo_factory.h

        libraries/FMWPC/DFMWPCTruthHit.h
        libraries/FMWPC/DFMWPCHit.h
        libraries/FMWPC/FMWPC_init.cc

        libraries/HDGEOMETRY/DGeometry.cc
        libraries/HDGEOMETRY/DGeometry.h
        libraries/HDGEOMETRY/DLorentzDeflections.cc
        libraries/HDGEOMETRY/DLorentzDeflections.h
        libraries/HDGEOMETRY/DLorentzMapCalibDB.cc
        libraries/HDGEOMETRY/DLorentzMapCalibDB.h
        libraries/HDGEOMETRY/DMagneticFieldMap.h
        libraries/HDGEOMETRY/DMagneticFieldMapCalibDB.cc
        libraries/HDGEOMETRY/DMagneticFieldMapCalibDB.h
        libraries/HDGEOMETRY/DMagneticFieldMapConst.cc
        libraries/HDGEOMETRY/DMagneticFieldMapConst.h
        libraries/HDGEOMETRY/DMagneticFieldMapFineMesh.cc
        libraries/HDGEOMETRY/DMagneticFieldMapFineMesh.h
        libraries/HDGEOMETRY/DMagneticFieldMapNoField.cc
        libraries/HDGEOMETRY/DMagneticFieldMapNoField.h
        libraries/HDGEOMETRY/DMagneticFieldMapParameterized.cc
        libraries/HDGEOMETRY/DMagneticFieldMapParameterized.h
        libraries/HDGEOMETRY/DMagneticFieldMapPS.h
        libraries/HDGEOMETRY/DMagneticFieldMapPS2DMap.cc
        libraries/HDGEOMETRY/DMagneticFieldMapPS2DMap.h
        libraries/HDGEOMETRY/DMagneticFieldMapPSConst.cc
        libraries/HDGEOMETRY/DMagneticFieldMapPSConst.h
        libraries/HDGEOMETRY/DMagneticFieldMapSpoiled.cc
        libraries/HDGEOMETRY/DMagneticFieldMapSpoiled.h
        libraries/HDGEOMETRY/DMaterial.cc
        libraries/HDGEOMETRY/DMaterial.h
        libraries/HDGEOMETRY/DMaterialMap.cc
        libraries/HDGEOMETRY/DMaterialMap.h
        libraries/HDGEOMETRY/DRootGeom.cc
        libraries/HDGEOMETRY/DRootGeom.h

        libraries/KINFITTER/DKinFitChain.h
        libraries/KINFITTER/DKinFitChainStep.h
        libraries/KINFITTER/DKinFitConstraint.h
        libraries/KINFITTER/DKinFitConstraint_Mass.h
        libraries/KINFITTER/DKinFitConstraint_P4.h
        libraries/KINFITTER/DKinFitConstraint_Spacetime.h
        libraries/KINFITTER/DKinFitConstraint_Vertex.h
        libraries/KINFITTER/DKinFitParticle.h
        libraries/KINFITTER/DKinFitter.cc
        libraries/KINFITTER/DKinFitter.h
        libraries/KINFITTER/DKinFitUtils.cc
        libraries/KINFITTER/DKinFitUtils.h

        libraries/PAIR_SPECTROMETER/DLumi.cc
        libraries/PAIR_SPECTROMETER/DLumi.h
        libraries/PAIR_SPECTROMETER/DLumi_factory.h
        libraries/PAIR_SPECTROMETER/DPSCDigiHit.h
        libraries/PAIR_SPECTROMETER/DPSCHit.h
        libraries/PAIR_SPECTROMETER/DPSCHit_factory.cc
        libraries/PAIR_SPECTROMETER/DPSCHit_factory.h
        libraries/PAIR_SPECTROMETER/DPSCPair.h
        libraries/PAIR_SPECTROMETER/DPSCPair_factory.cc
        libraries/PAIR_SPECTROMETER/DPSCPair_factory.h
        libraries/PAIR_SPECTROMETER/DPSCTDCDigiHit.h
        libraries/PAIR_SPECTROMETER/DPSCTruthHit.h
        libraries/PAIR_SPECTROMETER/DPSDigiHit.h
        libraries/PAIR_SPECTROMETER/DPSGeometry.cc
        libraries/PAIR_SPECTROMETER/DPSGeometry.h
        libraries/PAIR_SPECTROMETER/DPSGeometry_factory.h
        libraries/PAIR_SPECTROMETER/DPSHit.h
        libraries/PAIR_SPECTROMETER/DPSHit_factory.cc
        libraries/PAIR_SPECTROMETER/DPSHit_factory.h
        libraries/PAIR_SPECTROMETER/DPSPair.h
        libraries/PAIR_SPECTROMETER/DPSPair_factory.cc
        libraries/PAIR_SPECTROMETER/DPSPair_factory.h
        libraries/PAIR_SPECTROMETER/DPSTruthHit.h
        libraries/PAIR_SPECTROMETER/PAIR_SPECTROMETER_init.cc


        libraries/PID/DBeamPhoton.h
        libraries/PID/DBeamPhoton_factory.cc
        libraries/PID/DBeamPhoton_factory.h
        libraries/PID/DBeamPhoton_factory_MCGEN.cc
        libraries/PID/DBeamPhoton_factory_MCGEN.h
        libraries/PID/DBeamPhoton_factory_TAGGEDMCGEN.cc
        libraries/PID/DBeamPhoton_factory_TAGGEDMCGEN.h
        libraries/PID/DBeamPhoton_factory_TRUTH.cc
        libraries/PID/DBeamPhoton_factory_TRUTH.h
        libraries/PID/DChargedTrack.h
        libraries/PID/DChargedTrack_factory.cc
        libraries/PID/DChargedTrack_factory.h
        libraries/PID/DChargedTrack_factory_PreSelect.cc
        libraries/PID/DChargedTrack_factory_PreSelect.h
        libraries/PID/DChargedTrackHypothesis.h
        libraries/PID/DChargedTrackHypothesis_factory.cc
        libraries/PID/DChargedTrackHypothesis_factory.h
        libraries/PID/DDetectorMatches.h
        libraries/PID/DDetectorMatches_factory.cc
        libraries/PID/DDetectorMatches_factory.h
        libraries/PID/DDetectorMatches_factory_WireBased.cc
        libraries/PID/DDetectorMatches_factory_WireBased.h
        libraries/PID/DEventRFBunch.h
        libraries/PID/DEventRFBunch_factory.cc
        libraries/PID/DEventRFBunch_factory.h
#        libraries/PID/DEventRFBunch_factory_Calibrations.cc  # Depends on ANALYSIS everything
#        libraries/PID/DEventRFBunch_factory_Calibrations.h
        libraries/PID/DEventRFBunch_factory_CalorimeterOnly.cc
        libraries/PID/DEventRFBunch_factory_CalorimeterOnly.h
        libraries/PID/DEventRFBunch_factory_Thrown.cc
        libraries/PID/DEventRFBunch_factory_Thrown.h
        libraries/PID/DKinematicData.h
        libraries/PID/DKinematicPools.cc
        libraries/PID/DMCReaction.h
#        libraries/PID/DMCThrown_factory_Decaying.cc
#        libraries/PID/DMCThrown_factory_Decaying.h
#        libraries/PID/DMCThrown_factory_FinalState.cc
#        libraries/PID/DMCThrown_factory_FinalState.h
#        libraries/PID/DMCThrown_factory_Primary.cc
#        libraries/PID/DMCThrown_factory_Primary.h
        libraries/PID/DNeutralParticle.h
#        libraries/PID/DNeutralParticle_factory.cc
#        libraries/PID/DNeutralParticle_factory.h
#        libraries/PID/DNeutralParticle_factory_PreSelect.cc
#        libraries/PID/DNeutralParticle_factory_PreSelect.h
        libraries/PID/DNeutralParticleHypothesis.h
        libraries/PID/DNeutralParticleHypothesis_factory.cc
        libraries/PID/DNeutralParticleHypothesis_factory.h
        libraries/PID/DNeutralShower.h
#        libraries/PID/DNeutralShower_factory.cc
#        libraries/PID/DNeutralShower_factory.h
#        libraries/PID/DNeutralShower_factory_PreSelect.cc
#        libraries/PID/DNeutralShower_factory_PreSelect.h
#        libraries/PID/DNeutralShower_FCALQualityMLP.h
        libraries/PID/DParticleID.cc
        libraries/PID/DParticleID.h
#        libraries/PID/DParticleID_factory.h
#        libraries/PID/DParticleID_factory_PID1.h
#        libraries/PID/DParticleID_PID1.cc
#        libraries/PID/DParticleID_PID1.h
        libraries/PID/DVertex.h
#        libraries/PID/DVertex_factory.cc
#        libraries/PID/DVertex_factory.h
#        libraries/PID/DVertex_factory_THROWN.cc
#        libraries/PID/DVertex_factory_THROWN.h
#        libraries/PID/PID_init.cc


        libraries/RF/DRFDigiTime.h
        libraries/RF/DRFTDCDigiTime.h
        libraries/RF/DRFTime.h
        libraries/RF/DRFTime_factory.cc
        libraries/RF/DRFTime_factory.h
        libraries/RF/DRFTime_factory_FDC.cc
        libraries/RF/DRFTime_factory_FDC.h
        libraries/RF/DRFTime_factory_PSC.cc
        libraries/RF/DRFTime_factory_PSC.h
        libraries/RF/DRFTime_factory_TAGH.cc
        libraries/RF/DRFTime_factory_TAGH.h
        libraries/RF/DRFTime_factory_TOF.cc
        libraries/RF/DRFTime_factory_TOF.h
        libraries/RF/RF_init.cc


        libraries/START_COUNTER/DSCDigiHit.h
        libraries/START_COUNTER/DSCHit.h
        libraries/START_COUNTER/DSCHit_factory.cc
        libraries/START_COUNTER/DSCHit_factory.h
        libraries/START_COUNTER/DSCTDCDigiHit.h
        libraries/START_COUNTER/DSCTruthHit.h
        libraries/START_COUNTER/START_COUNTER_init.cc


        libraries/TAC/DRebuildFromRawFADC_factory.cc
        libraries/TAC/DRebuildFromRawFADC_factory.h
        libraries/TAC/DTACDigiHit.cc
        libraries/TAC/DTACDigiHit.h
        libraries/TAC/DTACHit.cc
        libraries/TAC/DTACHit.h
        libraries/TAC/DTACHit_factory.cc
        libraries/TAC/DTACHit_factory.h
        libraries/TAC/DTACTDCDigiHit.cc
        libraries/TAC/DTACTDCDigiHit.h
        libraries/TAC/HitRebuilderByFit.cc
        libraries/TAC/HitRebuilderByFit.h
        libraries/TAC/HitRebuilderInterfaceTAC.cc
        libraries/TAC/HitRebuilderInterfaceTAC.h
        libraries/TAC/HitRebuilderTAC.cc
        libraries/TAC/HitRebuilderTAC.h
        libraries/TAC/TAC_init.cc
        libraries/TAC/WaveformErfcFunctor.cc
        libraries/TAC/WaveformErfcFunctor.h
        libraries/TAC/WaveformSpikeFunctor.cc
        libraries/TAC/WaveformSpikeFunctor.h

        libraries/TAGGER/DTAGHDigiHit.h
        libraries/TAGGER/DTAGHGeometry.cc
        libraries/TAGGER/DTAGHGeometry.h
        libraries/TAGGER/DTAGHGeometry_factory.h
        libraries/TAGGER/DTAGHHit.h
        libraries/TAGGER/DTAGHHit_factory.cc
        libraries/TAGGER/DTAGHHit_factory.h
        libraries/TAGGER/DTAGHHit_factory_Calib.cc
        libraries/TAGGER/DTAGHHit_factory_Calib.h
        libraries/TAGGER/DTAGHTDCDigiHit.h
        libraries/TAGGER/DTAGMDigiHit.h
        libraries/TAGGER/DTAGMTDCDigiHit.h
        libraries/TAGGER/DTAGMGeometry.cc
        libraries/TAGGER/DTAGMGeometry.h
        libraries/TAGGER/DTAGMGeometry_factory.h
        libraries/TAGGER/DTAGMHit.h
        libraries/TAGGER/DTAGMHit_factory.cc
        libraries/TAGGER/DTAGMHit_factory.h
        libraries/TAGGER/DTAGMHit_factory_Calib.cc
        libraries/TAGGER/DTAGMHit_factory_Calib.h
        libraries/TAGGER/TAGGER_init.cc


        libraries/TOF/DTOFDigiHit.h
        libraries/TOF/DTOFGeometry.cc
        libraries/TOF/DTOFGeometry.h
        libraries/TOF/DTOFGeometry_factory.h
        libraries/TOF/DTOFHit.h
        libraries/TOF/DTOFHit_factory.cc
        libraries/TOF/DTOFHit_factory.h
        libraries/TOF/DTOFHitMC.h
        libraries/TOF/DTOFPaddleHit.h
        libraries/TOF/DTOFPaddleHit_factory.cc
        libraries/TOF/DTOFPaddleHit_factory.h
        libraries/TOF/DTOFPoint.h
        libraries/TOF/DTOFPoint_factory.cc
        libraries/TOF/DTOFPoint_factory.h
        libraries/TOF/DTOFTDCDigiHit.h
        libraries/TOF/DTOFTruth.h
        libraries/TOF/TOF_init.cc

        libraries/TPOL/DTPOLHit.h
        libraries/TPOL/DTPOLHit_factory.cc
        libraries/TPOL/DTPOLHit_factory.h
        libraries/TPOL/DTPOLRingDigiHit.h
        libraries/TPOL/DTPOLSectorDigiHit.h
        libraries/TPOL/DTPOLTruthHit.h
        libraries/TPOL/TPOL_init.cc

        libraries/TRACKING/DHelicalFit.cc
        libraries/TRACKING/DHelicalFit.h
        libraries/TRACKING/DHoughFind.cc
        libraries/TRACKING/DHoughFind.h
        libraries/TRACKING/DMagneticFieldStepper.cc
        libraries/TRACKING/DMagneticFieldStepper.h
        libraries/TRACKING/DMatrixPools.cc
        libraries/TRACKING/DMCThrown.h
        libraries/TRACKING/DMCTrackHit.h
        libraries/TRACKING/DMCTrajectoryPoint.h
        libraries/TRACKING/DQuickFit.cc
        libraries/TRACKING/DQuickFit.h
        libraries/TRACKING/DReferenceTrajectory.cc
        libraries/TRACKING/DReferenceTrajectory.h
        libraries/TRACKING/DRiemannFit.cc
        libraries/TRACKING/DRiemannFit.h
        libraries/TRACKING/DTrackCandidate.h
        libraries/TRACKING/DTrackCandidate_factory.cc
        libraries/TRACKING/DTrackCandidate_factory.h
        libraries/TRACKING/DTrackCandidate_factory_CDC.cc
        libraries/TRACKING/DTrackCandidate_factory_CDC.h
        libraries/TRACKING/DTrackCandidate_factory_CDC_or_FDCpseudo.cc
        libraries/TRACKING/DTrackCandidate_factory_CDC_or_FDCpseudo.h
        libraries/TRACKING/DTrackCandidate_factory_FDC.cc
        libraries/TRACKING/DTrackCandidate_factory_FDC.h
        libraries/TRACKING/DTrackCandidate_factory_FDCCathodes.cc
        libraries/TRACKING/DTrackCandidate_factory_FDCCathodes.h
        libraries/TRACKING/DTrackCandidate_factory_FDCpseudo.cc
        libraries/TRACKING/DTrackCandidate_factory_FDCpseudo.h
        libraries/TRACKING/DTrackCandidate_factory_StraightLine.cc
        libraries/TRACKING/DTrackCandidate_factory_StraightLine.h
        libraries/TRACKING/DTrackCandidate_factory_THROWN.cc
        libraries/TRACKING/DTrackCandidate_factory_THROWN.h
        libraries/TRACKING/DTrackFinder.cc
        libraries/TRACKING/DTrackFinder.h
        libraries/TRACKING/DTrackFinder_factory.h
        libraries/TRACKING/DTrackFitter.cc
        libraries/TRACKING/DTrackFitter.h
        libraries/TRACKING/DTrackFitter_factory.h
        libraries/TRACKING/DTrackFitter_factory_ALT1.h
        libraries/TRACKING/DTrackFitter_factory_KalmanSIMD.h
        libraries/TRACKING/DTrackFitter_factory_KalmanSIMD_ALT1.h
        libraries/TRACKING/DTrackFitter_factory_Riemann.h
        libraries/TRACKING/DTrackFitter_factory_StraightTrack.h
        libraries/TRACKING/DTrackFitterALT1.cc
        libraries/TRACKING/DTrackFitterALT1.h
        libraries/TRACKING/DTrackFitterKalmanSIMD.cc
        libraries/TRACKING/DTrackFitterKalmanSIMD.h
        libraries/TRACKING/DTrackFitterKalmanSIMD_ALT1.cc
        libraries/TRACKING/DTrackFitterKalmanSIMD_ALT1.h
        libraries/TRACKING/DTrackFitterRiemann.cc
        libraries/TRACKING/DTrackFitterRiemann.h
        libraries/TRACKING/DTrackFitterStraightTrack.cc
        libraries/TRACKING/DTrackFitterStraightTrack.h
        libraries/TRACKING/DTrackHit.h
        libraries/TRACKING/DTrackHitSelector.cc
        libraries/TRACKING/DTrackHitSelector.h
        libraries/TRACKING/DTrackHitSelector_factory.h
        libraries/TRACKING/DTrackHitSelector_factory_ALT1.h
        libraries/TRACKING/DTrackHitSelector_factory_ALT2.h
        libraries/TRACKING/DTrackHitSelector_factory_THROWN.h
        libraries/TRACKING/DTrackHitSelectorALT1.cc
        libraries/TRACKING/DTrackHitSelectorALT1.h
        libraries/TRACKING/DTrackHitSelectorALT2.cc
        libraries/TRACKING/DTrackHitSelectorALT2.h
        libraries/TRACKING/DTrackHitSelectorTHROWN.cc
        libraries/TRACKING/DTrackHitSelectorTHROWN.h
        libraries/TRACKING/DTrackingData.h
        libraries/TRACKING/DTrackTimeBased.h
        libraries/TRACKING/DTrackTimeBased_factory.cc
        libraries/TRACKING/DTrackTimeBased_factory.h
        libraries/TRACKING/DTrackTimeBased_factory_StraightLine.cc
        libraries/TRACKING/DTrackTimeBased_factory_StraightLine.h
        libraries/TRACKING/DTrackTimeBased_factory_THROWN.cc
        libraries/TRACKING/DTrackTimeBased_factory_THROWN.h
        libraries/TRACKING/DTrackWireBased.h
        libraries/TRACKING/DTrackWireBased_factory.cc
        libraries/TRACKING/DTrackWireBased_factory.h
        libraries/TRACKING/DTrackWireBased_factory_StraightLine.cc
        libraries/TRACKING/DTrackWireBased_factory_StraightLine.h
        libraries/TRACKING/DTrackWireBased_factory_THROWN.cc
        libraries/TRACKING/DTrackWireBased_factory_THROWN.h
        libraries/TRACKING/fdc_covariance.cc
        libraries/TRACKING/grkuta.cc
        libraries/TRACKING/TRACKING_init.cc

        libraries/TRD/DGEMDigiWindowRawData.h
        libraries/TRD/DGEMHit.h
        libraries/TRD/DGEMHit_factory.cc
        libraries/TRD/DGEMHit_factory.h
        libraries/TRD/DGEMPoint.h
        libraries/TRD/DGEMPoint_factory.cc
        libraries/TRD/DGEMPoint_factory.h
        libraries/TRD/DGEMStripCluster.h
        libraries/TRD/DGEMStripCluster_factory.cc
        libraries/TRD/DGEMStripCluster_factory.h
        libraries/TRD/DTRDDigiHit.h
        libraries/TRD/DTRDHit.h
        libraries/TRD/DTRDHit_factory.cc
        libraries/TRD/DTRDHit_factory.h
        libraries/TRD/DTRDPoint.h
        libraries/TRD/DTRDPoint_factory.cc
        libraries/TRD/DTRDPoint_factory.h
        libraries/TRD/DTRDStripCluster.h
        libraries/TRD/DTRDStripCluster_factory.cc
        libraries/TRD/DTRDStripCluster_factory.h
        libraries/TRD/TRD_init.cc

        libraries/TRIGGER/DTrigger.h
        libraries/TRIGGER/DL3Trigger.h
        libraries/TRIGGER/DMCTrigger.h
        libraries/TRIGGER/DL1MCTrigger.h
        libraries/TRIGGER/DL1Trigger.h
        libraries/TRIGGER/DTrigger_factory.cc
        libraries/TRIGGER/DTrigger_factory.h
        libraries/TRIGGER/DMCTrigger_factory.cc
        libraries/TRIGGER/DMCTrigger_factory.h
        libraries/TRIGGER/DL3Trigger_factory.cc
        libraries/TRIGGER/DL3Trigger_factory.h
        libraries/TRIGGER/DL1Trigger_factory.cc
        libraries/TRIGGER/DL1Trigger_factory.h
#        libraries/TRIGGER/DL1MCTrigger_factory.cc     # DEventSource_HDDM
        libraries/TRIGGER/DL1MCTrigger_factory.h
#        libraries/TRIGGER/TRIGGER_init.cc

        libraries/TTAB/DTranslationTable.h
        libraries/TTAB/DTranslationTable.cc
        libraries/TTAB/DTranslationTable_factory.h
        libraries/TTAB/DTTabUtilities.cc
        libraries/TTAB/DTTabUtilities.h
        libraries/TTAB/DTTabUtilities_factory.cc
        libraries/TTAB/DTTabUtilities_factory.h
        libraries/TTAB/TTab_init.cc

        )

add_executable(halld_recon
        programs/Analysis/hd_port/hd_port.cc
#        programs/Analysis/hd_dump/hd_dump.cc
#        programs/Analysis/hd_dump/MyProcessor.cc
#        programs/Analysis/hd_dump/MyProcessor.h
        ${HALLD_RECON_SOURCES}
        )


target_include_directories(halld_recon PRIVATE libraries libraries/include src/include
        ${CCDB_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${JANA_INCLUDE_DIR} ${EVIO_INCLUDE_DIR} ${HDDS_INCLUDE_DIR})

message(WARN ${EVIO_LIBS})
target_link_libraries(halld_recon ${JANA_LIBRARY} ${EVIO_LIBS} ${EXPAT_LIBRARY} ${ROOT_LIBRARIES} ${HDDS_LIBRARY})


