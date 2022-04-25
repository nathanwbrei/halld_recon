//
//    File: JEventProcessor_danarest.h
// Created: Mon Jul 1 09:08:37 EDT 2012
// Creator: Richard Jones
//

#ifndef _JEventProcessor_regressiontest_
#define _JEventProcessor_regressiontest_

#include <string>
#include <map>
#include <ostream>
#include <tuple>
#include <mutex>

using std::map;
using std::tuple;
using std::string;

#include <JANA/JEventProcessor.h>
#include <JANA/JApplication.h>


class JEventProcessor_regressiontest : public jana::JEventProcessor
{
	public:
		const char* className() {return "JEventProcessor_regressiontest";}
		jerror_t init() override;
		jerror_t brun(jana::JEventLoop* lel, int lrn) override;
		jerror_t evnt(jana::JEventLoop* lol, uint64_t len) override;
		jerror_t erun() override;
		jerror_t fini() override;

private:
		uint64_t m_next_event_nr = 0;
		std::mutex m_mutex;
		map<tuple<uint64_t, string, string>, uint64_t> counts;
		map<tuple<uint64_t, string, string, int>, string> summaries;
		map<tuple<uint64_t, string, string, int, string>, string> summaries_expanded;
		bool expand_summaries = false;
		std::ofstream counts_file;
		std::ofstream summaries_file;
		std::string counts_file_name = "objcounts.tsv";
		std::string summaries_file_name = "objsummaries.tsv";
};

#endif // _JEventProcessor_regressiontest_
