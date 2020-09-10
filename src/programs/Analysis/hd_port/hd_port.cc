
#include <JANA/JApplication.h>
#include <JANA/JCsvWriter.h>

#include <BCAL/DBCALCluster.h>
#include <BCAL/DBCALTDCHit.h>
#include <BCAL/DBCALTDCDigiHit.h>
#include <BCAL/DBCALGeometry.h>
#include <BCAL/DBCALIncidentParticle.h>
#include <BCAL/DBCALUnifiedHit.h>
#include <BCAL/DBCALTruthCell.h>
#include <BCAL/DBCALClump.h>
#include <BCAL/DBCALPoint.h>
#include <BCAL/DBCALSiPMHit.h>
#include <BCAL/DBCALDigiHit.h>
#include <BCAL/DBCALTruthShower.h>

#include <CDC/DCDCHit.h>
#include <CDC/DCDCDigiHit.h>

#include <CCAL/DCCALDigiHit.h>
#include <CCAL/DCCALRefDigiHit.h>
#include <CCAL/DCCALHit.h>

#include <DIRC/DDIRCLEDRef.h>
#include <DIRC/DDIRCTruthPmtHit.h>
#include <DIRC/DDIRCTDCDigiHit.h>
#include <DIRC/DDIRCPmtHit.h>
#include <DIRC/DDIRCGeometry.h>
#include <DIRC/DDIRCTruthBarHit.h>

#include <EVENTSTORE/DESSkimData.h>

#include <FCAL/DFCALHit.h>
#include <FCAL/DFCALDigiHit.h>

#include <FDC/DFDCCathodeDigiHit.h>
#include <FDC/DFDCWireDigiHit.h>
#include <FDC/DFDCHit.h>
#include <FDC/DFDCCathodeCluster.h>

#include <FMWPC/DFMWPCTruthHit.h>
#include <FMWPC/DFMWPCHit.h>

#include <PAIR_SPECTROMETER/DPSDigiHit.h>
#include <PAIR_SPECTROMETER/DPSCDigiHit.h>
#include <PAIR_SPECTROMETER/DPSCTDCDigiHit.h>
#include <PAIR_SPECTROMETER/DPSTruthHit.h>
#include <PAIR_SPECTROMETER/DPSCTruthHit.h>
#include <PAIR_SPECTROMETER/DPSPair.h>
#include <PAIR_SPECTROMETER/DPSCHit.h>
#include <PAIR_SPECTROMETER/DPSCPair.h>

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

#include <TPOL/DTPOLTruthHit.h>
#include <TPOL/DTPOLHit.h>
#include <TPOL/DTPOLSectorDigiHit.h>
#include <TPOL/DTPOLRingDigiHit.h>

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
#include <TTAB/DTTabUtilities.h>


int main(int argc, char* argv[]) {
	JApplication app;
    //app.Add(new JEventSourceGenerator_EVIO());
    //app.Add(new JCsvWriter<>);
    app.Run();
    return 0;
}

