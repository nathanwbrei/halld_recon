
#include "CCAL/DCCALShower_factory.h"

//==========================================================
//
//   main_island()
//
//==========================================================

void DCCALShower_factory::main_island( vector<int> &ia, vector<int> &id, vector<gamma_t> &gammas ) 
{

	//----------------------------------------------
	// Initialize some useful variables:

	int nhits;          // number of hits in detector
	int ncl;	    // number of clusters
	vector<int> lencl;  // length of a cluster
	
	
	nhits = static_cast<int>(ia.size());
	if( nhits == 0 ) return;
	
	/*
	The vector 'ia' will hold the addresses of modules that were hit.
	They're defined as 100*(i+1)+(j+1) where i is column and j is row of the hit module.
	Row 0, column 0 is bottom right corner of CCAL (looking upstream).
	
	The 'id' vector holds the energies of the hit modules (in units of 0.1 MeVs).
	*/
	
	
	//------------------ Search for clusters: ------------------//
	
	
	ncl = clus_hyc( nhits, ia, id, lencl );
	
	/*
	At this point, the vectors ia and id are sorted such that they are grouped
	together by simply-connected clusters. For example, the first lencl[0] elements of ia give 
	the addresses of the elements in the first cluster. The next lencl[1] elements
	after that give the addresses of the elements in the second cluster.
	
	The clusters here are just created by grouping everything that is physically 
	next to each other into one cluster - all simply-connected cells are joined in a cluster.
	*/
	
	
	//-------------------- Process Cluster: --------------------//
	
	int nadcgam = 0; // number of found clusters
	
	if( ncl <= 0 ) return;
	
	int ipncl = 0;
	for( int icl = 0; icl < ncl; icl++ ) {
	  
	  int ecl = 0;
	  for( int ii = 0; ii < lencl[icl]; ii++ ) 
	    ecl += id[ipncl+ii];
	  if( ecl > MIN_ENERGY ) {
	    
	    vector< int > icl_a; // addresses of current cluster
	    vector< int > icl_d; // energies of current cluster
	    
	    icl_a.insert( icl_a.begin(), ia.begin()+ipncl, ia.begin()+ipncl+lencl[icl] );
	    icl_d.insert( icl_d.begin(), id.begin()+ipncl, id.begin()+ipncl+lencl[icl] );
	    
	    
	    if( SHOWER_DEBUG ) {
	    
	      cout << "\n\n======================" << endl;
	      cout << "Processing Cluster " << icl << ":" << endl;
	      for( unsigned int ih = 0; ih < icl_a.size(); ih++ ) {
	        cout << icl_a[ih] << " " << icl_d[ih] << endl;
	      }
	    
	    }
	    
	    int before = nadcgam;
	    gams_hyc( lencl[icl], icl_a, icl_d, nadcgam, gammas );
	    int after  = nadcgam;
	    
	    
	    if( SHOWER_DEBUG ) {
	      cout << "Reconstructed " << after-before << " gammas. \n\n" << endl;
	    }
	    
	    
	    if( nadcgam > MADCGAM ) {
	      nadcgam = MADCGAM;
	      break;
	    }
	  }
	  ipncl = ipncl + lencl[icl];
	}
	
	
	//-------------------- Prepare gammas for final processing --------------------//
	
	
	// convert position to units of cm:
	
	for( int ig = 0; ig < nadcgam; ig++ ) {
	  gammas[ig].energy /= 10000.;
	  gammas[ig].x  = (2.*gammas[ig].x-static_cast<double>(MCOL+1))*xsize/2.;
	  gammas[ig].y  = (2.*gammas[ig].y-static_cast<double>(MROW+1))*ysize/2.;
	  gammas[ig].xc = -gammas[ig].xc*xsize;
	  gammas[ig].yc = -gammas[ig].yc*ysize;
	}
	
	
	if( nadcgam < 1 ) return;	
	
	
	// sort gammas in order of decreasing energy:
	
	for( int ig = 1; ig < nadcgam; ig++ ) 
	{ 
	  if( gammas[ig].energy > gammas[ig-1].energy ) { 
	    gamma_t ref_gam = gammas[ig]; 
	    
	    for( int ii = ig-1; ii >= -1; ii-- ) {	      
	      if( ii >= 0 ) {
	        if( ref_gam.energy > gammas[ii].energy ) {
	          gammas[ii+1] = gammas[ii];
	        } else {
	          gammas[ii+1] = ref_gam;
		  break;
	        }
	      } else {
	        gammas[0] = ref_gam;
	      }
	    }
	  }
	}
	  
	

	return;
}





//==========================================================
//
//   clus_hyc
//
//==========================================================

int DCCALShower_factory::clus_hyc( int nw, vector<int> &ia, vector<int> &id, vector<int> &lencl ) 
{

	//---------------- Local Declarations ---------------//
	
	int maxcl = 200;    // maximumum number of clusters allowed
	int ncl;            // number of clusters
	int next, iak;      // 
	int ib, ie;         // 
	int ias, iaf;       // 
	int last, lastcl;   // 
	int leng;           // 
	                    // 
	int loclencl[200];  // stores the lengths of clusters locally
	
	
	//---------------- Event Analysis Code --------------//
	
	ncl = 0;
	if( nw < 1 ) return ncl;
	if( nw < 2 ) { // if only one hit
	  ncl = 1;
	  lencl.push_back(1);
	  return ncl;
	}
	
	order_hyc( nw, ia, id ); // sort the addresses (ia) in increasing order
	
	ncl  = 0;
	next = 0;
	
	for( int k = 1; k < (nw+1); k++ ) {
	  
	  if( k < nw ) iak = ia[k];
	  if( (iak-ia[k-1] <= 1) && (k < nw) ) continue;
	  
	  ib   = next;
	  ie   = k-1;
	  next = k;
	  
	  if( ncl >= maxcl ) return ncl; 
	  ncl++;
	  
	  loclencl[ncl-1] = next-ib;
	  if(ncl == 1) continue;
	  
	  ias    = ia[ib];
	  iaf    = ia[ie];
	  last   = ib-1;
	  lastcl = ncl-1;
	  
	  for( int icl = lastcl; icl > 0; icl-- ) {
	    
	    leng = loclencl[icl-1];
	    if( (ias-ia[last]) > 100 ) break; // no subclusters to be glued
	    
	    for( int ii = last; ii >= last-leng+1; ii-- ) {
	      if( ( ias-ia[ii] )  > 100 ) break;
	      if( ( iaf-ia[ii] ) >= 100 ) {
	      
	        if( (icl < (ncl-1)) && (leng <= 10800) ) {  
		
		  vector< int > iawork;
		  ucopy1( ia, iawork, last-leng+1, leng );
		  ucopy2( ia, last+1, last+1-leng, ib-last-1 );
		  ucopy3( iawork, ia, ib-leng, leng );
		  
		  vector< int > idwork;
		  ucopy1( id, idwork, last-leng+1, leng );
		  ucopy2( id, last+1, last+1-leng, ib-last-1 );
		  ucopy3( idwork, id, ib-leng, leng );
		  
		  for( int jj = icl; jj < ncl-1; jj++ ) {
                    loclencl[jj-1] = loclencl[jj];
                  }
		
		}
		
		ib = ib-leng;
                loclencl[ncl-2] = loclencl[ncl-1]+leng;
                ncl = ncl-1;
                break;
	      }
	    }
	    
	    last = last-leng;
	    
	  } // end loop over previous subclusters
	} // end loop over all hits
	
	for( int icl = 0; icl < ncl; icl++ ) {
	  lencl.push_back( loclencl[icl] );
	}
	

	return ncl;
}





//==========================================================
//
//   order_hyc
//
//==========================================================

void DCCALShower_factory::order_hyc( int nw, vector<int> &ia, vector<int> &id ) 
{
	// sort ia and id in order of increasing address
	
	if( nw < 2 ) return; // only one hit
	
	for( int k = 1; k < nw; k++ ) { // loop over hits
	  
	  if( ia[k] <= ia[k-1] ) { // check if address is less than previous entry
	    int iak = ia[k];
	    int idk = id[k];
	    
	    for( int ii = k-1; ii >= -1; ii = ii-1 ) { // loop over the previous entries
	      
	      if( ii >= 0 ) {
	        if( iak < ia[ii] ) {
	          ia[ii+1] = ia[ii];
		  id[ii+1] = id[ii];
	        } else {
	          ia[ii+1] = iak;
		  id[ii+1] = idk;
		  break;
	        }
	      } else {
	        ia[0] = iak;
	        id[0] = idk;
	      }
	      
	    } // end loop over previous entries
	  } // endif
	} // end loop over hits


	return;
}





//==========================================================
//
//   gams_hyc
//
//==========================================================

void DCCALShower_factory::gams_hyc( int nadc, vector<int> &ia, vector<int> &id, int &nadcgam, vector<gamma_t> &gammas )
{
	
	//-------------Local Declarations------------//
	
	int ngam0;                // number of gammas found before
	int niter;                // max number of iterations (6)
	int npk;                  // number of peaks in the cluster
	int ipnpk[10];            // counter number with max energy in a peak
	int igmpk[2][10];         // 
	int minpk;                // min energy of a counter in a cluster
	int idelta;               // 
	int itype;                // type of peak
	int leng;                 // 
	int ixypk, ixpk, iypk;    // 
	int ic, idc, in;          // 
	int ixy, ixymax, ixymin;  // 
	int ix, iy, iyc;          // 
	int iwk;                  // 
	int iia;                  // 
	int iwrk[13][10800];      // working array for resolved peaks
	
	int idp[13][10800];       // energy of each cell of the island belonging to each peak
	int ide;                  // 
	int idecorr;              // 
	
	double fw[13];
	
	double chisq;             // current value of chi2
	double chisq1;            // value of chi2 for preliminary gammas separation
	double chisq2;            // value of chi2 for final gammas separation
	double ratio;             // 
	double eg;                // 
	double epk[10];           // 
	double xpk[10];           // 
	double ypk[10];           // 
	double a, dx, dy;         // 
	double e1, x1, y1;        // 
	double e2, x2, y2;        // 
	double fwrk[13][10800];   // working array for resolved peaks
	double fe, fia;           // 
	
	int idsum;
	
	
	idelta = 0;
	chisq1 = 90.;
	chisq2 = 50.;
	niter  = 6;
	
	ngam0 = nadcgam;
	
	
	//------------------------------------------
	// peaks search:
	
	order_hyc( nadc, ia, id );
	
	idsum = 0;
	for( int ic = 0; ic < nadc; ic++ )
	  idsum += id[ic];
	  
	if( nadc < 3 )
	  minpk = 1;
	else {
	  int trial = 7.*log(1. + 0.0001*static_cast<double>(idsum)) + 0.5;
	  if( trial > 1 ) minpk = trial;
	  else minpk = 1;
	}
	minpk *= 100;
	
	
	npk = 0;
	
	for( ic = 0; ic < nadc; ic++ ) {
	  idc = id[ic];
	  if( idc < minpk ) continue;
	  ixy = ia[ic];
	  ixymax = ixy + 100 + 1;
	  ixymin = ixy - 100 - 1;
	  iyc = ixy - (ixy/100)*100;
	  
	  int peakVal = 1;
	  
	  in = ic+1;
	  while( in < nadc && ia[in] <= ixymax ) {
	    iy = ia[in] - (ia[in]/100)*100;
	    if( abs(iy-iyc) <= 1 && id[in] >= idc ) peakVal = 0;
	    in++;
	  }
	  
	  in = ic-1;
	  while( in >= 0 && ia[in] >= ixymin ) {
	    iy = ia[in] - (ia[in]/100)*100;
	    if( abs(iy-iyc) <= 1 && id[in] > idc ) peakVal = 0;
	    in -= 1;
	  }
	  
	  if( !peakVal ) continue;
	  
	  npk += 1;
	  ipnpk[npk-1] = ic;
	  if( npk == 10 || npk >= 10000/nadc-3 ) break;
	    	
	}
	
	if( npk <= 0 ) return;
	
	if( SHOWER_DEBUG ) cout << "Found " << npk << " peaks. Now processing..." << endl;
	
	//------------------------------------------
	// gammas search for one peak:
	
	if( npk == 1 ) {
	
	  if( nadcgam >= MADCGAM-1 ) return;
	  nadcgam = nadcgam+1;
	  chisq = chisq2;
	  
	  ic = ipnpk[0];
	  ix = ia[ic]/100;
	  iy = ia[ic] - ix*100;
	
	  itype = peak_type( ix, iy );
	  
	  e2 = 0.;
	  gamma_hyc( nadc, ia, id, chisq,
	  		e1, x1, y1, 
			e2, x2, y2 );
			
	  gamma_t gam1;
	  gamma_t gam2;
	  
	  gam1.type   = itype;
	  gam1.dime   = nadc;
	  gam1.id     = 0;
	    
	  gam1.chi2   = chisq;
	  gam1.energy = e1;
	  gam1.x      = x1;
	  gam1.y      = y1;
	  gam1.xc     = 0.;
	  gam1.yc     = 0.;
	  
	  if( e2 > 0. && nadcgam <= MADCGAM-1 ) {
	    nadcgam = nadcgam+1;
	    
	    gam2.type   = itype+10;
	    gam2.dime   = nadc;
	    gam2.id     = 2;
	    
	    gam2.chi2   = chisq;
	    gam2.energy = e2;
	    gam2.x      = x2;
	    gam2.y      = y2;
	    gam2.xc     = 0.5*(x2-x1);
	    gam2.yc     = 0.5*(y2-y1);
	    
	    gam1.type   = itype+10;
	    gam1.id     = 1;
	    gam1.xc     = 0.5*(x1-x2);
	    gam1.yc     = 0.5*(y1-y2);
	    
	    for( int jj = 0; jj < nadc; jj++ ) {
	      if( jj < MAX_CC ) {
	        gam1.icl_in[jj] = ia[jj];
		gam2.icl_in[jj] = ia[jj];
		gam1.icl_en[jj] = static_cast<int>(static_cast<double>(id[jj])*e1/(e1+e2) + 0.5);
		gam2.icl_en[jj] = static_cast<int>(static_cast<double>(id[jj])*e2/(e1+e2) + 0.5);
	      }
	    }
	    
	    gammas.push_back( gam1 );
	    gammas.push_back( gam2 );
	    
	  } else {
	    for( int jj = 0; jj < nadc; jj++ ) {
	      if( jj < MAX_CC ) {
	        gam1.icl_in[jj] = ia[jj];
		gam1.icl_en[jj] = id[jj];
	      }
	    }
	    
	    gammas.push_back( gam1 );
	    
	  }
	
	
	} else { // cluster with more than one peak
	
	//------------------------------------------
	/*
	First step - 1 gamma in each peak.
	Do a preliminary estimation of (E,x,y) of each peak, and split each peak into two hits 
	only if it is badly needed (chi2 improvement is too high).
	If this split occurs, it is only for better (E,x,y) estimation, as it will be 
	rejoined and reanalyzed in the second step.
	*/
	//------------------------------------------
	
	
	  if( nadcgam >= MADCGAM-1 ) return;
	  
	  ratio = 1.;
	  for( int iter = 0; iter < niter; iter++ ) {
	    for( int ii = 0; ii < nadc; ii++ ) {
	      iwrk[0][ii] = 0;
	      fwrk[0][ii] = 0.;
	    }
	    
	    for( int ipk = 0; ipk < npk; ipk++ ) {
	    
	      ic = ipnpk[ipk];
	      if( iter > 0 ) ratio = fwrk[ipk+1][ic]/fwrk[npk+1][ic];
	      eg = ratio*static_cast<double>(id[ic]);
	      ixypk = ia[ic];
	      ixpk  = ixypk/100;
	      iypk  = ixypk - ixpk*100;
	      epk[ipk] = eg;
	      xpk[ipk] = eg*static_cast<double>(ixpk);
	      ypk[ipk] = eg*static_cast<double>(iypk);
	      
	      if( ic < nadc-1 ) {
	        for( int ii = ic+1; ii < nadc; ii++ ) {
		  ixy = ia[ii];
		  ix  = ixy/100;
		  iy  = ixy - ix*100;
		  if( ixy-ixypk > 100+1 ) break;
		  if( abs(iy-iypk) <= 1 ) {
		    if( iter != 0 ) ratio = fwrk[ipk+1][ii]/fwrk[npk+1][ii];
		    eg = ratio*static_cast<double>(id[ii]);
		    epk[ipk] = epk[ipk] + eg;
		    xpk[ipk] = xpk[ipk] + eg*static_cast<double>(ix);
		    ypk[ipk] = ypk[ipk] + eg*static_cast<double>(iy);
		  }
		}
	      }
	      
	      if( ic > 0 ) {
	        for( int ii = ic-1; ii >= 0; ii--) {
		  ixy = ia[ii];
		  ix  = ixy/100;
		  iy  = ixy - ix*100;
		  if( ixypk-ixy > 100+1 ) break;
		  if( abs(iy-iypk) <= 1 ) {
		    if( iter != 0 ) ratio = fwrk[ipk+1][ii]/fwrk[npk+1][ii];
		    eg = ratio*static_cast<double>(id[ii]);
		    epk[ipk] = epk[ipk] + eg;
		    xpk[ipk] = xpk[ipk] + eg*static_cast<double>(ix);
		    ypk[ipk] = ypk[ipk] + eg*static_cast<double>(iy);
		  }
		}
	      }
	      
	      if( epk[ipk] > 0. ) {
	        xpk[ipk] = xpk[ipk]/epk[ipk];
		ypk[ipk] = ypk[ipk]/epk[ipk];
	      }
	      
	      for( int ii = 0; ii < nadc; ii++ ) {
	        ixy = ia[ii];
		ix  = ixy/100;
		iy  = ixy - ix*100;
		dx  = fabs( static_cast<double>(ix) - xpk[ipk] );
		dy  = fabs( static_cast<double>(iy) - ypk[ipk] );
		
		a = epk[ipk]*cell_hyc( dx, dy );
		
		fwrk[ipk+1][ii] = a;
		fwrk[0][ii]     = fwrk[0][ii] + fwrk[ipk+1][ii];
		iwrk[ipk+1][ii] = static_cast<int>(a + 0.5);
		iwrk[0][ii]     = iwrk[ii][0] + iwrk[ii][ipk+1];
	      }
	      
	      
	    } // end loop over peaks
	    
	    for( int ii = 0; ii < nadc; ii++ ) {
	      iwk = iwrk[0][ii];
	      if( iwk < 1 ) iwk = 1;
	      iwrk[npk+1][ii] = iwk;
	      
	      if( fwrk[0][ii] > 1.e-2 ) 
	        fwrk[npk+1][ii] = fwrk[0][ii];
	      else 
	        fwrk[npk+1][ii] = 1.e-2;
		
	    }
	  } // end of iterations to separate peaks in a cluster
	  
	  
	  
	  if( SHOWER_DEBUG ) {
	  
	  
	    cout << "\n\n\nAfter 6 iterations: " << endl;
	    for( int ipk = 0; ipk < npk; ipk++ ) {
	      cout << "peak " << ipk+1 << ": " << endl;
	      for( int jj = 0; jj < nadc; jj++ ) {
	        cout << ia[jj] <<"; "<< id[jj] <<"; "<< fwrk[ipk+1][jj] << endl;
	      }
	    }
	  
	  }
	  
	  
	  
	  
	  for( int ipk = 0; ipk < npk; ipk++ ) {
	    
	    vector<int> iwrk_a;
	    vector<int> iwrk_d;
	    
	    leng = 0;
	    for( int ii = 0; ii < nadc; ii++ ) {
	      if( fwrk[0][ii] > 1.e-2 ) {
	        ixy = ia[ii];
		fe  = static_cast<double>(id[ii])*fwrk[ipk+1][ii]/fwrk[0][ii];
		
		if( fe > idelta ) {
		  leng = leng+1;
		  iwrk_a.push_back( ixy );
		  iwrk_d.push_back( static_cast<int>(fe+0.5) );
		}
	      
	      }
	    }
	    
	    if( nadcgam >= MADCGAM-1 ) return;
	    
	    igmpk[1][ipk] = 0;
	    if( leng == 0 ) continue;
	    nadcgam = nadcgam + 1;
	    chisq = chisq1;
	    
	    ic = ipnpk[ipk];
	    ix = ia[ic]/100;
	    iy = ia[ic] - ix*100;
	    
	    itype = peak_type( ix, iy );
	    
	    e2 = 0.;
	    gamma_hyc( leng, iwrk_a, iwrk_d, chisq,
	    		e1, x1, y1, e2, x2, y2 );
			
	    gamma_t gam1;
	    gamma_t gam2;
	  
	    gam1.chi2   = chisq;
	    gam1.type   = itype;
	    gam1.energy = e1;
	    gam1.x      = x1;
	    gam1.y      = y1;
	    gam1.dime   = leng;
	    
	    gam1.id     = 0;
	    
	    igmpk[0][ipk] = nadcgam;
	    igmpk[1][ipk] = nadcgam;
	  
	    if( e2 > 0. && nadcgam <= MADCGAM-1 ) {
	      nadcgam = nadcgam+1;
	    
	      gam2.chi2   = chisq;
	      gam2.type   = itype;
	      gam2.energy = e2;
	      gam2.x      = x2;
	      gam2.y      = y2;
	      gam2.xc     = 0.5*(x2-x1);
	      gam2.yc     = 0.5*(y2-y1);
	      gam1.xc     = 0.5*(x1-x2);
	      gam1.yc     = 0.5*(y1-y2);
	    
	      gam2.id     = 92;
	      gam1.id     = 91;
	      
	      gam2.dime   = leng;
	      igmpk[1][ipk] = nadcgam;
	      
	      gammas.push_back( gam1 );
	      gammas.push_back( gam2 );
	      
	    } else { gammas.push_back( gam1 ); }
	    
	  } // end loop over peaks
	  
	  
	  
	  /*
	  This is the second step: ( 1 or 2 gamma in each peak )
	  (e,x,y) of hits were preliminarily estimated in the first step.	  
	  */
	  
	  for( int ii = 0; ii < nadc; ii++ ) {
	    iwrk[0][ii] = 0;
	    fwrk[0][ii] = 0.;
	    idp[0][ii]  = 0;
	  }
	  
	  for( int ipk = 0; ipk < npk; ipk++ ) {
	    for( int ii = 0; ii < nadc; ii++ ) {
	      iwrk[ipk+1][ii] = 0;
	      fwrk[ipk+1][ii] = 0.;
	      idp[ipk+1][ii]  = 0;
	      
	      if( igmpk[1][ipk] == 0 ) continue;
	      
	      for( int ig = igmpk[0][ipk]; ig <= igmpk[1][ipk]; ig++ ) {
	        ixy = ia[ii];
		ix  = ixy/100;
		iy  = ixy-(ix*100);
		dx  = static_cast<double>(ix) - gammas[ig-1].x;
		dy  = static_cast<double>(iy) - gammas[ig-1].y;
		
		fia = gammas[ig-1].energy*cell_hyc( dx, dy );
		iia = static_cast<int>(fia+0.5);
		
		// part of gamma 'ig' energy belonging to cell 'i' from peak 'ipk':
		iwrk[ipk+1][ii] += iia; 
		fwrk[ipk+1][ii] += fia;
		idp[ipk+1][ii]  += iia;
		
		iwrk[0][ii] += iia;
		fwrk[0][ii] += fia;
		
	      }
	      
	    } // end loop over hits
	  } // end loop over peaks
	  
	  
	  
	  
	  // Recover working array and renormalize total sum to the original cell energy:
	  
	  for( int ii = 0; ii < nadc; ii++ ) {
	    
	    idp[0][ii] = 0;
	    for( int ipk = 0; ipk < npk; ipk++ ) {
	      idp[0][ii] += idp[ipk+1][ii];
	    }
	    
	    ide = id[ii] - idp[0][ii];
	    if( ide == 0 ) continue;
	    if( fwrk[0][ii] == 0. ) continue;
	    
	    for( int ipk = 0; ipk < npk; ipk++ ) {
	      fw[ipk+1] = fwrk[ipk+1][ii]/fwrk[0][ii];
	    }
	    
	    idecorr = 0;
	    for( int ipk = 0; ipk < npk; ipk++ ) {
	      fia = ide*fw[ipk+1];
	      if( (fwrk[ipk+1][ii] + fia) > 0. ) {
	        fwrk[ipk+1][ii] += fia;
		fwrk[0][ii] += fia;
	      }
	      iia = static_cast<int>(fia+0.5);
	      if( (iwrk[ipk+1][ii] + iia) > 0 ) {
	        iwrk[ipk+1][ii] += iia;
		iwrk[0][ii] += iia;
		idecorr += iia;
	      } else if( (iwrk[ipk+1][ii] + iia) < 0 ) {
	        //cout << "WARNING NEGATIVE CORR: ia = " << ia[ii] << "; id = " << id[ii] << endl;
	      }
	    } // end loop over peaks
	    
	  } // end loop over hits
	  
	  
	  
	  // erase the gammas found in the previous step:
	  
	  nadcgam = ngam0;
	  gammas.erase( gammas.begin()+nadcgam, gammas.end() );
	  
	  
	  
	  
	  
	  
	  for( int ipk = 0; ipk < npk; ipk++ ) {
	    leng = 0;
	    
	    vector<int> iwrk_a;
	    vector<int> iwrk_d;
	    
	    for( int ii = 0; ii < nadc; ii++ ){
	      if( iwrk[0][ii] > 0 ) {
	        fe = id[ii]*fwrk[ipk+1][ii]/fwrk[0][ii];
		if( fe > idelta ) {
		  leng++;
		  iwrk_a.push_back( ia[ii] );
		  iwrk_d.push_back( static_cast<int>(fe+0.5) );
		}
	      }
	    }
	  
	    if( nadcgam >= MADCGAM-1 ) return;
	    
	  
	    if( leng == 0 ) continue; 
	    
	    nadcgam++;
	    
	    chisq = chisq2;
	    
	    ic = ipnpk[ipk];
	    ix = ia[ic]/100;
	    iy = ia[ic] - ix*100;
	    
	    itype = peak_type( ix, iy );
	    
	    e2 = 0.;
	    gamma_hyc( leng, iwrk_a, iwrk_d, chisq,
	    		e1, x1, y1, e2, x2, y2 );
			
	    gamma_t gam1;
	    gamma_t gam2;
	    
	    gam1.type   = itype;
	    gam1.dime   = leng;
	    gam1.id     = 10;
	    
	    gam1.chi2   = chisq;
	    gam1.energy = e1;
	    gam1.x      = x1;
	    gam1.y      = y1;
	    gam1.xc     = 0.;
	    gam1.yc     = 0.;
	    
	    if( e2 > 0. && nadcgam <= MADCGAM-1 ) {
	      nadcgam = nadcgam+1;
	      
	      gam2.type   = itype+10;
	      gam2.dime   = leng;
	      gam2.id     = 12;
	    
	      gam2.chi2   = chisq;
	      gam2.energy = e2;
	      gam2.x      = x2;
	      gam2.y      = y2;
	      gam2.xc     = 0.5*(x2-x1);
	      gam2.yc     = 0.5*(y2-y1);
	      
	      gam1.type   = itype+10;
	      gam1.id     = 11;
	      gam1.xc     = 0.5*(x1-x2);
	      gam1.yc     = 0.5*(x1-x2);
	      
	      for( int jj = 0; jj < leng; jj++ ) {
	        if( jj < MAX_CC ) {
		  gam1.icl_in[jj] = iwrk_a[jj];
		  gam2.icl_in[jj] = iwrk_a[jj];
		  gam1.icl_en[jj] = static_cast<int>(static_cast<double>(iwrk_d[jj])*e1/(e1+e2) + 0.5);
		  gam2.icl_en[jj] = static_cast<int>(static_cast<double>(iwrk_d[jj])*e2/(e1+e2) + 0.5);
		}
	      }
	      
	      gammas.push_back( gam1 );
	      gammas.push_back( gam2 );
	    
	    } else {
	      for( int jj = 0; jj < leng; jj++ ) {
	        if( jj < MAX_CC ) {
	          gam1.icl_in[jj] = iwrk_a[jj];
		  gam1.icl_en[jj] = iwrk_d[jj];
	        }
	      }
	      
	      gammas.push_back( gam1 );
	      
	    }
	    	    
	    
	  } // end loop over peaks
	  
	  
	  	
	} // end looping over multi-peak cluster
	
	
	
	

	return;
}





//==========================================================
//
//   peak_type
//
//==========================================================

int DCCALShower_factory::peak_type( int ix, int iy )
{
	/*
	itype = 2 : if the peak is in the most outer layer
	itype = 1 : if the peak is in the most inner layer
	itype = 0 : if the peak is anywhere else
	*/

	int itype = 0;
	if( (ix == MCOL/2-1 || ix == MCOL/2+2) && (iy >= MROW/2-1 && iy <= MROW/2+2) ) itype = 1;
	if( (iy == MROW/2-1 || iy == MROW/2+2) && (ix >= MCOL/2-1 && ix <= MCOL/2+2) ) itype = 1;
	if( ix == 1 || ix == MCOL || iy == 1 || iy == MROW ) itype = 2;

	return itype;
}





//==========================================================
//
//   gamma_hyc
//
//==========================================================

void DCCALShower_factory::gamma_hyc( int nadc, vector<int> ia, vector<int> id, double &chisq, 
		double &e1, double &x1, double &y1, 
		double &e2, double &x2, double &y2 )
{
	//-------------Local Declarations------------//
	
	int dof;
	
	double dxy;                    // initial step for iteration
	double stepmin;                // minimum step for iteration
	double stepx, stepy;           // current steps
	double parx, pary;             // 
	double chimem, chisq0, chi0;   //
	double chiold;                 // 
	double chi00;                  // 
	double x0, y0;                 // 
	double ee, xx, yy;             // 
	double d2, xm2, xm2cut;        // 
	double chir, chil, chiu, chid; //
	
	dxy     = 0.05;
	stepmin = 0.002;
	xm2cut  = 1.7;
	
	int nzero;
	vector<int> iaz;
	
	//-------------Event Analysis Code------------//
	
	e2 = 0.;
	x2 = 0.;
	y2 = 0.;	
	
	
	fill_zeros( nadc, ia, nzero, iaz );
	mom1_pht( nadc, ia, id, nzero, iaz, e1, x1, y1 ); // calculate initial values of (E,x,y)
	
	if( nadc <= 0 ) return;
	
	chimem = chisq;
	chisq1_hyc( nadc, ia, id, nzero, iaz, e1, x1, y1, chi0 ); // initial value of chi2
	
	
	chisq0 = chi0;
	dof = nzero + nadc - 2;
	if( dof < 1 ) dof = 1;
	chisq = chi0/dof;
	x0 = x1;
	y0 = y1;
	
	
	// start of iterations
	
	int rounds = 0;
	while( 1 ) {
	
	  chisq1_hyc( nadc, ia, id, nzero, iaz, e1, x0+dxy, y0, chir );
	  chisq1_hyc( nadc, ia, id, nzero, iaz, e1, x0-dxy, y0, chil );
	  chisq1_hyc( nadc, ia, id, nzero, iaz, e1, x0, y0+dxy, chiu );
	  chisq1_hyc( nadc, ia, id, nzero, iaz, e1, x0, y0-dxy, chid );
	  
	  if( chi0 > chir || chi0 > chil ) {
	    stepx = dxy;
	    if( chir > chil ) stepx = -stepx;
	  } else {
	    stepx = 0.;
	    parx = chir + chil - 2.*chi0;
	    if( parx > 0. ) stepx = -dxy*(chir-chil)/(2.*parx);
	  }
	  if( chi0 > chiu || chi0 > chid ) {
	    stepy = dxy;
	    if( chiu > chid ) stepy = -stepy;
	  } else {
	    stepy = 0.;
	    pary = chiu + chid - 2.*chi0;
	    if( pary > 0. ) stepy = -dxy*(chiu-chid)/(2.*pary);
	  }
	
	
	  // if steps at minimum, end iterations
	
	  if( fabs(stepx) < stepmin && fabs(stepy) < stepmin ) break;
	    
	  chisq1_hyc( nadc, ia, id, nzero, iaz, e1, x0+stepx, y0+stepy, chi00 );
	  
	  // if chi2 at minimum, end iterations
	  
	  if( chi00 >= chi0 ) break;
	  
	  chi0 = chi00;
	  x0 = x0+stepx;
	  y0 = y0+stepy;
	  
	  rounds++;
	  if( rounds > 10000 ) { cout << "max rounds" << endl; break; }
	    
	}
	
	if( chi0 < chisq0 ) { // if chi2 improved, then fix the improved values
	  x1 = x0;
	  y1 = y0;
	  chisq = chi0/dof;
	}
	
	// if chi2 is less than maximum allowed for one gamma in a peak, return.
	// otherwise, try separating the peak into two gammas:
	
	if( chisq <= chimem ) return; 
	
	chiold = chisq;
	tgamma_hyc( nadc, ia, id, nzero, iaz, chisq, ee, xx, yy, e2, x2, y2 );
	
	if( e2 >= 0. ) {  // if chi2 improved, decide if the separation
		          // has physical meaning by calculating the 
			  // effective mass of the two gammas
			  
	  d2 = pow((xx-x2)*xsize, 2.0) + pow((yy-y2)*ysize, 2.0);
	  xm2 = ee*e2*d2;
	  
	  if( xm2 > 0. ) xm2 = sqrt(xm2)/1270.*0.1; // mass in MeV; 1270 = zccal
	  
	  if( xm2 > xm2cut*xsize ) { // if the separation has physical meaning
	    e1 = ee;		     // fix the parameters of the first gamma
	    x1 = xx;
	    y1 = yy;
	  } else {		     // if no physical meaning e2=0 
	    e2 = 0.;		     // (second gamma is killed)
	    chisq = chiold;
	  }
	}
	
	return;
}





//==========================================================
//
//   fill_zeros
//
//==========================================================

void DCCALShower_factory::fill_zeros( int nadc, vector<int> ia, int &nneib, vector<int> &iaz )
{

	int ix, iy, nneibnew;
	vector<int> ian;
	
	nneib = 0;
	for( int ii = 0; ii < nadc; ii++ ) {
	  ix = ia[ii]/100;
	  iy = ia[ii] - ix*100;
	  
	  if( ix > 1 ) { // fill left neib
	    nneib = nneib+1;
	    ian.push_back(iy + (ix-1)*100);
	  
	    if( iy > 1 ) { // fill bottom left neib
	      nneib = nneib+1;
	      ian.push_back(iy-1 + (ix-1)*100);
	    }
	    if( iy < MROW ) { // fill top left neib
	      nneib = nneib+1;
	      ian.push_back(iy+1 + (ix-1)*100);
	    }
	  }
	  if( ix < MCOL ) {
	    nneib = nneib+1;
	    ian.push_back(iy + (ix+1)*100);
	    
	    if( iy > 1 ) { // fill bottom right neib
	      nneib = nneib+1;
	      ian.push_back(iy-1 + (ix+1)*100);
	    }
	    if( iy < MROW ) { // fill top right neib
	      nneib = nneib+1;
	      ian.push_back(iy+1 + (ix+1)*100);
	    }
	  }
	  
	  if( iy > 1 ) { // fill bottom neib
	    nneib = nneib+1;
	    ian.push_back(iy-1 + ix*100);
	  }
	  if( iy < MROW ) { // fill top neib
	    nneib = nneib+1;
	    ian.push_back(iy+1 + ix*100);
	  }
	}
	
	
	for( int ii = 0; ii < nneib; ii++ ) {
	  for( int jj = 0; jj < nadc; jj++ ) {
	    if( ia[jj] == ian[ii] ) ian[ii] = -1;
	  }
	}
	
	for( int ii = 0; ii < nneib; ii++ ) {
	  if( ian[ii] == -1 ) continue;
	  for( int jj = ii+1; jj < nneib; jj++ ) {
	    if( ian[jj] == ian[ii] ) ian[jj] = -1;
	  }
	}
	
	nneibnew = 0;
	for( int ii = 0; ii < nneib; ii++ ) {
	  ix = ian[ii]/100;
	  iy = ian[ii] - ix*100;
	  if( ian[ii] != -1 ) {
	    if( stat_ch[iy-1][ix-1] == 0 ) {
	      nneibnew = nneibnew+1;
	      iaz.push_back( ian[ii] );
	    }
	  }
	}
	nneib = nneibnew;
	

	return;
}





//==========================================================
//
//   mom1_pht
//
//==========================================================

void DCCALShower_factory::mom1_pht( int nadc, vector<int> ia, vector<int> id, 
		int nzero, vector<int> iaz, double &e1, double &x1, double &y1 )
{
	//-------------Local Declarations------------//

	int ix, iy;
	double dx, dy;
	
	double a;          // 
	double corr;       // correction to energy
	
	//--------------Event Analysis Code-------------//
	
	e1 = 0.;
	x1 = 0.;
	y1 = 0.;
	
	if( nadc <= 0 ) return;
	for( int ii = 0; ii < nadc; ii++ ) {
	  a  = static_cast<double>(id[ii]);
	  ix = ia[ii]/100;
	  iy = ia[ii] - ix*100;
	  e1 = e1 + a;
	  x1 = x1 + a*static_cast<double>(ix);
	  y1 = y1 + a*static_cast<double>(iy);
	}
	if( e1 <= 0. ) return;
	x1 = x1/e1;
	y1 = y1/e1;
	corr = 0.;
	for( int ii = 0; ii < nadc; ii++ ) {
	  ix = ia[ii]/100;
	  iy = ia[ii] - ix*100;
	  dx = static_cast<double>(ix) - x1;
	  dy = static_cast<double>(iy) - y1;
	  corr = corr + cell_hyc( dx, dy );
	}
	
	if( COUNT_ZERO ) {
	  for( int ii = 0; ii < nzero; ii++ ) {
	    ix = iaz[ii]/100;
	    iy = iaz[ii] - ix*100;
	    dx = static_cast<double>(ix) - x1;
	    dy = static_cast<double>(iy) - y1;
	    corr = corr + cell_hyc( dx, dy );
	  }
	}
	
	corr = corr/1.006;
	
	if( corr < 0.8 ) {
	  if( SHOWER_DEBUG ) {
	    cout << "corr = " << corr << ", " << e1 << ", " << x1 << ", " << y1;
	    cout << "! Too many around central hole!" << endl;
	  }
	  corr = 0.8;
	} else if( corr > 1.0 ) {
	  corr = 1.;
	}
	e1 = e1/corr;
	
	
	return;
}





//==========================================================
//
//    chisq1_hyc
//
//==========================================================

void DCCALShower_factory::chisq1_hyc( int nadc, vector<int> ia, vector<int> id, 
	int nneib, vector<int> iaz, double e1, double x1, double y1, double &chisq )
{
	//-------------Local Declarations------------//

	int ix, iy;
	double dx, dy;
	double fa, fcell;
	

	//--------------Event Analysis Code-------------//
	
	chisq = 0.;
	
	for( int ii = 0; ii < nadc; ii++ ) {
	  fa = static_cast<double>(id[ii]);
	  ix = ia[ii]/100;
	  iy = ia[ii] - ix*100;
	  dx = x1 - static_cast<double>(ix);
	  dy = y1 - static_cast<double>(iy);
	  if( e1 != 0. ) {
	    if( fabs(dx) <= 6.0 && fabs(dy) <= 6.0 ) {
	      fcell = cell_hyc( dx, dy );
	      chisq = chisq + e1*pow((fcell-(fa/e1)), 2.)/sigma2(dx, dy, fcell, e1);
	    }
	  } else {
	    chisq = chisq + fa*fa/9.;
	    cout << "case 0 ch" << endl;
	    //if( SHOWER_DEBUG ) cout << "case 0 ch" << endl;
	  }
	}
	
	for(int ii = 0; ii < nneib; ii++ ) {
	  ix = iaz[ii]/100;
	  iy = iaz[ii] - ix*100;
	  dx = x1 - static_cast<double>(ix);
	  dy = y1 - static_cast<double>(iy);
	  if( e1 != 0. ) {
	    if( fabs(dx) < 6.0 && fabs(dy) < 6.0 ) {
	      fcell = cell_hyc( dx, dy );
	      chisq = chisq + e1*fcell*fcell/sigma2(dx, dy, fcell, e1);
	    }
	  } else {
	    //chisq = chisq + id[ii]*id[ii]/9.;
	    cout << "case 0 ch" << endl;
	    //if( SHOWER_DEBUG ) cout << "case 0 ch" << endl;
	  }
	}
	

	return;
}





//==========================================================
//
//   sigma2, d2c, & cell_hyc
//
//==========================================================

double DCCALShower_factory::sigma2( double dx, double dy, double fc, double e ) 
{
	double sig2;
	double alp = 0.816;
	double bet1 = 32.1;
	double bet2 = 1.72;
	
	double r = dx*dx + dy*dy;
	if( r > 25. ) {
	  sig2 = 100.;
	  return sig2;
	}
	
	sig2 = alp*fc + (bet1 + bet2*sqrt(e/100.))*d2c( dx, dy ) + 0.2/(e/100.); 
	if( TEST_P ) sig2 = sig2/pow(0.0001*e, 0.166);
	sig2 *= 100.;

	return sig2;
}


double DCCALShower_factory::d2c( double dx, double dy ) 
{
	int i, j;
	double ax, ay, wx, wy;
	double d2c;
	
	ax = fabs(dx*100.);
	ay = fabs(dy*100.);
	i = int(ax);
	j = int(ay);
	
	if( i < 500. && j < 500. ) {
	
	  wx = ax-static_cast<double>(i);
	  wy = ay-static_cast<double>(j);
	  
	  d2c = ad2c[i][j]     * (1.-wx) * (1.-wy) + 
	  	ad2c[i+1][j]   *     wx  * (1.-wy) + 
		ad2c[i][j+1]   * (1.-wx) *     wy  +
		ad2c[i+1][j+1] *     wx  *     wy; 
	
	} else d2c = 1.;

	return d2c;
}


double DCCALShower_factory::cell_hyc( double dx, double dy )
{
	int i, j;
	double ax, ay, wx, wy;
	double cell_hyc;
	
	ax = fabs(dx*100.);
	ay = fabs(dy*100.);
	i = int(ax);
	j = int(ay);
	
	if( i < 500. && j < 500. ) {
	
	  wx = ax-static_cast<double>(i);
	  wy = ay-static_cast<double>(j);
	  
	  cell_hyc = acell[i][j]     * (1.-wx) * (1.-wy) + 
	  	     acell[i+1][j]   *     wx  * (1.-wy) + 
		     acell[i][j+1]   * (1.-wx) *     wy  +
		     acell[i+1][j+1] *     wx  *     wy; 
	
	} else cell_hyc = 0.;

	return cell_hyc;
}





//==========================================================
//
//   tgamma_hyc( int, vector<int>, vector<int>, int, vector<int>, double,
//		double, double, double, double, double, double )
//
//==========================================================

void DCCALShower_factory::tgamma_hyc( int nadc, vector<int> ia, vector<int> id, 
	int nzero, vector<int> iaz, double &chisq, double &e1, double &x1, double &y1, 
	double &e2, double &x2, double &y2 )
{
	//-------------- Local Declarations -------------//
	
	int ix, iy;
	int dof;
	
	double dx, dy;
	double dxy;
	double dxc, dyc;
	double dx0, dy0;
	double dx1, dy1;
	double dx2, dy2;
	double u, r, rsq, rsq2;
	double epsc, eps0, eps1, eps2;
	double stepmin, epsmax;
	double delch;
	double step;
	double cosi, scal;
	double chisq2, chisqc;
	double dchi, dchida, dchi0;
	double a1, a2;
	double ex;
	double e1c, x1c, y1c;
	double e2c, x2c, y2c;
	double gr, gre, grx, gry;
	double grc;
	double grec, grxc, gryc;
	double gx1, gy1;
	double gx2, gy2;
	double e0, x0, y0;
	double xx, yy, yx;
	
	double f1c, f2c, f1x, f2x, f1y, f2y;
	double chisqt, chisqt0, chisqt1, chisqt2;
	double chisqtx1, chisqtx2, chisqty1, chisqty2;
	double dchidax1, dchidax2, dchiday1, dchiday2;
	
	stepmin = 0.5;
	epsmax  = 0.9999l;
	delch   = 10.;
	
	//-------------- Event Analysis Code -------------//
	
	yx   = 0.;
	grx  = 0.; gry = 0.;
	gre  = 0.; gr  = 0.;
	dx0  = 0.; dy0 = 0.;
	eps0 = 0.;
	
	mom2_pht( nadc, ia, id, nzero, iaz, e0, x0, y0, xx, yy, yx );
	
	e2 = 0.;
	x2 = 0.;
	y2 = 0.;
	
	if( nadc <= 0 ) return;
	
	// choosing of the starting point
	
	dxy  = xx-yy;
	rsq2 = dxy*dxy + 4.*yx*yx;
	if( rsq2 < 1.e-20 ) rsq2 = 1.e-20;
	rsq = sqrt(rsq2);
	dxc = -sqrt((rsq+dxy)*2.);
	dyc =  sqrt((rsq-dxy)*2.);
	if( yx >= 0. ) dyc = -dyc;
	r = sqrt(dxc*dxc + dyc*dyc);
	epsc = 0.;
	for( int ii = 0; ii < nadc; ii++ ) {
	  ix = ia[ii]/100;
	  iy = ia[ii] - ix*100;
	  dx = static_cast<double>(ix) - x0;
	  dy = static_cast<double>(iy) - y0;
	  u  = dx*dxc/r + dy*dyc/r;
	  epsc = epsc - 0.01*id[ii]*u*fabs(u);
	}
	epsc = epsc/(0.01*e0*rsq);
	if(  epsc > 0.8 ) epsc = 0.8;
	if( epsc < -0.8 ) epsc = -0.8;
	dxc = dxc/sqrt(1.-(epsc*epsc));
	dyc = dyc/sqrt(1.-(epsc*epsc));
	
	
	// start of iterations:
	
	step   = 0.1;
	cosi   = 0.0;
	chisq2 = 1.e35;
	
	for( int iter = 0; iter < 100; iter++ ) {
	  c3to5_pht( e0, x0, y0, epsc, dxc, dyc, e1c, x1c, y1c, e2c, x2c, y2c );
	  eps1 = (1.+epsc)/2.;
	  eps2 = (1.-epsc)/2.;
	  chisqc = 0.;
	  for( int ii = 0; ii < nadc; ii++ ) {
	    ex = static_cast<double>(id[ii]);
	    ix = ia[ii]/100;
	    iy = ia[ii] - ix*100;
	    dx1 = x1c - static_cast<double>(ix);
	    dy1 = y1c - static_cast<double>(iy);
	    dx2 = x2c - static_cast<double>(ix);
	    dy2 = y2c - static_cast<double>(iy);
	    f1c = cell_hyc( dx1, dy1 );
	    f2c = cell_hyc( dx2, dy2 );
	    chisq2t_hyc( ex, e1c, dx1, dy1, e2c, dx2, dy2, f1c, f2c, chisqt );
	    chisqc += chisqt;
	  }
	  for( int ii = 0; ii < nzero; ii++ ) {
	    ex = 0;
	    ix = iaz[ii]/100;
	    iy = iaz[ii] - ix*100;
	    dx1 = x1c - static_cast<double>(ix);
	    dy1 = y1c - static_cast<double>(iy);
	    dx2 = x2c - static_cast<double>(ix);
	    dy2 = y2c - static_cast<double>(iy);
	    f1c = cell_hyc( dx1, dy1 );
	    f2c = cell_hyc( dx2, dy2 );
	    chisq2t_hyc( ex, e1c, dx1, dy1, e2c, dx2, dy2, f1c, f2c, chisqt );
	    chisqc += chisqt;
	  }
	  
	  if( chisqc >= chisq2 ) { // new step if no improvement
	    if( iter > 0 ) {
	      dchi = chisqc - chisq2;
	      dchi0 = gr*step;
	      step = 0.5*step/sqrt(1.+dchi/dchi0); 
	    }
	    step = 0.5*step;
	  } else { // calculate gradient
	    grec = 0.;
	    grxc = 0.;
	    gryc = 0.;
	    
	    for( int ii = 0; ii < nadc+nzero; ii++ ) {
	      if( ii < nadc ) {
	        ex = static_cast<double>(id[ii]);
		ix = ia[ii]/100;
		iy = ia[ii] - ix*100;
	      } else {
	        ex = 0.;
		if( ii-nadc >= nzero ) break;
	        ix = iaz[ii-nadc]/100;
	        iy = iaz[ii-nadc] - ix*100;
	      }
	      dx1 = x1c - static_cast<double>(ix);
	      dy1 = y1c - static_cast<double>(iy);
	      dx2 = x2c - static_cast<double>(ix);
	      dy2 = y2c - static_cast<double>(iy);
	      
	      f1c = cell_hyc( dx1, dy1 );
	      f2c = cell_hyc( dx2, dy2 );
	    
	      a1 = e1c*f1c;
	      a2 = e2c*f2c;
	      //a0 = a1 + a2;
	      
	      chisq2t_hyc( ex, e1c, dx1, dy1, e2c, dx2, dy2, f1c, f2c, chisqt0 );
	      chisq2t_hyc( ex, e1c+1., dx1, dy1, e2c, dx2, dy2, f1c, f2c, chisqt1 );
	      chisq2t_hyc( ex, e1c, dx1, dy1, e2c+1., dx2, dy2, f1c, f2c, chisqt2 );
	      
	      f1x = cell_hyc( x1c+0.05-static_cast<double>(ix), dy1 );
	      f2x = cell_hyc( x2c+0.05-static_cast<double>(ix), dy2 );
	      f1y = cell_hyc( dx1, y1c+0.05-static_cast<double>(iy) );
	      f2y = cell_hyc( dx2, y2c+0.05-static_cast<double>(iy) );
	      
	      chisq2t_hyc( ex, e1c, dx1+0.05, dy1, e2c, dx2, dy2, f1x, f2c, chisqtx1 );
	      chisq2t_hyc( ex, e1c, dx1, dy1, e2c, dx2+0.05, dy2, f1c, f2x, chisqtx2 );
	      chisq2t_hyc( ex, e1c, dx1, dy1+0.05, e2c, dx2, dy2, f1y, f2c, chisqty1 );
	      chisq2t_hyc( ex, e1c, dx1, dy1, e2c, dx2, dy2+0.05, f1c, f2y, chisqty2 );
	      
	      dchidax1 = 20.*(chisqtx1 - chisqt0);
	      dchidax2 = 20.*(chisqtx2 - chisqt0);
	      dchiday1 = 20.*(chisqty1 - chisqt0);
	      dchiday2 = 20.*(chisqty2 - chisqt0);
	      dchida   = 0.5*(chisqt1 + chisqt2 - chisqt0);
	      
	      gx1 = (e1c*f1x-a1)*dchidax1;
	      gx2 = (e2c*f2x-a2)*dchidax2;
	      gy1 = (e1c*f1y-a1)*dchiday1;
	      gy2 = (e2c*f2y-a2)*dchiday2;
	      
	      grec += dchida*(f1c-f2c)*e0 - ( (gx1+gx2)*dxc + (gy1+gy2)*dyc );
	      grxc += gx1*eps2 - gx2*eps1;
	      gryc += gy1*eps2 - gy2*eps1;
	      
	    }
	    
	    grc = sqrt( grec*grec + grxc*grxc + gryc*gryc );
	    if( grc < 1.e-6 ) grc = 1.e-6;
	    if( iter > 0 ) {
	      cosi = ( gre*grec + grx*grxc + gry*gryc )/(gr*grc);
	      scal = fabs((gr/grc) - cosi);
	      if( scal < 0.1 ) scal = 0.1;
	      step = step/scal;
	    }
	    chisq2 = chisqc;
	    eps0   = epsc;
	    dx0    = dxc;
	    dy0    = dyc;
	    gre    = grec;
	    grx    = grxc;
	    gry    = gryc;
	    gr     = grc;
	  }
	  epsc = eps0 - step*gre/gr;
	  while( fabs(epsc) > epsmax ) {
	    step = step/2.;
	    epsc = eps0 - step*gre/gr;
	  }
	  dxc = dx0 - step*grx/gr;
	  dyc = dy0 - step*gry/gr;
	  if( step*gr < stepmin ) break;
	}
	
	if( chisq*(nadc+nzero-2) - chisq2 < delch ) return;
	dof = nzero+nadc-5;
	if( dof < 1 ) dof = 1;
	chisq = chisq2/dof;
	
	c3to5_pht( e0, x0, y0, eps0, dx0, dy0, e1, x1, y1, e2, x2, y2 );	
	
	return;
}





//==========================================================
//
//   mom2_pht( int, vector<int>, vector<int>, int, vector<int>,
//		double, double, double, double, double, double )
//
//==========================================================

void DCCALShower_factory::mom2_pht( int nadc, vector<int> ia, vector<int> id, 
	int nzero, vector<int> iaz, double &a0, double &x0, double &y0, 
	double &xx, double &yy, double &yx)
{
	//-------------- Local Declarations --------------//

	int ix, iy;
	double dx, dy;
	double a;
	double corr;
	
	//-------------- Event Analysis Code -------------//
	
	a0 = 0.;
	x0 = 0.;
	y0 = 0.;
	xx = 0.;
	yy = 0.;
	yx = 0.;
	
	if( nadc <= 0 ) return;
	for( int ii = 0; ii < nadc; ii++ ) {
	  a  = static_cast<double>(id[ii]);
	  ix = ia[ii]/100;
	  iy = ia[ii] - ix*100;
	  a0 = a0 + a;
	  x0 = x0 + a*static_cast<double>(ix);
	  y0 = y0 + a*static_cast<double>(iy);
	}
	if( a0 <= 0. ) return;
	x0 = x0/a0;
	y0 = y0/a0;
	
	for( int ii = 0; ii < nadc; ii++ ) {
	  a  = static_cast<double>(id[ii])/a0;
	  ix = ia[ii]/100;
	  iy = ia[ii] - ix*100;
	  dx = static_cast<double>(ix) - x0;
	  dy = static_cast<double>(iy) - y0;
	  xx = xx + a*dx*dx;
	  yy = yy + a*dy*dy;
	  yx = yx + a*dx*dy;
	}
	
	corr = 0.;
	for( int ii = 0; ii < nadc; ii++ ) {
	  ix = ia[ii]/100;
	  iy = ia[ii] - ix*100;
	  dx = static_cast<double>(ix) - x0;
	  dy = static_cast<double>(iy) - y0;
	  corr = corr + cell_hyc( dx, dy );
	}
	if( COUNT_ZERO ) {
	  for( int ii = 0; ii < nzero; ii++ ) {
	    ix = iaz[ii]/100;
	    iy = iaz[ii] - ix*100;
	    dx = static_cast<double>(ix) - x0;
	    dy = static_cast<double>(iy) - y0;
	    corr = corr + cell_hyc( dx, dy );
	  }
	}
	
	corr = corr/1.006;
	if( corr < 0.8 ) {
	  corr = 0.8;
	} else if( corr > 1. ) {
	  corr = 1.0;
	}
	a0 = a0/corr;
	

	return;
}





//==========================================================
//
//   c3to5_pht( double, double, double, double, double, double,
//		double, double, double, double, double, double )
//
//==========================================================

void DCCALShower_factory::c3to5_pht( double e0, double x0, double y0, double eps, 
		double dx, double dy, double &e1, double &x1, double &y1, double &e2, 
		double &x2, double &y2 )
{
	e1 = e0*(1.+eps)/2.;
	e2 = e0 - e1;
	x1 = x0 + dx*(1.-eps)/2.;
	y1 = y0 + dy*(1.-eps)/2.;
	x2 = x0 - dx*(1.+eps)/2.;
	y2 = y0 - dy*(1.+eps)/2.;
	
	return;
}





//==========================================================
//
//   chisq2t_hyc( double, double, double, double, double, double,
//		double, double, double, double )
//
//==========================================================

void DCCALShower_factory::chisq2t_hyc( double ecell, double e1, double dx1, double dy1, 
	double e2, double dx2, double dy2, double f1, double f2, double &chisqt )
{
	double s;
	double p1, p2;
	
	if( TEST_P ) {
	  p1 = pow(0.0001*e1, 0.166);
	  p2 = pow(0.0001*e2, 0.166);
	} else {
	  p1 = 1.;
	  p2 = 1.;
	}
	
	if( e1 != 0. && e2 != 0. )
	  s = e1*sigma2(dx1, dy1, f1, e1)/p1 + e2*sigma2(dx2, dy2, f2, e2)/p2;
	else if( e1 == 0. && e2 == 0. )
	  s = 90000.;
	else if( e1 == 0. )
	  s = e2*sigma2(dx2, dy2, f2, e2)/p2;
	else 
	  s = e1*sigma2(dx1, dy1, f1, e1)/p1;
	
	chisqt = pow((e1*f1 + e2*f2 - ecell), 2.)/s;

	return;
}






//==========================================================
//
//   ucopy1( vector<int>, vector<int>, int, int )
//
//==========================================================

void DCCALShower_factory::ucopy1( vector<int> &ia, vector<int> &iwork, int start, int length )
{
	
	for( int ii = 0; ii < length; ii++ ) {
	  iwork.push_back( ia[start+ii] );
	}
	
	
	return;
}



//==========================================================
//
//   ucopy2( vector<int>, int, int, int )
//
//==========================================================

void DCCALShower_factory::ucopy2( vector<int> &ia, int start1, int start2, int length )
{
	
	vector<int> work;
	for( int ii = 0; ii < length; ii++ ) {
	  work.push_back( ia[start1+ii] );
	}
	
	for( int ii = 0; ii < length; ii++ ) {
	  ia[start2+ii] = work[ii];
	}
	
	
	return;
}




//==========================================================
//
//   ucopy3( vector<int>, vector<int>, int, int )
//
//==========================================================

void DCCALShower_factory::ucopy3( vector<int> &iwork, vector<int> &ia, int start, int length )
{
	
	for( int ii = 0; ii < length; ii++ ) {
	  ia[start+ii] = iwork[ii];	
	}
	
	
	return;
}


















