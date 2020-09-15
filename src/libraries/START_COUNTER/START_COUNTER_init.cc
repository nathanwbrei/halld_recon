// $Id: CDC_init.cc 2151 2006-10-23 18:01:37Z davidl $

#include <JANA/JFactorySet.h>

#include "DSCHit_factory.h"

void START_COUNTER_init(JFactorySet *factorySet)
{
	/// Create and register Start Counter data factories
	loop->AddFactory(new DSCHit_factory());
}
