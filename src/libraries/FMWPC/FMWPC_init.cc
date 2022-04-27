// $Id$
//
//    File: FMWPC_init.cc
// Created: Tue Jun 16 07:04:58 EDT 2015
// Creator: davidl (on Darwin harriet.jlab.org 13.4.0 i386)
//

#include "DFMWPCHit.h"
#include "DFMWPCHit_factory.h"
#include "DFMWPCCluster_factory.h"
#include "DFMWPCTruthHit.h"
#include "DFMWPCTruth.h"
#include "DCTOFTruth.h"
#include "DCTOFHit.h"
#include "DCTOFPoint_factory.h"
#include "DFMWPCMatchedTrack_factory.h"

#include <JANA/Compatibility/JGetObjectsFactory.h>

void FMWPC_init(JFactorySet *factorySet) {
  /// Create and register FMWPC data factories
  //factorySet->Add(new JFactory<DFMWPCHit>());
  factorySet->Add(new JFactoryT<DFMWPCDigiHit>());
  factorySet->Add(new JFactoryT<DFMWPCTruthHit>());
  factorySet->Add(new JFactoryT<DFMWPCTruth>());
  factorySet->Add(new DFMWPCHit_factory());
  factorySet->Add(new DFMWPCCluster_factory());
  factorySet->Add(new DFMWPCMatchedTrack_factory());

  /// Factories for downstream scintillators
  factorySet->Add(new JFactoryT<DCTOFHit>());
  factorySet->Add(new JFactoryT<DCTOFTruth>());
  factorySet->Add(new DCTOFPoint_factory());
}

