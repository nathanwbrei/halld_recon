
/*
 *  File: DCCALHit_factory.cc
 *
 * Created on 11/25/18 by A.S.
 * use structure similar to FCAL
 */

#include "CCAL/DCCALShower_factory.h"


static mutex CCAL_MUTEX;
static bool CCAL_PROFILE_LOADED = false;


//==========================================================
//
//   LoadCCALProfileData
//
//==========================================================

bool DCCALShower_factory::LoadCCALProfileData(JApplication *japp, int32_t runnumber)
{

	if( CCAL_PROFILE_LOADED )
	  return true;
	else
	  CCAL_PROFILE_LOADED = true;

	string ccal_profile_file;
	gPARMS->SetDefaultParameter("CCAL_PROFILE_FILE", ccal_profile_file, "CCAL profile data file name");
		
	// make sure we have a pointer to the JApplication
	if(japp == nullptr)
		return false;
	
	// follow similar procedure as other resources (DMagneticFieldMapFineMesh)
	map<string,string> profile_file_name;
	JCalibration *jcalib = japp->GetJCalibration(runnumber);
	if(jcalib->GetCalib("/CCAL/profile_data/profile_data_map", profile_file_name)) 
		jout << "Can't find requested /CCAL/profile_data/profile_data_map in CCDB for this run!" << endl;
	else if(profile_file_name.find("map_name") != profile_file_name.end() 
				&& profile_file_name["map_name"] != "None") {
		JResourceManager *jresman = japp->GetJResourceManager(runnumber);
		ccal_profile_file = jresman->GetResource(profile_file_name["map_name"]);
	}
	
	jout<<"Reading CCAL profile data from "<<ccal_profile_file<<" ..."<<endl;
	
	// check to see if we actually have a file
	if(ccal_profile_file.empty()) {
		jerr << "Empty file..." << endl;
		return false;
	}
	
	ifstream ccal_profile(ccal_profile_file.c_str());
	for(int i=0; i<=500; i++) {
		for(int j=0; j<=i; j++) {
			int id1, id2;
			double fcell_hyc, fd2c;
			
			ccal_profile >> id1 >> id2 >> fcell_hyc >> fd2c;
		
			acell[id1][id2] = fcell_hyc;
			acell[id2][id1] = fcell_hyc;
			ad2c[id1][id2] = fd2c;
			ad2c[id2][id1] = fd2c;
		}
	}
	ccal_profile.close();
	
	return true;
}




//==========================================================
//
//   Constructor
//
//==========================================================

DCCALShower_factory::DCCALShower_factory()
{
	// Set defaults:
	
    	MIN_CLUSTER_BLOCK_COUNT   =  2;
    	MIN_CLUSTER_SEED_ENERGY   =  0.035;  // GeV
	MIN_CLUSTER_ENERGY        =  0.05;   // GeV
	MAX_CLUSTER_ENERGY        =  15.9;   // GeV
	TIME_CUT                  =  15.0;   // ns
	MAX_HITS_FOR_CLUSTERING   =  80;
	
	SHOWER_DEBUG              =  0;
	DO_NONLINEAR_CORRECTION   =  1;
	
	CCAL_RADIATION_LENGTH     =  0.86;
	CCAL_CRITICAL_ENERGY      =  1.1e-3;
	

	gPARMS->SetDefaultParameter("CCAL:SHOWER_DEBUG",   SHOWER_DEBUG);
	gPARMS->SetDefaultParameter("CCAL:MIN_CLUSTER_BLOCK_COUNT", MIN_CLUSTER_BLOCK_COUNT);
	gPARMS->SetDefaultParameter("CCAL:MIN_CLUSTER_SEED_ENERGY", MIN_CLUSTER_SEED_ENERGY);
	gPARMS->SetDefaultParameter("CCAL:MIN_CLUSTER_ENERGY", MIN_CLUSTER_ENERGY);
	gPARMS->SetDefaultParameter("CCAL:MAX_CLUSTER_ENERGY", MAX_CLUSTER_ENERGY);
	gPARMS->SetDefaultParameter("CCAL:MAX_HITS_FOR_CLUSTERING", MAX_HITS_FOR_CLUSTERING);
	gPARMS->SetDefaultParameter("CCAL:TIME_CUT",TIME_CUT,"time cut for associating CCAL hits together into a cluster");
	gPARMS->SetDefaultParameter("CCAL:DO_NONLINEAR_CORRECTION", DO_NONLINEAR_CORRECTION);
	gPARMS->SetDefaultParameter("CCAL:CCAL_RADIATION_LENGTH", CCAL_RADIATION_LENGTH);
	gPARMS->SetDefaultParameter("CCAL:CCAL_CRITICAL_ENERGY", CCAL_CRITICAL_ENERGY);

}




//==========================================================
//
//   brun
//
//==========================================================

jerror_t DCCALShower_factory::brun(JEventLoop *locEventLoop, int32_t runnumber)
{

	DApplication *dapp = dynamic_cast<DApplication*>(eventLoop->GetJApplication());
    	const DGeometry *geom = dapp->GetDGeometry(runnumber);
	
	if (geom) {
      	  geom->GetTargetZ(m_zTarget);
      	  geom->GetCCALZ(m_CCALfront);
    	}
    	else{
      	  cerr << "No geometry accessbile." << endl;
      	  return RESOURCE_UNAVAILABLE;
    	}
	
	
	
	//------------------------------------------------------------
	// read in shower profile data
	
	std::unique_lock<std::mutex> lck(CCAL_MUTEX);
	//LoadCCALProfileData(eventLoop->GetJApplication(), runnumber);
	
	string ccal_profile_file;
	gPARMS->SetDefaultParameter("CCAL_PROFILE_FILE", ccal_profile_file, "CCAL profile data file name");
	
	// follow similar procedure as other resources (DMagneticFieldMapFineMesh)
	map<string,string> profile_file_name;
	JCalibration *jcalib = dapp->GetJCalibration(runnumber);
	if(jcalib->GetCalib("/CCAL/profile_data/profile_data_map", profile_file_name)) 
		jout << "Can't find requested /CCAL/profile_data/profile_data_map in CCDB for this run!" << endl;
	else if(profile_file_name.find("map_name") != profile_file_name.end() 
				&& profile_file_name["map_name"] != "None") {
		JResourceManager *jresman = dapp->GetJResourceManager(runnumber);
		ccal_profile_file = jresman->GetResource(profile_file_name["map_name"]);
	}
	
	jout<<"Reading CCAL profile data from "<<ccal_profile_file<<" ..."<<endl;
	
	// check to see if we actually have a file
	if(ccal_profile_file.empty()) {
		jerr << "Empty file..." << endl;
		return RESOURCE_UNAVAILABLE;
	}
	
	ifstream ccal_profile(ccal_profile_file.c_str());
	for(int i=0; i<=500; i++) {
		for(int j=0; j<=i; j++) {
			int id1, id2;
			double fcell_hyc, fd2c;
			
			ccal_profile >> id1 >> id2 >> fcell_hyc >> fd2c;
		
			acell[id1][id2] = fcell_hyc;
			acell[id2][id1] = fcell_hyc;
			ad2c[id1][id2] = fd2c;
			ad2c[id2][id1] = fd2c;
		}
	}
	ccal_profile.close();
	
	lck.unlock();
	
	
	
	
	//------------------------------------------------------------
	// initialize channel status array
	
	for(int icol = 0; icol < MCOL; ++icol) {
	  for(int irow = 0; irow < MROW; ++irow) {
	    if(icol>=5 && icol<=6 && irow>=5 && irow<=6) { stat_ch[irow][icol] = -1; }
	    else { stat_ch[irow][icol] = 0; }
	  }
	}



	//------------------------------------------------------------
	// read in shower timewalk parameters
	
	vector< vector<double> > timewalk_params;
	if( eventLoop->GetCalib("/CCAL/shower_timewalk_correction",timewalk_params) )
	  jout << "Error loading /CCAL/shower_timewalk_correction !" << endl;
	else {
	  if( (int)timewalk_params.size() != 2 ) {
	    cout << "DCCALShower_factory: Wrong number of entries to timewalk correction table (should be 144)." << endl;
	    for( int ii = 0; ii < 2; ++ii ) {
	      timewalk_p0.push_back(0.0);
	      timewalk_p1.push_back(0.0);
	      timewalk_p2.push_back(0.0);
	      timewalk_p3.push_back(0.0);
	    }
	  } else {
	    for( vector< vector<double> >::const_iterator iter = timewalk_params.begin(); iter != timewalk_params.end(); ++iter ) {
	      if( iter->size() != 4 ) {
	        cout << "DCCALShower_factory: Wrong number of values in timewalk correction table (should be 4)" << endl;
                continue;
	      }
	    
	      timewalk_p0.push_back( (*iter)[0] );
	      timewalk_p1.push_back( (*iter)[1] );
	      timewalk_p2.push_back( (*iter)[2] );
	      timewalk_p3.push_back( (*iter)[3] );
	    
	    }
	  }
	}


	//------------------------------------------------------------
	// read in the nonlinearity parameters:
	
	vector< vector<double> > nonlin_params;
	if( eventLoop->GetCalib("/CCAL/nonlinear_energy_correction",nonlin_params) )
	  jout << "Error loading /CCAL/nonlinear_energy_correction !" << endl;
	else {
	  if( (int)nonlin_params.size() != CCAL_CHANS ) {
	    cout << "DCCALShower_factory: Wrong number of entries to nonlinear energy correction table (should be 144)." << endl;
	    for( int ii = 0; ii < CCAL_CHANS; ++ii ) {
	      Nonlin_p0.push_back(0.0);
	      Nonlin_p1.push_back(0.0);
	      Nonlin_p2.push_back(0.0);
	      Nonlin_p3.push_back(0.0);
	    }
	  } else {
	    for( vector< vector<double> >::const_iterator iter = nonlin_params.begin(); iter != nonlin_params.end(); ++iter ) {
	      if( iter->size() != 4 ) {
	        cout << "DCCALShower_factory: Wrong number of values in nonlinear energy correction table (should be 4)" << endl;
                continue;
	      }
	    
	      Nonlin_p0.push_back( (*iter)[0] );
	      Nonlin_p1.push_back( (*iter)[1] );
	      Nonlin_p2.push_back( (*iter)[2] );
	      Nonlin_p3.push_back( (*iter)[3] );
	    
	    }
	  }
	}
	
	
	if( SHOWER_DEBUG ) {
	  cout << "\n\nNONLIN_P0  NONLIN_P1  NONLIN_P2  NONLIN_P3" << endl;
	  for(int ii = 0; ii < (int)Nonlin_p0.size(); ii++) {
	    cout << Nonlin_p0[ii] << " " << Nonlin_p1[ii] << " " << Nonlin_p2[ii] << " " << Nonlin_p3[ii] << endl;
	  }
	  cout << "\n\n";
	}
	
	

	return NOERROR;
}




//==========================================================
//
//   evnt
//
//==========================================================

jerror_t DCCALShower_factory::evnt(JEventLoop *locEventLoop, uint64_t eventnumber)
{
	
	
	//---------------------------------------------------------//
	//---------------   Get the CCAL Geometry   ---------------//
	
	vector< const DCCALGeometry* > ccalGeomVect;
    	locEventLoop->Get( ccalGeomVect );
	if (ccalGeomVect.size() < 1)
      	  return OBJECT_NOT_AVAILABLE;
    	const DCCALGeometry& ccalGeom = *(ccalGeomVect[0]);
	
	
	
	
	//---------------------------------------------------------//
	//------- Get the CCALHits and organize hit pattern -------//
	
	vector< const DCCALHit* > ccalhits;
	locEventLoop->Get( ccalhits );
	
	int n_hits = static_cast<int>( ccalhits.size() );
	if( n_hits < 1 || n_hits > MAX_HITS_FOR_CLUSTERING ) return NOERROR;
	
	
	vector< vector< const DCCALHit* > > hitPatterns;
	getHitPatterns( ccalhits, hitPatterns );
	
	int n_patterns = static_cast<int>( hitPatterns.size() );
	if( n_patterns < 1 ) return NOERROR;
	
	vector< ccalcluster_t > ccalClusters; // will hold all clusters
	vector< cluster_t > clusterStorage;   // will hold the constituents of every cluster
	
	
	
	
	//---------------------------------------------------------//
	//-------  Call island clusterizer on each pattern  -------//
	
	for( int ipat = 0; ipat < n_patterns; ipat++ ) 
	{
	
	  //----- prepare data in dimensionless format ----------//
	  
	  vector< const DCCALHit* > locHitPattern = hitPatterns[ipat];
	  int n_hits = static_cast<int>( locHitPattern.size() );
	  
	  vector< int > ia;
	  vector< int > id;
	  	  
	  for( int ih = 0; ih < n_hits; ih++ ) {
	    const DCCALHit *ccalhit = locHitPattern[ih];
	    int row = ccalhit->row;
	    int col = ccalhit->column;
	    int ie  = static_cast<int>( ccalhit->E*10. + 0.5 );
	    if( ie > 0 ) {
	      int address = 100*(col+1) + (row+1);
	      ia.push_back( address );
	      id.push_back( ie );
	    }	  
	  }
	  
	  //--------------  call to island  --------------//
	  
	  vector< gamma_t > gammas; // Output of main_island (holds reconstructed photons)
	  main_island( ia, id, gammas );
	
	
	  //----------  post-island processing  ----------//
	
	  int init_clusters = static_cast<int>( gammas.size() );
	  if( !init_clusters ) continue;
	
	  process_showers( gammas, ccalGeom, locHitPattern, ccalClusters, clusterStorage );
	
	}
	
	
	
	
	//---------------------------------------------------------//
	//--------------   Fill DCCALShower Object   --------------//
	
	int n_clusters = static_cast<int>( ccalClusters.size() );
	
	for( int k = 0; k < n_clusters; k++ ) {
	
	  DCCALShower *shower = new DCCALShower;
	  
	  shower->E        =   ccalClusters[k].E;
	  shower->x        =   ccalClusters[k].x;
	  shower->y        =   ccalClusters[k].y;
	  shower->z        =   m_CCALfront;
	  shower->x1       =   ccalClusters[k].x1;
	  shower->y1       =   ccalClusters[k].y1;
	  shower->time     =   ccalClusters[k].time;
	  shower->chi2     =   ccalClusters[k].chi2;
	  shower->type     =   ccalClusters[k].type;
	  shower->dime     =   ccalClusters[k].nhits;
	  shower->sigma_E  =   ccalClusters[k].sigma_E;
	  shower->Emax     =   ccalClusters[k].emax;
	  shower->id       =   ccalClusters[k].id;
	  shower->idmax    =   ccalClusters[k].idmax;
	  
	  for( int icell = 0; icell < ccalClusters[k].nhits; icell++ ) {
	    shower->id_storage[icell] = clusterStorage[k].id[icell];
	    shower->en_storage[icell] = clusterStorage[k].E[icell];
	    shower->t_storage[icell]  = clusterStorage[k].t[icell];
	    
	   // if(clusterStorage[k].E[icell] > 0.)  {   // maybe redundant if-statement???
  	   // 	shower->AddAssociatedObject(ccalhits[ clusterStorage[k].id[icell] ]);
	   //	}
	  }

	  _data.push_back( shower );
	}


	return NOERROR;
	
}




//==========================================================
//
//   getHitPatterns
//
//==========================================================

void DCCALShower_factory::getHitPatterns( vector< const DCCALHit* > hitarray, 
		vector< vector< const DCCALHit* > > &hitPatterns ) 
{

	/*
	Method for sorting hit patterns:
	
	1. Find hit with largest energy.
	2. Sort hit vector in order of increasing time-difference from this hit
	3. Push all hits within 15 ns of this hit to a new vector (first few elements of sorted vector)
	4. Erase the elements that were moved from the previous vector
		- maybe steps 3&4 can be done simultaneously? 
	5. Push the new vector back to the hitPatterns vector
	6. Repeat steps 1-5 until no hits remain in original hitarray	
	*/

	
	int n_hits = static_cast<int>( hitarray.size() );
	
	if( n_hits < 1 ) return;
	if( n_hits < 2 ) {
	  vector< const DCCALHit* > hitVec;
	  hitVec.push_back( hitarray[0] );
	  hitPatterns.push_back( hitVec );
	  return;
	}
	
	
	vector< const DCCALHit* > clonedHitArray = hitarray;
	
	while( clonedHitArray.size() ) 
	{
	
	  vector< const DCCALHit* > locHitVec;
	
	  float maxE  = -1.;
	  float maxT  = 1.e6;
	  
	  for( unsigned int ih = 0; ih < clonedHitArray.size(); ih++ ) {
	    float trialE = clonedHitArray[ih]->E;
	    if( trialE > maxE ) { maxE = trialE; maxT = clonedHitArray[ih]->t; }
	  }
	  
	  if( maxE < 0. ) break;
	  
	  
	  sortByTime( clonedHitArray, maxT );
	  
	  
	  n_hits = static_cast<int>( clonedHitArray.size() );
	  
	  int n_good_hits = 0;
	  for( int ih = 0; ih < n_hits; ih++ ) {
	  
	    const DCCALHit *locHit = clonedHitArray[ih];
	    float timeDiff = fabs( locHit->t - maxT );
	    
	    if( timeDiff < TIME_CUT ) {
	      locHitVec.push_back( locHit );
	      n_good_hits++;
	    } else { break; }
	  
	  }
	  
	  if( locHitVec.size() ) hitPatterns.push_back( locHitVec );
	  
	  clonedHitArray.erase( clonedHitArray.begin(), clonedHitArray.begin() + n_good_hits );
	  
	}
	
	
	return;

}




//==========================================================
//
//   sortByTime
//
//==========================================================

void DCCALShower_factory::sortByTime( vector< const DCCALHit* > &hitarray, float hitTime )
{
	
	int nhits = static_cast<int>( hitarray.size() );
	
	if( nhits < 2 ) return; // nothing to sort
	
	for( int ih = 1; ih < nhits; ih++ ) 
	{
	  
	  float timeDiff     = fabs( hitarray[ih]->t - hitTime );
	  float lastTimeDiff = fabs( hitarray[ih-1]->t - hitTime );
	  
	  if( timeDiff <= lastTimeDiff ) 
	  {
	  
	    const DCCALHit *Hit = hitarray[ih];
	    
	    for( int ii = ih-1; ii >= -1; ii-- ) { 
	      
	      if( ii >= 0 ) {
	      
	        const DCCALHit *locHit = hitarray[ii];
	        float locTimeDiff = fabs( locHit->t - hitTime );
	      
	        if( timeDiff < locTimeDiff ) {
	          hitarray[ii+1] = locHit;
	        } else {
	          hitarray[ii+1] = Hit;
		  break;
	        }
	      } else {
	        hitarray[0] = Hit;
	      }
	    }
	    
	  } // end if statement
	
	} // end loop over hits
	
	return;
}




//==========================================================
//
//   process_showers
//
//==========================================================

void DCCALShower_factory::process_showers( vector< gamma_t > gammas, DCCALGeometry ccalGeom, 
		vector< const DCCALHit* > locHitPattern, vector< ccalcluster_t > &ccalClusters, 
		vector< cluster_t > &clusterStorage )
{


	//-------------   Do some post-island processing   -------------//
	
	
	int n_clusters = 0;
	int n_hits = static_cast<int>( locHitPattern.size() );
	
	int init_clusters = static_cast<int>( gammas.size() );
	for( int k = 0; k < init_clusters; k++ )  {
	  
	  ccalcluster_t locCluster;    // stores cluster parameters
	  cluster_t locClusterStorage; // stores hit information of cluster cells
	  
	  int type     = gammas[k].type;
	  int dime     = gammas[k].dime;
	  int id       = gammas[k].id;
	  double chi2  = gammas[k].chi2;
	  double e     = gammas[k].energy;
	  double x     = gammas[k].x;
	  double y     = gammas[k].y;
	  double xc    = gammas[k].xc;
	  double yc    = gammas[k].yc;
	  
	  
	  // check that shower is not just a single module and that energy is reasonable:
	  
	  if( dime < MIN_CLUSTER_BLOCK_COUNT ) { continue; } 
	  if( e < MIN_CLUSTER_ENERGY || e > MAX_CLUSTER_ENERGY ) { continue; }
	  
	  n_clusters++;
	  
	  
	  //------------   Find cell with max energy   ------------//
	  
	  double ecellmax = -1; int idmax = -1;
	  double e1 = 0.0;
	  for( int j = 0; j < (dime>MAX_CC ? MAX_CC : dime); j++ ) {
	  
	    double ecell = 1.e-4*static_cast<double>( gammas[k].icl_en[j] );
	    int id = gammas[k].icl_in[j];
	    int kx = (id/100), ky = id%100;
	    id = (ky-1)*12 + (kx-1);
	    e1 += ecell;
	    if( ecell > ecellmax ) {
	      ecellmax = ecell;
	      idmax = id;
	    }
	  
	  }
	  
	  double xmax = ccalGeom.positionOnFace(idmax).X();
	  double ymax = ccalGeom.positionOnFace(idmax).Y();
	  
	  
	  //-----------   Loop over constituent hits   ------------//
	  
	  double sW   = 0.0;
	  double xpos = 0.0;
	  double ypos = 0.0;
	  double W;
	  
	  for( int j = 0; j < (dime>MAX_CC ? MAX_CC : dime); j++ ) {
	  
	    int id = gammas[k].icl_in[j];
	    int kx = (id/100), ky = id%100;
	    int ccal_id = (ky-1)*12 + (kx-1);
	  
	    double ecell  = 1.e-4*static_cast<double>( gammas[k].icl_en[j] );
	    double xcell  = ccalGeom.positionOnFace( ccal_id ).X();
	    double ycell  = ccalGeom.positionOnFace( ccal_id ).Y();
	    
	    if(id%10 == 1 || id%10 == 2) {
	      xcell += xc;
	      ycell += yc;
	    }
	    
	    double hittime = 0.;
	    for( int ihit = 0; ihit < n_hits; ihit++ ) {
	      int trialid = 12*( locHitPattern[ihit]->row) + locHitPattern[ihit]->column;
	      if( trialid == ccal_id ) {
	        hittime = locHitPattern[ihit]->t;
		break;
	      }
	    }
	    
	    locClusterStorage.id[j] = ccal_id;
	    locClusterStorage.E[j]  = ecell;
	    locClusterStorage.x[j]  = xcell;
	    locClusterStorage.y[j]  = ycell;
	    locClusterStorage.t[j]  = hittime;
	    
	    
	    // The shower position is calculated using logarithmic weighting:
	    
	    if( ecell > 0.009 && fabs(xcell-xmax) < 6. && fabs(ycell-ymax) < 6.) {
	      W = 4.2 + log(ecell/e);
	      if( W > 0 ) {
		sW   += W;
		xpos += xcell*W;
		ypos += ycell*W;
	      }
	    }
	    
	  }
	  
	  for( int j = dime; j < MAX_CC; j++ )  // zero the rest
	    locClusterStorage.id[j] = -1;
	  
	  double weightedTime = getEnergyWeightedTime( locClusterStorage, dime );
	  double showerTime   = getCorrectedTime( weightedTime, e );
	  
	  
	  
	  //-------  Get position at surface of Calorimeter -------//
	  
	  DVector3 vertex(0.0, 0.0, m_zTarget); // for now, use center of target as vertex
	  
	  double x1, y1;
	  double zV = vertex.Z();
	  double z0 = m_CCALfront - zV;
	  if(sW) {
	    double dz = getShowerDepth( e );
	    double zk = 1. / (1. + dz/z0);
	    x1 = zk*xpos/sW;
	    y1 = zk*ypos/sW;
	  } else {
	    printf("WRN bad cluster log. coord, center id = %i %f\n", idmax, e);
	    x1 = 0.0;
	    y1 = 0.0;
	  }
	  
	  //-----------------  Fill cluster bank  -----------------//
	  
	  locCluster.type   = type;
	  locCluster.nhits  = dime;
	  locCluster.id     = id;
	  locCluster.idmax  = idmax;
	  locCluster.E      = e;
	  locCluster.time   = showerTime;
	  locCluster.x      = x;
	  locCluster.y      = y;
	  locCluster.chi2   = chi2;
	  locCluster.x1     = x1;
	  locCluster.y1     = y1;
	  locCluster.emax   = ecellmax;
	  
	  
	  clusterStorage.push_back( locClusterStorage );
	  ccalClusters.push_back( locCluster );
	
	}
	

	if( n_clusters == 0 ) return;
	final_cluster_processing( ccalClusters );



	return;
}











//==========================================================
//
//   final_cluster_processing
//
//==========================================================

void DCCALShower_factory::final_cluster_processing( vector< ccalcluster_t > &ccalClusters ) 
{
	//--------------------------
	// Final Cluster Processing:
	//  - apply nolinear energy correction
	//  - add energy resolution


	int n_clusters = static_cast<int>( ccalClusters.size() );

    	for( int icl = 0; icl < n_clusters; icl++ )
	{
	
	  double e     = ccalClusters[icl].E;
	  int idmax    = ccalClusters[icl].idmax;
	 
	  double ecorr = e;
	  if( DO_NONLINEAR_CORRECTION ) ecorr = getCorrectedEnergy( e, idmax );
	  
	  if( SHOWER_DEBUG ) {
	    cout << "\n\nShower energy before correction: " << e << " GeV" << endl;
	    cout << "Shower energy after  correction: " << ecorr << " GeV\n\n" << endl;
	  }

      	  //float x   = ccalClusters[i].x1;
      	  //float y   = ccalClusters[i].y1;
          // coord_align(i, e, idmax);
	  
	  int type = ccalClusters[icl].type;
	  
      	  double se = sqrt( 0.9*0.9*e*e + 2.5*2.5*e + 1.0 ); // from HYCAL reconstruction, need to tune
      	  se /= 100.;
	  
      	  if( (type%10)==1 )
            se *= 1.5;
      	  else if( (type%10)==2 )
            se *= 1.25;
	  
	  ccalClusters[icl].E       = ecorr;
      	  ccalClusters[icl].sigma_E = se;
	  
	}

	return;

}




//==========================================================
//
//   getEnergyWeightedTime
//
//==========================================================

double DCCALShower_factory::getEnergyWeightedTime( cluster_t clusterStorage, int nHits )
{

	double weightedtime = 0.;
	double totEn = 0;
	for( int j = 0; j < (nHits > MAX_CC ? MAX_CC : nHits); j++ ) {
	  weightedtime += clusterStorage.t[j]*clusterStorage.E[j];
	  totEn += clusterStorage.E[j];
	}
	weightedtime /= totEn;
	
	return weightedtime;

}




//==========================================================
//
//   getCorrectedTime
//
//==========================================================

double DCCALShower_factory::getCorrectedTime( double time, double energy ) 
{
	// timewalk correction:
	
	double p0[2] = {0.9531, 0.3847};
	double p1[2] = {0.9374, 1.5006};
	double p2[2] = {-2.5223, -0.3751};
	double p3[2] = {0.9413, -0.1129};
	
	int iPar;
	if( energy < 1.0 ) iPar = 0;
	else iPar = 1;
	
	double dt = p0[iPar]*exp(p1[iPar] + p2[iPar]*energy) + p3[iPar];
	double t_cor = time - dt;
	
	return t_cor;
	
}




//==========================================================
//
//   getShowerDepth
//
//==========================================================

double DCCALShower_factory::getShowerDepth( double energy ) 
{

	double z0 = CCAL_RADIATION_LENGTH, e0 = CCAL_CRITICAL_ENERGY;
	double depth = (energy > 0.) ? z0*log(1.+energy/e0) : 0.;
	return depth;

}




//==========================================================
//
//   getCorrectedEnergy
//
//==========================================================

double DCCALShower_factory::getCorrectedEnergy( double energy, int id ) 
{

	if( Nonlin_p1[id] == 0. && Nonlin_p2[id] == 0. && Nonlin_p3[id] == 0.) return energy;
	if( Nonlin_p0[id] == 0. ) return energy;
  	if( energy < 0.5 || energy > 12. ) return energy;


	double emin = 0., emax = 12.;
  	double e0 = (emin+emax)/2.;

  	double de1 = energy - emin*nonlin_func( emin, id );
  	double de2 = energy - emax*nonlin_func( emax, id );
  	double de  = energy - e0*nonlin_func( e0, id );

  	while( fabs(emin-emax) > 1.e-5 ) {
    	  if( de1*de > 0. && de2*de < 0.) {
      	    emin = e0;
      	    de1 = energy - emin*nonlin_func( emin, id );
    	  } else {
      	    emax = e0;
      	    de2 = energy - emax*nonlin_func( emax, id );
    	  }
    	  e0 = (emin+emax)/2.;
    	  de  = energy - e0*nonlin_func( e0, id );
  	}
	
  	return e0;

}




//==========================================================
//
//   nonlin_func
//
//==========================================================

double DCCALShower_factory::nonlin_func( double e, int id ) 
{

  	return pow( (e/Nonlin_p0[id]), Nonlin_p1[id] + Nonlin_p2[id]*e + Nonlin_p3[id]*e*e );

}



