TEMPLATE = app
TARGET   = chef

INCLUDEPATH += ../include
INCLUDEPATH += /home/michelotti/projects/CURRENT/first_order_jl/installed/include/basic_toolkit
INCLUDEPATH += /home/michelotti/projects/CURRENT/first_order_jl/installed/include/bmlfactory       
INCLUDEPATH += /home/michelotti/projects/CURRENT/first_order_jl/installed/include/mxyzptlk
INCLUDEPATH += /home/michelotti/projects/CURRENT/first_order_jl/installed/include/beamline
INCLUDEPATH += /home/michelotti/projects/CURRENT/first_order_jl/installed/include/integrator
INCLUDEPATH += /home/michelotti/projects/CURRENT/first_order_jl/installed/include/physics_toolkit
INCLUDEPATH += /home/michelotti/projects/CURRENT/first_order_jl/installed/include/widget_toolkit

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
QMAKE_LIBDIR += /home/michelotti/projects/CURRENT/first_order_jl/installed/lib
QMAKE_LIBDIR += /usr/local/ap/lib
