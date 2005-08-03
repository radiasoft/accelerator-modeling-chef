TEMPLATE = app
TARGET   = chef

INCLUDEPATH += ../include
INCLUDEPATH += $(INSTALLDIR)/include/basic_toolkit
INCLUDEPATH += $(INSTALLDIR)/include/gms
INCLUDEPATH += $(INSTALLDIR)/include/bmlfactory
INCLUDEPATH += $(INSTALLDIR)/include/mxyzptlk
INCLUDEPATH += $(INSTALLDIR)/include/beamline
INCLUDEPATH += $(INSTALLDIR)/include/integrator
INCLUDEPATH += $(INSTALLDIR)/include/physics_toolkit
INCLUDEPATH += $(INSTALLDIR)/include/widget_toolkit

INCLUDEPATH	+= /usr/local/include/boost-1_32

QMAKE_EXT_MOC = .cc

DESTDIR = .

CONFIG += qt
CONFIG += thread
CONFIG += opengl
CONFIG += x11
CONFIG += rtti
CONFIG += stl

SOURCES = chef.cc
HEADERS = 
LIBS += -L../lib -L$(INSTALLDIR)/lib -L/usr/local/lib -L/usr/lib
LIBS += -lchef -lchefplot -lqwt -lqt-mt
LIBS += -lbeamline -lbmlfactory -lintegrator -lmxyzptlk -lphysics_toolkit -lbasic_toolkit
LIBS += -lglib-2.0 -lglut -lGLU -lGL
QMAKE_LIBDIR += $(INSTALLDIR)/lib
QMAKE_LIBDIR += /usr/local/ap/lib
