// $Id: DTOFHitRawMC.h Wed Jan 19 14:22:41 EST 2011
//
/// File:    DTOFHitMC.h
/// Created: Wed Jan 19 14:22:41 EST 2011
/// Creator: B. Zihlmann
/// Purpose: Container class to hold Monte Carlo track data, 
///          like track id number, particle type ect.
//

#ifndef _DTOFHitMC_
#define _DTOFHitMC_

#include "JANA/JObject.h"

class DTOFHitMC: public JObject{
  
 public:
  JOBJECT_PUBLIC(DTOFHitMC);
  
  int plane;		// plane (0: vertical, 1: horizontal)
  int bar;		// bar number
  int end;              // 0: north (beam-left), 1: south (beam-right)
  int ptype;		// GEANT particle type
  int itrack;           // Track number of primary particle causing the hit
  float dist;           // Hit distance from center of paddle (or x=0)
  float x;              // hit location in global coordiantes
  float y;
  float z;
  float px;		// particle momentum
  float py;
  float pz;
  float E;		// particle Energy
  
  void Summarize(JObjectSummary& summary) const override {
    summary.add(bar, "bar", "%d");
    summary.add(plane, "plane", "%d");
    summary.add(end, "end", "%d");
    summary.add(dist, "dist", "%12.4e");
    summary.add(x, "x", "%12.4e");
    summary.add(y, "y", "%12.4e");
    summary.add(z, "z", "%12.4e");
    summary.add(px, "px", "%12.4e");
    summary.add(py, "py", "%12.4e");
    summary.add(pz, "pz", "%12.4e");
    summary.add(E, "E", "%12.4e");
    summary.add(ptype, "ptype", "%d");
    summary.add(itrack, "itrack", "%d");
  }
};

#endif // _DTOFHitMC_

