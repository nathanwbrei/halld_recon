// $Id$
//
//    File: JEventProcessor_FCAL_TimingOffsets_Primex.h
// Created: Mon Dec 02 23:35:41 EST 2019
// Creator: andrsmit (on Linux ifarm1102 2.6.32-220.7.1.el6.x86_64 x86_64)
//

#ifndef _JEventProcessor_FCAL_TimingOffsets_Primex_
#define _JEventProcessor_FCAL_TimingOffsets_Primex_

#include <JANA/JEventProcessor.h>
#include <JANA/JApplication.h>

#include "TTree.h"
#include "TH1.h"
#include "TH2.h"

#include <PID/DBeamPhoton.h>
#include <PID/DEventRFBunch.h>

#include <FCAL/DFCALGeometry.h>
#include <FCAL/DFCALHit.h>
#include <FCAL/DFCALCluster.h>
#include <FCAL/DFCALShower.h>

#include <TOF/DTOFPoint.h>

#include <TRIGGER/DL1Trigger.h>
#include <TRIGGER/DTrigger.h>
#include <DAQ/Df250WindowRawData.h>
#include <DAQ/Df250PulseData.h>

#include <HDGEOMETRY/DGeometry.h>

#include "units.h"
#include "DLorentzVector.h"
#include "DVector3.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <thread>
#include <mutex>

using namespace jana;
using namespace std;


class JEventProcessor_FCAL_TimingOffsets_Primex:public JEventProcessor{
 	public:
		JEventProcessor_FCAL_TimingOffsets_Primex(){};
  		~JEventProcessor_FCAL_TimingOffsets_Primex(){};
  		const char* className(void){return "JEventProcessor_FCAL_TimingOffsets_Primex";}
	
 	private:
		jerror_t init(void);
  		jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);
  		jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);
  		jerror_t erun(void);
  		jerror_t fini(void);
		
		
		double targetZ, ccalZ, fcalZ;
		const double c = 29.9792;
		
		const double FCAL_C_EFFECTIVE             = 15.0;
		const double MIN_FCAL_ENERGY_TimingCalib  =  0.5; 
			/* Minimum energy of showers to use for timing calibration [GeV] */
		
		
		TH1I *h_fcal_rf_dt;
		TH2I *h_fcal_rf_dt_VS_chan;		
		TH1I *h_fcal_beam_dt;
		TH2I *h_fcal_beam_dt_VS_chan;
		
		
		
};

#endif // _JEventProcessor_FCAL_TimingOffsets_Primex_

