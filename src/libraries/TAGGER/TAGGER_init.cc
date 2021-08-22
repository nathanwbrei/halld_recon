// $Id$

#include <JANA/JFactorySet.h>

#include "DTAGMGeometry_factory.h"
#include "DTAGHGeometry_factory.h"
#include "DTAGMHit_factory.h"
#include "DTAGMHit_factory_Calib.h"
#include "DTAGHHit_factory.h"
#include "DTAGHHit_factory_Calib.h"


void TAGGER_init(JFactorySet *factorySet)
{
  /// Create and register TAGGER data factories
  factorySet->Add(new DTAGMHit_factory());
  factorySet->Add(new DTAGMHit_factory_Calib());
  factorySet->Add(new DTAGHHit_factory());
  factorySet->Add(new DTAGHHit_factory_Calib());
  factorySet->Add(new DTAGMGeometry_factory());
  factorySet->Add(new DTAGHGeometry_factory());
}
