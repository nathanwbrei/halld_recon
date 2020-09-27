//***********************************************
// FDC_init.cc: defines a function called by
// the framework to register the FDC factories.
// Author: Craig Bookwalter
// Date: Apr 2006
//***********************************************

#include <JANA/JEventLoop.h>

#include "DFDCHit_factory.h"
#include "DFDCPseudo_factory.h"
#include "DFDCCathodeCluster_factory.h"
#include "DFDCSegment_factory.h"
#include "DFDCIntersection_factory.h"
#include "DFDCPseudo_factory_WIRESONLY.h"


void FDC_init(JFactorySet *factorySet)
{
	/// Create and register FDC data factories
	factorySet->Add(new DFDCHit_factory());
	factorySet->Add(new DFDCPseudo_factory());
	factorySet->Add(new DFDCCathodeCluster_factory());
	factorySet->Add(new DFDCSegment_factory());
	factorySet->Add(new DFDCIntersection_factory());
	factorySet->Add(new DFDCPseudo_factory_WIRESONLY());
}
