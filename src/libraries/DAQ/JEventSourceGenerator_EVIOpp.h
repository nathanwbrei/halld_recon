// $Id$
//
//    File: JEventSourceGenerator_EVIOpp.h
// Created: Tue Mar 29 08:14:42 EDT 2016
// Creator: davidl (on Darwin harriet.jlab.org 13.4.0 i386)
//

#ifndef _JEventSourceGenerator_EVIOpp_
#define _JEventSourceGenerator_EVIOpp_

#include <JANA/Compatibility/jerror.h>
#include <JANA/JEventSourceGenerator.h>

#include <DAQ/HDEVIO.h>

#include "JEventSource_EVIOpp.h"

class JEventSourceGenerator_EVIOpp: public JEventSourceGenerator{
	public:
		JEventSourceGenerator_EVIOpp(){}
		virtual ~JEventSourceGenerator_EVIOpp(){}
		virtual const char* className(void){return static_className();}
		static const char* static_className(void){return "JEventSourceGenerator_EVIOpp";}

		std::string GetDescription() override;
		double CheckOpenable(string source) override;
		JEventSource* MakeJEventSource(string source) override;
};

#endif // _JEventSourceGenerator_EVIOpp_

