#ifndef _DEventWriterROOT_factory_
#define _DEventWriterROOT_factory_

#include <JANA/JFactoryT.h>
#include <JANA/JEvent.h>

#include "ANALYSIS/DEventWriterROOT.h"

class DEventWriterROOT_factory : public JFactoryT<DEventWriterROOT>
{
	public:
		DEventWriterROOT_factory(){use_factory = 1;}; //prevents JANA from searching the input file for these objects
		~DEventWriterROOT_factory(){};
		
		DEventWriterROOT *dROOTEventWriter = nullptr;

	private:
	
		//------------------
		// BeginRun
		//------------------
		void BeginRun(const std::shared_ptr<const JEvent>& loop, int32_t runnumber)
		{
			// (See DTAGHGeometry_factory.h)
			SetFactoryFlag(NOT_OBJECT_OWNER);
			ClearFactoryFlag(WRITE_TO_OUTPUT);
			
			if( dROOTEventWriter == nullptr ) {
				dROOTEventWriter = new DEventWriterROOT();
				dROOTEventWriter->Initialize(loop);
			} else {
				dROOTEventWriter->Run_Update(loop);
			}

			return;
		}

		//------------------
		// Process
		//------------------
		 void Process(const std::shared_ptr<const JEvent>& loop, uint64_t eventnumber)
		 {
			// Reuse existing DBCALGeometry object.
			if( dROOTEventWriter ) _data.push_back( dROOTEventWriter );
			 
			return;
		 }


		//------------------
		// Finish
		//------------------
		jerror_t fini(void)
		{
			// Delete object: Must be "this" thread so that interfaces deleted properly
			delete dROOTEventWriter;
			return;
		}
};

#endif // _DEventWriterROOT_factory_

