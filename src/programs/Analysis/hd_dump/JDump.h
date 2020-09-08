#ifndef HALLD_RECON_JDUMP_H
#define HALLD_RECON_JDUMP_H

#include <JANA/JEventProcessor.h>

class JDump : public JEventProcessor {

	bool m_pause_between_events;
	bool m_skip_boring_events;
	bool m_print_all;
	bool m_print_core;
	bool m_list_associated_objects;
	bool m_print_summary_all;
	bool m_print_summary_header;
	bool m_print_status_bits;
	bool m_activate_tagged_for_summary;

	std::vector<std::pair<string, string>> m_core_factories;
	std::vector<std::pair<string, string>> m_factories_to_print;
	std::vector<std::pair<string, string>> m_factories_to_summarize;

public:

	void Initialize()
	void Process(const std::shared_ptr<const JEvent>& event) override;

};


#endif //HALLD_RECON_JDUMP_H
