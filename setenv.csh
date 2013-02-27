#!/bin/tcsh
 
# This file was generated by the script "mk_setenv.csh"
#
# Generation date: Wed Feb 27 10:54:49 EST 2013
# User: marki
# Host: roentgen.jlab.org
# Platform: Linux roentgen.jlab.org 2.6.32-279.11.1.el6.x86_64 #1 SMP Sat Sep 22 07:10:26 EDT 2012 x86_64 x86_64 x86_64 GNU/Linux
# BMS_OSNAME: Linux_RHEL6-x86_64-gcc4.4.6
 
if ( ! $?BMS_OSNAME ) then
   setenv BMS_OSNAME `/group/halld/Software/scripts/osrelease.pl`
endif
 
if ( -e /home/marki/halld/sim-recon-2013-02-25/setenv_${BMS_OSNAME}.csh ) then
    # Try prepending path of cwd used in generating this file
    source /home/marki/halld/sim-recon-2013-02-25/setenv_${BMS_OSNAME}.csh
else if ( -e setenv_${BMS_OSNAME}.csh ) then
    source setenv_${BMS_OSNAME}.csh
endif 
 
