// $Id$
//
//    File: JEventProcessor_FMWPC_online.h
// Created: Fri Oct 22 13:34:04 EDT 2021
// Creator: aaustreg (on Linux ifarm1802.jlab.org 3.10.0-1062.4.1.el7.x86_64 x86_64)
//

#ifndef _JEventProcessor_FMWPC_online_
#define _JEventProcessor_FMWPC_online_

#include <JANA/JEventProcessor.h>
#include <JANA/Compatibility/JLockService.h>

class JEventProcessor_FMWPC_online:public JEventProcessor{
	public:
		JEventProcessor_FMWPC_online();
		~JEventProcessor_FMWPC_online();

	private:
                void Init() override;
                void BeginRun(const std::shared_ptr<const JEvent>& event) override;
                void Process(const std::shared_ptr<const JEvent>& event) override;
                void EndRun() override;
                void Finish() override;
                std::shared_ptr<JLockService> lockService;
};

#endif // _JEventProcessor_FMWPC_online_

