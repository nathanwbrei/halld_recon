// Copyright 2020, Jefferson Science Associates, LLC.
// Subject to the terms in the LICENSE file found in the top-level directory.

#ifndef _JInspector_h_
#define _JInspector_h_

#include <string>
#include <map>
#include <vector>
#include <tuple>
#include <mutex>
#include <iostream>

#include <JANA/JParameterManager.h>
#include <JANA/JEventLoop.h>
#include <JANA/JObject.h>
#include <JANA/JFactory_base.h>

using namespace jana;
class JInspector {

public:
    enum class Format {Table=0, Json, Tsv};

private:
    bool m_enabled = true;
    int m_format = (int) Format::Table;
    JEventLoop* m_event;
    bool m_indexes_built = false;
    std::map<std::string, std::pair<int, const JFactory_base*>> m_factory_index;
    std::ostream& m_out = std::cout;
    std::istream& m_in = std::cin;

public:
    explicit JInspector(JEventLoop* event);
    void SetEvent(JEventLoop* event);

    void PrintEvent();
    void PrintFactories(int filter_level);
    void PrintFactoryDetails(std::string factory_key);
    void PrintObjects(std::string factory_key);
    void PrintObject(std::string factory_key, int object_idx);
    void PrintFactoryParents(std::string factory_key);
    void PrintObjectParents(std::string factory_key, int object_idx);
    void PrintObjectAncestors(std::string factory_key, int object_idx);
    void PrintHelp();
    void Loop();

    static void ToText(JEventLoop* event, bool asJson=false, std::ostream& out=std::cout);
    static void ToText(const std::vector<JFactory_base*>& factories, int filter_level, bool asJson=false, std::ostream& out=std::cout);
    static void ToText(JFactory_base* factory, bool asJson=false, std::ostream& out=std::cout);
    static void ToText(std::vector<JObject*> objs, bool as_json, std::ostream& out= std::cout);
    static void ToText(const JObject* obj, bool asJson, std::ostream& out=std::cout);

private:
    void BuildIndices();
    static std::vector<const JObject*> FindAllAncestors(const JObject*);
    static std::tuple<JFactory_base*, size_t, size_t> LocateObject(JEventLoop&, const JObject* obj);
};
/*
inline std::ostream& operator<<(std::ostream os&, JInspector::Format value) {
    switch (value) {
        case JInspector::Format::Table: os << "table"; break;
        case JInspector::Format::Json: os << "json"; break;
        case JInspector::Format::Tsv: os << "tsv"; break;
        default: os << "unknown"; break;
    }
    return os;
}

template <>
inline JInspector::Format JParameterManager::parse(const std::string& value) {
    auto lowered = JParameterManager::to_lower(value);
    if (lowered == "table") return JInspector::Format::Table;
    if (lowered == "json") return JInspector::Format::Json;
    if (lowered == "tsv") return JInspector::Format::Tsv;
    else return JInspector::Format::Table;
}
*/

#endif // _JInspector_h_
