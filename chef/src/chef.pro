TEMPLATE = lib

INCLUDEPATH += ../include

INCLUDEPATH += /home/michelotti/software/test_installed/include/basic_toolkit
INCLUDEPATH += /home/michelotti/software/test_installed/include/bmlfactory       
INCLUDEPATH += /home/michelotti/software/test_installed/include/mxyzptlk
INCLUDEPATH += /home/michelotti/software/test_installed/include/beamline
INCLUDEPATH += /home/michelotti/software/test_installed/include/integrator
INCLUDEPATH += /home/michelotti/software/test_installed/include/physics_toolkit
INCLUDEPATH += /home/michelotti/software/test_installed/include/widget_toolkit/chefplot
INCLUDEPATH += /home/michelotti/software/test_installed/include/widget_toolkit/devices
INCLUDEPATH += /home/michelotti/software/test_installed/include/widget_toolkit/interpreter
INCLUDEPATH += /usr/local/ap/include/boost-1_32

QMAKE_EXT_MOC = .cc

DESTDIR = /home/michelotti/software/test_installed/lib

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
          ExpressionBrowser.cc \
          LattFncPlt.cc \
          ETFncPlt.cc \
          DspnFncPlt.cc \
          MomentsFncPlt.cc \
          LBFncPlt.cc \
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
          ../include/ExpressionBrowser.h \
          ../include/LattFncPlt.h \
          ../include/ETFncPlt.h \
          ../include/DspnFncPlt.h \
          ../include/MomentsFncPlt.h \
          ../include/LBFncPlt.h \
          ../include/PointEdit.h \
          ../include/QtMonitor.h \
          ../include/QueryDialog.h \
          ../include/RayTrace.h \
          ../include/SiteViewer.h \
          ../include/Tracker.h \
          ../include/TrbWidget.h \
          ../include/InitCondDialogLF.h \
          ../include/initconddialoglfbase.h
