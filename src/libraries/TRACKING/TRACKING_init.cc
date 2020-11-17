// $Id$

#include "JANA/JFactorySet.h"
#include "DTrackWireBased_factory.h"
#include "DTrackTimeBased_factory.h"
#include "DTrackWireBased_factory_StraightLine.h"
#include "DTrackTimeBased_factory_StraightLine.h"
#include "DTrackCandidate_factory.h"
#include "DTrackCandidate_factory_THROWN.h"
#include "DTrackCandidate_factory_CDC.h"
#include "DTrackCandidate_factory_FDC.h"
#include "DTrackCandidate_factory_FDCCathodes.h"
#include "DTrackCandidate_factory_FDCpseudo.h"
#include "DTrackCandidate_factory_CDC_or_FDCpseudo.h"
#include "DTrackCandidate_factory_StraightLine.h"
#include "DTrackWireBased_factory_THROWN.h"
#include "DTrackTimeBased_factory_THROWN.h"
#include "DTrackFinder_factory.h"
#include "DTrackFitter_factory.h"
#include "DTrackFitter_factory_ALT1.h"
#include "DTrackFitter_factory_Riemann.h"
#include "DTrackFitter_factory_StraightTrack.h"
#include "DTrackHitSelector_factory.h"
#include "DTrackHitSelector_factory_ALT1.h"
#include "DTrackHitSelector_factory_ALT2.h"
#include "DTrackHitSelector_factory_THROWN.h"
#include "DTrackFitter_factory_KalmanSIMD.h"
#include "DTrackFitter_factory_KalmanSIMD_ALT1.h"

void TRACKING_init(JFactorySet *factorySet)
{
   /// Create and register TRACKING data factories
   factorySet->Add(new DTrackFinder_factory());   
   factorySet->Add(new DTrackWireBased_factory());
   factorySet->Add(new DTrackTimeBased_factory());
   factorySet->Add(new DTrackCandidate_factory());
   factorySet->Add(new DTrackCandidate_factory_CDC());
   factorySet->Add(new DTrackCandidate_factory_FDC());
   factorySet->Add(new DTrackCandidate_factory_FDCCathodes());
   factorySet->Add(new DTrackCandidate_factory_FDCpseudo());
   factorySet->Add(new DTrackCandidate_factory_CDC_or_FDCpseudo());
   factorySet->Add(new DTrackCandidate_factory_THROWN());
   factorySet->Add(new DTrackCandidate_factory_StraightLine());
   factorySet->Add(new DTrackWireBased_factory_THROWN());
   factorySet->Add(new DTrackTimeBased_factory_THROWN()); 
   factorySet->Add(new DTrackWireBased_factory_StraightLine());
   factorySet->Add(new DTrackTimeBased_factory_StraightLine());
   factorySet->Add(new DTrackFitter_factory());
   factorySet->Add(new DTrackFitter_factory_ALT1());
   factorySet->Add(new DTrackFitter_factory_Riemann());
   factorySet->Add(new DTrackHitSelector_factory());
   factorySet->Add(new DTrackHitSelector_factory_ALT1());
   factorySet->Add(new DTrackHitSelector_factory_ALT2());
   factorySet->Add(new DTrackHitSelector_factory_THROWN());
   factorySet->Add(new DTrackFitter_factory_KalmanSIMD());   
   factorySet->Add(new DTrackFitter_factory_KalmanSIMD_ALT1());
   factorySet->Add(new DTrackFitter_factory_StraightTrack());

   factorySet->Add(new JFactoryT<DMCThrown>());
   // TODO: NWB: We need this but not the other JFactoryT's because a lot of code relies on `event->Get<DMCThrown>("")`
   //      silently returning the empty vector even when event sources (such as EVIO) don't insert anything because they
   //      don't even know about DMCThrowns. So this is an old-style 'dummy factory' which I had hoped to get rid of.
}
