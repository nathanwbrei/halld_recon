// $Id$
//
//    File: JEventProcessor_FCAL_Pi0TOF.h
// Created: Wed Aug 30 16:23:49 EDT 2017
// Creator: mstaib (on Linux egbert 2.6.32-696.10.1.el6.x86_64 x86_64)
//

#ifndef _JEventProcessor_FCAL_Pi0TOF_
#define _JEventProcessor_FCAL_Pi0TOF_
#include <JANA/JApplication.h>
#include <JANA/JEventProcessor.h>
#include <PID/DNeutralParticle.h>
#include <FCAL/DFCALShower.h>
#include <FCAL/DFCALCluster.h>
#include <HDGEOMETRY/DGeometry.h>
#include <FCAL/DFCALGeometry.h>
#include <TOF/DTOFPoint.h>
#include "units.h"
#include "DLorentzVector.h"
#include "DVector3.h"
#include "HistogramTools.h"
#include "DANA/DApplication.h"
#include "DANA/DStatusBits.h"
#include "TProfile.h"
#include "DVector3.h"
#include "PID/DVertex.h"

using namespace std;

class JEventProcessor_FCAL_Pi0TOF:public jana::JEventProcessor{
	public:
		JEventProcessor_FCAL_Pi0TOF();
		~JEventProcessor_FCAL_Pi0TOF();
		const char* className(void){return "JEventProcessor_FCAL_Pi0TOF";}

	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
		jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.
		
		//vector<const DTOFPoint*> tof_points;
		double bar2x( int bar );
		int TOF_Match(double kinVertexX, double kinVertexY, double kinVertexZ, double x, double y, double z);
                double m_beamSpotX;
                double m_beamSpotY;
                double m_targetZ;
		TH1F *imgg[3000];

      TProfile *hCurrentGainConstants;
};

#endif // _JEventProcessor_FCAL_Pi0TOF_
