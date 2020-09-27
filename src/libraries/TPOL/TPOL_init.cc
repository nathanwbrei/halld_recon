// $Id: CDC_init.cc 2151 2006-10-23 18:01:37Z davidl $

#include <JANA/JFactorySet.h>

#include "DTPOLTruthHit.h"
#include "DTPOLSectorDigiHit.h"
#include "DTPOLRingDigiHit.h"
#include "DTPOLHit_factory.h"

void TPOL_init(JFactorySet *factorySet)
{
	/// Create and register TPOL data factories
	factorySet->Add(new DTPOLHit_factory());
}
