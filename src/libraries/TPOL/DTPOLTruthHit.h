#ifndef _DTPOLTruthHit_
#define _DTPOLTruthHit_

#include <JANA/JObject.h>

class DTPOLTruthHit:public JObject{
public:
    JOBJECT_PUBLIC(DTPOLTruthHit);

    float dEdx;
    bool primary;
    int track;
    int itrack;
    int ptype;
    float r;
    float phi;
    float z;
    float t;
    int sector;

    void Summarize(JObjectSummary& summary) {
        summary.add(track, "track", "%d");
        summary.add(itrack, "itrack", "%d");
        summary.add(primary, "primary", "%d");
        summary.add(ptype, "ptype", "%d");
        summary.add(dEdx*1.0E3, "dEdx(MeV/cm)", "%1.3f");
        summary.add(t, "t", "%3.2f");
        summary.add(r, "r", "%3.1f");
        summary.add(phi, "phi", "%1.3f");
        summary.add(z, "z", "%3.1f");
        summary.add(sector, "sector", "%d");
    }
};

#endif // _DTPOLTruthHit_

