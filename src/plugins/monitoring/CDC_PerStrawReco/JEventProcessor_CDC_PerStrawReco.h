// $Id$
//
//    File: JEventProcessor_CDC_PerStrawReco.h
// Created: Mon Jul  6 13:00:51 EDT 2015
// Creator: mstaib (on Linux egbert 2.6.32-504.16.2.el6.x86_64 x86_64)
//

#ifndef _JEventProcessor_CDC_PerStrawReco_
#define _JEventProcessor_CDC_PerStrawReco_

#include <JANA/JEventProcessor.h>
#include <JANA/Compatibility/JLockService.h>

using std::vector;

class JEventProcessor_CDC_PerStrawReco:public JEventProcessor{
	public:
		JEventProcessor_CDC_PerStrawReco();
		~JEventProcessor_CDC_PerStrawReco();

	private:
		void Init() override;
		void BeginRun(const std::shared_ptr<const JEvent>& event) override;
		void Process(const std::shared_ptr<const JEvent>& event) override;
		void EndRun() override;
		void Finish() override;

		std::shared_ptr<JLockService> lockService;

        int EXCLUDERING;
        vector<vector<double> >max_sag;
        vector<vector<double> >sag_phi_offset;
};

#endif // _JEventProcessor_CDC_PerStrawReco_

