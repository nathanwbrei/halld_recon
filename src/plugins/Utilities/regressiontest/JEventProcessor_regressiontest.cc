//
// JEventProcessor_regressiontest.cc
//
// JANA event processor plugin writes out rest events to a file
//
// Richard Jones, 1-July-2012

#include "JEventProcessor_regressiontest.h"

#include <tuple>

// Make us a plugin
// for initializing plugins
extern "C" {
   void InitPlugin(JApplication *app) {
      InitJANAPlugin(app);
      app->Add(new JEventProcessor_regressiontest());
   }
} // "extern C"

//-------------------------------
// Init
//-------------------------------
void JEventProcessor_regressiontest::Init()
{
    auto app = GetApplication();
    app->SetDefaultParameter("regressiontest:outfile_name", outfile_name);

}

//-------------------------------
// BeginRun
//-------------------------------
void JEventProcessor_regressiontest::BeginRun(const std::shared_ptr<const JEvent>& event)
{
}

//-------------------------------
// Process
//-------------------------------
void JEventProcessor_regressiontest::Process(const std::shared_ptr<const JEvent>& event)
{
    auto app = GetApplication();
    auto evt_nr = event->GetEventNumber();
    auto run_nr = event->GetRunNumber();
    auto facs = event->GetAllFactories();

    std::cout << "Processing event number " << evt_nr << std::endl;

    for (auto fac : facs) {
        auto item_ct = fac->Create(event, app, run_nr);
        auto key = make_tuple(evt_nr, fac->GetObjectName(), fac->GetTag());
        results.insert({key, item_ct});
    }
}

//-------------------------------
// EndRun
//-------------------------------
void JEventProcessor_regressiontest::EndRun()
{
}

//-------------------------------
// Finish
//-------------------------------
void JEventProcessor_regressiontest::Finish()
{
    outfile.open(outfile_name);

    for (auto pair : results) {
        string fac_name, fac_tag;
        uint64_t evt_nr, obj_count;
        std::tie(evt_nr, fac_name, fac_tag) = pair.first;
        obj_count = pair.second;
        outfile << evt_nr << "\t" << fac_name << "\t" << fac_tag << "\t" << obj_count << std::endl;
    }
    outfile.close();

}
