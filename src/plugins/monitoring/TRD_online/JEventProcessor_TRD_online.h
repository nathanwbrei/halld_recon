
#ifndef _JEventProcessor_TRD_online_
#define _JEventProcessor_TRD_online_

#include <JANA/JEventProcessor.h>


class JEventProcessor_TRD_online:public jana::JEventProcessor{
public:
    JEventProcessor_TRD_online();
    ~JEventProcessor_TRD_online();
    const char* className(void){return "JEventProcessor_TRD_online";}

    // static const UInt_t NSECTORS = 32;

private:
    jerror_t init(void); ///< Called once at program start.
    jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber); ///< Called everytime a new run number is detected.
    jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber); ///< Called every event.
    jerror_t erun(void); ///< Called everytime run number changes, provided brun has been called.
    jerror_t fini(void); ///< Called after last event of last event source has been processed.
};

#endif // _JEventProcessor_TRD_online_

