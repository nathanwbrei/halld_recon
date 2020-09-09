#ifndef _DTrigger_
#define _DTrigger_

#include <JANA/JObject.h>

class DTrigger : public JObject
{
	public:
		JOBJECT_PUBLIC(DTrigger);

		//GETTERS
		uint32_t Get_L1TriggerBits(void) const{return dL1TriggerBits;}
		uint32_t Get_L1FrontPanelTriggerBits(void) const{return dL1FrontPanelTriggerBits;}
		bool Get_IsPhysicsEvent(void) const;

		//SETTERS
		void Set_L1TriggerBits(uint32_t locL1TriggerBits){dL1TriggerBits = locL1TriggerBits;}
		void Set_L1FrontPanelTriggerBits(uint32_t locL1FrontPanelTriggerBits){dL1FrontPanelTriggerBits = locL1FrontPanelTriggerBits;}

		void Summarize(JObjectSummary& summary) const override {
			summary.add(dL1TriggerBits, "dL1TriggerBits", "%ld");
			summary.add(dL1FrontPanelTriggerBits, "dL1FrontPanelTriggerBits", "%ld");
		}

	private:
		//NOTE: If is EPICS/SYNC/etc. event, both L1 values will be 0
		uint32_t dL1TriggerBits;
		uint32_t dL1FrontPanelTriggerBits;
};

inline bool DTrigger::Get_IsPhysicsEvent(void) const
{
	//Both L1 = 0: EPICS/SYNC/etc. //L1 = 8: PS
	return ((dL1FrontPanelTriggerBits == 0) && (dL1TriggerBits != 0) && (dL1TriggerBits != 8));
}

#endif // _DTrigger_
