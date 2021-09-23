//
// JEventProcessor_tinytest.cc
//
// JANA event processor plugin writes out rest events to a file
//
// Richard Jones, 1-July-2012

#include "JEventProcessor_tinytest.h"
#include <TRD/DTRDStripCluster.h>

// Make us a plugin
// for initializing plugins
extern "C" {
   void InitPlugin(JApplication *app) {
      InitJANAPlugin(app);
      app->Add(new JEventProcessor_tinytest());
   }
} // "extern C"

//-------------------------------
// Init
//-------------------------------
void JEventProcessor_tinytest::Init()
{
}

//-------------------------------
// BeginRun
//-------------------------------
void JEventProcessor_tinytest::BeginRun(const std::shared_ptr<const JEvent>& event)
{
}

//-------------------------------
// Process
//-------------------------------
void JEventProcessor_tinytest::Process(const std::shared_ptr<const JEvent>& event)
{
    auto v = event->Get<DTRDStripCluster>();
}

//-------------------------------
// EndRun
//-------------------------------
void JEventProcessor_tinytest::EndRun()
{
}

//-------------------------------
// Finish
//-------------------------------
void JEventProcessor_tinytest::Finish()
{
}
