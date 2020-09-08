// $Id$
// Author: David Lawrence  June 25, 2004
//
//
// MyProcessor.h
//
/// hd_dump print event info to screen
///

#include <set>
using namespace std;

#include <JANA/JEventProcessor.h>
#include <JANA/JFactory.h>

extern bool PAUSE_BETWEEN_EVENTS;
extern bool SKIP_BORING_EVENTS;
extern bool PRINT_ALL;
extern bool PRINT_CORE;
extern bool LIST_ASSOCIATED_OBJECTS;
extern bool PRINT_SUMMARY_ALL;
extern bool PRINT_SUMMARY_HEADER;
extern bool PRINT_STATUS_BITS;
extern bool ACTIVATE_TAGGED_FOR_SUMMARY;

extern set<string> toprint;
extern set<string> tosummarize;

class MyProcessor:public JEventProcessor
{
	int64_t m_last_run_nr = -1;  // We need to simulate BeginRun ourselves now

	public:
		void BeginRun(const std::shared_ptr<const JEvent>& event);
		void Process(const std::shared_ptr<const JEvent>& event) override;

		typedef struct{
			string dataClassName;
			string tag;
			JFactory* fac;
		}factory_info_t;
		vector<factory_info_t> fac_info;
		
		void PrintAssociatedObjects(const std::shared_ptr<const JEvent>& event, const factory_info_t* fac_info);
};
