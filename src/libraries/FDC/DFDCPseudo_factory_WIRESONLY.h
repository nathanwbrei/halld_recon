// $Id$
//
//    File: DFDCPseudo_factory_WIRESONLY.h
// Created: Fri Nov  9 09:57:12 EST 2007
// Creator: davidl (on Darwin fwing-dhcp95.jlab.org 8.10.1 i386)
//

#ifndef _DFDCPseudo_factory_WIRESONLY_
#define _DFDCPseudo_factory_WIRESONLY_

#include <JANA/JFactoryT.h>

#include "DFDCPseudo.h"

class DFDCPseudo_factory_WIRESONLY:public JFactoryT<DFDCPseudo>{
	public:
		DFDCPseudo_factory_WIRESONLY(){
			SetTag("WIRESONLY");
		};
		~DFDCPseudo_factory_WIRESONLY(){};

	private:
		//void Init() override;						///< Called once at program start.
		//void BeginRun(const std::shared_ptr<const JEvent>& event) override;	///< Called everytime a new run number is detected.
		void Process(const std::shared_ptr<const JEvent>& event) override;
		//void EndRun() override;				///< Called everytime run number changes, provided brun has been called.
		//void Finish();						///< Called after last event of last event source has been processed.

		void MakePseudo(const DFDCHit *hit, const DFDCWire *wire, const DVector3 &pos);

};

#endif // _DFDCPseudo_factory_WIRESONLY_

