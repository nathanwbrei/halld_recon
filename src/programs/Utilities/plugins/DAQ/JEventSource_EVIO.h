// $Id$
//
//    File: JEventSource_EVIO.h
// Created: Tue Aug  7 15:22:29 EDT 2012
// Creator: davidl (on Darwin harriet.jlab.org 11.4.0 i386)
//

#ifndef _JEventSource_EVIO_
#define _JEventSource_EVIO_


#include <map>
#include <vector>
#include <queue>
#include <list>
using std::map;
using std::vector;
using std::queue;

#include <JANA/jerror.h>
#include <JANA/JEventSource.h>
#include <JANA/JEvent.h>

#include <evioChannel.hxx>
#include <evioUtil.hxx>
using namespace evio;

#include "DModuleType.h"
#include "Df250PulseIntegral.h"
#include "Df250StreamingRawData.h"
#include "Df250WindowSum.h"
#include "Df250PulseRawData.h"
#include "Df250TriggerTime.h"
#include "Df250PulseTime.h"
#include "Df250WindowRawData.h"
#include "DF1TDCHit.h"
#include "DF1TDCTriggerTime.h"

//-----------------------------------------------------------------------
/// The JEventSource_EVIO class implements a JEventSource capable of reading in
/// EVIO data from raw data files produced in Hall-D. It can read in entangled
/// (or blocked) events as well as single events. The low-level objects produced
/// reflect the data generated by the front end digitization electronics.
///
/// The type of boards it can understand can be expanded to include additional
/// boards. To do this, files must be edited in a few places:
///
/// Create Data Object files
/// -------------------------
///
/// 1.) For each type of data produced by the new module, create
///     a class to represent it. It is highly recommended to use
///     a naming scheme that reflects the naming convention used
///     in the manual. This makes it easier for people trying to
///     understand the low-level data objects in terms of the
///     manual for the module. See Df250WindowSum.h or
///     Df250PulseIntegral.h for examples.
///
///
/// In DModuleType.h
/// ------------------
/// 1.) The new module type must be added to the type_id_t enum.
///     Make sure "N_MODULE_TYPES" is the last item in the enum.
///
/// 2.) Add a case for the module type to the switch in the
///     GetModule method.
///
///
/// JEventSource_EVIO.h
/// -------------------
/// 1.) Add an appropriate #include near the top of the file for
///     each type of data object created in the first step.
///
/// 2.) Add an appropriate container for each new type of data
///     to the member class "ObjList". Stick to the existing
///     naming scheme which has a "v" as the first letter
///     followed by the data class name. This is needed since
///     some macros are used in another file that assume the
///     variable name is based on the data class name in this
///     way.
///     example:
///             vector<DF1TDCHit*>             vDF1TDCHits;
///
/// 3.) Add an appropriate declaration for a "ParseXXXBank"
///     where the "XXX" is the new module type.
///     example:
///        void ParseF1TDCBank(evioDOMNodeP bankPtr, list<ObjList*> &events);
///
///
/// JEventSource_EVIO.cc
/// --------------------
/// 1.) In the GetObjects() method, add a "CopyToFactory(...)"
///     call near the end for each data type the module produces.
///
/// 2.) Add appropriate code to the "GuessModuleType()" method.
///     This should look into the bank and try and determine if
///     if contains data from the new module type. See the
///     existing code for examples.
///
/// 3.) In the "MergeObjLists()" method near the end, add an
///     "AppendObjs(...)" line for each data type the module
///     produces.
///
/// 4.) In the "ParseEVIOEvent()" method, add a case for the
///     new module type that calls the new "ParseXXXBank()"
///     method.
///
/// 5.) Add the new ParseXXXBank() method. Preferrably to the
///     end of the file or more importantly, in the order the
///     method appears in the class definition.
///
///----------------------------------------------------------------------

class JEventSource_EVIO: public jana::JEventSource{
	public:
		                   JEventSource_EVIO(const char* source_name);
		           virtual ~JEventSource_EVIO();
		virtual const char* className(void){return static_className();}
		 static const char* static_className(void){return "JEventSource_EVIO";}
		
		  virtual jerror_t ReadEVIOEvent(void);
		
		          jerror_t GetEvent(jana::JEvent &event);
		              void FreeEvent(jana::JEvent &event);
				  jerror_t GetObjects(jana::JEvent &event, jana::JFactory_base *factory);
	
	protected:
	
		void ConnectToET(const char* source_name);
		
		int32_t last_run_number;
		
		evioChannel *chan;
		map<tagNum, MODULE_TYPE> module_type;

		bool AUTODETECT_MODULE_TYPES;
		bool DUMP_MODULE_MAP;

		// Utility class to hold pointers to containers for
		// all types of data objects we produce. This gets passed
		// into bank processor methods so that they can append
		// to the lists. Note that the naming scheme here needs to
		// include the exact name of the class with a "v" in front
		// and an "s" in back. (See #define in JEventSource_EVIO.cc
		// for more details.)
		class ObjList{
		public:
			
			int32_t run_number;
			bool own_objects; // keeps track of whether these objects were copied to factories or not
			
			vector<Df250PulseIntegral*>    vDf250PulseIntegrals;
			vector<Df250PulseRawData*>     vDf250PulseRawDatas;
			vector<Df250PulseTime*>        vDf250PulseTimes;
			vector<Df250StreamingRawData*> vDf250StreamingRawDatas;
			vector<Df250TriggerTime*>      vDf250TriggerTimes;
			vector<Df250WindowRawData*>    vDf250WindowRawDatas;
			vector<Df250WindowSum*>        vDf250WindowSums;
			vector<DF1TDCHit*>             vDF1TDCHits;
			vector<DF1TDCTriggerTime*>     vDF1TDCTriggerTimes;
		};
	
		// EVIO events with more than one DAQ event ("blocked" or
		// "entangled" events") are parsed and have the events
		// stored in the following container so they can be dispensed
		// as needed.
		queue<ObjList*> stored_events;
	
		int32_t GetRunNumber(evioDOMTree *evt);
		MODULE_TYPE GuessModuleType(const uint32_t *istart, const uint32_t *iend);
		bool IsF250ADC(const uint32_t *istart, const uint32_t *iend);
		bool IsF125ADC(const uint32_t *istart, const uint32_t *iend);
		bool IsF1TDC(const uint32_t *istart, const uint32_t *iend);
		bool IsTS(const uint32_t *istart, const uint32_t *iend);
		bool IsTI(const uint32_t *istart, const uint32_t *iend);
		void DumpModuleMap(void);
		
		void MergeObjLists(list<ObjList*> &events1, list<ObjList*> &events2);

		void ParseEVIOEvent(evioDOMTree *evt, uint32_t run_number);
		void ParseJLabModuleData(int32_t rocid, const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);
		void Parsef250Bank(int32_t rocid, const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);
		void Parsef125Bank(int32_t rocid, const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);
		void ParseF1TDCBank(int32_t rocid, const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);
		void ParseF1TDCBank_style1(int32_t rocid, const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);
		void ParseF1TDCBank_style2(int32_t rocid, const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);
		void ParseTSBank(int32_t rocid, const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);
		void ParseTIBank(int32_t rocid, const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);

		// f250 methods
		void MakeDf250WindowRawData(ObjList *objs, uint32_t rocid, uint32_t slot, uint32_t itrigger, const uint32_t* &iptr);
		void MakeDf250PulseRawData(ObjList *objs, uint32_t rocid, uint32_t slot, uint32_t itrigger, const uint32_t* &iptr);

	
};

#endif // _JEventSourceGenerator_DAQ_

