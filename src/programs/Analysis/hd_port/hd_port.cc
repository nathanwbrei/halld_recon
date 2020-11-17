
#include <JANA/JApplication.h>
#include <JANA/JCsvWriter.h>

#include <HDGEOMETRY/DRootGeom.h>
#include <DANA/DApplication.h>
#include <DANA/DGeometryManager.h>

int main(int argc, char* argv[]) {
	JApplication app;
	auto dapp = DApplication(&app);
    //app.Add(new JEventSourceGenerator_EVIO());
    //app.Add(new JCsvWriter<>);
    // app.Run();
    int run_nr = 0;

	auto geo_manager = app.GetService<DGeometryManager>();
	const DRootGeom *RootGeom = geo_manager->GetRootGeom(run_nr);
	// Get material properties for chamber gas
	double rho_Z_over_A_LnI=0,radlen=0;
	double dRhoZoverA_CDC;
	RootGeom->FindMat("CDchamberGas", dRhoZoverA_CDC, rho_Z_over_A_LnI, radlen);

    return 0;
}

