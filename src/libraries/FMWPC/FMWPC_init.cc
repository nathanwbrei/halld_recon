// $Id$
//
//    File: FMWPC_init.cc
// Created: Tue Jun 16 07:04:58 EDT 2015
// Creator: davidl (on Darwin harriet.jlab.org 13.4.0 i386)
//

#include "DFMWPCHit.h"
#include "DFMWPCTruthHit.h"

#include <JANA/Compatibility/JGetObjectsFactory.h>

void FMWPC_init(JFactorySet* factorySet) {

	/// Create and register FMWPC data factories
	factorySet->Add(new JGetObjectsFactory<DFMWPCHit>());
	factorySet->Add(new JGetObjectsFactory<DFMWPCTruthHit>());
}

