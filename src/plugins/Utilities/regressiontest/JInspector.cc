#include "JInspector.h"

#include <JANA/JEventSource.h>
#include <JANA/JFactory_base.h>
#include <JANA/JApplication.h>
#include <stack>
#include "JTablePrinter.h"


JInspector::JInspector(JEventLoop* event) : m_event(event) {}

void JInspector::BuildIndices() {
    if (m_indexes_built) return;
    m_factories.clear();
    for (auto fac: m_event->GetFactories()) {
        m_factories.push_back(fac);
    }
    std::sort(m_factories.begin(), m_factories.end(), [](JFactory_base* first, JFactory_base* second){
        return std::make_pair(std::string(first->GetDataClassName()), std::string(first->Tag())) <
               std::make_pair(std::string(second->GetDataClassName()), std::string(second->Tag()));});

    int i = 0;
    for (JFactory_base* fac : m_factories) {
        std::string key = MakeFactoryKey(fac->GetDataClassName(), fac->Tag());
        std::pair<int, JFactory_base*> value(i++, fac);
        m_factory_index.insert({key, value});
        m_factory_index.insert({std::to_string(i), value});
    }
    m_indexes_built = true;
}


std::vector<const JObject*> JInspector::FindAllAncestors(const JObject* root) {
    std::vector<const JObject*> all_ancestors;
    std::stack<const JObject*> to_visit;
    to_visit.push(root);
    while (!to_visit.empty()) {
        auto current_obj = to_visit.top();
        to_visit.pop();
        all_ancestors.push_back(current_obj);
        std::vector<const JObject*> current_ancestors;
        current_obj->GetT<JObject>(current_ancestors);
        for (auto obj : current_ancestors) {
            to_visit.push(obj);
        }
    }
    return all_ancestors;
}

std::tuple<JFactory_base*, size_t, size_t> JInspector::LocateObject(JEventLoop& event, const JObject* obj) {
    auto objName = obj->className();
    size_t fac_idx = 0;
    for (auto fac : m_factories) {
        if (fac->GetDataClassName() == objName) {
            size_t obj_idx = 0;
	    std::vector<void*> data = fac->Get();
            for (auto o : data) { // Won't trigger factory creation if it hasn't already happened
                if (obj == (JObject*) o) { // TODO: Probably won't work in the case of multiple inheritance
                    return std::make_tuple(fac, fac_idx, obj_idx);
                }
                obj_idx++;
            }
        }
        fac_idx++;
    }
    return std::make_tuple(nullptr, -1, -1);
}

void JInspector::PrintEvent() {
    ToText(m_event, m_format==Format::Json, m_out);
}
void JInspector::PrintFactories(int filter_level=0) {
    BuildIndices();
    ToText(m_factories, filter_level, m_format==Format::Json, m_out);
}
void JInspector::PrintObjects(std::string factory_key) {
    BuildIndices();
    auto result = m_factory_index.find(factory_key);
    if (result == m_factory_index.end()) {
        m_out << "(Error: Invalid factory name or index)\n";
        return;
    }
    auto fac = const_cast<JFactory_base*>(result->second.second);
    auto vobjs = fac->Get();
    std::vector<JObject*> objs;
    for (void* vobj: vobjs) {
        objs.push_back((JObject*) vobj);
    }
    ToText(objs, m_format==Format::Json, m_out);
}
void JInspector::PrintFactoryDetails(std::string fac_key) {
    BuildIndices();
    auto result = m_factory_index.find(fac_key);
    if (result == m_factory_index.end()) {
        m_out << "(Error: Invalid factory name or index)\n";
        return;
    }
    auto fac = result->second.second;
    ToText(fac, m_format==Format::Json, m_out);
}
void JInspector::PrintObject(std::string fac_key, int object_idx) {
    BuildIndices();
    auto result = m_factory_index.find(fac_key);
    if (result == m_factory_index.end()) {
        m_out << "(Error: Invalid factory name or index)\n";
        return;
    }
    JFactory_base* fac = const_cast<JFactory_base*>(result->second.second);
    auto vobjs = fac->Get();
    if ((size_t) object_idx >= vobjs.size()) {
        m_out << "(Error: Object index out of range)" << std::endl;
        return;
    }
    auto obj = (JObject*) vobjs[object_idx];
    ToText(obj, m_format==Format::Json, m_out);
}
std::string JInspector::MakeFactoryKey(std::string name, std::string tag) {
    std::ostringstream ss;
    ss << name;
    if (!tag.empty()) {
        ss << ":" << tag;
    }
    return ss.str();
}
void JInspector::PrintHelp() {
    m_out << "  -----------------------------------------" << std::endl;
    m_out << "  Available commands" << std::endl;
    m_out << "  -----------------------------------------" << std::endl;
    m_out << "  pe   PrintEvent" << std::endl;
    m_out << "  pf   PrintFactories [filter_level <- {0,1,2,3}]" << std::endl;
    m_out << "  pfd  PrintFactoryDetails fac_idx" << std::endl;
    m_out << "  po   PrintObjects fac_idx" << std::endl;
    m_out << "  po   PrintObject fac_idx obj_idx" << std::endl;
    m_out << "  pfp  PrintFactoryParents fac_idx" << std::endl;
    m_out << "  pop  PrintObjectParents fac_idx obj_idx" << std::endl;
    m_out << "  poa  PrintObjectAncestors fac_idx obj_idx" << std::endl;
    m_out << "  vt   ViewAsTable" << std::endl;
    m_out << "  vj   ViewAsJson" << std::endl;
    m_out << "  x    Exit" << std::endl;
    m_out << "  h    Help" << std::endl;
    m_out << "  -----------------------------------------" << std::endl;
}
void JInspector::ToText(JEventLoop* event, bool asJson, std::ostream& out) {
    std::string className = event->GetJEvent().GetJEventSource()->className();
    if (className.empty()) className = "(unknown)";
    if (asJson) {
        out << "{ \"run_number\": " << event->GetJEvent().GetRunNumber();
        out << ", \"event_number\": " << event->GetJEvent().GetEventNumber();
        out << ", \"source\": \"" << event->GetJEvent().GetJEventSource()->GetSourceName();
        out << "\", \"class_name\": \"" << className;
        out << "\", \"is_sequential\": \"" << event->GetJEvent().GetSequential() << "\"}";
        out << std::endl;
    }
    else {
        out << "Run number:   " << event->GetJEvent().GetRunNumber() << std::endl;
        out << "Event number: " << event->GetJEvent().GetEventNumber() << std::endl;
        out << "Event source: " << event->GetJEvent().GetJEventSource()->GetSourceName() << std::endl;
        out << "Class name:   " << className << std::endl;
        out << "Sequential:   " << event->GetJEvent().GetSequential() << std::endl;
    }
}

void JInspector::ToText(JFactory_base* fac, bool asJson, std::ostream& out) {

    std::string pluginName = "(no plugin)";
    std::string factoryName = "(dummy factory)";

    std::string tag = fac->Tag();
    if (tag.empty()) tag = "(no tag)";

    std::string creationStatus = "Unknown";
    /*
    switch (fac->GetCreationStatus()) {
        case JFactory::CreationStatus::NotCreatedYet: creationStatus = "NotCreatedYet"; break;
        case JFactory::CreationStatus::Created: creationStatus = "Created"; break;
        case JFactory::CreationStatus::Inserted: creationStatus = "Inserted"; break;
        case JFactory::CreationStatus::InsertedViaGetObjects: creationStatus = "InsertedViaGetObjects"; break;
        case JFactory::CreationStatus::NeverCreated: creationStatus = "NeverCreated"; break;
        default: creationStatus = "Unknown";
    }
    */

    if (!asJson) {
        out << "Plugin name          " << pluginName << std::endl;
        // out << "Factory name         " << factoryName << std::endl;
        out << "Object name          " << fac->GetDataClassName() << std::endl;
        out << "Tag                  " << tag << std::endl;
        out << "Creation status      " << creationStatus << std::endl;
        out << "Object count         " << fac->GetNrows(false, true) << std::endl;
        out << "Persistent flag      " << fac->TestFactoryFlag(JFactory_base::JFactory_Flags_t::PERSISTANT) << std::endl;
        out << "NotObjectOwner flag  " << fac->TestFactoryFlag(JFactory_base::JFactory_Flags_t::NOT_OBJECT_OWNER) << std::endl;
    }
    else {
        out << "{" << std::endl;
        out << "  \"plugin_name\":   \"" << pluginName << "\"," << std::endl;
        // out << "  \"factory_name\":  \"" << factoryName << "\"," << std::endl;
        out << "  \"object_name\":   \"" << fac->GetDataClassName() << "\"," << std::endl;
        out << "  \"tag\":           \"" << tag << "\"," << std::endl;
        out << "  \"creation\":      \"" << creationStatus << "\"," << std::endl;
        out << "  \"object_count\":  " << fac->GetNrows(false, true) << "," << std::endl;
        out << "  \"persistent\":    " << fac->TestFactoryFlag(JFactory_base::JFactory_Flags_t::PERSISTANT) << "," << std::endl;
        out << "  \"not_obj_owner\": " << fac->TestFactoryFlag(JFactory_base::JFactory_Flags_t::NOT_OBJECT_OWNER) << std::endl;
        out << "}" << std::endl;
    }
}

void JInspector::ToText(const std::vector<JFactory_base*>& factories, int filterlevel, bool asJson, std::ostream &out) {
    size_t idx = -1;
    if (!asJson) {
        JTablePrinter t;
        t.AddColumn("Index", JTablePrinter::Justify::Right);
        // t.AddColumn("Factory name");
        t.AddColumn("Object name");
        t.AddColumn("Tag");
        t.AddColumn("Creation status");
        t.AddColumn("Object count", JTablePrinter::Justify::Right);
        for (auto fac : factories) {
	    std::string tag = fac->Tag();
            if (tag.empty()) tag = "(no tag)";
            std::string creationStatus = "Unknown";
	    /*
            switch (fac->GetCreationStatus()) {
                case JFactory::CreationStatus::NotCreatedYet: creationStatus = "NotCreatedYet"; break;
                case JFactory::CreationStatus::Created: creationStatus = "Created"; break;
                case JFactory::CreationStatus::Inserted: creationStatus = "Inserted"; break;
                case JFactory::CreationStatus::InsertedViaGetObjects: creationStatus = "InsertedViaGetObjects"; break;
                case JFactory::CreationStatus::NeverCreated: creationStatus = "NeverCreated"; break;
                default: creationStatus = "Unknown";
            }
	    */
            idx += 1;
	    /*
            if (filterlevel > 0 && (fac->GetCreationStatus()==JFactory::CreationStatus::NeverCreated ||
                           fac->GetCreationStatus()==JFactory::CreationStatus::NotCreatedYet)) continue;
            if (filterlevel > 1 && (fac->GetNrows(false, true)== 0)) continue;
            if (filterlevel > 2 && (fac->GetCreationStatus()==JFactory::CreationStatus::Inserted ||
                                    fac->GetCreationStatus()==JFactory::CreationStatus::InsertedViaGetObjects)) continue;
	    */

            t | idx | fac->GetDataClassName() | tag | creationStatus | fac->GetNrows(false, true);
        }
        t.Render(out);
    }
    else {
        out << "{" << std::endl;
        for (auto fac : factories) {
	    std::string tag = fac->Tag();
            if (tag.empty()) tag = "null";
            out << "  " << idx++ << ": { ";
            out << "\"object_name\": \"" << fac->GetDataClassName() << "\", \"tag\": ";
            if (*(fac->Tag()) == 0) {
                out << "null, ";
            }
            else {
                out << "\"" << fac->Tag() << "\", ";
            }
            out << "\"object_count\": " << fac->GetNrows(false, true);
            out << "}," << std::endl;
        }
        out << "}" << std::endl;
    }
}

void JInspector::ToText(std::vector<JObject*> objs, bool as_json, std::ostream& out) {

    if (objs.empty()) {
        out << "(No objects found)" << std::endl;
        return;
    }
    std::set<std::string> objnames;
    for (auto obj: objs) {
        objnames.insert(obj->className());
    }
    if (objnames.size() == 1) {
        out << *(objnames.begin()) << std::endl;
    }
    else {
        out << "{ ";
        for (auto name : objnames) {
            out << name << ",";
        }
        out << "}" << std::endl;
    }
    if (as_json) {
        out << "{" << std::endl;
        for (size_t i = 0; i < objs.size(); ++i) {
            auto obj = objs[i];
	    std::vector<std::pair<std::string,std::string>> summary;
            obj->toStrings(summary);
            out << "  " << i << ":  {";
            for (auto& field : summary) {
                out << "\"" << field.first << "\": \"" << field.second << "\", ";
            }
            out << "}" << std::endl;
        }
        out << "}" << std::endl;
    }
    else {
        JTablePrinter t;
        std::set<std::string> fieldnames_seen;
        std::vector<std::string> fieldnames_in_order;
        for (auto obj : objs) {
	    std::vector<std::pair<std::string,std::string>> summary;
            obj->toStrings(summary);
            for (auto field : summary) {
                if (fieldnames_seen.find(field.first) == fieldnames_seen.end()) {
                    fieldnames_in_order.push_back(field.first);
                    fieldnames_seen.insert(field.first);
                }
            }
        }
        t.AddColumn("Index");
        for (auto fieldname : fieldnames_in_order) {
            t.AddColumn(fieldname);
        }
        for (size_t i = 0; i < objs.size(); ++i) {
            auto obj = objs[i];
            t | i;
            std::map<std::string, std::string> summary_map;
	    std::vector<std::pair<std::string,std::string>> summary;
            obj->toStrings(summary);
            for (auto& field : summary) {
                summary_map[field.first] = field.second;
            }
            for (auto& fieldname : fieldnames_in_order) {
                auto result = summary_map.find(fieldname);
                if (result == summary_map.end()) {
                    t | "(missing)";
                }
                else {
                    t | result->second;
                }
            }
        }
        t.Render(out);
    }
}

void JInspector::ToText(const JObject* obj, bool asJson, std::ostream& out) {
    out << obj->className() << std::endl;
    std::vector<std::pair<std::string,std::string>> summary;
    obj->toStrings(summary);
    if (asJson) {
        out << "[" << std::endl;
        for (auto& field : summary) {
            out << "  { \"name\": \"" << field.first << "\", ";
            out << "\"value\": \"" << field.second << "\" }" << std::endl;
            // out << "\"description\": \"" << field.description << "\" }" << std::endl;
        }
        out << "]" << std::endl;
    }
    else {
        JTablePrinter t;
        t.AddColumn("Field name");
        t.AddColumn("Value");
        for (auto& field : summary) {
            t | field.first | field.second;
        }
        t.Render(out);
    }
}

void JInspector::PrintFactoryParents(std::string factory_key) {

    bool callgraph_on = m_event->GetCallStackRecordingStatus();
    if (!callgraph_on) {
        m_out << "(Error: Callgraph recording is currently disabled)" << std::endl;
    }
    BuildIndices();  // So that we can retrieve the integer index given the factory name/tag pair
    auto result = m_factory_index.find(factory_key);
    if (result == m_factory_index.end()) {
        m_out << "(Error: Invalid factory name or index)\n";
        return;
    }
    auto fac = result->second.second;
    auto obj_name = fac->GetDataClassName();
    std::string fac_tag = fac->Tag();
    if (fac_tag.empty()) {
        m_out << obj_name << std::endl;
    }
    else {
        m_out << obj_name << ":" << fac_tag << std::endl;
    }

    if (m_format != Format::Json) {
        JTablePrinter t;
        t.AddColumn("Index", JTablePrinter::Justify::Right);
        t.AddColumn("Object name");
        t.AddColumn("Tag");
        auto callgraph = m_event->GetCallStack();
        bool found_anything = false;
        for (const auto& node : callgraph) {
            if ((node.caller_name == obj_name) && (node.caller_tag == fac_tag)) {
                found_anything = true;
                auto idx = m_factory_index[MakeFactoryKey(node.callee_name, node.callee_tag)].first;
		std::string tag = node.callee_tag;
                if (tag.empty()) tag = "(no tag)";
                t | idx | node.callee_name | tag;
            }
        }
        if (!found_anything) {
            m_out << "(No parents found)" << std::endl;
            return;
        }
        t.Render(m_out);
    }
    else {
        auto callgraph = m_event->GetCallStack();
        bool found_anything = false;
        m_out << "[" << std::endl;
        for (const auto& node : callgraph) {
            if ((node.caller_name == obj_name) && (node.caller_tag == fac_tag)) {
                found_anything = true;
                auto idx = m_factory_index[MakeFactoryKey(node.callee_name, node.callee_tag)].first;
		std::string tag = node.callee_tag;
                m_out << "  { \"index\": " << idx << ", \"object_name\": \"" << node.callee_name << "\", \"tag\": ";
                if (tag.empty()) {
                    m_out << "null }," << std::endl;
                }
                else {
                    m_out << "\"" << tag << "\" }," << std::endl;
                }
            }
        }
        m_out << "]" << std::endl;
        if (!found_anything) {
            m_out << "(No ancestors found)" << std::endl;
            return;
        }
    }
}

void JInspector::PrintObjectParents(std::string factory_key, int object_idx) {

    BuildIndices();  // So that we can retrieve the integer index given the factory name/tag pair
    auto result = m_factory_index.find(factory_key);
    if (result == m_factory_index.end()) {
        m_out << "(Error: Invalid factory name or index)\n";
        return;
    }
    auto fac = const_cast<JFactory_base*>(result->second.second);
    auto objs = fac->Get();
    if ((size_t) object_idx >= objs.size()) {
        m_out << "(Error: Object index out of range)" << std::endl;
        return;
    }
    auto obj = (JObject*) objs[object_idx];
    m_out << obj->className() << std::endl;
    std::vector<const JObject*> parents;
    obj->GetT<JObject>(parents);
    if (parents.empty()) {
        m_out << "(No parents found)" << std::endl;
        return;
    }

    if (m_format == Format::Table) {
        JTablePrinter t;
        t.AddColumn("Object name");
        t.AddColumn("Tag");
        t.AddColumn("Factory Index", JTablePrinter::Justify::Right);
        t.AddColumn("Object Index", JTablePrinter::Justify::Right);
        t.AddColumn("Object contents");
        for (auto parent : parents) {
            JFactory_base* fac;
            size_t fac_idx;
            size_t obj_idx;
            std::tie(fac, fac_idx, obj_idx) = LocateObject(*m_event, parent);
            if (fac == nullptr) {
                m_out << "(Error: Unable to find factory containing object with classname '" << obj->className() << "')" << std::endl;
                continue;
            }
    	    std::vector<std::pair<std::string,std::string>> summary;
            parent->toStrings(summary);

	    std::string tag = fac->Tag();
            if (tag.empty()) tag = "(no tag)";
            std::ostringstream objval;
            objval << "{";
            for (auto& field : summary) {
                objval << field.first << ": " << field.second << ", ";
            }
            objval << "}";

            t | fac->GetDataClassName() | tag | fac_idx | obj_idx | objval.str();
        }
        t.Render(m_out);
    }
    else {
        m_out << "[" << std::endl;
        for (auto ancestor : FindAllAncestors(obj)) {
            JFactory_base* fac;
            size_t fac_idx;
            size_t obj_idx;
            std::tie(fac, fac_idx, obj_idx) = LocateObject(*m_event, ancestor);
            if (fac == nullptr) {
                m_out << "(Error: Unable to find factory containing object with classname '" << obj->className() << "')" << std::endl;
                continue;
            }
	    std::string tag = fac->Tag();
            if (tag.empty()) tag = "(no tag)";

            m_out << "  " << "{ " << std::endl << "    \"object_name\": \"" << fac->GetDataClassName() << "\", ";
            if (tag.empty()) {
                m_out << "\"tag\": null, ";
            }
            else {
                m_out << "\"tag\": \"" << tag << "\", ";
            }
            m_out << "\"fac_index\": " << fac_idx << ", \"obj_index\": " << obj_idx << "," << std::endl;
            m_out << "    \"object_contents\": ";

    	    std::vector<std::pair<std::string,std::string>> summary;
            ancestor->toStrings(summary);
            m_out << "{";
            for (auto& field : summary) {
                m_out << "\"" << field.first << "\": \"" << field.second << "\", ";
            }
            m_out << "}" << std::endl;
            m_out << "  }, " << std::endl;
        }
        m_out << "]" << std::endl;
    }
}

void JInspector::PrintObjectAncestors(std::string factory_key, int object_idx) {

    BuildIndices();  // So that we can retrieve the integer index given the factory name/tag pair
    auto result = m_factory_index.find(factory_key);
    if (result == m_factory_index.end()) {
        m_out << "(Error: Invalid factory name or index)\n";
        return;
    }
    auto fac = const_cast<JFactory_base*>(result->second.second);
    auto objs = fac->Get();
    if ((size_t) object_idx >= objs.size()) {
        m_out << "(Error: Object index out of range)" << std::endl;
        return;
    }
    auto obj = (JObject*) objs[object_idx];
    m_out << obj->className() << std::endl;
    auto ancestors = FindAllAncestors(obj);
    if (ancestors.empty()) {
         m_out << "(No ancestors found)" << std::endl;
         return;
    }

    if (m_format == Format::Table) {
        JTablePrinter t;
        t.AddColumn("Object name");
        t.AddColumn("Tag");
        t.AddColumn("Factory index", JTablePrinter::Justify::Right);
        t.AddColumn("Object index", JTablePrinter::Justify::Right);
        t.AddColumn("Object contents");
        for (auto ancestor : FindAllAncestors(obj)) {
            JFactory_base* fac;
            size_t fac_idx, obj_idx;
            std::tie(fac, fac_idx, obj_idx) = LocateObject(*m_event, ancestor);
            if (fac == nullptr) {
                m_out << "(Error: Unable to find factory containing object with classname '" << obj->className() << "')" << std::endl;
                continue;
            }
    	    std::vector<std::pair<std::string,std::string>> summary;
            ancestor->toStrings(summary);

	    std::string tag = fac->Tag();
            if (tag.empty()) tag = "(no tag)";
            std::ostringstream objval;
            objval << "{";
            for (auto& field : summary) {
                objval << field.first << ": " << field.second << ", ";
            }
            objval << "}";

            t | fac->GetDataClassName() | tag | fac_idx | obj_idx | objval.str();
        }
        t.Render(m_out);
    }
    else {
        m_out << "[" << std::endl;
        for (auto ancestor : FindAllAncestors(obj)) {
            JFactory_base* fac;
            size_t fac_idx, obj_idx;
            std::tie(fac, fac_idx, obj_idx) = LocateObject(*m_event, ancestor);
            if (fac == nullptr) {
                m_out << "(Error: Unable to find factory containing object with classname '" << obj->className() << "')" << std::endl;
                continue;
            }
	    std::string tag = fac->Tag();
            if (tag.empty()) tag = "(no tag)";

            m_out << "  " << "{ " << std::endl << "    \"object_name\": \"" << fac->GetDataClassName() << "\", ";
            if (tag.empty()) {
                m_out << "\"tag\": null, ";
            }
            else {
                m_out << "\"tag\": \"" << tag << "\", ";
            }
            m_out << "\"fac_index\": " << fac_idx << ", \"obj_index\": " << obj_idx << "," << std::endl;
            m_out << "    \"object_contents\": ";

    	    std::vector<std::pair<std::string,std::string>> summary;
            ancestor->toStrings(summary);
            m_out << "{";
            for (auto& field : summary) {
                m_out << "\"" << field.first << "\": \"" << field.second << "\", ";
            }
            m_out << "}" << std::endl;
            m_out << "  }, " << std::endl;
        }
        m_out << "]" << std::endl;
    }
}


void JInspector::Loop() {
    bool stay_in_loop = true;
    m_out << std::endl;
    m_out << "--------------------------------------------------------------------------------------" << std::endl;
    m_out << "Welcome to JANA's interactive inspector! Type `Help` or `h` to see available commands." << std::endl;
    m_out << "--------------------------------------------------------------------------------------" << std::endl;
    PrintEvent();
    while (stay_in_loop) {
        std::string user_input;
        m_out << std::endl << "JANA: ";
        // Obtain a single line
        std::getline(m_in, user_input);
        // Split into tokens
        std::stringstream ss(user_input);
        std::string token;
        ss >> token;
        std::vector<std::string> args;
        std::string arg;
        while (ss >> arg) {
            args.push_back(arg);
        }
        if (token == "PrintEvent" || token == "pe") {
            PrintEvent();
        }
        else if ((token == "PrintFactories" || token == "pf") && args.empty()) {
            PrintFactories(0);
        }
        else if ((token == "PrintFactories" || token == "pf") && args.size() == 1) {
            PrintFactories(std::stoi(args[0]));
        }
        else if ((token == "PrintFactoryDetails" || token == "pfd") && (args.size() == 1)) {
            PrintFactoryDetails(args[0]);
        }
        else if ((token == "PrintObjects" || token == "po") && (args.size() == 1)) {
            PrintObjects(args[0]);
        }
        else if ((token == "PrintObject" || token == "po") && (args.size() == 2)) {
            PrintObject(args[0], std::stoi(args[1]));
        }
        else if ((token == "PrintFactoryParents" || token == "pfp") && (args.size() == 1)) {
            PrintFactoryParents(args[0]);
        }
        else if ((token == "PrintObjectParents" || token == "pop") && (args.size() == 2)) {
            PrintObjectParents(args[0], std::stoi(args[1]));
        }
        else if ((token == "PrintObjectAncestors" || token == "poa") && (args.size() == 2)) {
            PrintObjectAncestors(args[0], std::stoi(args[1]));
        }
        else if (token == "ViewAsTable" || token == "vt") {
            m_format = Format::Table;
            m_out << "(Switching to table view mode)" << std::endl;
        }
        else if (token == "ViewAsJson" || token == "vj") {
            m_format = Format::Json;
            m_out << "(Switching to JSON view mode)" << std::endl;
        }
        else if (token == "Continue" || token == "c") {
            stay_in_loop = false;
        }
        else if (token == "Exit" || token == "x") {
            stay_in_loop = false;
        }
        else if (token == "Help" || token == "h") {
            PrintHelp();
        }
        else if (token == "") {
            // Do nothing
        }
        else {
            m_out << "(Error: Unrecognized command, or wrong argument count)" << std::endl;
            PrintHelp();
        }
    }
}
