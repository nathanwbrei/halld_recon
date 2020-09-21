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
            print(contents)


def main():
    r = Replacement()

    r.add(re.compile(r'jana::'), '')

    r.add(re.compile(r'#include <JANA/JEventLoop.h>'), '')

    r.add(re.compile(r'return NOERROR;'), 'return;')

    r.add(re.compile(r'eventLoop'), 'event')

    r.add(re.compile(r'loop->'), 'event->')

    r.add(re.compile(r"JFactory<([a-zA-Z_0-9]+)>"), r'JFactoryT<\1>')

    r.add(re.compile(r'#include <JANA/JFactory.h>'), r'#include <JANA/JFactoryT.h>')

    r.add(re.compile(r'jerror_t init\(\);'), r'void Init() override;')

    r.add(re.compile(r'jerror_t init\(void\);'), r'void Init() override;')

    r.add(re.compile(r'_data\.push_back\(([a-zA-Z_]+)\);'), r'Insert(\1);')

    r.add(re.compile(r'jerror_t brun\(JEventLoop \*[a-zA-Z]+, int32_t runnumber\);'),
          r'void BeginRun(const std::shared_ptr<const JEvent>& event) override;')

    r.add(re.compile(r'jerror_t evnt\(JEventLoop \*[a-zA-Z_]+, uint64_t [a-zA-Z_]+\);'),
          r'void Process(const std::shared_ptr<const JEvent>& event) override;')

    r.add(re.compile(r'jerror_t erun\(void\);'), r'void EndRun() override;')

    r.add(re.compile(r'jerror_t fini\(void\);'), r'void Finish();')

    r.add(re.compile(r'jerror_t ([a-zA-Z_0-9]+)::init\(void\)'), r'void \1::Init()')

    r.add(re.compile(r'jerror_t ([a-zA-Z_0-9]+)::brun\(JEventLoop \*[a-zA-Z_]+, int32_t runnumber\)'),
          r'void \1::BeginRun(const std::shared_ptr<const JEvent>& event)'),

    r.add(re.compile(r'jerror_t ([a-zA-Z_0-9]+)::evnt\(JEventLoop \*[a-zA-Z_]+, uint64_t [a-zA-Z_]+\)'),
          r'void \1::Process(const std::shared_ptr<const JEvent>& event)'),

    r.add(re.compile(r'jerror_t ([a-zA-Z_0-9]+)::erun\(void\)'), r'void \1::EndRun()'),

    r.add(re.compile(r'jerror_t ([a-zA-Z_0-9]+)::fini\(void\)'), r'void \1::Finish()'),

    r.add(re.compile(r'using namespace jana;'), ''),

    r.add(re.compile(r'#include <JANA/jerror\.h>\n'), ''),

    r.add(re.compile(r'gPARMS'), 'app'),

    r.add(re.compile(r'jout((?:(?! endl).)*) endl;'), r'jout\1 jendl;'),

    r.add(re.compile(r'jerr((?:(?!>endl).)*)>endl;'), r'jerr\1>jendl;')

    r.add(re.compile(r'// init'), '// Init')
    r.add(re.compile(r'// brun'), '// BeginRun')
    r.add(re.compile(r'// evnt'), '// Process')
    r.add(re.compile(r'// erun'), '// EndRun')
    r.add(re.compile(r'// fini'), '// Finish')
    r.add(re.compile(r'event->GetCalib'), 'calibration->Get')

    r.add(re.compile(r'JEventLoop\*'), 'const std::shared_ptr<const JEvent>&')
    r.add(re.compile(r'JEventLoop \*'), 'const std::shared_ptr<const JEvent>& ')
    r.add(re.compile(r'Tag'), '~~~~Tag~~~~')
    r.add(re.compile(r'dapp->Lock();'), 'root_lock->acquire_write_lock();')
    r.add(re.compile(r'dapp->Unlock();'), 'root_lock->release_lock();')

    filename = sys.argv[1]
    r.process(filename)


if __name__ == '__main__':
    main()
