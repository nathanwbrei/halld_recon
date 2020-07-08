
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Jefferson Science Associates LLC Copyright Notice:
# Copyright 251 2014 Jefferson Science Associates LLC All Rights Reserved. Redistribution
# and use in source and binary forms, with or without modification, are permitted as a
# licensed user provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice, this
#    list of conditions and the following disclaimer in the documentation and/or other
#    materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products derived
#    from this software without specific prior written permission.
# This material resulted from work developed under a United States Government Contract.
# The Government retains a paid-up, nonexclusive, irrevocable worldwide license in such
# copyrighted data to reproduce, distribute copies to the public, prepare derivative works,
# perform publicly and display publicly and to permit others to do so.
# THIS SOFTWARE IS PROVIDED BY JEFFERSON SCIENCE ASSOCIATES LLC "AS IS" AND ANY EXPRESS
# OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
# JEFFERSON SCIENCE ASSOCIATES, LLC OR THE U.S. GOVERNMENT BE LIABLE TO LICENSEE OR ANY
# THIRD PARTES FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
# OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#[=======================================================================[.rst:
FindJANA
--------

Finds the JANA library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``EVIO::evio``
  The EVIO library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``EVIO_FOUND``
  True if the system has the JANA library.
``EVIO_VERSION``
  The version of the JANA library which was found.
``EVIO_INCLUDE_DIRS``
  Include directories needed to use JANA.
``EVIO_LIBRARIES``
  Libraries needed to link to JANA.

#]=======================================================================]


if (DEFINED EVIOROOT)
    set(EVIO_ROOT_DIR ${EVIOROOT})
    message(STATUS "Using EVIO_ROOT = ${EVIO_ROOT_DIR} (From CMake EVIOROOT variable)")

elseif (DEFINED ENV{EVIOROOT})
    set(EVIO_ROOT_DIR $ENV{EVIOROOT})
    message(STATUS "Using EVIO_ROOT = ${EVIO_ROOT_DIR} (From EVIOROOT environment variable)")

else()
    message(FATAL_ERROR "Missing $EVIOROOT")
endif()

set(EVIO_VERSION 4.4.6)

find_path(EVIO_INCLUDE_DIR
        NAMES "evio.h"
        PATHS ${EVIO_ROOT_DIR}/include
        )

find_library(EVIO_LIB
        NAMES evio
        PATHS ${EVIO_ROOT_DIR}/lib
        )

find_library(EVIOXX_LIB
        NAMES evioxx
        PATHS ${EVIO_ROOT_DIR}/lib
        )

set(EVIO_LIBS ${EVIO_LIB} ${EVIOXX_LIB})

set(EVIO_INCLUDE_DIRS ${EVIO_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(EVIO
        FOUND_VAR EVIO_FOUND
        VERSION_VAR EVIO_VERSION
        REQUIRED_VARS EVIO_ROOT_DIR EVIO_INCLUDE_DIR EVIO_LIBS
        )