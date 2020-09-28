
#include <JANA/JApplication.h>
#include <JANA/JCsvWriter.h>

// #include <BCAL/DBCALCluster.h>    // DBCALGeometry
#include <BCAL/DBCALTDCHit.h>
#include <BCAL/DBCALTDCDigiHit.h>
#include <BCAL/DBCALGeometry.h>      // JGeometry
#include <BCAL/DBCALIncidentParticle.h>
#include <BCAL/DBCALSiPMSpectrum.h>
#include <BCAL/DBCALUnifiedHit.h>
#include <BCAL/DBCALTruthCell.h>
#include <BCAL/DBCALClump.h>
#include <BCAL/DBCALPoint.h>
#include <BCAL/DBCALSiPMHit.h>
#include <BCAL/DBCALShower.h>
#include <BCAL/DBCALDigiHit.h>
#include <BCAL/DBCALTruthShower.h>
#include <BCAL/DBCALGeometry_factory.h>

#include <CDC/DCDCHit.h>
#include <CDC/DCDCTrackHit.h>
#include <CDC/DCDCDigiHit.h>

#include <CCAL/DCCALShower.h>
#include <CCAL/DCCALDigiHit.h>
#include <CCAL/DCCALRefDigiHit.h>
#include <CCAL/DCCALHit.h>
#include <CCAL/DCCALGeometry.h>
#include <CCAL/DCCALTruthShower.h>

#include <DAQ/DF1TDCConfig.h>
// #include <DAQ/Df250EmulatorAlgorithm_v3.h>  // JEventLoop
#include <DAQ/DF1TDCHit.h>
#include <DAQ/Df125EmulatorAlgorithm.h>
#include <DAQ/Df125Config.h>
#include <DAQ/Df125PulseIntegral.h>
#include <DAQ/DCAEN1290TDCConfig.h>
#include <DAQ/Df250Scaler.h>
#include <DAQ/Df250TriggerTime.h>
#include <DAQ/Df125BORConfig.h>
#include <DAQ/Df125FDCPulse.h>
#include <DAQ/Df250BORConfig.h>
#include <DAQ/DCAEN1290TDCHit.h>
#include <DAQ/DTSscalers.h>
#include <DAQ/DCODAROCInfo.h>
#include <DAQ/DGEMSRSWindowRawData.h>
#include <DAQ/DDIRCTDCHit.h>
#include <DAQ/DCODAControlEvent.h>
#include <DAQ/Df250Config.h>
#include <DAQ/Df250WindowRawData.h>
#include <DAQ/DF1TDCTriggerTime.h>
#include <DAQ/DCODAEventInfo.h>
#include <DAQ/Df125PulsePedestal.h>
#include <DAQ/Df250AsyncPedestal.h>
#include <DAQ/DTSGBORConfig.h>
#include <DAQ/Df125PulseRawData.h>
#include <DAQ/Df250StreamingRawData.h>
#include <DAQ/Df125PulseTime.h>
#include <DAQ/Df125CDCPulse.h>
#include <DAQ/DEPICSvalue.h>
#include <DAQ/Df250PulseTime.h>
#include <DAQ/DCAEN1290TDCBORConfig.h>
#include <DAQ/DL1Info.h>
#include <DAQ/DF1TDCBORConfig.h>
#include <DAQ/Df250EmulatorAlgorithm_v1.h>
#include <DAQ/Df250EmulatorAlgorithm.h>
#include <DAQ/Df125TriggerTime.h>
#include <DAQ/DEventTag.h>
#include <DAQ/Df250PulseIntegral.h>
#include <DAQ/DBeamCurrent.h>
#include <DAQ/DDIRCADCHit.h>
#include <DAQ/Df250PulseRawData.h>
#include <DAQ/DDIRCTriggerTime.h>
#include <DAQ/Df250PulseData.h>
#include <DAQ/DDAQConfig.h>
#include <DAQ/Df250PulsePedestal.h>
#include <DAQ/Df250WindowSum.h>
// #include <DAQ/Df250EmulatorAlgorithm_v2.h>      # JEventLoop
// #include <DAQ/DParsedEvent.h>                # JEventLoop
#include <DAQ/Df125WindowRawData.h>
#include <DAQ/DDAQAddress.h>

#include <DIRC/DDIRCLEDRef.h>
#include <DIRC/DDIRCTruthPmtHit.h>
#include <DIRC/DDIRCTDCDigiHit.h>
#include <DIRC/DDIRCPmtHit.h>
#include <DIRC/DDIRCGeometry.h>
#include <DIRC/DDIRCTruthBarHit.h>

#include <EVENTSTORE/DESSkimData.h>

#include <FCAL/DFCALHit.h>
#include <FCAL/DFCALDigiHit.h>
#include <FCAL/DFCALGeometry.h>
#include <FCAL/DFCALShower.h>
#include <FCAL/DFCALTruthShower.h>
#include <FCAL/DFCALCluster.h>

#include <FDC/DFDCCathodeDigiHit.h>
#include <FDC/DFDCWireDigiHit.h>
#include <FDC/DFDCIntersection.h>
#include <FDC/DFDCHit.h>
#include <FDC/DFDCCathodeCluster.h>
#include <FDC/DFDCPseudo.h>

#include <FMWPC/DFMWPCTruthHit.h>
#include <FMWPC/DFMWPCHit.h>

#include <HDGEOMETRY/DMaterial.h>
#include <HDGEOMETRY/DMagneticFieldMap.h>
#include <HDGEOMETRY/DMagneticFieldMapConst.h>
#include <HDGEOMETRY/DMagneticFieldMapPSConst.h>
#include <HDGEOMETRY/DMagneticFieldMapNoField.h>
#include <HDGEOMETRY/DMaterialMap.h>

#include <PAIR_SPECTROMETER/DPSDigiHit.h>
#include <PAIR_SPECTROMETER/DPSCDigiHit.h>
#include <PAIR_SPECTROMETER/DPSCTDCDigiHit.h>
#include <PAIR_SPECTROMETER/DPSTruthHit.h>
#include <PAIR_SPECTROMETER/DPSCTruthHit.h>
#include <PAIR_SPECTROMETER/DPSPair.h>
#include <PAIR_SPECTROMETER/DPSCHit.h>
#include <PAIR_SPECTROMETER/DPSHit.h>
#include <PAIR_SPECTROMETER/DPSCPair.h>

#include <PID/DChargedTrack.h>              // DTrackFitter
#include <PID/DKinematicData.h>
// #include <PID/DNeutralShower.h>             // DNeutralParticleHypothesis
#include <PID/DNeutralParticleHypothesis.h> // DTrackFitter
#include <PID/DChargedTrackHypothesis.h> // DTrackFitter
#include <PID/DDetectorMatches.h>
// #include <PID/DParticleID.h>                // JGeometry
// #include <PID/DNeutralParticle.h>           // DNeutralParticleHypothesis
#include <PID/DMCReaction.h>
#include <PID/DVertex.h>
#include <PID/DEventRFBunch.h>
#include <PID/DBeamPhoton.h>

#include <RF/DRFTDCDigiTime.h>
#include <RF/DRFTime.h>
#include <RF/DRFDigiTime.h>

#include <TAC/DTACHit.h>
#include <TAC/DTACTDCDigiHit.h>
#include <TAC/DTACDigiHit.h>

#include <TAGGER/DTAGHHit.h>
#include <TAGGER/DTAGHDigiHit.h>
#include <TAGGER/DTAGMHit.h>
#include <TAGGER/DTAGMDigiHit.h>
#include <TAGGER/DTAGMTDCDigiHit.h>
#include <TAGGER/DTAGHTDCDigiHit.h>

#include <TOF/DTOFHit.h>
#include <TOF/DTOFHitMC.h>
#include <TOF/DTOFTruth.h>
#include <TOF/DTOFPaddleHit.h>
#include <TOF/DTOFDigiHit.h>
#include <TOF/DTOFTDCDigiHit.h>
#include <TOF/DTOFPoint.h>

#include <TPOL/DTPOLTruthHit.h>
#include <TPOL/DTPOLHit.h>
#include <TPOL/DTPOLSectorDigiHit.h>
#include <TPOL/DTPOLRingDigiHit.h>

#include <TRACKING/DMCTrackHit.h>
#include <TRACKING/DHoughFind.h>
#include <TRACKING/DTrackHit.h>
#include <TRACKING/DMCTrajectoryPoint.h>
#include <TRACKING/DMCThrown.h>
// #include <TRACKING/DTrackCandidate.h>
// #include <TRACKING/DTrackTimeBased.h>
#include <TRACKING/DTrackFitter.h>
// #include <TRACKING/DTrackHitSelector.h>


#include <TRD/DGEMPoint.h>
#include <TRD/DTRDStripCluster.h>
#include <TRD/DGEMHit.h>
#include <TRD/DTRDDigiHit.h>
#include <TRD/DGEMDigiWindowRawData.h>
#include <TRD/DGEMStripCluster.h>
#include <TRD/DTRDHit.h>
#include <TRD/DTRDPoint.h>

#include <TRIGGER/DTrigger.h>
#include <TRIGGER/DL3Trigger.h>
#include <TRIGGER/DMCTrigger.h>
#include <TRIGGER/DL1MCTrigger.h>
#include <TRIGGER/DL1Trigger.h>

#include <TTAB/DTranslationTable.h>
#include <TTAB/DTranslationTable_factory.h>
#include <TTAB/DTTabUtilities.h>


int main(int argc, char* argv[]) {
	JApplication app;
    //app.Add(new JEventSourceGenerator_EVIO());
    //app.Add(new JCsvWriter<>);
    app.Run();
    return 0;
}
