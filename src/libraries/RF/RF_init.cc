// $Id: RF_init.cc 14984 2015-03-31 14:33:22Z pmatt $

#include <JANA/JEventLoop.h>
using namespace jana;

#include "DRFDigiTime.h"
#include "DRFTDCDigiTime.h"
#include "DRFTime_factory.h"
#include "DRFTime_factory_FDC.h"
#include "DRFTime_factory_PSC.h"
#include "DRFTime_factory_TAGH.h"
#include "DRFTime_factory_TOF.h"

jerror_t RF_init(JEventLoop *loop)
{
	/// Create and register RF data factories
	loop->AddFactory(new JFactory<DRFDigiTime>());
	loop->AddFactory(new JFactory<DRFTDCDigiTime>());
	loop->AddFactory(new DRFTime_factory());
	loop->AddFactory(new DRFTime_factory_FDC());
	loop->AddFactory(new DRFTime_factory_PSC());
	loop->AddFactory(new DRFTime_factory_TAGH());
	loop->AddFactory(new DRFTime_factory_TOF());
	loop->AddFactory(new JFactory<DRFTime>("TRUTH"));

	return NOERROR;
}
