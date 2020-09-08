// Author: David Lawrence  June 25, 2004
//
//
// hd_ana.cc
//

#include <iostream>
using namespace std;

#include <termios.h>

#include "MyProcessor.h"
//#include "DANA/DApplication.h"
// DApplication is going away. hd_dump (and janadump) will use JApplication instead.

void PrintFactoryList(JApplication *app);
void ParseCommandLineArguments(int &narg, char *argv[]);
void Usage(void);

bool LIST_FACTORIES = false;
bool SPARSIFY_SUMMARY = true;
bool ALLOW_SPARSIFIED_EVIO = true;

//-----------
// main
//-----------
int main(int narg, char *argv[])
{
	// Parse the command line
	ParseCommandLineArguments(narg, argv);

	// Instantiate our event processor
	MyProcessor myproc;

	// Instantiate an event loop object
	JApplication *app = new JApplication(narg, argv);
	
	// Set tag prefix for JANA streams to empty
	// TODO: come back to this maybe
	// jout.SetTag("");
	
	// If LIST_FACTORIES is set, print all factories and exit
	if(LIST_FACTORIES){
		PrintFactoryList(app);
		return 0;
	}

	// This monkeyshines is needed to get getchar() to return single
	// characters without waiting for the user to hit return
	struct termios t;
	tcgetattr(fileno(stdin), &t);
	t.c_lflag &= (~ICANON);
	//t.c_cc[VMIN] = 1;
	tcsetattr(fileno(stdin), TCSANOW, &t);

	// If only DEPICSvalue is specified to print, then automatically
	// turn on sparsified reading unless user specifically requests
	// that we don't.
	if(toprint.size()==1 && ALLOW_SPARSIFIED_EVIO){
		if( toprint.find("EPICSvalue") != toprint.end() ){
			cout << endl;
			cout << "-- Only DEPICSvalue objects requested" << endl;
			cout << "-- * Enabling EVIO file mapping and sparse readout" << endl;
			cout << "-- * Automatically invoking -f and -s options" << endl;
			cout << endl;
			app->SetParameterValue("EVIO:SPARSE_READ", true);
			app->SetParameterValue("EVIO:EVENT_MASK", string("EPICS"));
			PRINT_SUMMARY_HEADER = false;
			SKIP_BORING_EVENTS = 1;
		}
	}

	// Run though all events, calling our event processor's methods
	app->SetTicker(false);
	// TODO: Re-add heartbeat to jana2
	// app->monitor_heartbeat = false;
	app->Add(&myproc);
	app->Run();
	
	delete app;

	return 0;
}

//-----------
// PrintFactoryList
//-----------
void PrintFactoryList(JApplication *app)
{
	// Print a summary of all factories registered with this JApplication.
	// Note that we could also just do `cout << app->GetComponentSummary`,
	// but this keeps the formatting consistent with the old version.

	cout << endl;
	cout << "  Factory List" << endl;
	cout << "-------------------------" << endl;

	auto factory_summary = app->GetComponentSummary().factories;
	for (const auto & fac : factory_summary) {
		cout << " " << fac.object_name;
		if (fac.factory_tag.empty()) {
			cout << " : " << fac.factory_tag;
		}
		cout << endl;
	}
	cout << endl;
	cout << " " << factory_summary.size() << " factories registered" << endl;
	cout << endl;

	auto summary = app->GetComponentSummary().factories;
}

//-----------
// ParseCommandLineArguments
//-----------
void ParseCommandLineArguments(int &narg, char *argv[])
{
	if(narg==1)Usage();

	for(int i=1;i<narg;i++){
		if(argv[i][0] != '-')continue;
		switch(argv[i][1]){
			case 'h':
				Usage();
				break;
			case 'D':
				toprint.insert(&argv[i][2]);
				break;
			case 'p':
				PAUSE_BETWEEN_EVENTS = false;
				break;
			case 's':
				SKIP_BORING_EVENTS = true;
				break;
			case 'A':
				PRINT_ALL = true;
				break;
			case 'c':
				PRINT_CORE = true;
				PRINT_SUMMARY_HEADER = true;
				break;
			case 'L':
				LIST_FACTORIES = true;
				break;
			case 'a':
				LIST_ASSOCIATED_OBJECTS = true;
				break;
			case 'S':
				SPARSIFY_SUMMARY = false;
				break;
			case 'f':
				cout << "WARNING: -f option is deprecated as it is now the default" << endl;
				break;
			case 'V':
				PRINT_SUMMARY_ALL    = true;
				PRINT_SUMMARY_HEADER = true;
				break;
			case 'b':
				PRINT_STATUS_BITS = true;
				break;
			case 'e':
				ALLOW_SPARSIFIED_EVIO = false;
				break;
		}
	}
	
	if(toprint.empty() && !PRINT_ALL ){
		PRINT_CORE = true;
		PRINT_SUMMARY_HEADER = true;
	}
}

//-----------
// Usage
//-----------
void Usage(void)
{
	JApplication app;

	cout<<"Usage:"<<endl;
	cout<<"       hd_dump [options] source1 source2 ..."<<endl;
	cout<<endl;
	cout<<"Print the contents of a Hall-D data source (e.g. a file)"<<endl;
	cout<<"to the screen."<<endl;
	cout<<endl;
	cout<<"Options:"<<endl;
	cout<<endl;
	cout<<"   -h        Print this message"<<endl;
	cout<<"   -Dname    Print the data of type \"name\" (can be used multiple times)"<<endl;
	cout<<"   -A        Print ALL data types (overrides and -DXXX options)"<<endl;
	cout<<"   -L        List available factories and exit"<<endl;
	cout<<"   -p        Don't pause for keystroke between events (def. is to pause)"<<endl;
	cout<<"   -s        Skip events which don't have any of the specified data types"<<endl;
	cout<<"   -a        List types and number of associated objects"<<endl;
	cout<<"   -S        Don't supress printing of factories with no objects in summary"<<endl;
	cout<<"   -c        Print summary header lisiting for select factories."<<endl;
	cout<<"   -V        Print summary header lisiting for all factories."<<endl;
	cout<<"             (warning: this activates every single factory!)"<<endl;
	cout<<"   -b        Print event status bits"<<endl;
	cout<<"   -e        Don't allow automatic EVIO sparse readout for EPICS data"<<endl;
	cout<<endl;


	// TODO: Pull in usage from CLI header
	app.Usage();

	exit(0);
}


