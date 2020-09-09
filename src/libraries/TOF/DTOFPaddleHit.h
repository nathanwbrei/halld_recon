// $Id$
//
//    File: DTOFPaddleHit.h
// Created: Thu Jun  9 10:05:21 EDT 2005
// Creator: davidl (on Darwin wire129.jlab.org 7.8.0 powerpc)
//

#ifndef _DTOFPaddleHit_
#define _DTOFPaddleHit_

#include <JANA/JObject.h>

class DTOFPaddleHit: public JObject{
 public:
  JOBJECT_PUBLIC(DTOFPaddleHit);
  
  int orientation;  // 0: vertical,  1: horizontal
  int bar;          // bar number
  float t_north;    // time of light at end of bar  (calibrated) 
  float E_north;    // attenuated energy deposition  (calibrated)
  float t_south;    // time of light at end of bar  (calibrated) 
  float E_south;    // attenuated energy deposition  (calibrated)
  
  float meantime;   // equivalent to time of flight
  float timediff;    // north - south time difference
  float pos;        // hit position in paddle
  float dpos;       // estimated uncertainty in hitposition
  float dE;         // weighted energy deposition
  
  void Summarize(JObjectSummary& summary) const override {
    summary.add(orientation, "orientation", "%d");
    summary.add(bar, "bar", "%d");
    summary.add(t_north, "t_north", "%1.3f");
    summary.add(E_north, "E_north", "%1.3f");
    summary.add(t_south, "t_south", "%1.3f");
    summary.add(E_south, "E_south", "%1.3f");
    summary.add(meantime, "meantime", "%1.3f");
    summary.add(timediff, "timediff", "%1.3f");
    summary.add(pos, "pos", "%1.3f");
    summary.add(dpos, "dpos", "%1.3f");
    summary.add(dE, "dE", "%1.3f");
  }
};

#endif // _DTOFPaddleHit_

