// $Id$
//
//    File: DNeutralParticle_factory_PreSelect.h
// Created: Mon Dec  7 14:29:24 EST 2009
// Creator: staylor (on Linux ifarml6 2.6.18-128.el5 x86_64)
//

#ifndef _DNeutralParticle_factory_PreSelect_
#define _DNeutralParticle_factory_PreSelect_

#include <JANA/JFactoryT.h>
#include <PID/DNeutralParticle.h>
#include <PID/DNeutralParticleHypothesis.h>
#include <PID/DNeutralShower.h>

using namespace std;


class DNeutralParticle_factory_PreSelect : public JFactoryT<DNeutralParticle>
{
	public:
		DNeutralParticle_factory_PreSelect(){
			SetTag("PreSelect");
		}
		~DNeutralParticle_factory_PreSelect(){};

	private:
		void Init() override;
		void BeginRun(const std::shared_ptr<const JEvent>& event) override;
		void Process(const std::shared_ptr<const JEvent>& event) override;
		void EndRun() override;
		void Finish();
};

#endif // _DNeutralParticle_factory_PreSelect_

