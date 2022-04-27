// $Id$
//
//    File: DCTOFPoint.h
// Created: Thu Oct 28 07:48:04 EDT 2021
// Creator: staylor (on Linux ifarm1901.jlab.org 3.10.0-1062.4.1.el7.x86_64 x86_64)
//

#ifndef _DCTOFPoint_
#define _DCTOFPoint_

#include <JANA/JObject.h>

class DCTOFPoint:public JObject{
 public:
  JOBJECT_PUBLIC(DCTOFPoint);

  int bar; ///< bar number
  double dE; ///< Energy depostion in GeV
  double t; ///< Time in ns 
  double y; ///< position along scintillator in cm
		
  // This method is used primarily for pretty printing
  // the second argument to summary.add is printf style format
  void Summarize(JObjectSummary& summary) const {
    summary.add(bar, "bar", "%4d");
    summary.add(dE, "dE", "%f");
    summary.add(t, "t", "%f");
    summary.add(y, "y", "%f");
  }
		
};

#endif // _DCTOFPoint_

