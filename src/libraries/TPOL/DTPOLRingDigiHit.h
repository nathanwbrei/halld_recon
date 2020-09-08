#ifndef _DTPOLRingDigiHit_
#define _DTPOLRingDigiHit_

#include <JANA/JObject.h>

class DTPOLRingDigiHit: public JObject{
	public:
		JOBJECT_PUBLIC(DTPOLRingDigiHit);

		int      ring;                 // ring number 1-24
		uint32_t pulse_integral;       // identified pulse integral as returned by FPGA algorithm
		uint32_t pulse_time;           // identified pulse time as returned by FPGA algorithm
		uint32_t pedestal;             // pedestal info used by FPGA (if any)
		uint32_t QF;                   // Quality Factor from FPGA algorithms
		uint32_t nsamples_integral;    // number of samples used in integral 
		uint32_t nsamples_pedestal;    // number of samples used in pedestal


		void Summarize(JObjectSummary& summary) {
			summary.add(ring, "ring", "%d");
			summary.add(pulse_integral, "pulse_integral", "%d");
			summary.add(pulse_time, "pulse_time", "%d");
			summary.add(pedestal, "pedestal", "%d");
			summary.add(QF, "QF", "%d");
			summary.add(nsamples_integral, "nsamples_integral", "%d");
			summary.add(nsamples_pedestal, "nsamples_pedestal", "%d");
		}
		
};

#endif // _DTPOLRingDigiHit_
