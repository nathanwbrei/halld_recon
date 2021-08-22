// $Id$
//
//    File: JEventProcessor_TrackingPulls.h
// Created: Thu Nov  3 14:30:19 EDT 2016
// Creator: mstaib (on Linux ifarm1401 2.6.32-431.el6.x86_64 x86_64)
//

#ifndef _JEventProcessor_TrackingPulls_
#define _JEventProcessor_TrackingPulls_

#include <JANA/JEventProcessor.h>
#include <JANA/Compatibility/JLockService.h>
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

  std::shared_ptr<JLockService> lockService;


  TTree *tree_;
  int eventnumber_;
  int track_index_;
  double chi2_;
  int ndf_;
  double mom_;
  double phi_;
  double theta_;
  double pos_x_;
  double pos_y_;
  double pos_z_;
  int smoothed_;  // 1: smoothed, 0: not smoothed
  int any_nan_;   // 1: anyNaN = true, 0: anyNaN = false
  int cdc_ring_multi_hits_;
  double fdc_resi_[kNumFdcPlanes];
  double fdc_resic_[kNumFdcPlanes];
  double fdc_err_[kNumFdcPlanes];
  double fdc_errc_[kNumFdcPlanes];
  double fdc_x_[kNumFdcPlanes];
  double fdc_y_[kNumFdcPlanes];
  double fdc_z_[kNumFdcPlanes];
  double fdc_w_[kNumFdcPlanes];
  double fdc_s_[kNumFdcPlanes];
  double fdc_d_[kNumFdcPlanes];
  double fdc_tdrift_[kNumFdcPlanes];
  int fdc_wire_[kNumFdcPlanes];
  int fdc_left_right_[kNumFdcPlanes];  // -1: left, +1: right
  double cdc_resi_[kNumCdcRings];
  double cdc_err_[kNumCdcRings];
  double cdc_z_[kNumCdcRings];
  double cdc_tdrift_[kNumCdcRings];
  int cdc_straw_[kNumCdcRings];
  int cdc_left_right_[kNumCdcRings];  // -1: left, +1: right
  double cdc_phi_intersect_[kNumCdcRings];
};

#endif  // _JEventProcessor_TrackingPulls_
