// $Id$
//
//    File: DTOFPaddleHit_factory.cc
// Created: Thu Jun  9 10:05:21 EDT 2005
// Creator: davidl (on Darwin wire129.jlab.org 7.8.0 powerpc)
//
// Modified: Wed Feb 12 13:19:10 EST 2014 by B. Zihlmann
//           reflect the changes in the TOF geometry with
//           19 LWB, 2 LNB, 2 SB, 2LNB, 19 LWB
//                          2 SB
//           LWB: long wide bars
//           LNB: long narrow bars
//           SB:  short bars
//           the bar numbering goes from 1 all through 46 with
//           bar 22 and 23 are the 4 short bars distinguished by north/south
//


#include <iostream>
using namespace std;

#include <JANA/JEvent.h>
#include <JANA/Calibrations/JCalibrationManager.h>

#include "DTOFPaddleHit_factory.h"
#include "DTOFHit.h"
#include "DTOFHitMC.h"
#include "DTOFPaddleHit.h"
#include <math.h>

//#define NaN std::numeric_limits<double>::quiet_NaN()
#define BuiltInNaN __builtin_nan("")

//------------------
// Init
//------------------
void DTOFPaddleHit_factory::Init()
{
	TOF_POINT_TAG="";
	GetApplication()->SetDefaultParameter("TOF:TOF_POINT_TAG", TOF_POINT_TAG,"");
}

//------------------
// BeginRun
//------------------
void DTOFPaddleHit_factory::BeginRun(const std::shared_ptr<const JEvent>& event)
{
  auto run_number = event->GetRunNumber();
  auto app = event->GetJApplication();
  auto calibration = app->GetService<JCalibrationManager>()->GetJCalibration(run_number);

  // load values from geometry
  event->Get(TOFGeom);
  TOF_NUM_PLANES = TOFGeom[0]->Get_NPlanes();
  TOF_NUM_BARS = TOFGeom[0]->Get_NBars();
  HALFPADDLE = TOFGeom[0]->Get_HalfLongBarLength();


  map<string, double> tofparms; 
  string locTOFParmsTable = TOFGeom[0]->Get_CCDB_DirectoryName() + "/tof_parms";
  if( !calibration->Get(locTOFParmsTable.c_str(), tofparms)) {
    //cout<<"DTOFPaddleHit_factory: loading values from TOF data base"<<endl;

    C_EFFECTIVE    =    tofparms["TOF_C_EFFECTIVE"];
    //HALFPADDLE     =    tofparms["TOF_HALFPADDLE"];
    E_THRESHOLD    =    tofparms["TOF_E_THRESHOLD"];
    ATTEN_LENGTH   =    tofparms["TOF_ATTEN_LENGTH"];
  } else {
    cout << "DTOFPaddleHit_factory: Error loading values from TOF data base" <<endl;

    C_EFFECTIVE = 15.;    // set to some reasonable value
    //HALFPADDLE = 126;     // set to some reasonable value
    E_THRESHOLD = 0.0005; // energy threshold in GeV
    ATTEN_LENGTH = 400.;  // 400cm attenuation length
  }

  ENERGY_ATTEN_FACTOR=exp(HALFPADDLE/ATTEN_LENGTH);
  TIME_COINCIDENCE_CUT=2.*HALFPADDLE/C_EFFECTIVE;

  string locTOFPropSpeedTable = TOFGeom[0]->Get_CCDB_DirectoryName() + "/propagation_speed";
  if(eventLoop->GetCalib(locTOFPropSpeedTable.c_str(), propagation_speed))
    jout << "Error loading " << locTOFPropSpeedTable << " !" << endl;
  string locTOFAttenLengthTable = TOFGeom[0]->Get_CCDB_DirectoryName() + "/attenuation_lengths";
  if(eventLoop->GetCalib(locTOFAttenLengthTable.c_str(), AttenuationLengths))
    jout << "Error loading " << locTOFAttenLengthTable << " !" << endl;
}

//------------------
// Process
//------------------
void DTOFPaddleHit_factory::Process(const std::shared_ptr<const JEvent>& event)
{

  vector<const DTOFHit*> hits;
  event->Get(hits,TOF_POINT_TAG.c_str());

  vector<const DTOFHit*> P1hitsL;
  vector<const DTOFHit*> P1hitsR;
  vector<const DTOFHit*> P2hitsL;
  vector<const DTOFHit*> P2hitsR;

  //int P1L[100];
  //int P1R[100];
  //int P2L[100];
  //int P2R[100];

  //int c1l = 0;
  //int c1r = 0;
  //int c2l = 0;
  //int c2r = 0;

  // sort the tof hits into left and right PMTs for both planes

  for (unsigned int i = 0; i < hits.size(); i++){
    const DTOFHit *hit = hits[i];
    if (hit->has_fADC && hit->has_TDC){ // good hits have both ADC and TDC info
      if (hit->plane){
	if (hit->end){
	  P2hitsR.push_back(hit);
	  //P2R[c2r++] = i;
	} else {
	  P2hitsL.push_back(hit);	
	  //P2L[c2l++] = i;
	}
      } else {
	if (hit->end){
	  P1hitsR.push_back(hit);
	  //P1R[c1r++] = i;
	} else {
	  P1hitsL.push_back(hit);
	  //P1L[c1l++] = i;
	}
      }
    }
  }

  // find matching Up/Down TOFHits
  for (unsigned int i=0; i<P1hitsL.size(); i++){

    int bar = P1hitsL[i]->bar;

    if ((bar < TOFGeom[0]->Get_FirstShortBar() ) || (bar > TOFGeom[0]->Get_LastShortBar())) {
      
      // we are dealing with double ended readout paddles:
      for (unsigned int j=0; j<P1hitsR.size(); j++){      
	if (bar==P1hitsR[j]->bar 
	    && fabs(P1hitsR[j]->t-P1hitsL[i]->t)<TIME_COINCIDENCE_CUT
	    && (P1hitsL[i]->dE>E_THRESHOLD || P1hitsR[j]->dE>E_THRESHOLD)){
	  DTOFPaddleHit *hit = new DTOFPaddleHit;
	  hit->bar = bar;
	  hit->orientation   = P1hitsL[i]->plane;
	  hit->E_north = P1hitsL[i]->dE;
	  hit->t_north = P1hitsL[i]->t;
	  hit->AddAssociatedObject(P1hitsL[i]);
	  hit->E_south = P1hitsR[j]->dE;
	  hit->t_south = P1hitsR[j]->t;      
	  hit->AddAssociatedObject(P1hitsR[j]);  

	  Insert(hit);
	}
      }
    } 
  }
  

  // find Full length bar with not corresponding hit on the right side
  for (unsigned int i=0; i<P1hitsL.size(); i++){ 
    int bar = P1hitsL[i]->bar;
    int found = 0;
    
    if ((bar < TOFGeom[0]->Get_FirstShortBar()) || (bar > TOFGeom[0]->Get_LastShortBar())) {
      for (unsigned int j=0; j<P1hitsR.size(); j++){      
	if (bar==P1hitsR[j]->bar){
	  found = 1;
	}
      }
    }

    if (!found){
      if (P1hitsL[i]->dE>E_THRESHOLD){
	DTOFPaddleHit *hit = new DTOFPaddleHit;
	hit->bar = bar;
	hit->orientation   = P1hitsL[i]->plane;
	hit->E_north = P1hitsL[i]->dE;
	hit->t_north = P1hitsL[i]->t;
	hit->E_south = 0.;
	hit->t_south = 0.;  
	hit->AddAssociatedObject(P1hitsL[i]);

	Insert(hit);
      }
    }
  }

  // find full length bar with no corresponding hit on the left side
  for (unsigned int i=0; i<P1hitsR.size(); i++){   
    int bar = P1hitsR[i]->bar;
    int found = 0;

    if ((bar < TOFGeom[0]->Get_FirstShortBar()) || (bar > TOFGeom[0]->Get_LastShortBar())) {
      for (unsigned int j=0; j<P1hitsL.size(); j++){      
	if (bar==P1hitsL[j]->bar){
	  found = 1;
	}
      }
    }

    if (!found){
      if (P1hitsR[i]->dE>E_THRESHOLD){
	DTOFPaddleHit *hit = new DTOFPaddleHit;
	hit->bar = bar;
	hit->orientation   = P1hitsR[i]->plane;
	hit->E_south = P1hitsR[i]->dE;
	hit->t_south = P1hitsR[i]->t;
	hit->E_north = 0.;
	hit->t_north = 0.;      
	hit->AddAssociatedObject(P1hitsR[i]);

	Insert(hit);
      }
    }
  }
  

  // now the same thing for plane 2
  for (unsigned int i=0; i<P2hitsL.size(); i++){
    int bar = P2hitsL[i]->bar; 
    if ((bar <  TOFGeom[0]->Get_FirstShortBar()) || (bar > TOFGeom[0]->Get_LastShortBar() )){
      for (unsigned int j=0; j<P2hitsR.size(); j++){      
	if (bar==P2hitsR[j]->bar 
	    && fabs(P2hitsR[j]->t-P2hitsL[i]->t)<TIME_COINCIDENCE_CUT
	    && (P2hitsL[i]->dE>E_THRESHOLD || P2hitsR[j]->dE>E_THRESHOLD)){
	  DTOFPaddleHit *hit = new DTOFPaddleHit;
	  hit->bar = bar;
	  hit->orientation   = P2hitsL[i]->plane;
	  hit->E_north = P2hitsL[i]->dE;
	  hit->t_north = P2hitsL[i]->t;
	  hit->AddAssociatedObject(P2hitsL[i]);
	  hit->E_south = P2hitsR[j]->dE;
	  hit->t_south = P2hitsR[j]->t;      
	  hit->AddAssociatedObject(P2hitsR[j]);
	  
	  Insert(hit);
	}
      }
    }
  }
  


  // Plane 2 full length paddles with hit only on the left
  for (unsigned int i=0; i<P2hitsL.size(); i++){   
    int bar = P2hitsL[i]->bar;
    int found = 0;
    
    if ((bar < TOFGeom[0]->Get_FirstShortBar()) || (bar > TOFGeom[0]->Get_LastShortBar())) {
      for (unsigned int j=0; j<P2hitsR.size(); j++){      
	if (bar==P2hitsR[j]->bar){
	  found = 1;
	}
      }
    }
    
    if (!found){
      if (P2hitsL[i]->dE>E_THRESHOLD){
	DTOFPaddleHit *hit = new DTOFPaddleHit;
	hit->bar = bar;
	hit->orientation   = P2hitsL[i]->plane;
	hit->E_north = P2hitsL[i]->dE;
	hit->t_north = P2hitsL[i]->t;
	hit->E_south = 0.;
	hit->t_south = 0.;      
	hit->AddAssociatedObject(P2hitsL[i]);

	Insert(hit);
      }
    }
  }

  // Plane 2 with full length paddle and hit only on the right.
  for (unsigned int i=0; i<P2hitsR.size(); i++){   
    int bar = P2hitsR[i]->bar;
    int found = 0;

    if ((bar < TOFGeom[0]->Get_FirstShortBar()) || (bar > TOFGeom[0]->Get_LastShortBar())) {
      for (unsigned int j=0; j<P2hitsL.size(); j++){      
	if (bar==P2hitsL[j]->bar){
	  found = 1;
	}
      }
    }

    if (!found){
       if (P2hitsR[i]->dE>E_THRESHOLD){
	DTOFPaddleHit *hit = new DTOFPaddleHit;
	hit->bar = bar;
	hit->orientation   = P2hitsR[i]->plane;
	hit->E_south = P2hitsR[i]->dE;
	hit->t_south = P2hitsR[i]->t;
	hit->E_north = 0.;
	hit->t_north = 0.;      
	hit->AddAssociatedObject(P2hitsR[i]);

	Insert(hit);
      }
    }
  }


  for (int i=0;i<(int)mData.size(); i++) {
    
    DTOFPaddleHit *hit = mData[i];

    int check = -1;
    if (hit->E_north > E_THRESHOLD) {
      check++;
    }
    if (hit->E_south > E_THRESHOLD) {
      check++;
    }
    
    if (check > 0 ){

      int id=TOF_NUM_BARS*hit->orientation+hit->bar-1;
      double v=propagation_speed[id];
      hit->meantime = (hit->t_north+hit->t_south)/2. - HALFPADDLE/v;
      hit->timediff = (hit->t_south - hit->t_north)/2.;
      float pos = hit->timediff * v;  
      hit->pos = pos;
      hit->dpos      = 2.;  // manually/artificially set to 2cm. 
      
      // mean energy deposition at the location of the hit position
      // use geometrical mean
      //hit->dE = ENERGY_ATTEN_FACTOR*sqrt(hit->E_north*hit->E_south);

      float xl =  pos; // distance to left PMT 
      float xr =  pos; // distance to right PMT
      int idl = hit->orientation*TOF_NUM_PLANES*TOF_NUM_BARS + hit->bar-1;
      int idr = idl+TOF_NUM_BARS;
      float d1 = AttenuationLengths[idl][0];
      float d2 = AttenuationLengths[idl][1];

      // reference distance is 144cm from PMT
      // if pos==0 (center) no change in dE is required => att_i = 1.
      // so change ref. distance to HALFPADDLE
      //float att_left = ( TMath::Exp(-144./d1) +  TMath::Exp(-144./d2)) / 
      //( TMath::Exp(-xl/d1) +  TMath::Exp(-xl/d2));
      // adc2E values in DTOFHit_factory are for pos=0

      float att_left = (TMath::Exp(-xl/d1) +  TMath::Exp(-xl/d2))/2.;

      d1 = AttenuationLengths[idr][0];
      d2 = AttenuationLengths[idr][1];
      //float att_right = ( TMath::Exp(-144./d1) +  TMath::Exp(-144./d2)) / 
      //( TMath::Exp(-xr/d1) +  TMath::Exp(-xr/d2));

      float att_right = ( TMath::Exp(xr/d1) +  TMath::Exp(xr/d2))/2.;

      hit->dE = (hit->E_north*att_left + hit->E_south*att_right)/2.;
    } else {
      hit->meantime = BuiltInNaN;
      hit->timediff = BuiltInNaN;
      hit->pos = BuiltInNaN;
      hit->dpos = BuiltInNaN;
      hit->dE = BuiltInNaN;
   }

  }
}

