// $Id$
//
//    File: DDetectorMatches_factory.h
// Created: Tue Aug  9 14:29:24 EST 2011
// Creator: pmatt (on Linux ifarml6 2.6.18-128.el5 x86_64)
//

#ifndef _DDetectorMatches_factory_
#define _DDetectorMatches_factory_

#include <iostream>
#include <iomanip>
#include <memory>

#include <JANA/JFactory.h>
#include <PID/DDetectorMatches.h>
#include <TRACKING/DTrackTimeBased.h>
#include <PID/DParticleID.h>
#include <TOF/DTOFPoint.h>
#include <BCAL/DBCALShower.h>
#include <FCAL/DFCALShower.h>
#include <DIRC/DDIRCPmtHit.h>
#include <DIRC/DDIRCTruthBarHit.h>
#include <TMath.h>

using namespace std;
using namespace jana;

class DDetectorMatches_factory : public jana::JFactory<DDetectorMatches>
{
	public:
		DDetectorMatches_factory(){};
		~DDetectorMatches_factory(){};

		//called by DDetectorMatches tag=Combo factory
		DDetectorMatches* Create_DDetectorMatches(jana::JEventLoop* locEventLoop, vector<const DTrackTimeBased*>& locTrackTimeBasedVector);

	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *locEventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *locEventLoop, uint64_t eventnumber);	///< Called every event.

		//matching tracks to hits/showers routines
		void MatchToTOF(const DParticleID* locParticleID, const DTrackTimeBased* locTrackTimeBased, const vector<const DTOFPoint*>& locTOFPoints, DDetectorMatches* locDetectorMatches) const;
		void MatchToBCAL(const DParticleID* locParticleID, const DTrackTimeBased* locTrackTimeBased, const vector<const DBCALShower*>& locBCALShowers, DDetectorMatches* locDetectorMatches) const;
		void MatchToFCAL(const DParticleID* locParticleID, const DTrackTimeBased* locTrackTimeBased, const vector<const DFCALShower*>& locFCALShowers, DDetectorMatches* locDetectorMatches) const;
		void MatchToSC(const DParticleID* locParticleID, const DTrackTimeBased* locTrackTimeBased, const vector<const DSCHit*>& locSCHits, DDetectorMatches* locDetectorMatches) const;
		void MatchToDIRC(const DParticleID* locParticleID, const DTrackTimeBased* locTrackTimeBased, const vector<const DDIRCPmtHit*>& locDIRCHits, DDetectorMatches* locDetectorMatches, const vector<const DDIRCTruthBarHit*>& locDIRCBarHits) const;

		void MatchToFCAL(const DParticleID* locParticleID,
				 const DTrackTimeBased *locTrackTimeBased,
				 vector<const DFCALHit *>&locSingleHits,
				 DDetectorMatches* locDetectorMatches) const;

		//matching showers to tracks routines
		void MatchToTrack(const DParticleID* locParticleID, const DBCALShower* locBCALShower, const vector<const DTrackTimeBased*>& locTrackTimeBasedVector, DDetectorMatches* locDetectorMatches) const;
		void MatchToTrack(const DParticleID* locParticleID, const DFCALShower* locFCALShower, const vector<const DTrackTimeBased*>& locTrackTimeBasedVector, DDetectorMatches* locDetectorMatches) const;

		bool ENABLE_FCAL_SINGLE_HITS;
};

#endif // _DDetectorMatches_factory_

