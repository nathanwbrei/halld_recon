//
//    File: JEventProcessor_danarest.h
// Created: Mon Jul 1 09:08:37 EDT 2012
// Creator: Richard Jones
//

#ifndef _JEventProcessor_tinytest_
#define _JEventProcessor_tinytest_

#include <string>
#include <map>
#include <ostream>

using std::map;
using std::tuple;
using std::string;

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
		map<tuple<uint64_t, string, string>, uint64_t> results;
		std::ofstream outfile;
		std::string outfile_name = "objcounts.tsv";
};

#endif // _JEventProcessor_tinytest_
