
#include <JANA/JApplication.h>
#include <JANA/JCsvWriter.h>

#include <TTAB/DTranslationTable.h>

#include <TAGGER/DTAGHHit.h>

#include <TAGGER/DTAGHDigiHit.h>
#include <TAGGER/DTAGMHit.h>
#include <TAGGER/DTAGMDigiHit.h>
#include <TAGGER/DTAGMTDCDigiHit.h>
#include <TAGGER/DTAGHTDCDigiHit.h>

#include <TTAB/DTranslationTable.h>
#include <TTAB/DTTabUtilities.h>

#include <FDC/DFDCCathodeDigiHit.h>
#include <FDC/DFDCWireDigiHit.h>
//#include <FDC/DFDCIntersection.h>
#include <FDC/DFDCHit.h>
#include <FDC/DFDCCathodeCluster.h>


#include <TRD/DGEMPoint.h>
#include <TRD/DTRDStripCluster.h>
#include <TRD/DGEMHit.h>
#include <TRD/DTRDDigiHit.h>
#include <TRD/DGEMDigiWindowRawData.h>
#include <TRD/DGEMStripCluster.h>
#include <TRD/DTRDHit.h>
#include <TRD/DTRDPoint.h>

#include <TPOL/DTPOLTruthHit.h>
#include <TPOL/DTPOLHit.h>
#include <TPOL/DTPOLSectorDigiHit.h>
#include <TPOL/DTPOLRingDigiHit.h>
		
int main(int argc, char* argv[]) {
	JApplication app;
    //app.Add(new JEventSourceGenerator_EVIO());
    //app.Add(new JCsvWriter<>);
    app.Run();
    return 0;
}

