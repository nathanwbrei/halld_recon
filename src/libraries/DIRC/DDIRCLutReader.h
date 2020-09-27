// $Id$
//
//    File: DDIRCLutReader.h
//

#ifndef _DDIRCLutReader_
#define _DDIRCLutReader_

#include <JANA/Compatibility/jerror.h>
#include <JANA/JApplication.h>
#include <JANA/Calibrations/JCalibration.h>
#include <JANA/Calibrations/JLargeCalibration.h>

#include <DIRC/DDIRCGeometry.h>

#include "TROOT.h"
#include "TVector3.h"
#include "TFile.h"
#include "TTree.h"

class DDIRCLutReader{

public:
	
	DDIRCLutReader(JApplication *japp, unsigned int run_number);
	virtual ~DDIRCLutReader();

	uint GetLutPixelAngleSize(int bar, int pixel) const;
	uint GetLutPixelTimeSize(int bar, int pixel) const;
	uint GetLutPixelPathSize(int bar, int pixel) const;
	TVector3 GetLutPixelAngle(int bar, int pixel, int entry) const;
	Float_t GetLutPixelTime(int bar, int pixel, int entry) const;
	Long64_t GetLutPixelPath(int bar, int pixel, int entry) const;		

private:

	pthread_mutex_t mutex;

	vector<TVector3> lutNodeAngle[DDIRCGeometry::kBars][DDIRCGeometry::kPMTs*DDIRCGeometry::kPixels];
	vector<Float_t> lutNodeTime[DDIRCGeometry::kBars][DDIRCGeometry::kPMTs*DDIRCGeometry::kPixels];
	vector<Long64_t> lutNodePath[DDIRCGeometry::kBars][DDIRCGeometry::kPMTs*DDIRCGeometry::kPixels];

protected:
	JCalibration *jcalib;
	JLargeCalibration *jresman;
};

#endif // _DDIRCLutReader_

