//
// JEventProcessor_regressiontest.cc
//
// JANA event processor plugin writes out rest events to a file
//
// Richard Jones, 1-July-2012

#include "JEventProcessor_regressiontest.h"
using namespace jana;

#include <tuple>
#include "JInspector.h"
#include <JANA/JEventLoop.h>
#include <JANA/JObject.h>

// Make us a plugin
// for initializing plugins
extern "C" {
   void InitPlugin(JApplication *app) {
      InitJANAPlugin(app);
      app->AddProcessor(new JEventProcessor_regressiontest());
      gPARMS->SetParameter("record_call_stack", true);
      gPARMS->SetParameter("jana:enable_inspector", true);
   }
} // "extern C"

//-------------------------------
// Init
//-------------------------------
jerror_t JEventProcessor_regressiontest::init()
{
    gPARMS->SetDefaultParameter("regressiontest:counts_file_name", counts_file_name);
    gPARMS->SetDefaultParameter("regressiontest:summaries_file_name", summaries_file_name);
    gPARMS->SetDefaultParameter("regressiontest:expand_summaries", expand_summaries);
    app->SetShowTicker(false);
    app->monitor_heartbeat = 0;
    return NOERROR;
}

//-------------------------------
// BeginRun
//-------------------------------
jerror_t JEventProcessor_regressiontest::brun(JEventLoop* lel, int run_nr)
{
	return NOERROR;
}

//-------------------------------
// Process
//-------------------------------
jerror_t JEventProcessor_regressiontest::evnt(JEventLoop* lel, uint64_t evt_nr)
{
    auto facs = lel->GetFactories();

    std::lock_guard<std::mutex> lock(m_mutex);
    JInspector inspector(lel);
    if ((evt_nr == m_next_event_nr) || (m_next_event_nr == 0)) {
	    m_next_event_nr = inspector.DoReplLoop(evt_nr);
    }

    for (auto fac : facs) {
        auto item_ct = fac->GetNrows();
        auto key = std::make_tuple(evt_nr, fac->GetDataClassName(), fac->Tag());
        counts.insert({key, item_ct});


	int i=0;
	for (auto untyped_obj : fac->Get()) {

		JObject* obj = (JObject*) untyped_obj;

		std::vector<std::pair<std::string, std::string>> summary;
		obj->toStrings(summary);

		if (expand_summaries) {
			for (auto pair : summary) {
				auto key = std::make_tuple(evt_nr, fac->GetDataClassName(), fac->Tag(), i++, pair.first);
				summaries_expanded.insert({key, pair.second});
			}
		}
		else {
			std::stringstream ss;
			ss << "{";
			for (auto pair : summary) {
				ss << pair.first << ": " << pair.second << ", ";
			}
			ss << "}";
			auto key = std::make_tuple(evt_nr, fac->GetDataClassName(), fac->Tag(), i++);
			summaries.insert({key, ss.str()});
		}
	}

    }
    return NOERROR;
}

//-------------------------------
// EndRun
//-------------------------------
jerror_t JEventProcessor_regressiontest::erun()
{
	return NOERROR;
}

//-------------------------------
// Finish
//-------------------------------
jerror_t JEventProcessor_regressiontest::fini()
{
    counts_file.open(counts_file_name);

    for (auto pair : counts) {
        string fac_name, fac_tag;
        uint64_t evt_nr, obj_count;
        std::tie(evt_nr, fac_name, fac_tag) = pair.first;
        obj_count = pair.second;
        counts_file << evt_nr << "\t" << fac_name << "\t" << fac_tag << "\t" << obj_count << std::endl;
    }
    counts_file.close();


    summaries_file.open(summaries_file_name);
    if (!expand_summaries) {
	    for (auto pair : summaries) {
		    std::string fac_name, fac_tag, obj_summary;
		    uint64_t evt_nr;
		    int obj_idx;
		    std::tie(evt_nr, fac_name, fac_tag, obj_idx) = pair.first;
		    obj_summary = pair.second;
		    summaries_file << evt_nr << "\t" << fac_name << "\t" << fac_tag << "\t" << obj_idx << "\t" << obj_summary << std::endl;
	    }
    }
    else {
	    for (auto pair : summaries_expanded) {
		    string fac_name, fac_tag, field_name, field_val;
		    uint64_t evt_nr;
		    int obj_idx;
		    std::tie(evt_nr, fac_name, fac_tag, obj_idx, field_name) = pair.first;
		    field_val = pair.second;
		    summaries_file << evt_nr << "\t" << fac_name << "\t" << fac_tag << "\t" << obj_idx << "\t" << field_name << "\t" << field_val << std::endl;
	    }
    }
    summaries_file.close();

    return NOERROR;

}
