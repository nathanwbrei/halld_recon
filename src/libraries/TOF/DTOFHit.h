// $Id: DTOFHit.h Tue Jan 18 16:15:26 EST 2011
//
/// File:    DTOFHit.h
/// Created: Tue Jan 18 16:15:26 EST 2011
/// Creator: B. Zihlmann
/// Purpose: Container class to hold Monte Carlo data, unsmeared and 
///          smeared with the MC tag.
//

#ifndef _DTOFHit_
#define _DTOFHit_

#include "JANA/JObject.h"

class DTOFHit: public JObject{
  
 public:
  JOBJECT_PUBLIC(DTOFHit);
  
  int plane;      // plane (0: vertical, 1: horizontal)
  int bar;        // bar number
  int end;        // left/right 0/1 or North/South 0/1
  float dE;       // attenuated energy deposition
  float Amp;      // Signal max Amplitude, ADC counts
  float t_fADC;   // time from adc
  float t_TDC;  // time from tdc
  float t; // walk corrected time
  bool has_fADC;
  bool has_TDC;
  
  void Summarize(JObjectSummary& summary) const override {
    summary.add(bar, "bar", "%d");
    summary.add(plane, "plane", "%d");
    summary.add(end, "end", "%d");
    summary.add(dE, "dE", "%12.4f");
    summary.add(Amp, "Amp", "%12.4f");
    summary.add(t, "t", "%12.4f");
    summary.add(t_TDC, "t_TDC", "%12.4f");
    summary.add(t_fADC, "t_fADC", "%12.4f");
  }
};

#endif // _DTOFHit_

