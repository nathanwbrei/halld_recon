// $Id$
//
//    File: DNeutralParticle_factory_PreSelect.cc
// Created: Thu Dec  3 17:27:55 EST 2009
// Creator: staylor (on Linux ifarml6 2.6.18-128.el5 x86_64)
//

#include "DNeutralParticle_factory_PreSelect.h"
#include <JANA/JEvent.h>

//------------------
// Init
//------------------
void DNeutralParticle_factory_PreSelect::Init()
{
	//Setting this flag makes it so that JANA does not delete the objects in _data.  This factory will manage this memory. 
		//This is because some/all of these pointers are just copied from earlier objects, and should not be deleted.  
	SetFactoryFlag(NOT_OBJECT_OWNER);
}

//------------------
// BeginRun
//------------------
void DNeutralParticle_factory_PreSelect::BeginRun(const std::shared_ptr<const JEvent>& event)
{
}

//------------------
// Process
//------------------
void DNeutralParticle_factory_PreSelect::Process(const std::shared_ptr<const JEvent>& event)
{
	//Clear objects from last event
	mData.clear();

	vector<const DNeutralParticle*> locNeutralParticles;
	event->Get(locNeutralParticles);

	vector<const DNeutralShower*> locNeutralShowers;
	event->Get(locNeutralShowers, "PreSelect");

	set<const DNeutralShower*> locNeutralShowerSet;
	for(size_t loc_i = 0; loc_i < locNeutralShowers.size(); ++loc_i)
		locNeutralShowerSet.insert(locNeutralShowers[loc_i]);

	//if neutral shower was good, keep particle, else ignore it
	for(size_t loc_i = 0; loc_i < locNeutralParticles.size(); ++loc_i)
	{
		if(locNeutralShowerSet.find(locNeutralParticles[loc_i]->dNeutralShower) != locNeutralShowerSet.end())
			mData.push_back(const_cast<DNeutralParticle*>(locNeutralParticles[loc_i]));
	}
}

//------------------
// EndRun
//------------------
void DNeutralParticle_factory_PreSelect::EndRun()
{
}

//------------------
// Finish
//------------------
void DNeutralParticle_factory_PreSelect::Finish()
{
}


