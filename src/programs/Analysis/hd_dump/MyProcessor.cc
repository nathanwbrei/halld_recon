// Author: David Lawrence  June 25, 2004
//
//
// MyProcessor.cc
//

#include <iostream>
using namespace std;

#include <cstdio>
#include <unistd.h>

#include <JANA/JApplication.h>
#include "DANA/DStatusBits.h"

#include "MyProcessor.h"


bool PAUSE_BETWEEN_EVENTS = true;
bool SKIP_BORING_EVENTS = false;
bool PRINT_ALL = false;
bool PRINT_CORE = false;
bool LIST_ASSOCIATED_OBJECTS = false;
bool PRINT_SUMMARY_ALL = false;
bool PRINT_SUMMARY_HEADER = false;
bool PRINT_STATUS_BITS = false;
bool ACTIVATE_TAGGED_FOR_SUMMARY = false;
extern bool SPARSIFY_SUMMARY;

set<string> toprint;
set<string> tosummarize;

#define ansi_escape		((char)0x1b)
#define ansi_bold 		ansi_escape<<"[1m"
#define ansi_black		ansi_escape<<"[30m"
#define ansi_red			ansi_escape<<"[31m"
#define ansi_green		ansi_escape<<"[32m"
#define ansi_blue			ansi_escape<<"[34m"
#define ansi_normal		ansi_escape<<"[0m"
#define ansi_up(A)		ansi_escape<<"["<<(A)<<"A"
#define ansi_down(A)		ansi_escape<<"["<<(A)<<"B"
#define ansi_forward(A)	ansi_escape<<"["<<(A)<<"C"
#define ansi_back(A)		ansi_escape<<"["<<(A)<<"D"

void MyProcessor::BeginRun(const std::shared_ptr<const JEvent>& event) {

	vector<string> factory_names;
	for (auto fac : GetApplication()->GetComponentSummary().factories) {
		if (fac.factory_tag.empty()) {
			factory_names.push_back(fac.object_name);
		}
		else {
			factory_names.push_back(fac.object_name + ":" + fac.factory_tag);
		}
	}

	usleep(100000); //this just gives the Main thread a chance to finish printing the "Launching threads" message
	cout<<endl;

	// If int PRINT_ALL is set then add EVERYTHING.
	if(PRINT_ALL){
		for(auto fac_name : factory_names) toprint.insert(fac_name);
		SKIP_BORING_EVENTS = 0; // with PRINT_ALL, nothing is boring!
	}else if(PRINT_SUMMARY_ALL){
		for(auto fac_name : factory_names) tosummarize.insert(fac_name);
		SKIP_BORING_EVENTS = 0; // with PRINT_ALL, nothing is boring!
	}else if(PRINT_CORE){
		
		// Make list of "core" factories. n.b. these high level
		// objects will automatically activate lower level ones
		set<string> core_factories;

		tosummarize.insert("DChargedTrack");
		tosummarize.insert("DNeutralShower");

		tosummarize.insert("DMCTrackHit");
		tosummarize.insert("DMCThrown");
		tosummarize.insert("DMCTrajectoryPoint");
		tosummarize.insert("DMCReaction");

        tosummarize.insert("DTrigger");
        tosummarize.insert("DEventHitStatistics");
		
	}else{
		// make sure factories exist for all requested data types
		// If a factory isn't found, but one with a "D" prefixed
		// is, go ahead and correct the name.
		set<string> really_toprint;
		for(auto fac_name : toprint){
			bool found  = false;
			bool dfound = false;
			for(unsigned int j=0;j<factory_names.size();j++){
				if(factory_names[j] == fac_name)found = true;
				if(factory_names[j] == "D" + fac_name)dfound = true;
			}
			if(found)
				really_toprint.insert(fac_name);
			else if(dfound)
				really_toprint.insert("D" + fac_name);
			else
				cout<<ansi_red<<"WARNING:"<<ansi_normal
					<< " Couldn't find factory for \""
					<< ansi_bold << fac_name << ansi_normal
					<< "\"!"<<endl;
		}
		
		toprint = really_toprint;
	}

	// Make sure all factories to print are also included in summary
	tosummarize.insert(toprint.begin(), toprint.end());
	
	// At this point, toprint should contain a list of all factories
	// in dataClassName:tag format, that both exist and were requested.
	// Separate the tag from the name and fill the fac_info vector.
	fac_info.clear();
	for(auto fac_name : toprint){
		string tag = "";
		unsigned int pos = fac_name.rfind(":",fac_name.size()-1);
		if(pos != (unsigned int)string::npos){
			tag = fac_name.substr(pos+1,fac_name.size());
			fac_name.erase(pos);
		}
		factory_info_t f;
		f.dataClassName = fac_name;
		f.tag = tag;
		f.fac = event->GetFactory(f.dataClassName, f.tag.c_str());
		fac_info.push_back(f);
	}
	
	cout<<endl;
}

void MyProcessor::Process(const std::shared_ptr<const JEvent>& event) {

	// If we're skipping boring events (events with no rows for any of
	// the types we're printing) we must find out first if the event is
	// "boring".
	int event_is_boring = 1;

	for(unsigned int i=0;i<toprint.size();i++){

		string name =fac_info[i].dataClassName;
		string tag = fac_info[i].tag;
		JFactory *factory = event->GetFactory(name,tag.c_str());
		if(!factory)factory = event->GetFactory("D" + name,tag.c_str());
		if(factory){
			try{
				if(factory->GetNrows()>0){
					event_is_boring=0;
					if(PRINT_SUMMARY_HEADER)break;
				}
			}catch(...){
				// someone threw an exception
			}
		}
	}
	
	if(SKIP_BORING_EVENTS && event_is_boring) return;
	if(!SKIP_BORING_EVENTS)event_is_boring= 0;
	
	// Print event separator
	cout<<"================================================================"<<endl;
	cout<<"Event: "<<event->GetEventNumber()<<endl;

	// We want to print info about all factories results, even if we aren't
	// printing the actual data. To make sure the informational messages often
	// printed during brun are printed first, call the GetNrows() method of all factories
	// ourself first.
	if(PRINT_SUMMARY_HEADER){
		for( auto fac : eventLoop->GetFactories() ){
			string name = fac->GetDataClassName();
			string tag  = fac->Tag()==NULL ? "":fac->Tag();
			if(tag.size()>0)name = name + ":" + tag;
			if( tosummarize.count(name) ) fac->GetNrows();
		}
	}

	if(PRINT_STATUS_BITS) {
		auto status = event->GetSingle<DStatusBits>();
		if (status != nullptr) {
			cout << status->ToString() << endl;
		}
	}
	if(PRINT_SUMMARY_HEADER) eventLoop->PrintFactories(SPARSIFY_SUMMARY ? 2:0);

	// Print data for all specified factories
	for(unsigned int i=0;i<fac_info.size();i++){
		try{
			string name =fac_info[i].dataClassName;
			string tag = fac_info[i].tag;
			event->Print(name,tag.c_str());

			if(LIST_ASSOCIATED_OBJECTS)PrintAssociatedObjects(event, &fac_info[i]);
		}catch(...){
			// exception thrown
		}
	}
	
	// If the program is quitting, then don't bother waiting for the user
	if(GetApplication()->IsQuitting()) return;
	
	// Wait for user input if pausing
	if(PAUSE_BETWEEN_EVENTS && !event_is_boring){
		cerr.flush();
		cout<<endl<<"< Hit return for the next event (P=prev. Q=quit) >";
		cout.flush();
		char c = getchar(); // see note in hd_dump.cc:main()
		if(c=='\n')cout<<ansi_up(1);
		cout<<endl;
		switch(toupper(c)){
			case 'Q':
				GetApplication()->Quit();
				break;
			case 'P':
				//eventLoop->GotoEvent(eventnumber-1);
				break;
			case 'N':
				break;
		}

		cout<<ansi_up(1)<<"\r                                                     \r";
		cout.flush();
	}
}

//------------------------------------------------------------------
// PrintAssociatedObjects
//------------------------------------------------------------------
void MyProcessor::PrintAssociatedObjects(const std::shared_ptr<const JEvent>& event, const factory_info_t *fac_info)
{
	// cast away const-ness of JFactory class pointer
	JFactory *fac = const_cast<JFactory*>(fac_info->fac);
	if(!fac)return;

	// Get list of all objects from this factory
	vector<JObject*> objs = fac->GetAs<JObject>();

	// Loop over objects from this factory
	for(unsigned int i=0; i<objs.size(); i++){
	
		// First, get a list of all associated objects
		vector<const JObject*> aobjs;
		objs[i]->Get(aobjs);
		// If no associated objects, just go on to the next object
		if(aobjs.size()==0)continue;
		
		// Print separator
		cout<<"  [== Associated objects for row "<<i<<" ==]"<<endl;

		// Make a list of all factories that made objects associated to this one
		map<JFactory*, vector<const JObject*> > aofacs;
		for(unsigned int j=0; j<aobjs.size(); j++){
			JFactory *aofac = loop->FindOwner(aobjs[j]);
			
			map<JFactory*, vector<const JObject*> >::iterator iter = aofacs.find(aofac);
			if(iter==aofacs.end()){
				vector<const JObject*> tmp;
				aofacs[aofac] = tmp;
			}
			// Record this object as belonging to this factory
			aofacs[aofac].push_back(aobjs[j]);
		}
		// Figure out number of spaces to indent objects based on factory name length
		map<JFactory*, vector<const JObject*> >::iterator iter;
		unsigned int indent=4; // some minimal string length
		for(iter=aofacs.begin(); iter!=aofacs.end(); iter++){
			JFactory *fac = iter->first;
			string name = fac->GetName();
			auto tag = fac->GetTag();
			if (!tag.empty()) {
				name += ":" + tag;
			}
			if(name.length()>indent)indent=name.length();
		}
		indent += 4; // indent the factory name itself
		
		// Loop over factories that produced associated objects for this object and
		// list the objects it created
		for(iter=aofacs.begin(); iter!=aofacs.end(); iter++){
			JFactory *fac = iter->first;
			vector<const JObject*> &ptrs = iter->second;
			
			// Print out factory name
			string name = fac->GetName();
			string tag = fac->GetTag();
			if (!tag.empty()) {
				name += ":" + tag;
			}
			cout<<string(indent-name.length()-1,' ');
			cout<<name<<" ";
			
			// Loop over objects from this factory that are in the list
			for(unsigned int j=0; j<ptrs.size(); j++){
				if(j!=0)cout<<string(indent,' ');
				cout<<"0x"<<hex<<(unsigned long)ptrs[j]<<dec<<endl;
			}
		}
	}
}

