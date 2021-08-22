// $Id$
//
//    File: JEventProcessor_BCAL_SiPM_saturation.h
//          Modified file from BCAL_neutron_discriminator.h   ES 5/10/2018
// Created: Thu Apr  5 16:36:00 EDT 2018
// Creator: dalton (on Linux gluon119.jlab.org 2.6.32-642.3.1.el6.x86_64 x86_64)
//

#ifndef _JEventProcessor_BCAL_SiPM_saturation_
#define _JEventProcessor_BCAL_SiPM_saturation_

#include <JANA/JEventProcessor.h>
using std::vector;

class JEventProcessor_BCAL_SiPM_saturation:public JEventProcessor{
	public:
		JEventProcessor_BCAL_SiPM_saturation();
		~JEventProcessor_BCAL_SiPM_saturation();

	private:
		void Init() override;
		void BeginRun(const std::shared_ptr<const JEvent>& event) override;
		void Process(const std::shared_ptr<const JEvent>& event) override;
		void EndRun() override;
		void Finish() override;

		uint32_t VERBOSE;


		vector< vector<double> > attenuation_parameters;    // store attenuation length
};

#endif // _JEventProcessor_BCAL_SiPM_saturation_

