TEMPLATE = lib
LANGUAGE = C++

CONFIG	+= qt warn_on debug thread

LIBS += -lqwt -lbmlfactory -lphysics_toolkit -lbeamline -lmxyzptlk -lbasic_toolkit

INCLUDEPATH += ../include
INCLUDEPATH += /home/michelotti/briefcase/first_order_jl/installed/include/beamline 
INCLUDEPATH += /home/michelotti/briefcase/first_order_jl/installed/include/bmlfactory 
INCLUDEPATH += /home/michelotti/briefcase/first_order_jl/installed/include/mxyzptlk 
INCLUDEPATH += /home/michelotti/briefcase/first_order_jl/installed/include/basic_toolkit 
INCLUDEPATH += /home/michelotti/briefcase/first_order_jl/installed/include/physics_toolkit 

DESTDIR = /home/michelotti/briefcase/first_order_jl/installed/lib

HEADERS	+= ../include/plot.h \
	../include/chefplot.h \
	../include/chefplotdata.h \
	../include/plotproperties.h \
	../include/chefplotzoomer.h \
	../include/chefplotmain.h \
	../include/chefplotmainbase.h \
	../include/chefplotmainbase.ui.h \
	../include/lego.h

SOURCES	+= chefplot.cpp \
        chefplotmain.cpp \
        chefplotmainbase.cpp \
	plot.cpp \
	linearinterpolator.cpp \
	chefplotdata.cpp \
	chefcurve.cpp \
	chefplotzoomer.cpp \
	lego.cpp

QMAKE_LIBDIR_FLAGS += -L/home/michelotti/briefcase/first_order_jl/installed/lib 
QMAKE_LFLAGS_SHAPP += -Wl,-rpath,/usr/local/ap/lib -Wl,-rpath,/home/michelotti/briefcase/first_order_jl/installed/lib 

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}

QMAKE_CXXFLAGS_MT        += -pipe   
QMAKE_CXXFLAGS_MT_DBG    += -pipe
QMAKE_CXXFLAGS_MT_DLL    += -pipe
QMAKE_CXXFLAGS_MT_DLLDBG += -pipe
QMAKE_CXXFLAGS_RELEASE   += -pipe
QMAKE_CXXFLAGS_SHLIB     += -pipe
