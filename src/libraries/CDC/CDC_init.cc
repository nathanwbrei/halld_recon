// $Id$

#include <JANA/JFactorySet.h>

#include "DCDCHit_factory.h"
#include "DCDCHit_factory_Calib.h"
#include "DCDCTrackHit_factory.h"

void CDC_init(JFactorySet *factorySet)
{
  /// Create and register CDC data factories
  factorySet->Add(new DCDCHit_factory());
  factorySet->Add(new DCDCHit_factory_Calib());
  factorySet->Add(new DCDCTrackHit_factory());
}
