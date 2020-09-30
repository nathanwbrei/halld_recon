
// Copyright 2020, Jefferson Science Associates, LLC.
// Subject to the terms in the LICENSE file found in the top-level directory.


#ifndef HALLD_RECON_DEVENT_H
#define HALLD_RECON_DEVENT_H

#include <JANA/JEvent.h>
#include <JANA/Calibrations/JCalibrationManager.h>
#include <JANA/Compatibility/JLockService.h>

#include "DANA/DStatusBits.h"
#include "DANA/DGeometryManager.h"
#include "HDGEOMETRY/DGeometry.h"


class DEvent {
	/// DEvent is a wrapper for JEvent which makes it easy to access DANA-specific
	/// services and to use JANA1-style getters. The goal is to minimize the friction
	/// caused by migrating from JANA1 to JANA2, although this may turn out to be a
	/// good pattern to use generally. While the new JService interface makes it easy to
	/// share components such as DGeometries among JANA components, it adds several awkward
	/// layers of indirection which the old DApplication does not, and requires a
	/// bunch of mysterious #include boilerplate for each file. DEvent resolves this
	/// awkwardness without introducing any of the problems that DApplication had.
	/// Specifically, halld-recon is no longer dependent on JANA internals, no casting
	/// is needed (which curtails our ability to compose halld-recon components into
	/// different programs), and run number information propagates automatically, reducing
	/// boilerplate further.
	/// Note: DEvent is not thread-safe, but it is lightweight. Instantiate on the stack
	/// inside (e.g.) JFactory::BeginRun and let it go out of scope when the method exits.

private:
	const std::shared_ptr<const JEvent> m_event;
	JApplication* m_app;
	JCalibration* m_calibration = nullptr;
	DGeometry* m_dgeometry = nullptr;

public:
	DEvent(const std::shared_ptr<const JEvent>& event)
		: m_event(event)
		, m_app(event->GetJApplication()) {};

	inline JCalibration* GetJCalibration() {
		if (m_calibration == nullptr) {
			auto run_number = m_event->GetRunNumber();
			m_calibration = m_app->GetService<JCalibrationManager>()->GetJCalibration(run_number);
		}
		// TODO: Assert not null?
		return m_calibration;
	}

	inline bool GetStatusBit(StatusBitType status_bit) {
		return m_event->GetSingle<DStatusBits>()->GetStatusBit(status_bit);
	}

	inline DGeometry* GetDGeometry() {
		if (m_dgeometry == nullptr) {
			auto d_geo_man = m_app->GetService<DGeometryManager>();
			m_dgeometry = d_geo_man->GetDGeometry(m_event->GetRunNumber());
		}
		return m_dgeometry;
	}

	inline JApplication* GetJApplication() {
		return m_app;
	}

	inline DMagneticFieldMap* GetBfield() {
		auto run_number = m_event->GetRunNumber();
		return m_app->GetService<DGeometryManager>()->GetBfield(run_number);
	}

	inline std::shared_ptr<JLockService> GetLockService() {
		return m_app->GetService<JLockService>();
	}

	// TODO: Get JGeometry
	// TODO: Get DGeometry
	// TODO: Get locks
	// TODO: Get JLargeCalibration

};


inline DGeometry* GetDGeometry(const std::shared_ptr<const JEvent>& event) {
	return event->GetJApplication()->GetService<DGeometryManager>()->GetDGeometry(event->GetRunNumber());
}

inline std::shared_ptr<JLockService> GetLockService(const std::shared_ptr<const JEvent>& event) {
	return event->GetJApplication()->GetService<JLockService>();
}


template<class T>
inline bool GetCalib(const std::shared_ptr<const JEvent>& event, string namepath, std::map<string,T> &vals)
{
	/// Get the JCalibration object from JApplication for the run number of
	/// the current event and call its Get() method to get the constants.

	vals.clear();
	auto app = event->GetJApplication();
	JCalibration *calib = app->GetService<JCalibrationManager>()->GetJCalibration(event->GetRunNumber());
	if(!calib){ return true; }
	return calib->Get(namepath, vals, event->GetEventNumber());
}

template<class T>
inline bool GetCalib(const std::shared_ptr<const JEvent>& event, string namepath, vector<T> &vals)
{
	/// Get the JCalibration object from JApplication for the run number of
	/// the current event and call its Get() method to get the constants.
	vals.clear();
	auto app = event->GetJApplication();
	JCalibration *calib = app->GetService<JCalibrationManager>()->GetJCalibration(event->GetRunNumber());
	if(!calib){ return true; }
	return calib->Get(namepath, vals, event->GetEventNumber());
}

template<class T>
inline bool GetCalib(const std::shared_ptr<const JEvent>& event, std::string namepath, T &val)
{
	/// This is a convenience method for getting a single entry. It
	/// simply calls the vector version and returns the first entry.
	/// It returns true if the vector version returns true AND there
	/// is at least one entry in the vector. No check is made for there
	/// there being more than one entry in the vector.

	std::vector<T> vals;
	bool ret = GetCalib(event, namepath, vals);
	if(vals.empty()) return true;
	val = vals[0];
	return ret;
}

#endif //HALLD_RECON_DEVENT_H
