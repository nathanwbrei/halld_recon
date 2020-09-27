// $Id$
//
//    File: DTRDHit_factory.h
//

#ifndef _DTRDHit_factory_
#define _DTRDHit_factory_

#include <vector>
using namespace std;

#include <JANA/JFactoryT.h>
#include "TTAB/DTranslationTable.h"
#include "DTRDDigiHit.h"
#include "DTRDHit.h"

// store constants so that they can be accessed by pixel number
typedef  vector<double>  trd_digi_constants_t;

class DTRDHit_factory:public JFactoryT<DTRDHit>{
	public:
		DTRDHit_factory(){};
		~DTRDHit_factory(){};

		// calibration constants stored in channel format
		vector<trd_digi_constants_t> time_offsets;

	private:
		void Init() override;						///< Called once at program start.2
		void BeginRun(const std::shared_ptr<const JEvent>& event) override;	///< Called everytime a new run number is detected.
		void Process(const std::shared_ptr<const JEvent>& event) override;	///< Called every event.
		void EndRun() override;						///< Called everytime run number changes, provided brun has been called.
		void Finish();						///< Called after last event of last event source has been processed.

		double t_base[7];
		double pulse_peak_threshold;
};

#endif // _DTRDHit_factory_

