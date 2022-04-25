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
    std::map<std::pair<std::string, std::string>, std::pair<int, const JFactory_base*>> m_factory_index;
    std::ostream& m_out = std::cout;
    std::istream& m_in = std::cin;

public:
    explicit JInspector(JEventLoop* event);
    void SetEvent(JEventLoop* event);

    void PrintEvent();
    void PrintFactories(int filter_level);
    void PrintFactory(int factory_idx);
    void PrintObjects(int factory_idx);
    void PrintObject(int factory_idx, int object_idx);
    void PrintFactoryParents(int factory_idx);
    void PrintObjectParents(int factory_idx, int object_idx);
    void PrintObjectAncestors(int factory_idx, int object_idx);
    void PrintHelp();

    uint64_t DoReplLoop(uint64_t current_evt_nr);

    static void ToText(JEventLoop* event, bool asJson=false, std::ostream& out=std::cout);
    static void ToText(const std::vector<JFactory_base*>& factories, int filter_level, bool asJson=false, std::ostream& out=std::cout);
    static void ToText(JFactory_base* factory, bool asJson=false, std::ostream& out=std::cout);
    static void ToText(std::vector<JObject*> objs, bool as_json, std::ostream& out= std::cout);
    static void ToText(const JObject* obj, bool asJson, std::ostream& out=std::cout);

private:
    void BuildIndices();
    static std::vector<const JObject*> FindAllAncestors(const JObject*);
    static std::tuple<JFactory_base*, size_t, size_t> LocateObject(JEventLoop&, const JObject* obj);
    static std::pair<std::string, std::vector<int>> Parse(const std::string&);
};


#endif // _JIntrospection_h_
