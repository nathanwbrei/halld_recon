
#include <JANA/JFactoryGenerator.h>

#include "DBeamCurrent_factory.h"
#include "Df125EmulatorAlgorithm_factory.h"
#include "Df125EmulatorAlgorithm_factory_v2.h"
#include "Df250EmulatorAlgorithm_factory.h"
#include "Df250EmulatorAlgorithm_factory_v1.h"
#include "Df250EmulatorAlgorithm_factory_v2.h"
#include "Df250EmulatorAlgorithm_factory_v3.h"

void DAQ_init(JFactorySet *factorySet) {

    factorySet->Add(new DBeamCurrent_factory());
    factorySet->Add(new Df125EmulatorAlgorithm_factory());
    factorySet->Add(new Df125EmulatorAlgorithm_factory_v2());
    factorySet->Add(new Df250EmulatorAlgorithm_factory());
    factorySet->Add(new Df250EmulatorAlgorithm_factory_v1());
    factorySet->Add(new Df250EmulatorAlgorithm_factory_v2());
    factorySet->Add(new Df250EmulatorAlgorithm_factory_v3());
}

