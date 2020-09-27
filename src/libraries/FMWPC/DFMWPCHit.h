// $Id$
//
//    File: DFMWPCHit.h
// Created: Tue Jun 16 07:07:54 EDT 2015
// Creator: davidl (on Darwin harriet.jlab.org 13.4.0 i386)
//

#ifndef _DFMWPCHit_
#define _DFMWPCHit_

#include <JANA/JObject.h>

class DFMWPCHit: public JObject{
	public:
		JOBJECT_PUBLIC(DFMWPCHit);

		int layer;   // 1-8
		int wire;    // 1-144
		float dE;    // energy in GeV
		float t;     // time in ns

		void Summarize(JObjectSummary& summary) const override {
			summary.add(layer, "layer", "%d");
			summary.add(wire, "wire", "%d");
			summary.add(dE*1.0E6, "dE(keV)", "%3.1f");
			summary.add(t, "t", "%3.3f");
		}

};

#endif // _DFMWPCHit_

