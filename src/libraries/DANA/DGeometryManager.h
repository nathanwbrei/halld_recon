
// Copyright 2020, Jefferson Science Associates, LLC.
// Subject to the terms in the LICENSE file found in the top-level directory.


#ifndef HALLD_RECON_DGEOMETRYMANAGER_H
#define HALLD_RECON_DGEOMETRYMANAGER_H

#include <JANA/JApplication.h>
#include <JANA/Services/JServiceLocator.h>

#include <vector>
#include <mutex>


class DMagneticFieldMap;
class DLorentzDeflections;
class DGeometry;
class DRootGeom;
class DDIRCLutReader;

class DGeometryManager: public JService {

public:
	DGeometryManager(JApplication* app);
	~DGeometryManager() override;

	DMagneticFieldMap* GetBfield(unsigned int run_number=1);
	DLorentzDeflections *GetLorentzDeflections(unsigned int run_number=1);
	DGeometry* GetDGeometry(unsigned int run_number);
	DRootGeom *GetRootGeom(unsigned int run_number);
	DDIRCLutReader *GetDIRCLut(unsigned int run_number);

private:
	DMagneticFieldMap *bfield = nullptr;
	DLorentzDeflections *lorentz_def = nullptr;
	DRootGeom *RootGeom = nullptr;
	std::vector<DGeometry*> geometries;
	DDIRCLutReader *dircLut = nullptr;

	// These are used to ensure that the cached objects correspond to the desired run number
	size_t bfield_run_nr;
	size_t lorenzdef_run_nr;
	size_t rootgeom_run_nr;
	size_t dirclut_run_nr;

	JApplication* app;

	std::mutex mutex;
};


#endif //HALLD_RECON_DGEOMETRYMANAGER_H
