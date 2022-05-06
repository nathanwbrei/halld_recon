//
// JEventProcessor_regressiontest.cc
//
// JANA event processor plugin writes out rest events to a file
//
// Richard Jones, 1-July-2012

#include "JEventProcessor_regressiontest.h"
using namespace jana;

#include <tuple>
#include <queue>
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
   }
} // "extern C"

//-------------------------------
// Init
//-------------------------------
jerror_t JEventProcessor_regressiontest::init()
{
    app->SetShowTicker(false);
    app->monitor_heartbeat = 0;
    gPARMS->SetDefaultParameter("regressiontest:interactive", interactive);
    gPARMS->SetDefaultParameter("regressiontest:old_log", old_log_file_name);
    gPARMS->SetDefaultParameter("regressiontest:new_log", new_log_file_name);

    jout << "Running regressiontest plugin" << std::endl;

    old_log_file.open(old_log_file_name);
    if (old_log_file.good()) {
        have_old_log_file = true;
    }
    new_log_file.open(new_log_file_name);

    blacklist_file.open(blacklist_file_name);
    std::string line;
    while (std::getline(blacklist_file, line)) {
        blacklist.insert(line);
    }
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
    JInspector inspector(lel);
    auto run_nr = lel->GetJEvent().GetRunNumber();
    for (auto fac : lel->GetFactories()) {
        fac->Get(); // Make sure factory has run
    }
    auto facs = GetFactoriesTopologicallyOrdered(*lel);

    std::lock_guard<std::mutex> lock(m_mutex);

    for (JFactory_base* fac : facs) {

        bool found_discrepancy = false;
        auto jobs = fac->Get();
        auto item_ct = jobs.size();
        int old_item_ct = 0;

        // Generate line describing factory counts
        std::ostringstream os;
        os << evt_nr << "\t" << fac->GetDataClassName() << "\t" << fac->Tag() << "\t" << item_ct;
        std::string count_line = os.str();

        new_log_file << count_line << std::endl;

        if (have_old_log_file) {
            std::string old_count_line;
            std::getline(old_log_file, old_count_line);
            old_item_ct = ParseOldItemCount(old_count_line);

            if (old_count_line != count_line) {
                std::cout << "MISMATCH" << std::endl;
                std::cout << "OLD COUNT: " << old_count_line << std::endl;
                std::cout << "NEW COUNT: " << count_line << std::endl;
                found_discrepancy = true;
            }
            else {
                // std::cout << "MATCH " << count_line << std::endl;
            }
        }

        std::vector<std::string> new_object_lines;

        for (void* vobj : jobs) {

            JObject* obj = (JObject*) vobj; // vobj is a void*
            std::vector<std::pair<std::string, std::string>> summary;
            obj->toStrings(summary);

            std::stringstream ss;
            ss << evt_nr << "\t" << fac->GetDataClassName() << "\t" << fac->Tag() << "\t";
            ss << "{";
            for (auto& pair : summary) {
                std::stringstream oss;
                oss << fac->GetDataClassName() << "\t" << fac->Tag() << "\t" << pair.first;
                std::string blacklist_entry = oss.str();
                if (blacklist.find(blacklist_entry) == blacklist.end()) {
                    ss << pair.first << ": " << pair.second << ", ";
                }
            }
            ss << "}";
            new_object_lines.push_back(ss.str());
        }

        std::sort(new_object_lines.begin(), new_object_lines.end());
        for (const auto& s : new_object_lines) {
            new_log_file << s << std::endl;
        }

        if (have_old_log_file) {
            for (int i=0; i<old_item_ct; ++i) {
                std::string old_object_line;
                std::getline(old_log_file, old_object_line);
                if ((size_t) i >= new_object_lines.size()) {
                    std::cout << "MISMATCH: " << old_item_ct << " vs " << item_ct << std::endl;
                    std::cout << "OLD OBJ: " << old_object_line << std::endl;
                    std::cout << "NEW OBJ: missing" << std::endl;
                    found_discrepancy = true;
                }
                else if (old_object_line != new_object_lines[i]) {
                    found_discrepancy = true;
                    std::cout << "MISMATCH" << std::endl;
                    std::cout << "OLD OBJ: " << old_object_line << std::endl;
                    std::cout << "NEW OBJ: " << new_object_lines[i] << std::endl;
                }
                else {
                    // std::cout << "MATCH" << old_object_line << std::endl;
                }
            }
        }
        if (found_discrepancy) {
            inspector.Loop();
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
    new_log_file.close();
    if (have_old_log_file) {
        old_log_file.close();
    }
    return NOERROR;
}

std::vector<JFactory_base*> JEventProcessor_regressiontest::GetFactoriesTopologicallyOrdered(JEventLoop& event) {

    std::vector<JFactory_base*> sorted_factories;


        using FacName = std::pair<std::string, std::string>;
        struct FacEdges {
            std::vector<FacName> incoming;
            std::vector<FacName> outgoing;
        };

        // Build adjacency matrix
        std::map<FacName, FacEdges> adjacency;
        for (const JEventLoop::call_stack_t& node : event->GetCallStack()) {

            adjacency[{node.caller_name, node.caller_tag}].incoming.emplace_back(node.callee_name, node.callee_tag);
            adjacency[{node.callee_name, node.callee_tag}].outgoing.emplace_back(node.caller_name, node.caller_tag);
        }

        std::queue<FacName> ready;

        // Populate frontier of "ready" elements with no incoming edges
        for (auto& p : adjacency) {
            if (p.second.incoming.empty()) ready.push(p.first);
        }

        // Process each ready element
        while (!ready.empty()) {
            auto n = ready.front();
            ready.pop();
            sorted_factories.push_back(n);
            for (auto& m : adjacency[n].outgoing) {
                auto& incoming = adjacency[m].incoming;
                incoming.erase(std::remove(incoming.begin(), incoming.end(), n), incoming.end());
                if (incoming.empty()) {
                    ready.push(m);
                }
            }
        }

    for (auto pair : sorted_factories) {
        auto fac_name = pair.first;
        auto fac_tag = pair.second;
        JFactory_base* fac = event.GetFactory(fac_name, fac_tag);
        sorted_factories.push_back(fac);
    }
    return sorted_factories;
}

int JEventProcessor_regressiontest::ParseOldItemCount(std::string old_count_line) {

    std::istringstream iss(old_count_line);
    std::string split;
    std::getline(iss, split, '\t');
    std::getline(iss, split, '\t');
    std::getline(iss, split, '\t');
    int result;
    iss >> result;
    return result;

}
