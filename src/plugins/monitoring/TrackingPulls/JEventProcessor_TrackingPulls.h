// $Id$
//
//    File: JEventProcessor_TrackingPulls.h
// Created: Thu Nov  3 14:30:19 EDT 2016
// Creator: mstaib (on Linux ifarm1401 2.6.32-431.el6.x86_64 x86_64)
//

#ifndef _JEventProcessor_TrackingPulls_
#define _JEventProcessor_TrackingPulls_

#include <JANA/JEventProcessor.h>
#include "ANALYSIS/DTreeInterface.h"
#include <TTree.h>

class JEventProcessor_TrackingPulls : public JEventProcessor {
 public:
  JEventProcessor_TrackingPulls();
  ~JEventProcessor_TrackingPulls();

  static const int kNumFdcPlanes = 24;
  static const int kNumCdcRings = 28;

 private:
  void Init() override;
  void BeginRun(const std::shared_ptr<const JEvent>& event) override;
  void Process(const std::shared_ptr<const JEvent>& event) override;
  void EndRun() override;
  void Finish() override;

  //TREE
  bool MAKE_TREE;
  DTreeInterface* dTreeInterface;
  static thread_local DTreeFillData dTreeFillData;
};

#endif  // _JEventProcessor_TrackingPulls_
