// $Id$

#include <JANA/JFactorySet.h>
#include "DBCALGeometry_factory.h"
#include "DBCALShower_factory_IU.h"
#include "DBCALShower_factory_KLOE.h"
#include "DBCALShower_factory_CURVATURE.h"
#include "DBCALShower_factory.h"
#include "DBCALCluster_factory.h"
#include "DBCALCluster_factory_SINGLE.h"
#include "DBCALPoint_factory.h"
#include "DBCALUnifiedHit_factory.h"
#include "DBCALHit_factory.h"
#include "DBCALTDCHit_factory.h"
#include "DBCALClump.h"
#include "DBCALClump_factory.h"
#include "DBCALShower_factory_JLAB.h"


void BCAL_init(JFactorySet *factorySet)
{
	/// Create and register BCAL data factories
	factorySet->Add(new DBCALHit_factory());
	factorySet->Add(new DBCALTDCHit_factory());
	factorySet->Add(new DBCALGeometry_factory());
	factorySet->Add(new DBCALShower_factory_IU());
	factorySet->Add(new DBCALShower_factory_KLOE());
	factorySet->Add(new DBCALShower_factory_CURVATURE());
	factorySet->Add(new DBCALShower_factory());
	factorySet->Add(new DBCALCluster_factory());
	factorySet->Add(new DBCALCluster_factory_SINGLE());
	factorySet->Add(new DBCALPoint_factory());
	factorySet->Add(new DBCALUnifiedHit_factory());
	factorySet->Add(new DBCALClump_factory());
	factorySet->Add(new DBCALShower_factory_JLAB());
}
