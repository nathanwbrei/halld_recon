/*
 *  File: DCCALHit_factory.h
 *
 * Created on 11/25/18 by A.S. 
 * use structure similar to FCAL
 */


#ifndef _DCCALShower_
#define _DCCALShower_

#include <DVector3.h>
#include "DCCALHit.h"
#include "ccal.h"

using namespace std;

#include <JANA/JObject.h>
#include <JANA/JFactory.h>
using namespace jana;

#define CCAL_USER_HITS_MAX  1000

class DCCALShower : public JObject {
  public:
      JOBJECT_PUBLIC(DCCALShower);
      
      DCCALShower();
      ~DCCALShower();
      
      /*----------------------------------------//
      
      Description of DCCALShower variables:
      
      - type: 
      
      	type%10:
      	0 - shower is in middle layers of calorimeter
	1 - shower is in most inner layer (under absorber)
	2 - shower is in most outer layer
	
	type/10:
	0 - shower is in a single-peak cluster with just 1 gamma in the peak
	1 - shower is in a single-peak cluster with 2 gammas in the peak
	2 - shower is in a multi-peak cluster with just 1 gamma in this peak
	3 - shower is in a multi-peak cluster with 2 gammas in this peak
	
      - dime:  number of hits contributing to shower
      - id:    only non-zero for showers that share a single peak
      - idmax: the channel number of cell with maximum energy
      
      - E: energy of reconstructed shower from island
      - Esum: energy of shower calculated as sum of block energies
      - x,y: position reported from island algorithm
      - x1, y1: position calculated using logarithmic weights
      	x1 = Sum( xi*wi ) where wi = 4.2 + ln( Ei / E )
      - chi2: The chi2-value reported from island
      - sigma_E: energy resolution (needs work)
      - Emax: energy of maximum cell
      - time: energy-weighted average of shower's constituent times
      	After a time-walk correction     
      
      //----------------------------------------*/

      double E;
      double Esum;
      double x;
      double y;
      double z;
      double x1;
      double y1;
      double chi2;
      double sigma_E;
      double Emax;
      double time;
      double sigma_t;
      
      int type;
      int dime;
      int id;
      int idmax;
      
      int id_storage[MAX_CC];
      double en_storage[MAX_CC];
      double t_storage[MAX_CC];
      
      void toStrings(vector<pair<string,string> > &items) const {
	AddString(items, "E(GeV)",    "%2.3f",  E);
	AddString(items, "Emax(GeV)", "%2.3f",  Emax);
	AddString(items, "x(cm)",     "%3.3f",  x);
	AddString(items, "y(cm)",     "%3.3f",  y);
	AddString(items, "z(cm)",     "%3.3f",  z);
	AddString(items, "x1(cm)",    "%3.3f",  x1);
	AddString(items, "y1(cm)",    "%3.3f",  y1);
	AddString(items, "chi2",      "%3.3f",  chi2);
	AddString(items, "type",      "%3d",    type);
	AddString(items, "dime",      "%3d",    dime);
	AddString(items, "id",        "%3d",    id);
	AddString(items, "idmax",     "%3d",    idmax);
	AddString(items, "sigma_E",   "%3.1f",  sigma_E);
	AddString(items, "t(ns)",     "%2.3f",  time);
      }
      
 private:
      
};


#endif  //  _DCCALShower_

