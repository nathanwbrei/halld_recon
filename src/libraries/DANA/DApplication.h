// $Id$
//
//    File: DApplication.h
// Created: Mon Jul  3 21:46:01 EDT 2006
// Creator: davidl (on Darwin Harriet.local 8.6.0 powerpc)
//

#ifndef _DApplication_
#define _DApplication_

class JApplication;
class JEventSourceGenerator;
class JFactoryGenerator;

struct DApplication {

	JEventSourceGenerator *event_source_generator;
	JFactoryGenerator *factory_generator;

	void Init(JApplication *app);
	void CheckCpuSimdSupport();
	void CopySQLiteToLocalDisk(JApplication *app);

};

#endif // _DApplication_

