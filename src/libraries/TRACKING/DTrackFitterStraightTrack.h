// $Id$
//
//    File: DTrackFitterStraightTrack.h
// Created: Tue Mar 12 10:22:32 EDT 2019
// Creator: staylor (on Linux ifarm1402.jlab.org 3.10.0-327.el7.x86_64 x86_64)
//

#ifndef _DTrackFitterStraightTrack_
#define _DTrackFitterStraightTrack_

#include <JANA/jerror.h>

#include <TRACKING/DTrackFitter.h>

#include <CDC/DCDCTrackHit.h>
#include <FDC/DFDCPseudo.h> 
#include <DMatrixSIMD.h>
#include <deque>
#include <TMatrixFSym.h>
#include "DResourcePool.h"

class DTrackFitterStraightTrack: public DTrackFitter { 
public:
  DTrackFitterStraightTrack(JEventLoop *loop);
  ~DTrackFitterStraightTrack();  
  
  enum state_vector{
    state_x,
    state_y,
    state_tx,
    state_ty,
  };

  enum state_cartesian{
    state_Px,
    state_Py,
    state_Pz,
    state_X,
    state_Y,
    state_Z,
    state_T
  };

  class trajectory_t{
  public:
  trajectory_t(double z,double t,DMatrix4x1 S,DMatrix4x4 J,DMatrix4x1 Skk,
	       DMatrix4x4 Ckk,unsigned int id=0,unsigned int numhits=0)
    :z(z),t(t),S(S),J(J),Skk(Skk),Ckk(Ckk),id(id),numhits(numhits){}
    double z,t; 
    DMatrix4x1 S;
    DMatrix4x4 J;
    DMatrix4x1 Skk;
    DMatrix4x4 Ckk;
    unsigned int id,numhits;   
  };
  
  typedef struct{
    double resi,err,d,delta,tdrift,ddrift,s,V;
    DMatrix4x1 S;
    DMatrix4x4 C;
  }cdc_update_t; 

  typedef struct{
    double d,tdrift,s;
    DMatrix4x1 S;
    DMatrix4x4 C;
    DMatrix2x2 V;
  }fdc_update_t;
   
  // Virtual methods from TrackFitter base class
  string Name(void) const {return string("StraightTrack");}
  fit_status_t FitTrack(void);
  double ChiSq(fit_type_t fit_type, DReferenceTrajectory *rt, double *chisq_ptr=NULL, int *dof_ptr=NULL, vector<pull_t> *pulls_ptr=NULL){return 0.;};
  
  
  unsigned int Locate(const vector<double>&xx,double x) const;
  double CDCDriftVariance(double t) const;
  double CDCDriftDistance(double dphi,double delta,double t) const;
  jerror_t SetReferenceTrajectory(double t0,double z,DMatrix4x1 &S,
				  const DCDCTrackHit *last_cdc,double &dzsign);
  jerror_t KalmanFilter(DMatrix4x1 &S,DMatrix4x4 &C,vector<int>&used_hits,
			vector<cdc_update_t>&updates,double &chi2,
			int &ndof,unsigned int iter);
  jerror_t Smooth(vector<cdc_update_t>&cdc_updates);
  DTrackFitter::fit_status_t FitCentralTrack(double z0,double t0,double dzsign,
					     DMatrix4x1 &Sbest,
					     DMatrix4x4 &Cbest,
					     double &chi2_best,int &ndof_best);
  double fdc_drift_distance(double time) const;
  double fdc_drift_variance(double t) const;
  DTrackFitter::fit_status_t FitForwardTrack(double t0,double start_z,
					     DMatrix4x1 &Sbest,
					     DMatrix4x4 &Cbest,
					     double &chi2_best,int &ndof_best);
  jerror_t SetReferenceTrajectory(double t0,double z,DMatrix4x1 &S);
  jerror_t KalmanFilter(DMatrix4x1 &S,DMatrix4x4 &C,vector<int>&used_fdc_hits,
			vector<int>&used_cdc_hits,vector<fdc_update_t>&updates,
			vector<cdc_update_t>&cdc_updates,double &chi2,
			int &ndof);
  jerror_t Smooth(vector<fdc_update_t>&fdc_updates,
		  vector<cdc_update_t>&cdc_updates); 
  shared_ptr<TMatrixFSym> Get7x7ErrorMatrix(shared_ptr<TMatrixFSym>C,
					    DMatrix4x1 &S);
	
 private:
  deque<trajectory_t>trajectory;
  deque<trajectory_t>best_trajectory;

  bool COSMICS,DO_PRUNING;
  int VERBOSE;
  double CHI2CUT;  
  int PLANE_TO_SKIP;
 
  
  // drift time tables
  vector<double>cdc_drift_table;
  vector<double>fdc_drift_table;
  
  // Resolution parameters
  double CDC_RES_PAR1,CDC_RES_PAR2,CDC_RES_PAR3;
  // Parameters for fdc drift resolution
  double DRIFT_RES_PARMS[3];
  double DRIFT_FUNC_PARMS[4];

  // variables to deal with CDC straw sag
  vector<vector<double> >max_sag;
  vector<vector<double> >sag_phi_offset;
  double long_drift_func[3][3];
  double short_drift_func[3][3];
  
  double downstreamEndplate,upstreamEndplate,cdc_endplate_rmin,cdc_endplate_rmax;
  
  shared_ptr<DResourcePool<TMatrixFSym>> dResourcePool_TMatrixFSym;
  
};

#endif // _DTrackFitterStraightTrack_

