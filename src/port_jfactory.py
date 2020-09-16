#!/usr/bin/env python3

import sys
import re


print("BAH!")
regexes = [

    (re.compile(r'jana::'), ''),
    (re.compile(r'\#include <JANA/JEventLoop.h>'), ''),
    (re.compile(r'return NOERROR;'), 'return;'),

    (re.compile(r'eventLoop'), 'event'),
    (re.compile(r'loop'), 'event'),
    (re.compile(r'eventnumber'), 'event->GetEventNumber()'),

    (re.compile(r"JFactory<([a-zA-Z_0-9]+)>"), r'JFactoryT<\1>'),

    (re.compile(r'#include <JANA/JFactory.h>'), r'#include <JANA/JFactoryT.h>'),

    (re.compile(r'jerror_t init\(void\);'), r'void Init() override;'),

    (re.compile(r'_data\.push_back\(([a-zA-Z_]+)\);'), r'Insert(\1);'),

    (re.compile(r'jerror_t brun\(JEventLoop \*[a-zA-Z]+, int32_t runnumber\);'),
     r'void BeginRun(const std::shared_ptr<const JEvent>& event) override;'),

    (re.compile(r'jerror_t evnt\(JEventLoop \*[a-zA-Z]+, uint64_t eventnumber\);'),
     r'void Process(const std::shared_ptr<const JEvent>& aEvent) override;'),

    (re.compile(
        r'jerror_t erun\(void\);'),
     r'void EndRun() override;'),

    (re.compile(r'jerror_t fini\(void\);'),
     r'void Finish();'),

    (re.compile(r'jerror_t ([a-zA-Z_0-9]+)::init\(void\)'),
     r'void \1::Init()'),

    (re.compile(r'jerror_t ([a-zA-Z_0-9]+)::brun\(JEventLoop \*eventLoop, int32_t runnumber\)'),
     r'void \1::BeginRun(const std::shared_ptr<const JEvent>& event)'),

    (re.compile(r'jerror_t ([a-zA-Z_0-9]+)::evnt\(JEventLoop \*loop, uint64_t eventnumber\)'),
     r'void \1::Process(const std::shared_ptr<const JEvent>& event)'),

    (re.compile(r'jerror_t ([a-zA-Z_0-9]+)::erun\(void\)'),
     r'void \1::EndRun()'),

    (re.compile(r'jerror_t ([a-zA-Z_0-9]+)::fini\(void\)'),
     r'void \1::Finish()'),


    (re.compile(r'using namespace jana;'), ''),

    (re.compile(r'#include <JANA/jerror\.h>\n'), ''),

    (re.compile(r'jout((?:(?! endl).)*) endl;'),
     r'jout\1 jendl;'),

    (re.compile(r'jout((?:(?!>endl).)*)>endl;'),
     r'jout\1>jendl;')
]


def main():
    filename = sys.argv[1]
    with open(filename, 'r') as f:
        contents = f.read()

    for (before, after) in regexes:
        contents = re.sub(before, after, contents)

    with open(filename, 'w') as f:
        f.write(contents)
        print(contents)


if __name__ == '__main__':
    main()
