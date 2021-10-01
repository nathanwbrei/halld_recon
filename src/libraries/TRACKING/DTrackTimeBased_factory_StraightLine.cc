// $Id$
//
//    File: DTrackTimeBased_factory_StraightLine.cc
// Created: Wed Mar 13 10:00:17 EDT 2019
// Creator: staylor (on Linux ifarm1402.jlab.org 3.10.0-327.el7.x86_64 x86_64)
//


#include <iostream>
#include <iomanip>
using namespace std;

#include "DTrackTimeBased_factory_StraightLine.h"

#include <JANA/JEvent.h>
#include "DANA/DGeometryManager.h"
#include "HDGEOMETRY/DGeometry.h"

#include <CDC/DCDCTrackHit.h>
#include <FDC/DFDCPseudo.h>
#include <BCAL/DBCALShower.h>
#include <FCAL/DFCALShower.h>
#include <TOF/DTOFPoint.h>
#include <START_COUNTER/DSCHit.h>



//------------------
// Init
//------------------
void DTrackTimeBased_factory_StraightLine::Init()
{
  auto app = GetApplication();
  CDC_MATCH_CUT=2.;
  app->SetDefaultParameter("TRKFIT:CDC_MATCH_CUT",CDC_MATCH_CUT); 
  FDC_MATCH_CUT=1.25;
  app->SetDefaultParameter("TRKFIT:FDC_MATCH_CUT",FDC_MATCH_CUT); 
}

//------------------
// BeginRun
//------------------
void DTrackTimeBased_factory_StraightLine::BeginRun(const std::shared_ptr<const JEvent>& event)
{
  // Get the geometry
  auto runnumber = event->GetRunNumber();
  auto app = event->GetJApplication();
  auto geo_manager = app->GetService<DGeometryManager>();
  auto geom = geo_manager->GetDGeometry(runnumber);

  // Get the particle ID algorithms
  event->GetSingle(dPIDAlgorithm);
  
  // Outer detector geometry parameters
  if (geom->GetDIRCZ(dDIRCz)==false) dDIRCz=1000.;
  geom->GetFCALZ(dFCALz); 
  vector<double>tof_face;
  geom->Get("//section/composition/posXYZ[@volume='ForwardTOF']/@X_Y_Z",
	    tof_face);
  vector<double>tof_plane;  
  geom->Get("//composition[@name='ForwardTOF']/posXYZ[@volume='forwardTOF']/@X_Y_Z/plane[@value='0']", tof_plane);
  dTOFz=tof_face[2]+tof_plane[2]; 
  geom->Get("//composition[@name='ForwardTOF']/posXYZ[@volume='forwardTOF']/@X_Y_Z/plane[@value='1']", tof_plane);
  dTOFz+=tof_face[2]+tof_plane[2];
  dTOFz*=0.5;  // mid plane between tof planes
  
  // Get start counter geometry;
  if (geom->GetStartCounterGeom(sc_pos,sc_norm)){
    // Create vector of direction vectors in scintillator planes
    for (int i=0;i<30;i++){
      vector<DVector3>temp;
      for (unsigned int j=0;j<sc_pos[i].size()-1;j++){
	double dx=sc_pos[i][j+1].x()-sc_pos[i][j].x();
	double dy=sc_pos[i][j+1].y()-sc_pos[i][j].y();
	double dz=sc_pos[i][j+1].z()-sc_pos[i][j].z();
	temp.push_back(DVector3(dx/dz,dy/dz,1.));
      }
      sc_dir.push_back(temp);
    }
    SC_END_NOSE_Z=sc_pos[0][12].z();
    SC_BARREL_R=sc_pos[0][0].Perp();
    SC_PHI_SECTOR1=sc_pos[0][0].Phi();
  }

  // Get pointer to TrackFinder object 
  vector<const DTrackFinder *> finders;
  event->Get(finders);
  
  if(finders.size()<1){
    _DBG_<<"Unable to get a DTrackFinder object!"<<endl;
    return; // RESOURCE_UNAVAILABLE;
  }
  
   // Drop the const qualifier from the DTrackFinder pointer
  finder = const_cast<DTrackFinder*>(finders[0]);

  // Get pointer to DTrackFitter object that actually fits a track
  vector<const DTrackFitter *> fitters;
  event->Get(fitters,"StraightTrack");
  if(fitters.size()<1){
    _DBG_<<"Unable to get a DTrackFitter object!"<<endl;
    return; // RESOURCE_UNAVAILABLE;
  }
  
  // Drop the const qualifier from the DTrackFitter pointer
  fitter = const_cast<DTrackFitter*>(fitters[0]);
}

//------------------
// Process
//------------------
void DTrackTimeBased_factory_StraightLine::Process(const std::shared_ptr<const JEvent>& event)
{
  // Get wire-based tracks
  vector<const DTrackWireBased*> tracks;
  event->Get(tracks);

  // Get hits
  vector<const DCDCTrackHit *>cdchits;
  event->Get(cdchits);
  vector<const DFDCPseudo *>fdchits;
  event->Get(fdchits);
  
  // get start counter hits
  vector<const DSCHit*>sc_hits;
  event->Get(sc_hits);
  
  // Get TOF points
  vector<const DTOFPoint*> tof_points;
  event->Get(tof_points);

  // Get BCAL and FCAL showers
  vector<const DBCALShower*>bcal_showers;
  event->Get(bcal_showers);

  vector<const DFCALShower*>fcal_showers;
  event->Get(fcal_showers);

  // Start with wire-based results, refit with drift times 
  for (unsigned int i=0;i<tracks.size();i++){
    // Reset the fitter
    fitter->Reset();
    fitter->SetFitType(DTrackFitter::kTimeBased); 

    const DTrackWireBased *track = tracks[i];
    DVector3 pos=track->position();
    DVector3 dir=track->momentum();
    // Select hits that belong to the track
    for (unsigned int j=0;j<cdchits.size();j++){
      double d=finder->FindDoca(pos,dir,cdchits[j]->wire->origin,
				cdchits[j]->wire->udir);
      if (d<CDC_MATCH_CUT) fitter->AddHit(cdchits[j]);
    }
    for (unsigned int i=0;i<fdchits.size();i++){
      double pz=dir.z();
      double tx=dir.x()/pz;
      double ty=dir.y()/pz;
      double dz=fdchits[i]->wire->origin.z()-pos.z();
      DVector2 predpos(pos.x()+tx*dz,pos.y()+ty*dz);
      DVector2 diff=predpos-fdchits[i]->xy;
      if (diff.Mod()<FDC_MATCH_CUT) fitter->AddHit(fdchits[i]);
    }
    
    // Estimate t0 for this track
    double t0=0.;
    DetectorSystem_t t0_detector=SYS_NULL;
    GetStartTime(track,sc_hits,tof_points,bcal_showers,fcal_showers,t0,
		 t0_detector);

    // Fit the track using the list of hits we gathered above
    if (fitter->FitTrack(pos,dir,1.,0.,t0,t0_detector)==DTrackFitter::kFitSuccess){
      DTrackTimeBased *timebased_track = new DTrackTimeBased();
      timebased_track->candidateid=track->candidateid;
      *static_cast<DTrackingData*>(timebased_track) = fitter->GetFitParameters();
      timebased_track->chisq = fitter->GetChisq();
      timebased_track->Ndof = fitter->GetNdof();
      timebased_track->setPID(PiPlus);
      timebased_track->FOM = TMath::Prob(timebased_track->chisq, timebased_track->Ndof);
      timebased_track->pulls =std::move(fitter->GetPulls());
      timebased_track->extrapolations=std::move(fitter->GetExtrapolations());
      timebased_track->IsSmoothed = fitter->GetIsSmoothed();
      
      // Add hits used as associated objects
      vector<const DCDCTrackHit*> cdchits_on_track = fitter->GetCDCFitHits();
      vector<const DFDCPseudo*> fdchits_on_track = fitter->GetFDCFitHits();
      
      for (unsigned int k=0;k<cdchits_on_track.size();k++){
	timebased_track->AddAssociatedObject(cdchits_on_track[k]);
      }
      for (unsigned int k=0;k<fdchits_on_track.size();k++){
	timebased_track->AddAssociatedObject(fdchits_on_track[k]);
      }
      timebased_track->measured_cdc_hits_on_track = cdchits_on_track.size();
      timebased_track->measured_fdc_hits_on_track = fdchits_on_track.size();
      
      timebased_track->AddAssociatedObject(track);
      timebased_track->dCDCRings = dPIDAlgorithm->Get_CDCRingBitPattern(cdchits_on_track);
      timebased_track->dFDCPlanes = dPIDAlgorithm->Get_FDCPlaneBitPattern(fdchits_on_track);
      
      // TODO: figure out the potential hits on straight line tracks
      timebased_track->potential_cdc_hits_on_track = 0;
      timebased_track->potential_fdc_hits_on_track = 0;
      
      Insert(timebased_track);
      
    }
  }
}

//------------------
// EndRun
//------------------
void DTrackTimeBased_factory_StraightLine::EndRun()
{
}

//------------------
// Finish
//------------------
void DTrackTimeBased_factory_StraightLine::Finish()
{
}

// Get an estimate for the start time for this track
void 
DTrackTimeBased_factory_StraightLine::GetStartTime(const DTrackWireBased *track,
						   vector<const DSCHit*>&sc_hits,
						   vector<const DTOFPoint*>&tof_points,
						   vector<const DBCALShower*>&bcal_showers,	
						   vector<const DFCALShower*>&fcal_showers,
						   double &t0,DetectorSystem_t &t0_detector) const {
  t0=track->t0();
  t0_detector=track->t0_detector();
  double track_t0=t0;
  double locStartTime = track_t0;  // initial guess from tracking
 
  // Get start time estimate from Start Counter
  if (dPIDAlgorithm->Get_StartTime(track->extrapolations.at(SYS_START),sc_hits,locStartTime)){
    t0=locStartTime;
    t0_detector=SYS_START;
    return;
  }

  // Get start time estimate from TOF
  locStartTime = track_t0;  // initial guess from tracking
  if (dPIDAlgorithm->Get_StartTime(track->extrapolations.at(SYS_TOF),tof_points,locStartTime)){
    t0=locStartTime;
    t0_detector=SYS_TOF;
    return;
  }

  // Get start time estimate from FCAL
  locStartTime = track_t0;  // Initial guess from tracking
  if (dPIDAlgorithm->Get_StartTime(track->extrapolations.at(SYS_FCAL),fcal_showers,locStartTime)){
    t0=locStartTime;
    t0_detector=SYS_FCAL;
    return;
  }

  // Get start time estimate from BCAL
  locStartTime=track_t0;
  if (dPIDAlgorithm->Get_StartTime(track->extrapolations.at(SYS_BCAL),bcal_showers,locStartTime)){
    t0=locStartTime;
    t0_detector=SYS_BCAL;
    return;
  }
}
