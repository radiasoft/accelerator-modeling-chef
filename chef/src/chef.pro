TEMPLATE = lib

INCLUDEPATH += ../include

INCLUDEPATH += $(INSTALLDIR)/include/basic_toolkit
INCLUDEPATH += $(INSTALLDIR)/include/gms
INCLUDEPATH += $(INSTALLDIR)/include/bmlfactory       
INCLUDEPATH += $(INSTALLDIR)/include/mxyzptlk
INCLUDEPATH += $(INSTALLDIR)/include/beamline
INCLUDEPATH += $(INSTALLDIR)/include/integrator
INCLUDEPATH += $(INSTALLDIR)/include/physics_toolkit
INCLUDEPATH += $(INSTALLDIR)/include/widget_toolkit/devices
INCLUDEPATH += $(INSTALLDIR)/include/widget_toolkit/interpreter
INCLUDEPATH += ../../widget_toolkit/chefplot/include
INCLUDEPATH += ../../widget_toolkit/chefplot/src/ui
INCLUDEPATH += $(BOOST_INC_DIR)
INCLUDEPATH += $(GLIB_INC_DIR)
INCLUDEPATH += $(GLIBCONFIG_INC_DIR)


QMAKE_EXT_MOC = .cc

DESTDIR = $(INSTALLDIR)/lib

CONFIG += debug
CONFIG += qt
CONFIG += thread
CONFIG += opengl
CONFIG += x11
CONFIG += staticlib
CONFIG += rtti
CONFIG += stl

SOURCES = CHEF.builders.cc \
          CHEF.cc \
          BeamlineBrowser.cc \
          DistributionWidget.cc \
          DspnFncData.cc \
          LBFncData.cc \
          LattFncData.cc \
          MomentsFncData.cc \
          ETFncData.cc \
          EditDialog.cc \
          PointEdit.cc \
          QtMonitor.cc \
          QueryDialog.cc \
          RayTrace.cc \
          SiteViewer.cc \
          Tracker.cc \
          TrbWidget.cc \
          InitCondDialogLF.cc \
          initconddialoglfbase.cc

HEADERS = ../include/CHEF.h \
          ../include/BeamlineBrowser.h \
          ../include/DistributionWidget.h \
          ../include/EditDialog.h \
          ../include/PointEdit.h \
          ../include/QtMonitor.h \
          ../include/QueryDialog.h \
          ../include/RayTrace.h \
          ../include/SiteViewer.h \
          ../include/Tracker.h \
          ../include/TrbWidget.h \
          ../include/InitCondDialogLF.h \
          ../include/initconddialoglfbase.h
