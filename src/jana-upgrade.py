#!/usr/bin/env python3

import sys
import re


class Replacement:
    regexes = []

    def add(self, before, after):
        self.regexes.append((before, after))

    def process(self, filename):
        with open(filename, 'r') as f:
            contents = f.read()

        for (before, after) in self.regexes:
            contents = re.sub(before, after, contents)

        with open(filename, 'w') as f:
            f.write(contents)
            # print(contents)


def main():
    r = Replacement()

    r.add(re.compile(r'jana::'), '')

    r.add(re.compile(r'#include <JANA/JEventLoop\.h>'), '#include <JANA/JEvent.h>')
    r.add(re.compile(r'#include "JANA/JEventLoop\.h"'), '#include <JANA/JEvent.h>')

    r.add(re.compile(r'#include <JANA/JFactory.h>'), r'#include <JANA/JFactoryT.h>')

    r.add(re.compile(r'return NOERROR;'), 'return;')

    r.add(re.compile(r'eventLoop'), 'event')

    r.add(re.compile(r'JFactory_base'), 'JFactory')

    r.add(re.compile(r'loop->'), 'event->')

    r.add(re.compile(r"JFactory<([a-zA-Z_0-9]+)>"), r'JFactoryT<\1>')

    r.add(re.compile(r'jerror_t init\(void\);[^\n]+\n'), 'void Init() override;\n')
    r.add(re.compile(r'jerror_t init\(\);[^\n]+\n'), 'void Init() override;\n')

    r.add(re.compile(r'jerror_t brun\(JEventLoop \*[a-zA-Z_]+, int32_t runnumber\);[^\n]+\n'),
          'void BeginRun(const std::shared_ptr<const JEvent>& event) override;\n')

    r.add(re.compile(r'jerror_t evnt\(JEventLoop \*[a-zA-Z_]+, uint64_t [a-zA-Z_]+\);[^\n]+\n'),
          'void Process(const std::shared_ptr<const JEvent>& event) override;\n')

    r.add(re.compile(r'jerror_t erun\(void\);[^\n]+\n'), 'void EndRun() override;\n')
    r.add(re.compile(r'jerror_t fini\(void\);[^\n]+\n'), 'void Finish() override;\n')

    r.add(re.compile(r'jerror_t brun'), r'void BeginRun')
    r.add(re.compile(r'jerror_t evnt'), r'void Process')
    r.add(re.compile(r'jerror_t erun'), r'void EndRun')

    r.add(re.compile(r'jerror_t ([a-zA-Z_0-9]+)::init\(void\)'), r'void \1::Init()')

    r.add(re.compile(r'jerror_t ([a-zA-Z_0-9]+)::brun\(JEventLoop \*[a-zA-Z_]+, int32_t runnumber\)'),
          r'void \1::BeginRun(const std::shared_ptr<const JEvent>& event)'),

    r.add(re.compile(r'jerror_t ([a-zA-Z_0-9]+)::evnt\(JEventLoop \*[a-zA-Z_]+, uint64_t [a-zA-Z_]+\)'),
          r'void \1::Process(const std::shared_ptr<const JEvent>& event)'),

    r.add(re.compile(r'jerror_t ([a-zA-Z_0-9]+)::erun\(void\)'), r'void \1::EndRun()'),

    r.add(re.compile(r'jerror_t ([a-zA-Z_0-9]+)::fini\(void\)'), r'void \1::Finish()'),

    r.add(re.compile(r'using namespace jana;\n'), ''),

    r.add(re.compile(r'#include <JANA/jerror\.h>'), r'#include <JANA/Compatibility/jerror\.h>'),

    r.add(re.compile(r'gPARMS'), 'app'),
    r.add(re.compile(r'JObject::oid'), 'oid'),

    r.add(re.compile(r'_data\.push_back\(([a-zA-Z_]+)\);'), r'Insert(\1);')

    r.add(re.compile(r'// init'), '// Init')
    r.add(re.compile(r'// brun'), '// BeginRun')
    r.add(re.compile(r'// evnt'), '// Process')
    r.add(re.compile(r'// erun'), '// EndRun')
    r.add(re.compile(r'// fini'), '// Finish')

    r.add(re.compile(r'locEventLoop'), 'locEvent')
    r.add(re.compile(r'event->GetCalib'), 'calibration->Get')
    r.add(re.compile(r'locEvent->GetCalib'), 'calibration->Get')   # TODO: Consider reversing this

    r.add(re.compile(r'const char\* Tag\(void\){return \"([a-zA-Z_]+)\";}'), r'SetTag("\1")')

    r.add(re.compile(r'JEventLoop\*'), 'const std::shared_ptr<const JEvent>&')
    r.add(re.compile(r'JEventLoop \*'), 'const std::shared_ptr<const JEvent>& ')

    r.add(re.compile(r'japp->RootWriteLock\(\);'), 'GetLockService(locEvent)->RootWriteLock();')
    r.add(re.compile(r'japp->RootUnLock\(\);'), 'GetLockService(locEvent)->RootUnLock();')

    for filename in sys.argv[1:]:
        print("Porting " + filename)
        r.process(filename)


if __name__ == '__main__':
    main()
