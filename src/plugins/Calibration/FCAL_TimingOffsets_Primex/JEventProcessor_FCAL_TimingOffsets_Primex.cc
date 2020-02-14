
#include "JEventProcessor_FCAL_TimingOffsets_Primex.h"

extern "C"{
void InitPlugin(JApplication *app){
	InitJANAPlugin(app);
    	app->AddProcessor(new JEventProcessor_FCAL_TimingOffsets_Primex());
}
} // "C"




//------------------
// init
//------------------
jerror_t JEventProcessor_FCAL_TimingOffsets_Primex::init(void)
{
  	
	h_fcal_rf_dt         = new TH1I( "fcal_rf_dt", 
		"FCAL Hit Time - Event RF Time; t_{FCAL} - t_{RF} [ns]", 4000, -200., 200. );
	
	h_fcal_rf_dt_VS_chan = new TH2I( "fcal_rf_dt_VS_chan", 
		"FCAL Hit Time - Event RF Time; FCAL Channel Number; t_{FCAL} - t_{RF} [ns]", 
		2800, -0.5, 2799.5, 1000, -50., 50. );
	
	
	h_fcal_beam_dt 	       = new TH1I( "fcal_beam_dt", 
		"FCAL Hit Time - Beam Time; t_{FCAL} - t_{#gamma} [ns]", 4000, -200., 200. );
	
	h_fcal_beam_dt_VS_chan = new TH2I( "fcal_beam_dt_VS_chan", 
		"FCAL Hit Time - Beam Photon Time; FCAL Channel Number; t_{FCAL} - t_{#gamma} [ns]", 
		2800, -0.5, 2799.5, 1000, -50., 50. );
	
	
	
	
	
  	return NOERROR;
}




//------------------
// brun
//------------------
jerror_t JEventProcessor_FCAL_TimingOffsets_Primex::brun(JEventLoop *eventLoop, int32_t runnumber)
{
	
	DGeometry*   dgeom = NULL;
  	DApplication* dapp = dynamic_cast< DApplication* >( eventLoop->GetJApplication() );
  	if( dapp )   dgeom = dapp->GetDGeometry( runnumber );
   
  	if( dgeom ){
    	  	dgeom->GetTargetZ( targetZ );
		dgeom->GetFCALZ( fcalZ );
  	} else{
    	  	cerr << "No geometry accessbile to ccal_timing monitoring plugin." << endl;
    	  	return RESOURCE_UNAVAILABLE;
  	}
	
	
	
  	return NOERROR;
}




//------------------
// evnt
//------------------
jerror_t JEventProcessor_FCAL_TimingOffsets_Primex::evnt(JEventLoop *eventLoop, uint64_t eventnumber)
{
	
	
	const DL1Trigger *trig = NULL;
  	try {
      	  	eventLoop->GetSingle(trig);
  	} catch (...) {}
	if (trig == NULL) { return NOERROR; }
	
	uint32_t fp_trigmask = trig->fp_trig_mask;
	if( fp_trigmask ) return NOERROR;
	
	
	
	
	
	
	//---------------   FCAL-RF Timing Shifts   ---------------//	
	
	
	const DEventRFBunch *locRFBunch = NULL;
	try { 
	  	eventLoop->GetSingle( locRFBunch, "CalorimeterOnly" );
	} catch (...) { return NOERROR; }
	double rfTime = locRFBunch->dTime;
	if( locRFBunch->dNumParticleVotes < 2 ) return NOERROR;
	
	
	
	vector< const DFCALShower* > fcal_showers;
	vector< const DBeamPhoton* > beam_photons;
	eventLoop->Get( fcal_showers );
	eventLoop->Get( beam_photons );
	
	
	
	
	
	DVector3 vertex;
	vertex.SetXYZ( 0., 0., targetZ );  // for now we assume vertex is at center of target
	
	
	
	
	
	
	japp->RootFillLock(this);  // Acquire root lock
	
	
	for( vector< const DFCALShower* >::const_iterator show = fcal_showers.begin(); 
		show != fcal_showers.end(); show++ ) {
		
		
		double energy = (*show)->getEnergy();
		if( energy < MIN_FCAL_ENERGY_TimingCalib ) continue;
		
		double pos_corrected_z = (*show)->getPosition().Z();
		
		
		
		//----------   Loop over hits belonging to this shower   ----------//
		
		vector< const DFCALCluster* > clusterVec;
        	(*show)->Get( clusterVec );
		if( clusterVec.size() != 1 ) continue;
		
		const DFCALCluster *clus = clusterVec[0];
		const vector< DFCALCluster::DFCALClusterHit_t > hitVector = clus->GetHits();
		
		
		double maxHitE =  0.;
		int hitIndex   = -1;
		
		// find the hit with maximum energy deposition:
		
		for( unsigned int iHit = 0; iHit < hitVector.size(); iHit++ ) { 
			
			double hitEnergy = hitVector[iHit].E;
			if( hitEnergy > maxHitE ) { 
				maxHitE = hitEnergy;
				hitIndex = iHit;
			}
			
		}
		
		DFCALCluster::DFCALClusterHit_t fcalHit = hitVector[hitIndex];
		
		
		double  hitE      =  fcalHit.E;		
		double clusE      =  clus->getEnergy();
		double Emaxr      =  hitE / clusE;
		
		if( Emaxr < 0.6 ) continue; // this cut may not be necessary
		
		
		
		int ChannelNumber =  fcalHit.ch;
		double chanx      =  fcalHit.x       - vertex.X();
		double chany      =  fcalHit.y       - vertex.Y();
		double chanz      =  pos_corrected_z - vertex.Z();
		double hitTime    =  fcalHit.t;	
		
		double dR = sqrt( chanx*chanx + chany*chany + chanz*chanz );
		
		
		
		// propagate hit time to the interaction vertex:
		
		double tCorr   = ( fcalZ + DFCALGeometry::blockLength() - pos_corrected_z ) 
			/ FCAL_C_EFFECTIVE;
		
		hitTime = hitTime - tCorr - (dR/c);
		
		
		
		
		double fcal_rf_dt = hitTime - rfTime;
		
		h_fcal_rf_dt->Fill( fcal_rf_dt );
		h_fcal_rf_dt_VS_chan->Fill( ChannelNumber, fcal_rf_dt );
		
		
		
		
		// Also look at time difference between beam photon:
		
		for( unsigned int ib = 0; ib < beam_photons.size(); ib++ ) {
			
			double beam_time    = beam_photons[ib]->time();
			double fcal_beam_dt = hitTime - beam_time;
			
			h_fcal_beam_dt->Fill( fcal_beam_dt );
			h_fcal_beam_dt_VS_chan->Fill( ChannelNumber, fcal_beam_dt );
			
		}
		
	}
	
	
	
	japp->RootFillUnLock(this);  // Release root lock
	
	
	
	


  	return NOERROR;
}




//------------------
// erun
//------------------
jerror_t JEventProcessor_FCAL_TimingOffsets_Primex::erun(void)
{
  	
  	return NOERROR;
}




//------------------
// fini
//------------------
jerror_t JEventProcessor_FCAL_TimingOffsets_Primex::fini(void)
{
	
  	return NOERROR;
}


