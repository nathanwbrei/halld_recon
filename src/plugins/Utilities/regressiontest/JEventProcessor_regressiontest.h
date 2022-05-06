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
#include <JANA/JFactory_base.h>
#include <JANA/JEventLoop.h>
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
    bool interactive = true;
    bool have_old_log_file = false;
    std::ifstream old_log_file;
    std::string old_log_file_name = "regression_log_old.tsv";
    std::ofstream new_log_file;
    std::string new_log_file_name = "regression_log_new.tsv";
    std::ifstream blacklist_file;
    std::string blacklist_file_name = "blacklist.tsv";
    std::set<std::string> blacklist;

    std::vector<JFactory_base*> GetFactoriesTopologicallyOrdered(JEventLoop& event);
    int ParseOldItemCount(std::string old_count_line);
};

#endif // _JEventProcessor_regressiontest_
