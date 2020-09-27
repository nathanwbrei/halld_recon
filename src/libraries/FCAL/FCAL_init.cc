// $Id$

#include <JANA/JFactorySet.h>

#include "DFCALCluster_factory.h"
#include "DFCALGeometry_factory.h"
#include "DFCALShower_factory.h"
#include "DFCALHit_factory.h"

void FCAL_init(JFactorySet *factorySet)
{
	/// Create and register FCAL data factories
	factorySet->Add(new DFCALHit_factory());
	factorySet->Add(new DFCALCluster_factory());
	factorySet->Add(new DFCALShower_factory());
	factorySet->Add(new DFCALGeometry_factory());
}
