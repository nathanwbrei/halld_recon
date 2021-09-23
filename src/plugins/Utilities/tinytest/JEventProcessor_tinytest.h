//
//    File: JEventProcessor_danarest.h
// Created: Mon Jul 1 09:08:37 EDT 2012
// Creator: Richard Jones
//

#ifndef _JEventProcessor_tinytest_
#define _JEventProcessor_tinytest_

#include <string>
using namespace std;

#include <JANA/JEventProcessor.h>


class JEventProcessor_tinytest : public JEventProcessor
{
	public:
		void Init() override;
		void BeginRun(const std::shared_ptr<const JEvent>& event) override;
		void Process(const std::shared_ptr<const JEvent>& event) override;
		void EndRun() override;
		void Finish() override;

 private:
		std::string m_factory_name;
                std::string m_factory_tag;
};

#endif // _JEventProcessor_tinytest_
