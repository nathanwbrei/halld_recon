// $Id$
//
//		File: DDetectorMatches_factory.cc
// Created: Tue Aug	9 14:29:24 EST 2011
// Creator: pmatt (on Linux ifarml6 2.6.18-128.el5 x86_64)
//

#include "DDetectorMatches_factory.h"

//------------------
// init
//------------------
jerror_t DDetectorMatches_factory::init(void)
{
  ENABLE_FCAL_SINGLE_HITS = false;
  gPARMS->SetDefaultParameter("PID:ENABLE_FCAL_SINGLE_HITS",ENABLE_FCAL_SINGLE_HITS);	


	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t DDetectorMatches_factory::brun(jana::JEventLoop *locEventLoop, int32_t runnumber)
{
	//LEAVE THIS EMPTY!!! OR ELSE WON'T BE INITIALIZED PROPERLY WHEN "COMBO" FACTORY CALLS Create_DDetectorMatches ON REST DATA!!
	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t DDetectorMatches_factory::evnt(jana::JEventLoop* locEventLoop, uint64_t eventnumber)
{
	vector<const DTrackTimeBased*> locTrackTimeBasedVector;
	locEventLoop->Get(locTrackTimeBasedVector);

	DDetectorMatches* locDetectorMatches = Create_DDetectorMatches(locEventLoop, locTrackTimeBasedVector);
	_data.push_back(locDetectorMatches);

	return NOERROR;
}

DDetectorMatches* DDetectorMatches_factory::Create_DDetectorMatches(jana::JEventLoop* locEventLoop, vector<const DTrackTimeBased*>& locTrackTimeBasedVector)
{
	const DParticleID* locParticleID = NULL;
	locEventLoop->GetSingle(locParticleID);

	vector<const DSCHit*> locSCHits;
	locEventLoop->Get(locSCHits);

	vector<const DTOFPoint*> locTOFPoints;
	locEventLoop->Get(locTOFPoints);

	vector<const DFCALShower*> locFCALShowers;
	locEventLoop->Get(locFCALShowers);

	vector<const DBCALShower*> locBCALShowers;
	locEventLoop->Get(locBCALShowers);

	vector<const DDIRCPmtHit*> locDIRCHits;
	locEventLoop->Get(locDIRCHits);

	// cheat and get truth info of track at bar
	vector<const DDIRCTruthBarHit*> locDIRCBarHits;
	locEventLoop->Get(locDIRCBarHits);

	DDetectorMatches* locDetectorMatches = new DDetectorMatches();

	//Match tracks to showers/hits
	for(size_t loc_i = 0; loc_i < locTrackTimeBasedVector.size(); ++loc_i)
	{
		MatchToBCAL(locParticleID, locTrackTimeBasedVector[loc_i], locBCALShowers, locDetectorMatches);
		MatchToTOF(locParticleID, locTrackTimeBasedVector[loc_i], locTOFPoints, locDetectorMatches);
		MatchToFCAL(locParticleID, locTrackTimeBasedVector[loc_i], locFCALShowers, locDetectorMatches);
		MatchToSC(locParticleID, locTrackTimeBasedVector[loc_i], locSCHits, locDetectorMatches);
		MatchToDIRC(locParticleID, locTrackTimeBasedVector[loc_i], locDIRCHits, locDetectorMatches, locDIRCBarHits);
	}

	//Find nearest tracks to showers
	for(size_t loc_i = 0; loc_i < locBCALShowers.size(); ++loc_i)
		MatchToTrack(locParticleID, locBCALShowers[loc_i], locTrackTimeBasedVector, locDetectorMatches);
	for(size_t loc_i = 0; loc_i < locFCALShowers.size(); ++loc_i)
		MatchToTrack(locParticleID, locFCALShowers[loc_i], locTrackTimeBasedVector, locDetectorMatches);

	// Try to find matches between tracks and single hits in FCAL
	if (ENABLE_FCAL_SINGLE_HITS){
	  vector<const DFCALHit*> locFCALHits;
	  locEventLoop->Get(locFCALHits);
	  if (locFCALHits.size()>0){
	    vector<const DFCALHit*>locSingleHits;
	    locParticleID->GetSingleFCALHits(locFCALShowers,locFCALHits,
					     locSingleHits);
	    
	    for (size_t loc_j=0;loc_j<locTrackTimeBasedVector.size();loc_j++){
	      MatchToFCAL(locParticleID,locTrackTimeBasedVector[loc_j],
			  locSingleHits,locDetectorMatches);
	    }
	  }
	}

	//Set flight-time/p correlations
	for(size_t loc_i = 0; loc_i < locTrackTimeBasedVector.size(); ++loc_i)
	{
		double locFlightTimePCorrelation = locParticleID->Calc_BCALFlightTimePCorrelation(locTrackTimeBasedVector[loc_i], locDetectorMatches);
		if(isfinite(locFlightTimePCorrelation))
			locDetectorMatches->Set_FlightTimePCorrelation(locTrackTimeBasedVector[loc_i], SYS_BCAL, locFlightTimePCorrelation);

		locFlightTimePCorrelation = locParticleID->Calc_TOFFlightTimePCorrelation(locTrackTimeBasedVector[loc_i], locDetectorMatches);
		if(isfinite(locFlightTimePCorrelation))
			locDetectorMatches->Set_FlightTimePCorrelation(locTrackTimeBasedVector[loc_i], SYS_TOF, locFlightTimePCorrelation);

		locFlightTimePCorrelation = locParticleID->Calc_FCALFlightTimePCorrelation(locTrackTimeBasedVector[loc_i], locDetectorMatches);
		if(isfinite(locFlightTimePCorrelation))
			locDetectorMatches->Set_FlightTimePCorrelation(locTrackTimeBasedVector[loc_i], SYS_FCAL, locFlightTimePCorrelation);

		locFlightTimePCorrelation = locParticleID->Calc_SCFlightTimePCorrelation(locTrackTimeBasedVector[loc_i], locDetectorMatches);
		if(isfinite(locFlightTimePCorrelation))
			locDetectorMatches->Set_FlightTimePCorrelation(locTrackTimeBasedVector[loc_i], SYS_START, locFlightTimePCorrelation);
	}

	return locDetectorMatches;
}

void DDetectorMatches_factory::MatchToBCAL(const DParticleID* locParticleID, const DTrackTimeBased* locTrackTimeBased, const vector<const DBCALShower*>& locBCALShowers, DDetectorMatches* locDetectorMatches) const
{
  vector<DTrackFitter::Extrapolation_t>extrapolations=locTrackTimeBased->extrapolations.at(SYS_BCAL);
	if (extrapolations.size()==0) return;

	double locInputStartTime = locTrackTimeBased->t0();
	for(size_t loc_i = 0; loc_i < locBCALShowers.size(); ++loc_i)
	{
	  shared_ptr<DBCALShowerMatchParams> locShowerMatchParams;
	  if(locParticleID->Cut_MatchDistance(extrapolations, locBCALShowers[loc_i], locInputStartTime, locShowerMatchParams))
	    locDetectorMatches->Add_Match(locTrackTimeBased, locBCALShowers[loc_i], locShowerMatchParams);
	}
}

void DDetectorMatches_factory::MatchToTOF(const DParticleID* locParticleID, const DTrackTimeBased* locTrackTimeBased, const vector<const DTOFPoint*>& locTOFPoints, DDetectorMatches* locDetectorMatches) const
{
  vector<DTrackFitter::Extrapolation_t> extrapolations=locTrackTimeBased->extrapolations.at(SYS_TOF);
	if (extrapolations.size()==0) return;

	double locInputStartTime = locTrackTimeBased->t0();
	for(size_t loc_i = 0; loc_i < locTOFPoints.size(); ++loc_i)
	{
	  shared_ptr<DTOFHitMatchParams> locTOFHitMatchParams;
	  if(locParticleID->Cut_MatchDistance(extrapolations, locTOFPoints[loc_i], locInputStartTime, locTOFHitMatchParams))
	    locDetectorMatches->Add_Match(locTrackTimeBased, locTOFPoints[loc_i], locTOFHitMatchParams);
	}
}

void DDetectorMatches_factory::MatchToFCAL(const DParticleID* locParticleID, const DTrackTimeBased* locTrackTimeBased, const vector<const DFCALShower*>& locFCALShowers, DDetectorMatches* locDetectorMatches) const
{
  vector<DTrackFitter::Extrapolation_t> extrapolations=locTrackTimeBased->extrapolations.at(SYS_FCAL);
	if (extrapolations.size()==0) return;

	double locInputStartTime = locTrackTimeBased->t0();
	for(size_t loc_i = 0; loc_i < locFCALShowers.size(); ++loc_i)
	{
	  shared_ptr<DFCALShowerMatchParams>locShowerMatchParams;
	  if(locParticleID->Cut_MatchDistance(extrapolations, locFCALShowers[loc_i], locInputStartTime, locShowerMatchParams))
	    locDetectorMatches->Add_Match(locTrackTimeBased, locFCALShowers[loc_i], locShowerMatchParams);
	}
}

void DDetectorMatches_factory::MatchToSC(const DParticleID* locParticleID, const DTrackTimeBased* locTrackTimeBased, const vector<const DSCHit*>& locSCHits, DDetectorMatches* locDetectorMatches) const
{
  vector<DTrackFitter::Extrapolation_t> extrapolations=locTrackTimeBased->extrapolations.at(SYS_START);
	if (extrapolations.size()==0) return;

	double locInputStartTime = locTrackTimeBased->t0();
	for(size_t loc_i = 0; loc_i < locSCHits.size(); ++loc_i)
	{
	    shared_ptr<DSCHitMatchParams>locSCHitMatchParams;
	    if(locParticleID->Cut_MatchDistance(extrapolations, locSCHits[loc_i], locInputStartTime, locSCHitMatchParams, true))
	      locDetectorMatches->Add_Match(locTrackTimeBased, locSCHits[loc_i], locSCHitMatchParams);
	}
}

void DDetectorMatches_factory::MatchToDIRC(const DParticleID* locParticleID, const DTrackTimeBased* locTrackTimeBased, const vector<const DDIRCPmtHit*>& locDIRCHits, DDetectorMatches* locDetectorMatches, const vector<const DDIRCTruthBarHit*>& locDIRCBarHits) const
{
	vector<DTrackFitter::Extrapolation_t> extrapolations=locTrackTimeBased->extrapolations.at(SYS_DIRC);
	if(extrapolations.size()==0) return;

	double locInputStartTime = locTrackTimeBased->time();

	// objects to hold DIRC match parameters and links between tracks and DIRC hits
	shared_ptr<DDIRCMatchParams> locDIRCMatchParams;
	map<shared_ptr<const DDIRCMatchParams>, vector<const DDIRCPmtHit*> > locDIRCTrackMatchParams;
	locDetectorMatches->Get_DIRCTrackMatchParamsMap(locDIRCTrackMatchParams);

	// run DIRC LUT algorithm and add detector match
	if(locParticleID->Cut_MatchDIRC(extrapolations, locDIRCHits, locInputStartTime, locTrackTimeBased->PID(), locDIRCMatchParams, locDIRCBarHits, locDIRCTrackMatchParams))
		locDetectorMatches->Add_Match(locTrackTimeBased, locDIRCMatchParams);
}

void DDetectorMatches_factory::MatchToTrack(const DParticleID* locParticleID, const DBCALShower* locBCALShower, const vector<const DTrackTimeBased*>& locTrackTimeBasedVector, DDetectorMatches* locDetectorMatches) const
{
	double locMinDistance = 999.0;
	double locFinalDeltaPhi = 999.0, locFinalDeltaZ = 999.0;
	for(size_t loc_i = 0; loc_i < locTrackTimeBasedVector.size(); ++loc_i)
	{
		shared_ptr<DBCALShowerMatchParams> locShowerMatchParams;
		double locInputStartTime = locTrackTimeBasedVector[loc_i]->t0();
		
		map<DetectorSystem_t,vector<DTrackFitter::Extrapolation_t> >extrapolations=locTrackTimeBasedVector[loc_i]->extrapolations;
		if (extrapolations.size()==0) continue;

		if(!locParticleID->Distance_ToTrack(extrapolations.at(SYS_BCAL), locBCALShower, locInputStartTime, locShowerMatchParams))
			continue;

		double locRSq = locBCALShower->x*locBCALShower->x + locBCALShower->y*locBCALShower->y;
		double locDeltaPhi = locShowerMatchParams->dDeltaPhiToShower;
		double locDeltaZ = locShowerMatchParams->dDeltaZToShower;
		double locDistance = sqrt(locDeltaZ*locDeltaZ + locDeltaPhi*locDeltaPhi*locRSq);
		if(locDistance >= locMinDistance)
			continue;

		locMinDistance = locDistance;
		locFinalDeltaPhi = locDeltaPhi;
		locFinalDeltaZ = locDeltaZ;
	}
	locDetectorMatches->Set_DistanceToNearestTrack(locBCALShower, locFinalDeltaPhi, locFinalDeltaZ);
}

void DDetectorMatches_factory::MatchToTrack(const DParticleID* locParticleID, const DFCALShower* locFCALShower, const vector<const DTrackTimeBased*>& locTrackTimeBasedVector, DDetectorMatches* locDetectorMatches) const
{
	double locMinDistance = 999.0;
	for(size_t loc_i = 0; loc_i < locTrackTimeBasedVector.size(); ++loc_i)
	{
	  map<DetectorSystem_t,vector<DTrackFitter::Extrapolation_t> >extrapolations=locTrackTimeBasedVector[loc_i]->extrapolations;
	  if (extrapolations.size()==0) return;

	  shared_ptr<DFCALShowerMatchParams> locShowerMatchParams;
	  double locInputStartTime = locTrackTimeBasedVector[loc_i]->t0();
	  if(!locParticleID->Distance_ToTrack(extrapolations.at(SYS_FCAL), locFCALShower, locInputStartTime, locShowerMatchParams))
	    continue;
	  if(locShowerMatchParams->dDOCAToShower < locMinDistance)
			locMinDistance = locShowerMatchParams->dDOCAToShower;
	}
	locDetectorMatches->Set_DistanceToNearestTrack(locFCALShower, locMinDistance);
}

// Try to find matches between a track and a single hit in FCAL
void 
DDetectorMatches_factory::MatchToFCAL(const DParticleID* locParticleID,
				      const DTrackTimeBased *locTrackTimeBased,
				      vector<const DFCALHit *>&locSingleHits,
				      DDetectorMatches* locDetectorMatches) const {
  vector<DTrackFitter::Extrapolation_t> extrapolations=locTrackTimeBased->extrapolations.at(SYS_FCAL);
  if (extrapolations.size()==0) return;

  for (unsigned int i=0;i<locSingleHits.size();i++){
    double locDOCA=0.,locHitTime;
    if (locParticleID->Distance_ToTrack(locTrackTimeBased->t0(),
					extrapolations[0],locSingleHits[i],
					locDOCA,locHitTime)){
      shared_ptr<DFCALSingleHitMatchParams> locMatchParams=std::make_shared<DFCALSingleHitMatchParams>();
      
      locMatchParams->dEHit=locSingleHits[i]->E;
      locMatchParams->dTHit=locHitTime;
      locMatchParams->dFlightTime = extrapolations[0].t;
      locMatchParams->dFlightTimeVariance = 0.; // Fill this in!
      locMatchParams->dPathLength = extrapolations[0].s;
      locMatchParams->dDOCAToHit = locDOCA;
      
      locDetectorMatches->Add_Match(locTrackTimeBased,locMatchParams);
    }
  }
}

