#ifndef _DTPOLHit_factory_
#define _DTPOLHit_factory_

#include <JANA/JFactoryT.h>
#include "TTAB/DTranslationTable.h"
#include "DTPOLHit.h"


class DTPOLHit_factory:public JFactoryT<DTPOLHit>{
public:
    DTPOLHit_factory(){};
    ~DTPOLHit_factory(){};

    // Geometric information
    static const int NSECTORS   = 32;
    static const int NRINGS     = 24;

    static const double SECTOR_DIVISION;
    static const double INNER_RADIUS;      // From "ACTIVE INNER DIAMETER" in catalog
    static const double OUTER_RADIUS ;     // From "ACTIVE OUTER DIAMETER" in catalog
    static const double RING_DIVISION ;
    // (OUTER_RADIUS - INNER_RADIUS) / DTPOLRingDigiHit::NRINGS;
    // 1mm, agrees with "JUNCTION ELEMENT SEPARATION" in catalog


    // overall scale factors
    double a_scale;
    double t_scale;
    double t_base;

    // calibration constants stored by channel
    vector<double>  a_gains;
    vector<double>  a_pedestals;
    vector<double>  adc_time_offsets;

    double HIT_TIME_WINDOW;
    double ADC_THRESHOLD;

    double GetPhi(int sector);
    double GetPulseTime(const vector<uint16_t> waveform,double w_min,double w_max,double minpeakheight);
    DTPOLHit* FindMatch(int sector, double T);
    const double GetConstant(const vector<double>  &the_table,const int in_sector) const;
    const double GetConstant(const vector<double>  &the_table,const DTPOLHit *the_hit) const;
private:
    void Init() override;
    void BeginRun(const std::shared_ptr<const JEvent>& event) override;
    void Process(const std::shared_ptr<const JEvent>& event) override;
    void EndRun() override;
    void Finish() override;

};

#endif // _DTPOLHit_factory_
