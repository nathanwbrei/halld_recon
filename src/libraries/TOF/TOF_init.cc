// $Id$

#include <JANA/JFactorySet.h>

#include "DTOFGeometry_factory.h"
#include "DTOFHit_factory.h"
#include "DTOFPaddleHit_factory.h"
#include "DTOFPoint_factory.h"

#include "DTOFDigiHit.h"
#include "DTOFTDCDigiHit.h"
#include "DTOFHitMC.h"
#include "DTOFTruth.h"

void TOF_init(JFactorySet *factorySet)
{
  /// Create and register TOF data factories
  factorySet->Add(new DTOFGeometry_factory());
  factorySet->Add(new DTOFHit_factory());            // smeared MC data
  factorySet->Add(new DTOFPaddleHit_factory());
  factorySet->Add(new DTOFPoint_factory());
}
