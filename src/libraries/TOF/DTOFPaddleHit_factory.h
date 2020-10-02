// $Id$
//
///    File: DTOFPaddleHit_factory.h
/// Created: Thu Jun  9 10:05:21 EDT 2005
/// Creator: davidl (on Darwin wire129.jlab.org 7.8.0 powerpc)
///
/// Addition: command line parmeter -PTOF:TOF_POINT_TAG=TRUTH will initiate
///           the use of TOFHitRaw::TRUTH information to calculate the TOFHit positions
///           (e.q. use of unsmeared data)


#ifndef _DTOFPaddleHit_factory_
#define _DTOFPaddleHit_factory_

#include <JANA/JFactoryT.h>
#include "DTOFPaddleHit.h"
#include "DTOFGeometry.h"
#include "TMath.h"


/// \htmlonly
/// <A href="index.html#legend">
///	<IMG src="CORE.png" width="100">
///	</A>
/// \endhtmlonly

/// 2-ended TOF coincidences. The individual hits come from DTOFHit objects and
/// the 2 planes are combined into single hits in the DTOFPoint objects. This is the
/// intermediate set of objects between the two.

class DTOFPaddleHit_factory:public JFactoryT<DTOFPaddleHit>{
 public:
  DTOFPaddleHit_factory(){};
  ~DTOFPaddleHit_factory(){};
  
  string TOF_POINT_TAG;
  double C_EFFECTIVE;
  double HALFPADDLE;
  double E_THRESHOLD;
  double ATTEN_LENGTH;
  double ENERGY_ATTEN_FACTOR;
  double TIME_COINCIDENCE_CUT;

  int TOF_NUM_PLANES;
  int TOF_NUM_BARS;

  vector<double> propagation_speed;

  vector < vector <float> > AttenuationLengths;

  vector <const DTOFGeometry*> TOFGeom;

 protected:
  void Init() override;
  void BeginRun(const std::shared_ptr<const JEvent>& event) override;
  void Process(const std::shared_ptr<const JEvent>& event) override;
  //void EndRun() override;
  //void Finish() override;
};

#endif // _DTOFPaddleHit_factory_

