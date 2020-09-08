#ifndef _DTPOLHit_
#define _DTPOLHit_

#include <JANA/JObject.h>

class DTPOLHit: public JObject {
public:
    JOBJECT_PUBLIC(DTPOLHit);

    int sector;    // sector number 1-32
    double phi;
    int ring;      // ring number 1-24
    double theta;
    unsigned int nsamples;
    unsigned int w_samp1;
    unsigned int w_min;
    unsigned int w_max;
    unsigned int integral; 
    double pulse_peak;     // Energy loss in keV
    double dE_proxy;
    double t_proxy;
    double dE;
    double t;

    void Summarize(JObjectSummary& summary) {
        summary.add(sector, "sector", "%d");
        summary.add(phi, "phi", "%3.3f");
        summary.add(ring, "ring", "%d");
        summary.add(theta, "theta", "%3.3f");
        summary.add(nsamples, "nsamples", "%d");
        summary.add(w_samp1, "w_samp1", "%d");
        summary.add(w_min, "w_min", "%d");
        summary.add(w_max, "w_max", "%d");
        summary.add(integral, "integral", "%d");
        summary.add(pulse_peak, "pulse_peak", "%3.3f");
        summary.add(dE_proxy, "dE_proxy", "%3.3f");
        summary.add(t_proxy, "t_proxy", "%3.3f");
        summary.add(dE, "dE", "%3.3f");
        summary.add(t, "t", "%3.3f");
    }
};

#endif // _DTPOLHit_
