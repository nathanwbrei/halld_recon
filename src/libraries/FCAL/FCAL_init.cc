// $Id$

#include <JANA/JFactorySet.h>

#include "DFCALCluster_factory.h"
#include "DFCALCluster_factory_Island.h"
#include "DFCALGeometry_factory.h"
#include "DFCALShower_factory.h"
#include "DFCALHit_factory.h"

void FCAL_init(JFactorySet *factorySet)
{
	/// Create and register FCAL data factories
	loop->AddFactory(new JFactory<DFCALDigiHit>());
	loop->AddFactory(new DFCALHit_factory());
	loop->AddFactory(new JFactory<DFCALHit>("TRUTH"));
	loop->AddFactory(new DFCALCluster_factory());
	loop->AddFactory(new DFCALCluster_factory_Island());
	loop->AddFactory(new DFCALShower_factory());
	loop->AddFactory(new DFCALGeometry_factory());
	loop->AddFactory(new JFactory<DFCALTruthShower>());

	return NOERROR;
}
