TEMPLATE = lib

INCLUDEPATH += ../include

INCLUDEPATH += /home/michelotti/projects/CURRENT/first_order_jl/installed/include/basic_toolkit
INCLUDEPATH += /home/michelotti/projects/CURRENT/first_order_jl/installed/include/bmlfactory       
INCLUDEPATH += /home/michelotti/projects/CURRENT/first_order_jl/installed/include/mxyzptlk
INCLUDEPATH += /home/michelotti/projects/CURRENT/first_order_jl/installed/include/beamline
INCLUDEPATH += /home/michelotti/projects/CURRENT/first_order_jl/installed/include/integrator
INCLUDEPATH += /home/michelotti/projects/CURRENT/first_order_jl/installed/include/physics_toolkit
INCLUDEPATH += /home/michelotti/projects/CURRENT/first_order_jl/installed/include/widget_toolkit

QMAKE_EXT_MOC = .cc

DESTDIR = /home/michelotti/projects/CURRENT/first_order_jl/installed/lib

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
          PointEdit.cc \
          QtMonitor.cc \
          QueryDialog.cc \
          RayTrace.cc \
          SiteViewer.cc \
          Tracker.cc \
          TrbWidget.cc

HEADERS = ../include/CHEF.h \
          ../include/BeamlineBrowser.h \
          ../include/ExpressionBrowser.h \
          ../include/LattFncPlt.h \
          ../include/ETFncPlt.h \
          ../include/DspnFncPlt.h \
          ../include/MomentsFncPlt.h \
          ../include/PointEdit.h \
          ../include/QtMonitor.h \
          ../include/QueryDialog.h \
          ../include/RayTrace.h \
          ../include/SiteViewer.h \
          ../include/Tracker.h \
          ../include/TrbWidget.h
