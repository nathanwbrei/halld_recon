// $Id$

#include <JANA/JFactorySet.h>

#include "DPSHit_factory.h"
#include "DPSCHit_factory.h"
#include "DPSGeometry_factory.h"
#include "DPSCPair_factory.h"
#include "DPSPair_factory.h"
#include "DLumi_factory.h"


void PAIR_SPECTROMETER_init(JFactorySet *factorySet)
{
  /// Create and register Pair Spectrometer data factories
  factorySet->Add(new DPSGeometry_factory());
  factorySet->Add(new DPSHit_factory());
  factorySet->Add(new DPSCHit_factory());
  factorySet->Add(new DPSCPair_factory());
  factorySet->Add(new DPSPair_factory());
  factorySet->Add(new DLumi_factory());
}
