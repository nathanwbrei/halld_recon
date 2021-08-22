// $Id$
//
//    File: DSCHit.h
// Created: Wed Feb  7 10:46:20 EST 2007
// Creator: davidl (on Linux megrez.jlab.org 2.6.9-42.0.2.ELsmp x86_64)
//

#ifndef _DSCHit_
#define _DSCHit_

#include <JANA/JObject.h>

class DSCHit:public JObject{
	public:
		JOBJECT_PUBLIC(DSCHit);
		
		int sector;	    // sector number 1-30
		float dE;       // Energy loss in GeV
		float t;        // best time (walk-corrected tdc)
		float t_TDC;   // time from TDC, no walk correction
		float t_fADC; // time from fADC
		float pulse_height; // amplitude of pulse (used in time-walk corrections)
		bool has_fADC; 
		bool has_TDC;

		void Summarize(JObjectSummary& summary) const override {
			summary.add(sector, "sector", "%d");
			summary.add(dE, "dE", "%3.3f");
			summary.add(t, "t", "%3.3f");
			summary.add(t_TDC, "t_TDC", "%3.3f");
			summary.add(t_fADC, "t_fADC", "%3.3f");
			summary.add((int)has_fADC, "has_fADC", "%d");
			summary.add((int)has_TDC, "has_TDC", "%d");
		}
};

#endif // _DSCHit_

