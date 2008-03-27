#---------------------------------------------------------------------
# Makefile.am for Chef ILCLinac tests. 
# Authors: lebrun@fnal.gov based on work done by  amundson@fnal.gov
#---------------------------------------------------------------------

# This is machine/platform specific and has to be fixed... 
MYBOOSTDIR="/local/lebrun/Boost/installed"
BOOST_INC="-I/local/lebrun/Boost/installed/include"
GLIB_INC="-I/usr/lib/glib-2.0/include"

check_PROGRAMS=ILCLinacTest1 ILCLinacTest2 ILCLinacTest3 ILCLinacTest4

ILCLinacTest1_SOURCES=ILCLinacTest1.cc
AM_CPPFLAGS   =  -I$(top_srcdir)/tests/ILCLinac -I$(top_srcdir)/include/beamline \
  -I$(top_srcdir)/include/basic_toolkit \
  -I$(top_srcdir)/include/parsers/xsif  \
 -I$(top_srcdir)/include/parsers/xsif/tree-container \
  -I$(top_srcdir)/include -I$(top_srcdir)/parsers/xsif/src $(BOOST_INC) \
  $(GLIB_INC) $(GLIBCONFIG_INC)

ILCLinacTest1_LDADD= ../../physics_toolkit/src/libphysics_toolkit.la \
    ../../basic_toolkit/src/libbasic_toolkit.la \
    ../../beamline/src/libbeamline.la \
    ../../bmlfactory/src/libbmlfactory.la \
    -lglib-2.0 \
    ../../mxyzptlk/src/libmxyzptlk.la \
    ../../basic_toolkit/src/libbasic_toolkit.la \
    ../../parsers/xsif/src/libxsifparser.la \
    -lboost_regex-gcc -lboost_test_exec_monitor  -lboost_unit_test_framework 

ILCLinacTest2_SOURCES=ILCLinacTest2.cc PrepareILCLinac.cc MLBPM.cc MLBPMs.cc \
 SteerUtil.cc SteerAddUtil.cc myPositronBunch.cc ILCMisalignmentSets.cc

ILCLinacTest2_LDADD= ../../physics_toolkit/src/libphysics_toolkit.la \
    ../../basic_toolkit/src/libbasic_toolkit.la \
    ../../beamline/src/libbeamline.la \
    ../../bmlfactory/src/libbmlfactory.la \
    -lglib-2.0 \
    ../../mxyzptlk/src/libmxyzptlk.la \
    ../../basic_toolkit/src/libbasic_toolkit.la \
    ../../parsers/xsif/src/libxsifparser.la \
    -lboost_regex-gcc -lboost_test_exec_monitor  -lboost_unit_test_framework 

ILCLinacTest3_SOURCES=ILCLinacTest3.cc PrepareILCLinac.cc MLBPM.cc MLBPMs.cc \
 SteerUtil.cc SteerAddUtil.cc myPositronBunch.cc ILCMisalignmentSets.cc

ILCLinacTest3_LDADD= ../../physics_toolkit/src/libphysics_toolkit.la \
    ../../basic_toolkit/src/libbasic_toolkit.la \
    ../../beamline/src/libbeamline.la \
    ../../bmlfactory/src/libbmlfactory.la \
    -lglib-2.0 \
    ../../mxyzptlk/src/libmxyzptlk.la \
    ../../basic_toolkit/src/libbasic_toolkit.la \
    ../../parsers/xsif/src/libxsifparser.la \
    -lboost_regex-gcc -lboost_test_exec_monitor  -lboost_unit_test_framework 

ILCLinacTest4_SOURCES=ILCLinacTest4.cc PrepareILCLinac.cc MLBPM.cc MLBPMs.cc \
 SteerUtil.cc SteerAddUtil.cc myPositronBunch.cc ILCMisalignmentSets.cc \
 SteeringResponseMatrix.cc SteeringRespMatrixTwo.cc \
 SteerOneToOne.cc SteerDispersion.cc PitchCryoModules.cc \
 SteerWeightedDispersion.cc SteerAnalysis.cc SVDFitPL.cc 


ILCLinacTest4_LDADD= ../../physics_toolkit/src/libphysics_toolkit.la \
    ../../basic_toolkit/src/libbasic_toolkit.la \
    ../../beamline/src/libbeamline.la \
    ../../bmlfactory/src/libbmlfactory.la \
    -lglib-2.0 \
    ../../mxyzptlk/src/libmxyzptlk.la \
    ../../basic_toolkit/src/libbasic_toolkit.la \
    ../../parsers/xsif/src/libxsifparser.la \
    -lboost_regex-gcc -lboost_test_exec_monitor  -lboost_unit_test_framework 
