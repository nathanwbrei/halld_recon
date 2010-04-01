// Author: Elliott Wolin 19-Mar-2010


#include <JANA/JApplication.h>

#include "DDANAEVIOFactoryGenerator.h"
#include "DDANAEVIO_factory.h"
#include "JEventProcessor_danaevio.h"


jerror_t DDANAEVIOFactoryGenerator::GenerateFactories(JEventLoop *loop) {
  loop->AddFactory(new DDANAEVIO_factory());
  return NOERROR;
}



//----------------------------------------------------------------------------


// for initializing plugins
extern "C" {
  void InitPlugin(JApplication *app) {
    InitJANAPlugin(app);
    app->AddFactoryGenerator(new DDANAEVIOFactoryGenerator());


    // default for writing out evio DOM tree is true, inhibit with "-PEVIO:WRITEOUT=0"
    bool evioWriteOut = true;
    gPARMS->SetDefaultParameter("EVIO:WRITEOUT",evioWriteOut);
    //    if(evioWriteOut)app->AddProcessor(new JEventProcessor_danaevio(),true);  ??? need new DANA release ???
    if(evioWriteOut)app->AddProcessor(new JEventProcessor_danaevio());
  }
} // "extern C"


//----------------------------------------------------------------------------
