// $Id$
//
//    File: DFCALGeometry_factory.cc
// Created: Wed Aug 24 10:09:27 EST 2005
// Creator: shepherd (on Darwin 129-79-159-16.dhcp-bl.indiana.edu 8.2.0 powerpc)
//

#include <cassert>

#include "DFCALGeometry_factory.h"
#include "DFCALGeometry.h"

//------------------
// BeginRun
//------------------
void DFCALGeometry_factory::BeginRun(const std::shared_ptr<const JEvent>& event)
{
	assert( mData.empty() );
	SetFactoryFlag(PERSISTENT);
	Insert(new DFCALGeometry());
}

//------------------
// EndRun
//------------------
void DFCALGeometry_factory::EndRun()
{
	// We have to manually clear and delete the contents of mData because PERSISTENT flag was set.
	for (auto geom : mData) {
		delete geom;
	}
	mData.clear();
}

