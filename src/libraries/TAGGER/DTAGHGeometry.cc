//
// File: DTAGHGeometry.cc
// Created: Sat Jul 5 10:18:56 EST 2014
// Creator: jonesrt on gluey.phys.uconn.edu
//
//  10/19/2019 A.S 
//
//  Modify calculation of the photon beam energy to account 
//  for the fact that the energy of bremsstrahlung electrons detected
//  by each tagger counter does not depend on the electron beam energy.
//  The photon beam energy E_gamma has to be computed as
//  
//     E_gamma = R * E_endpoint_calib  +  DE,  where
//
// 

#include <stdlib.h>
#include <iostream>
#include <map>

#include <JANA/JEvent.h>
#include <JANA/Calibrations/JCalibrationManager.h>

#include "DTAGHGeometry.h"

using std::string;

const unsigned int DTAGHGeometry::kCounterCount = 274;

//---------------------------------
// DTAGHGeometry    (Constructor)
//---------------------------------
DTAGHGeometry::DTAGHGeometry(const std::shared_ptr<const JEvent>& event)
{
   auto run_number = event->GetRunNumber();
   auto app = event->GetJApplication();
   auto calibration = app->GetService<JCalibrationManager>()->GetJCalibration(run_number);

   /* read tagger set endpoint energy from calibdb */
   std::map<string,double> result1;
   calibration->Get("/PHOTON_BEAM/endpoint_energy", result1);
   if (result1.find("PHOTON_BEAM_ENDPOINT_ENERGY") == result1.end()) {
      std::cerr << "Error in DTAGHGeometry constructor: "
                << "failed to read photon beam endpoint energy "
                << "from calibdb at /PHOTON_BEAM/endpoint_energy" << std::endl;
      m_endpoint_energy_GeV = 0;
   }
   else {
      m_endpoint_energy_GeV = result1["PHOTON_BEAM_ENDPOINT_ENERGY"];
   }

   /* read hodoscope counter energy bounds from calibdb */
   std::vector<std::map<string,double> > result2;
   calibration->Get("/PHOTON_BEAM/hodoscope/scaled_energy_range", result2);
   if (result2.size() != kCounterCount) {
      jerr << "Error in DTAGHGeometry constructor: "
           << "failed to read photon beam scaled_energy_range table "
           << "from calibdb at /PHOTON_BEAM/hodoscope/scaled_energy_range" << jendl;
      for (unsigned int i=0; i <= TAGH_MAX_COUNTER; ++i) {
         m_counter_xlow[i] = 0;
         m_counter_xhigh[i] = 0;
      }
   }
   else {
      for (unsigned int i=0; i < result2.size(); ++i) {
	int ctr = (result2[i])["counter"];
         m_counter_xlow[ctr] = (result2[i])["xlow"];
         m_counter_xhigh[ctr] = (result2[i])["xhigh"];
      }
   }


   int status = 0;
   m_endpoint_energy_calib_GeV = 0.;
   
   std::map<string,double> result3;
   status = calibration->Get("/PHOTON_BEAM/hodoscope/endpoint_calib",result3);
   
   
   if(!status){
     if (result3.find("TAGGER_CALIB_ENERGY") == result3.end()) {
       std::cerr << "Error in DTAGHGeometry constructor: "
		 <<  "failed to read  endpoint_calib field "
		 <<  "from /PHOTON_BEAM/hodoscope/endpoint_calib table" << std::endl;
       
     } else {
       m_endpoint_energy_calib_GeV  = result3["TAGGER_CALIB_ENERGY"];

       printf(" Correct Beam Photon Energy  (TAGH)   %f \n\n", m_endpoint_energy_calib_GeV);

     }
   }
   
}

DTAGHGeometry::~DTAGHGeometry() { }


bool DTAGHGeometry::E_to_counter(double E, unsigned int &counter) const
{  
  for (counter = 1; counter <= kCounterCount; ++counter) {     
    
    double Emin = getElow(counter);
    double Emax = getEhigh(counter);
    
    if ( E >= Emin &&  E <= Emax )
      {
	return true;
      }
  }
  return false;
}


double DTAGHGeometry::getElow(unsigned int counter) const
{
  if (counter > 0 && counter <= kCounterCount){
    if(m_endpoint_energy_calib_GeV  > 0){

      double delta_E  =  m_endpoint_energy_GeV  -  m_endpoint_energy_calib_GeV;
      double Emin     =  m_counter_xlow[counter]*m_endpoint_energy_calib_GeV  +  delta_E;
      
      return Emin;

    } else
      return m_endpoint_energy_GeV * m_counter_xlow[counter];

  }  else
    return 0;
}


double DTAGHGeometry::getEhigh(unsigned int counter) const
{
  if (counter > 0 && counter <= kCounterCount){
    if(m_endpoint_energy_calib_GeV  > 0){
      
      double delta_E  =  m_endpoint_energy_GeV  -  m_endpoint_energy_calib_GeV;
      double Emax     =  m_counter_xhigh[counter]*m_endpoint_energy_calib_GeV  +  delta_E;
      
      return Emax;
      
    } else    
      return m_endpoint_energy_GeV * m_counter_xhigh[counter];
    
  } else
    return 0;
}
