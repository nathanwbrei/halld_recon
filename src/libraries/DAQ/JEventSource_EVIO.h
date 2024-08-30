// $Id$
// $HeadURL$
//
//    File: JEventSource_EVIO.h
// Created: Tue Aug  7 15:22:29 EDT 2012
// Creator: davidl (on Darwin harriet.jlab.org 11.4.0 i386)
//

#ifndef _JEventSource_EVIO_
#define _JEventSource_EVIO_


#include <sys/time.h> // TODO: what was this doing before?

#include <map>
#include <vector>
#include <queue>
#include <deque>
#include <list>
#include <set>
using std::map;
using std::vector;
using std::queue;
using std::set;

#include <JANA/JEventSource.h>
#include <JANA/JEvent.h>
#include <JANA/JFactoryT.h>
#include <JANA/Compatibility/JStreamLog.h>

#include "HDEVIO.h"

#ifdef HAVE_EVIO
//#include <evioChannel.hxx>
#include <evioUtil.hxx>
using namespace evio;
#else
class evioDOMTree;
typedef pair<int,int> tagNum;
#endif // HAVE_EVIO

#ifdef HAVE_ET
//#include <evioETChannel.hxx>
#include <et.h>
#endif // HAVE_ET

#include "daq_param_type.h"
#include "DModuleType.h"
#include "Df250Config.h"
#include "Df250PulseIntegral.h"
#include "Df250StreamingRawData.h"
#include "Df250WindowSum.h"
#include "Df250PulseRawData.h"
#include "Df250TriggerTime.h"
#include "Df250PulseTime.h"
#include "Df250PulsePedestal.h"
#include "Df250WindowRawData.h"
#include "Df125Config.h"
#include "Df125TriggerTime.h"
#include "Df125PulseIntegral.h"
#include "Df125PulseTime.h"
#include "Df125PulsePedestal.h"
#include "Df125PulseRawData.h"
#include "Df125WindowRawData.h"
#include "Df125CDCPulse.h"
#include "Df125FDCPulse.h"
#include "DF1TDCConfig.h"
#include "DF1TDCHit.h"
#include "DF1TDCTriggerTime.h"
#include "DCAEN1290TDCConfig.h"
#include "DCAEN1290TDCHit.h"
#include "DCODAEventInfo.h"
#include "DCODAROCInfo.h"
#include "DTSscalers.h"
#include "DEPICSvalue.h"
#include "DEventTag.h"
#include "Df250BORConfig.h"
#include "Df125BORConfig.h"
#include "DF1TDCBORConfig.h"
#include "DCAEN1290TDCBORConfig.h"
#include "DL1Info.h"
#include "Df250Scaler.h"
#include "Df250AsyncPedestal.h"

#include "Df125EmulatorAlgorithm.h"
#include "Df250EmulatorAlgorithm.h"
#include <JANA/Compatibility/jerror.h>

#include <PID/DVertex.h>
#include <DANA/DStatusBits.h>

extern set<uint32_t> ROCIDS_TO_PARSE;


//-----------------------------------------------------------------------
/// The JEventSource_EVIO class implements a JEventSource capable of reading in
/// EVIO data from raw data files produced in Hall-D. It can read in entangled
/// (or blocked) events as well as single events. The low-level objects produced
/// reflect the data generated by the front end digitization electronics.
///
/// The type of boards it can understand can be expanded to include additional
/// boards. To do this, files must be edited in a few places:
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
/// JEventSource_EVIO.h
/// -------------------
/// 1.) Add an appropriate #include near the top of the file for
///     each type of data object created in the previous step.
///
/// 2.) Add an appropriate declaration for a "ParseXXXBank"
///     where the "XXX" is the new module type.
///     example:
///        void ParseF1TDCBank(evioDOMNodeP bankPtr, list<ObjList*> &events);
///
/// 3.) If the routine JFactory_base_CopyTo() still exists at the
///     bottom of this file, then add a line for each data type to it.
///
///
/// JEventSource_EVIO.cc
/// --------------------
/// 1.) In the JEventSource_EVIO::JEventSource_EVIO() constructor,
///     add a line to insert the data type into event_source_data_types
///     for each data type the module produces.
///
/// 2.) In the "ParseEVIOEvent()" method, add a case for the
///     new module type that calls the new "ParseXXXBank()"
///     method. (Note if this is JLab module, then you'll
///     need to add a case to ParseJLabModuleData() ).
///
/// 3.) Add the new ParseXXXBank() method. Preferrably to the
///     end of the file or more importantly, in the order the
///     method appears in the class definition.
///
///
/// JFactoryGenerator_DAQ.h
/// --------------------
/// 1.) Add an include line to the top of the file for each new
///     data type.
///
/// 2.) Add a line for each new data type to the GenerateFactories()
///     method of JFactoryGenerator_DAQ.
///
///----------------------------------------------------------------------

class JEventSource_EVIO: public JEventSource {
	public:

		enum EVIOSourceType{
			kNoSource,
			kFileSource,
			kETSource
		};
		
		enum EmulationModeType{
			kEmulationNone,
			kEmulationAlways,
			kEmulationAuto
		};


				          JEventSource_EVIO(std::string source_name, JApplication* app);
		         virtual ~JEventSource_EVIO();

      				Result Emit(JEvent& event) override;
				    bool GetObjects(const std::shared_ptr<const JEvent> &event, JFactory *factory) override;
					void FinishEvent(JEvent &event) override;


	bool quit_on_next_ET_timeout;

		     inline double GetTime(void);
                    void ReadOptionalModuleTypeTranslation(void);
		         uint32_t* GetPoolBuffer(void);
		  virtual jerror_t ReadEVIOEvent(uint32_t* &buf);
             inline void GetEVIOBuffer(JEvent &jevent, uint32_t* &buff, uint32_t &size) const;
          EVIOSourceType GetEVIOSourceType(void){ return source_type; }
		            void AddROCIDtoParseList(uint32_t rocid){ ROCIDS_TO_PARSE.insert(rocid); }
		   set<uint32_t> GetROCIDParseList(uint32_t rocid){ return ROCIDS_TO_PARSE; }
        static uint32_t* GetEVIOBufferFromRef(void *ref){ return ((ObjList*)ref)->eviobuff; }
         static uint32_t GetEVIOBufferSizeFromRef(void *ref){ return ((ObjList*)ref)->eviobuff_size; }
           static double GetEVIOReadTimeFromRef(void *ref){ return ((ObjList*)ref)->time_evio_read; }
           static double GetDomTreeCreationTimeFromRef(void *ref){ return ((ObjList*)ref)->time_dom_tree; }
           static double GetEVIOParseTimeFromRef(void *ref){ return ((ObjList*)ref)->time_evio_parse; }

#ifdef HAVE_EVIO		
     inline evioDOMTree* GetEVIODOMTree(JEvent &jevent) const;
#endif // HAVE_EVIO		

	protected:
	
		void ConnectToET(const char* source_name);
		void Cleanup(void);
		
		int32_t last_run_number;
		int32_t filename_run_number;

		uint64_t Nevents_read = 0;
		uint32_t Nunparsed;
		bool no_more_events_in_source;
		bool et_connected;
		//evioChannel *chan;
		HDEVIO *hdevio;
		EVIOSourceType source_type;
		map<tagNum, MODULE_TYPE> module_type;
		map<MODULE_TYPE, MODULE_TYPE> modtype_translate;

		JStreamLog evioout;

		bool  AUTODETECT_MODULE_TYPES;
		bool  DUMP_MODULE_MAP;
		bool  PARSE_EVIO_EVENTS;
		bool  PARSE_F250;
		bool  PARSE_F125;
		bool  PARSE_F1TDC;
		bool  PARSE_CAEN1290TDC;
		bool  PARSE_CONFIG;
		bool  PARSE_BOR;
		bool  PARSE_EPICS;
		bool  PARSE_EVENTTAG;
		bool  PARSE_TRIGGER;
		bool  MAKE_DOM_TREE;
		int   ET_STATION_NEVENTS;
		bool  ET_STATION_CREATE_BLOCKING;
		int   ET_DEBUG_WORDS_TO_DUMP;
		bool  LOOP_FOREVER;
		int   VERBOSE;
		float TIMEOUT;
		string MODTYPE_MAP_FILENAME;
		bool ENABLE_DISENTANGLING;
		bool EVIO_SPARSE_READ;
		string EVENT_MASK;

        EmulationModeType F125_EMULATION_MODE; ///< F125 emulation mode
        EmulationModeType F250_EMULATION_MODE; ///< F250 emulation mode

		uint32_t F250_EMULATION_MIN_SWING;         ///< Minimum difference between max and min samples to do emulation
		uint32_t F250_THRESHOLD;                   ///< Threshold to use for firmware emulation
		uint32_t F250_SPARSIFICATION_THRESHOLD;    ///< Sparsification thresh. applied to non-ped-subtracted integral during emulation
		uint32_t F250_NSA;                         ///< Number of samples to integrate after threshold crossing during emulation
		uint32_t F250_NSB;                         ///< Number of samples to integrate before threshold crossing during emulation
		uint32_t F250_NSPED;                       ///< Number of samples to integrate for pedestal during emulation

		uint32_t F125_EMULATION_MIN_SWING;         ///< Minimum difference between max and min samples to do emulation
		uint32_t F125_THRESHOLD;                   ///< Threshold to use for firmware emulation
		uint32_t F125_SPARSIFICATION_THRESHOLD;    ///< Sparsification thresh. applied to non-ped-subtracted integral during emulation
		uint32_t F125_NSA;                         ///< Number of samples to integrate after threshold crossing during emulation
		uint32_t F125_NSB;                         ///< Number of samples to integrate before threshold crossing during emulation
		uint32_t F125_NSA_CDC;                     ///< Number of samples to integrate after thershold crossing during emulation rocid 24-28 only!
		uint32_t F125_NSB_CDC;                     ///< Number of samples to integrate before thershold crossing during emulation rocid 24-28 only!
		uint32_t F125_NSPED;                       ///< Number of samples to integrate for pedestal during emulation
		uint32_t F125_TIME_UPSAMPLE;               ///< Use the CMU upsampling algorithm when emulating f125 pulse times


        uint32_t F125_CDC_WS;                      ///< FA125 emulation CDC hit window start
        uint32_t F125_CDC_WE;                      ///< FA125 emulation CDC hit window end
        uint32_t F125_CDC_IE;                      ///< FA125 emulation CDC number of integrated samples (unless WE is reached)
        uint32_t F125_CDC_NP;                      ///< FA125 emulation CDC initial pedestal samples
        uint32_t F125_CDC_NP2;                     ///< FA125 emulation CDC local pedestal samples
        uint32_t F125_CDC_PG;                      ///< FA125 emulation CDC gap between pedestal and hit threshold crossing
        uint32_t F125_CDC_H;                       ///< FA125 emulation CDC hit threshold
        uint32_t F125_CDC_TH;                      ///< FA125 emulation CDC high timing threshold
        uint32_t F125_CDC_TL;                      ///< FA125 emulation CDC low timing threshold

        uint32_t F125_FDC_WS;                      ///< FA125 emulation FDC hit window start
        uint32_t F125_FDC_WE;                      ///< FA125 emulation FDC hit window end
        uint32_t F125_FDC_IE;                      ///< FA125 emulation FDC number of integrated samples (unless WE is reached)
        uint32_t F125_FDC_NP;                      ///< FA125 emulation FDC initial pedestal samples
        uint32_t F125_FDC_NP2;                     ///< FA125 emulation FDC local pedestal samples
        uint32_t F125_FDC_PG;                      ///< FA125 emulation FDC gap between pedestal and hit threshold crossing
        uint32_t F125_FDC_H;                       ///< FA125 emulation FDC hit threshold
        uint32_t F125_FDC_TH;                      ///< FA125 emulation FDC high timing threshold
        uint32_t F125_FDC_TL;                      ///< FA125 emulation FDC low timing threshold



		uint32_t USER_RUN_NUMBER;            ///< Run number supplied by user
		uint32_t F125PULSE_NUMBER_FILTER;    ///< Discard DF125PulseXXX objects with pulse number equal or greater than this
		uint32_t F250PULSE_NUMBER_FILTER;    ///< Discard DF250PulseXXX objects with pulse number equal or greater than this

		// Utility class with multiple roles:
		//
		// First is to hold pointers to input EVIO buffer and
		// the evioDOMTree made out of it. When an event is
		// first read in, the buffer pointer is set, but the
		// DOM tree is not made until either GetObjects or
		// FreeEvent are called. In the case of multiple physics
		// events in a single DAQ event, the buffer pointer
		// and DOM tree pointers will be NULL.
		//
		// Second is to hold pointers to containers for
		// all types of data objects we produce. This gets passed
		// into bank processor methods so that they can append
		// to the lists. Note that the naming scheme here needs to
		// include the exact name of the class with a "v" in front
		// and an "s" in back. (See #define in JEventSource_EVIO.cc
		// for more details.)
		vector< vector<DDAQAddress*> > hit_objs;
	public:
		class ObjList{
		public:

			ObjList():run_number(0),own_objects(true),eviobuff_parsed(false)
				,eviobuff(NULL),eviobuff_size(0),DOMTree(NULL)
				,time_evio_read(0),time_dom_tree(0),time_evio_parse(0){}
			
			int32_t run_number;
			uint64_t event_number;
			bool own_objects; // keeps track of whether these objects were copied to factories or not
			
			vector<DDAQAddress*>    hit_objs;
			vector<DDAQConfig*>     config_objs;
			vector<JObject*>        misc_objs;

			bool eviobuff_parsed;     // flag used to keep track of whether this buffer has been parsed
			uint32_t *eviobuff;       // Only holds original EVIO event buffer
			uint32_t eviobuff_size;   // size of eviobuff in bytes
			evioDOMTree *DOMTree;     // DOM tree which may be modified before generating output buffer from it
			double time_evio_read;
			double time_dom_tree;
			double time_evio_parse;
		};
	protected:
	
		// EVIO events with more than one DAQ event ("blocked" or
		// "entangled" events") are parsed and have the events
		// stored in the following container so they can be dispensed
		// as needed.
		pthread_mutex_t stored_events_mutex;
		queue<ObjList*> stored_events;

		// We need to keep the EVIO buffers around for events since they
		// may be needed again before we are done with the event (especially
		// for L3). It is more efficient to maintain a pool of such events
		// and recycle them.
		uint32_t BUFFER_SIZE;
		pthread_mutex_t evio_buffer_pool_mutex;
		deque<uint32_t*> evio_buffer_pool;
		
		// In order to efficiently free memory after this source has
		// exhausted it's event supply, the last calls to FreeEvent
		// must be able to recognize itself as such. Use the
		// current_event_count counter to keep track of how many events
		// are currently being processed by processing threads.
		pthread_mutex_t current_event_count_mutex;
		uint32_t current_event_count;
		
		// List of the data types this event source can provide
		// (filled in the constructor)
		set<string> event_source_data_types;
		
		// BOR events must be kept around untill another BOR event is
		// encountered. To make things simpler, a copy of all BOR objects
		// is made for each event. The originals are replaced when new
		// ones come in.
		pthread_rwlock_t BOR_lock;
		vector<JObject*> BORobjs;

		void CopyBOR(const std::shared_ptr<const JEvent>& event, map<string, vector<JObject*> > &hit_objs_by_type);
		void AddSourceObjectsToCallStack(const std::shared_ptr<const JEvent>& loop, string className);
		void AddEmulatedObjectsToCallStack(const std::shared_ptr<const JEvent>& loop, string caller, string callee);
        void EmulateDf250Firmware(const std::shared_ptr<const JEvent> &event, vector<JObject*> &wrd_objs, vector<JObject*> &pt_objs, vector<JObject*> &pp_objs, vector<JObject*> &pi_objs);
        void EmulateDf125Firmware(const std::shared_ptr<const JEvent> &event, vector<JObject*> &wrd_objs, vector<JObject*> &cp_objs, vector<JObject*> &fp_objs);

		jerror_t ParseEvents(ObjList *objs_ptr);
		int32_t FindRunNumber(uint32_t *iptr);
		int32_t EpicQuestForRunNumber(void);
		uint64_t FindEventNumber(uint32_t *iptr);
		void FindEventType(uint32_t *iptr, DStatusBits *bits);
		MODULE_TYPE GuessModuleType(const uint32_t *istart, const uint32_t *iend);
		bool IsF250ADC(const uint32_t *istart, const uint32_t *iend);
		bool IsF1TDC(const uint32_t *istart, const uint32_t *iend);
		void DumpModuleMap(void){}
		void DumpBinary(const uint32_t *iptr, const uint32_t *iend=NULL, uint32_t MaxWords=0, const uint32_t *imark=NULL);

		void MergeObjLists(list<ObjList*> &events1, list<ObjList*> &events2);

#if HAVE_EVIO
		int32_t GetRunNumber(evioDOMTree *evt);
		void ParseEVIOEvent(evioDOMTree *evt, list<ObjList*> &full_events);
		void ParseBuiltTriggerBank(evioDOMNodeP trigbank, list<ObjList*> &tmp_events);
		void ParseBORevent(evioDOMNodeP bankPtr);
		void ParseEPICSevent(evioDOMNodeP bankPtr, list<ObjList*> &events);
#endif // HAVE_EVIO		
		void ParseModuleConfiguration(int32_t rocid, const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);
		void ParseEventTag(const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);
		void ParseJLabModuleData(int32_t rocid, const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);
		void Parsef250Bank(int32_t rocid, const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);
		void Parsef125Bank(int32_t rocid, const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);
		void ParseF1TDCBank(int32_t rocid, const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);
		uint32_t F1TDC_channel(uint32_t chip, uint32_t chan_on_chip, int modtype);
		void ParseTSBank(int32_t rocid, const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);
		void ParseTIBank(int32_t rocid, const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);
		void ParseCAEN1190(int32_t rocid, const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);


		// f250 methods
		void MakeDf250WindowRawData(ObjList *objs, uint32_t rocid, uint32_t slot, uint32_t itrigger, const uint32_t* &iptr);
		void MakeDf250PulseRawData(ObjList *objs, uint32_t rocid, uint32_t slot, uint32_t itrigger, const uint32_t* &iptr);
		void MakeDf125WindowRawData(ObjList *objs, uint32_t rocid, uint32_t slot, uint32_t itrigger, const uint32_t* &iptr);
		void MakeDf125PulseRawData(ObjList *objs, uint32_t rocid, uint32_t slot, uint32_t itrigger, const uint32_t* &iptr);

#if HAVE_EVIO
		void ParseTSSync(evioDOMNodeP bankPtr, list<ObjList*> &events);
		void ParseDVertexBank(evioDOMNodeP bankPtr, list<ObjList*> &events);

		void ParseFA250Scalers(evioDOMNodeP bankPtr, list<ObjList*> &events, uint32_t rocid);
		void ParseFA250AsyncPedestals(evioDOMNodeP bankPtr, list<ObjList*> &events, uint32_t rocid);
#endif // HAVE_EVIO		


#ifdef HAVE_ET
		et_sys_id sys_id;
		et_att_id att_id;
		et_stat_id sta_id;
#endif
};




//======================================================================================
// Some of the following methods are inlined so that
// they can be used by programs that only have access 
// to this header at link time. (This class is normally
// compiled into a plugin so there is no library file
// available to link to.)
//
// There are also some templates that are used to make
// some of the code in the implmentation file cleaner.
//======================================================================================

//----------------
// GetTime
//----------------
double JEventSource_EVIO::GetTime(void)
{
	struct timeval tval;
	struct timezone tzone;
	gettimeofday(&tval, &tzone);
	double t = (double)tval.tv_sec+(double)tval.tv_usec/1.0E6;
	return t;
}

//----------------
// GetEVIOBuffer
//----------------
void JEventSource_EVIO::GetEVIOBuffer(JEvent &jevent, uint32_t* &buff, uint32_t &size) const
{
	/// Use the reference stored in the supplied JEvent to extract the evio
	/// buffer and size for the event. If there is no buffer for the event
	/// then buff will be set to NULL and size to zero. This can happen if
	/// reading entangled events and this is not the first event in the block.

	// In case we bail early
	buff = NULL;
	size = 0;

	// Make sure this JEvent actually came from this source
	if(jevent.GetJEventSource() != this){
		jerr<<" ERROR: Attempting to get EVIO buffer for event not produced by this source!!"<<endl;
		return;
	}

	// Get pointer to ObjList object
	const ObjList *objs_ptr = jevent.GetSingle<ObjList>();
	if(!objs_ptr) return;

	// Copy buffer pointer and size to user's variables
	buff = objs_ptr->eviobuff;
	size = objs_ptr->eviobuff_size;
}

#ifdef HAVE_EVIO		

//----------------
// GetEVIODOMTree
//----------------
evioDOMTree* JEventSource_EVIO::GetEVIODOMTree(JEvent &jevent) const
{
	/// Use the reference stored in the supplied JEvent to extract the evio
	/// DOM tree for the event. If there is no DOM tree for the event
	/// then NULL will be returned. This can happen if reading entangled events
	/// and this is not the first event in the block.

	// Make sure this JEvent actually came from this source
	if(jevent.GetJEventSource() != this){
		jerr<<" ERROR: Attempting to get EVIO buffer for event not produced by this source!!"<<endl;
		return NULL;
	}

	// Get pointer to ObjList object
	const ObjList *objs_ptr = jevent.GetSingle<ObjList>();
	if(!objs_ptr) return NULL;

	return objs_ptr->DOMTree;
}
#endif // HAVE_EVIO		



//----------------------------
// AddIfAppropriate
//----------------------------
template<class T>
void AddIfAppropriate(DDAQAddress *obj, vector<T*> &v)
{
	T *t = dynamic_cast<T*>(obj);
	if(t!= NULL) v.push_back(t);
}

//----------------------------
// LinkAssociationsWithPulseNumber
//----------------------------
template<class T, class U>
void LinkAssociationsWithPulseNumber(vector<T*> &a, vector<U*> &b)
{
	/// Template routine to loop over two vectors of pointers to
	/// objects derived from DDAQAddress. This will find any hits
	/// coming from the same DAQ channel and add each to the other's
	/// AssociatedObjects list. This will also check if the member
	/// "pulse_number" is the same (use LinkAssociations to not check
	/// the pulse_number such as when either "T" or "U" does not have
	/// a member named "pulse_number".)
	for(unsigned int j=0; j<a.size(); j++){
		for(unsigned int k=0; k<b.size(); k++){
			if(a[j]->pulse_number != b[k]->pulse_number) continue;
			if(*a[j] == *b[k]){ // compare only the DDAQAddress parts
				a[j]->AddAssociatedObject(b[k]);
				b[k]->AddAssociatedObject(a[j]);
			}
		}
	}
}

//----------------------------
// LinkAssociations
//----------------------------
template<class T, class U>
void LinkAssociations(vector<T*> &a, vector<U*> &b)
{
	/// Template routine to loop over two vectors of pointers to
	/// objects derived from DDAQAddress. This will find any hits
	/// coming from the same DAQ channel and add each to the other's
	/// AssociatedObjects list. This will NOT check if the member
	/// "pulse_number" is the same (use LinkAssociationsWithPulseNumber
	/// for that.)
	for(unsigned int j=0; j<a.size(); j++){
		for(unsigned int k=0; k<b.size(); k++){
			if( *((DDAQAddress*)a[j]) == *((DDAQAddress*)b[k]) ){ // compare only the DDAQAddress parts
				a[j]->AddAssociatedObject(b[k]);
				b[k]->AddAssociatedObject(a[j]);
			}
		}
	}
}

//----------------------------
// LinkAssociationsModuleOnly
//----------------------------
template<class T, class U>
void LinkAssociationsModuleOnly(vector<T*> &a, vector<U*> &b)
{
	/// Template routine to loop over two vectors of pointers to
	/// objects derived from DDAQAddress. This will find any hits
	/// coming from the same DAQ module (channel number is not checked)
	/// When a match is found, the pointer from "a" will be added
	/// to "b"'s AssociatedObjects list. This will NOT do the inverse
	/// of adding "b" to "a"'s list. It is intended for adding a module
	/// level trigger time object to all hits from that module. Adding
	/// all of the hits to the trigger time object seems like it would
	/// be a little expensive with no real use case.
	for(unsigned int j=0; j<a.size(); j++){
		for(unsigned int k=0; k<b.size(); k++){
			if(a[j]->rocid != b[k]->rocid) continue;
			if(a[j]->slot != b[k]->slot) continue;

			b[k]->AddAssociatedObject(a[j]);
		}
	}
}

//----------------------------
// LinkAssociationsModuleOnlyWithCast
//----------------------------
template<class T, class U>
void LinkAssociationsModuleOnlyWithCast(vector<JObject*> &a, vector<JObject*> &b)
{
	/// Template routine to loop over two vectors of pointers to
	/// JObjects derived from classes T and U. Both T and U must have
	/// "rocid" and "slot" members. It is also assumed that all JObjects
	/// in "a" are really of type "T" and that all objects in "b" are really
	/// of type "U". 	
	/// When a match is found, the pointer from "a" will be added
	/// to "b"'s AssociatedObjects list. This will NOT do the inverse
	/// of adding "b" to "a"'s list. It is intended for associating
	/// BOR config objects with hit objects.
	for(unsigned int j=0; j<a.size(); j++){
		T *t = (T*)a[j];
		for(unsigned int k=0; k<b.size(); k++){
			U *u = (U*)b[k];
			if(t->rocid != u->rocid) continue;
			if(t->slot != u->slot) continue;

			u->AddAssociatedObject(t);
		}
	}
}

//----------------------------
// LinkAssociationsROCIDOnly
//----------------------------
template<class T, class U>
void LinkAssociationsROCIDOnly(vector<T*> &a, vector<U*> &b)
{
	/// Template routine to loop over two vectors of pointers to
	/// objects derived from DDAQAddress. This will find any hits
	/// coming from the same DAQ module (channel number is not checked)
	/// When a match is found, the pointer from "a" will be added
	/// to "b"'s AssociatedObjects list. This will NOT do the inverse
	/// of adding "b" to "a"'s list. It is intended for adding a module
	/// level trigger time object to all hits from that module. Adding
	/// all of the hits to the trigger time object seems like it would
	/// be a little expensive with no real use case.
	for(unsigned int j=0; j<a.size(); j++){
		for(unsigned int k=0; k<b.size(); k++){
			if(a[j]->rocid != b[k]->rocid) continue;

			b[k]->AddAssociatedObject(a[j]);
		}
	}
}

//----------------------------
// CopyContainerElementsWithCast
//----------------------------
template<class T, class U>
void CopyContainerElementsWithCast(vector<T*> &a, vector<U*> &b)
{
	/// This is used to copy pointers from a vector of one type of
	/// pointer to a vector of another type, doing a static cast
	/// in the process. For example, to fill a vector<JObject*> a
	/// from a vector<Df250PulseIntegral*> b, call:
	///
	///  CopyContainerElementsWithCast(b, a);
	///
	/// Note that this does not do any dynamic_cast-ing to ensure
	/// that the objects really are of compatible types. So be
	/// cautious.

	for(uint32_t i=0; i<a.size(); i++){
		b.push_back((U*)a[i]);
	}
}



#endif // _JEventSourceGenerator_DAQ_

