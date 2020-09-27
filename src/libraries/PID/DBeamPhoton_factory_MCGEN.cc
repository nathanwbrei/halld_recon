#include <iostream>
#include <iomanip>
using namespace std;

#include <JANA/JEvent.h>
#include "DBeamPhoton_factory_MCGEN.h"


//------------------
// BeginRun
//------------------
void DBeamPhoton_factory_MCGEN::BeginRun(const std::shared_ptr<const JEvent>& event)
{
	//Setting this flag makes it so that JANA does not delete the objects in _data.  This factory will manage this memory. 
		//This is because some/all of these pointers are just copied from earlier objects, and should not be deleted.  
	bool locIsRESTEvent = event->GetSingle<DStatusBits>()->GetStatusBit(kSTATUS_REST);
	if(!locIsRESTEvent) //If REST, will grab from file: IS owner
		SetFactoryFlag(NOT_OBJECT_OWNER);
}

//------------------
// Process
//------------------
void DBeamPhoton_factory_MCGEN::Process(const std::shared_ptr<const JEvent>& event)
{
	if(!mData.empty())
	{
		if(mData[0]->dSystem == SYS_NULL)
			delete mData[0]; //CREATED BY THIS FACTORY //else copied from TRUTH factory!
		mData.clear();
	}

	//Check if MC
	vector<const DMCReaction*> locMCReactions;
	event->Get(locMCReactions);
	if(locMCReactions.empty())
		return; //Not a thrown event

	//First see if it was tagged: If so, use truth DBeamPhoton
	vector<const DBeamPhoton*> locTruthPhotons;
	event->Get(locTruthPhotons, "TRUTH");
	for(auto locBeamPhoton : locTruthPhotons)
	{
		vector<const DTAGMHit*> locTAGMHits;
		locBeamPhoton->Get(locTAGMHits);
		for(auto locTAGMHit : locTAGMHits)
		{
			if(locTAGMHit->bg != 0)
				continue;
			mData.push_back(const_cast<DBeamPhoton*>(locBeamPhoton));
			return;
		}

		vector<const DTAGHHit*> locTAGHHits;
		locBeamPhoton->Get(locTAGHHits);
		for(auto locTAGHHit : locTAGHHits)
		{
			if(locTAGHHit->bg != 0)
				continue;
			mData.push_back(const_cast<DBeamPhoton*>(locBeamPhoton));
			return;
		}
	}

	//Photon is NOT TAGGED //Create a beam object from the DMCReaction
	auto *locBeamPhoton = new DBeamPhoton;
	*(DKinematicData*)locBeamPhoton = locMCReactions[0]->beam;
	Insert(locBeamPhoton);
}

//------------------
// Finish
//------------------
void DBeamPhoton_factory_MCGEN::Finish()
{
	if(!mData.empty())
	{
		if(mData[0]->dSystem == SYS_NULL)
			delete mData[0]; //CREATED BY THIS FACTORY //else copied from TRUTH factory!
		mData.clear();
	}
}

