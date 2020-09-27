#ifndef _DL1Trigger_factory_
#define _DL1Trigger_factory_

#include <JANA/JFactoryT.h>
#include "DL1Trigger.h"

class DL1Trigger_factory:public JFactoryT<DL1Trigger>{
	public:
		DL1Trigger_factory(){};
		~DL1Trigger_factory(){};

	private:
		void Init() override;						///< Called once at program start.
		void BeginRun(const std::shared_ptr<const JEvent>& event) override;	///< Called everytime a new run number is detected.
		void Process(const std::shared_ptr<const JEvent>& event) override;	///< Called every event.
		void EndRun() override;						///< Called everytime run number changes, provided brun has been called.
		void Finish();						///< Called after last event of last event source has been processed.
};

#endif // _DL1Trigger_factory_

