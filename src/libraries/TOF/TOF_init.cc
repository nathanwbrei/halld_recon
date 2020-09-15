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
  loop->AddFactory(new DTOFGeometry_factory());
  loop->AddFactory(new DTOFHit_factory());            // smeared MC data
  loop->AddFactory(new DTOFPaddleHit_factory());
  loop->AddFactory(new DTOFPoint_factory());
}
