TEMPLATE = app
TARGET   = chef

INCLUDEPATH += ../include
INCLUDEPATH += /home/michelotti/software/test_installed/include/basic_toolkit
INCLUDEPATH += /home/michelotti/software/test_installed/include/bmlfactory       
INCLUDEPATH += /home/michelotti/software/test_installed/include/mxyzptlk
INCLUDEPATH += /home/michelotti/software/test_installed/include/beamline
INCLUDEPATH += /home/michelotti/software/test_installed/include/integrator
INCLUDEPATH += /home/michelotti/software/test_installed/include/physics_toolkit
INCLUDEPATH += /home/michelotti/software/test_installed/include/widget_toolkit

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
LIBS += -lchef -lwidget_toolkit -lqwt -lqt-mt
LIBS += -lbeamline -lbmlfactory -lintegrator -lmxyzptlk -lphysics_toolkit -lbasic_toolkit
LIBS += -lglib -lglut -lGLU -lGL
QMAKE_LIBDIR += /home/michelotti/software/test_installed/lib
QMAKE_LIBDIR += /usr/local/ap/lib
