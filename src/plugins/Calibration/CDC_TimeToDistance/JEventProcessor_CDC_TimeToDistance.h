// $Id$
//
//    File: JEventProcessor_CDC_TimeToDistance.h
// Created: Mon Nov  9 12:37:01 EST 2015
// Creator: mstaib (on Linux ifarm1102 2.6.32-431.el6.x86_64 x86_64)
//

#ifndef _JEventProcessor_CDC_TimeToDistance_
#define _JEventProcessor_CDC_TimeToDistance_

#include <JANA/JEventProcessor.h>
#include <JANA/Compatibility/JLockService.h>
#include "TProfile.h"
#include "HDGEOMETRY/DMagneticFieldMap.h"

using std::vector;

class JEventProcessor_CDC_TimeToDistance:public JEventProcessor{
	public:
		JEventProcessor_CDC_TimeToDistance();
		~JEventProcessor_CDC_TimeToDistance();

	private:
		void Init() override;
		void BeginRun(const std::shared_ptr<const JEvent>& event) override;
		void Process(const std::shared_ptr<const JEvent>& event) override;
		void EndRun() override;
		void Finish() override;

		std::shared_ptr<JLockService> lockService;

        vector<vector<double> >max_sag;
        vector<vector<double> >sag_phi_offset;
        TProfile *HistCurrentConstants;
        const DMagneticFieldMap* dMagneticField;
        int UNBIASED_RING;
        double MIN_FOM;
};

#endif // _JEventProcessor_CDC_TimeToDistance_

