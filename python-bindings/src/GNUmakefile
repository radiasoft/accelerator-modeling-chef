.SUFFIXES:
.SUFFIXES: .cpp .o .so .h

#CXXFLAGS   = -g -fPIC -DPIC -D_REENTRANT -mthreads
CXXFLAGS   = -g -fPIC -DPIC -D_REENTRANT 
LDFLAGS    = -Wl,--export-dynamic -mthreads -shared

# setup for wally.fnal.gov

#FNALROOT     =  /home/ostiguy/FNALTEST
#BOOST_INC    =  /usr/local/ap/include/boost-1_32
#BOOST_LIB    =  /usr/local/ap/lib
#BOOST_VER    =  1_32
#QTDIR        =  /usr/local/ap/qt3
#QT_INC       =  /usr/local/ap/qt3/include
#PYTHON_DIR   =  /usr/local/ap
#PYTHON_VER   =  2.4
#INSTALL_DIR  = /home/ostiguy/FNALTEST
#GLIB_DIR     = /usr/local/ap
#GLIB_INC_DIR           =  $(GLIB_DIR)/include/glib-2.0
#GLIBCONFIG_INC_DIR     =  $(GLIB_DIR)/lib/glib-2.0/include

# setup for ironchef.fnal.gov

#CHEF_INSTALL_TOP_DIR   =  /home/ostiguy/chef-tmp-2
#FNALROOT               =  /home/ostiguy/chef-tmp-2
#BOOST_INC              =  /usr/local/include/boost-1_32
#BOOST_LIB              =  /usr/local/lib
#BOOST_VER              =  1_32
#QTDIR                  =  /usr/local/qt
#QT_INC                 =  /usr/local/qt/include
#PYTHON_DIR             =  /usr/local
#PYTHON_VER             =  2.3
#INSTALL_DIR            = /home/ostiguy/chef-tmp-2
#GLIB_DIR               = /usr
#GLIB_INC_DIR           =  $(GLIB_DIR)/include/glib-2.0
#GLIBCONFIG_INC_DIR     =  $(GLIB_DIR)/lib/glib-2.0/include


# private setup (heaviside linux)
CHEF_INSTALL_TOP_DIR   =  $(INSTALLDIR)
FNALROOT               =  $(INSTALLDIR)
BOOST_INC              =  /usr/local/include/boost-1_32
BOOST_LIB              =  /usr/local/lib
BOOST_VER              =  1_32
QTDIR                  =  /usr/local/qt
QT_INC                 =  /usr/local/qt/include
PYTHON_DIR             =  /usr/local
PYTHON_VER             =  2.3
INSTALL_DIR            = $(INSTALLDIR)
GLIB_DIR               = /usr
GLIB_INC_DIR           =  $(GLIB_DIR)/include/glib-2.0
GLIBCONFIG_INC_DIR     =  $(GLIB_DIR)/lib/glib-2.0/include


LIBS = -lbmlfactory -lbeamline -lphysics_toolkit -lmxyzptlk -lbasic_toolkit -lglib-2.0

# WIN32 (msys)

INCS_LOCAL  =  -I. -I../include \
               -I../../chef-mdi/src \
               -I../../chef-mdi/include \
               -I../../chef-mdi/src/ui \
               -I../../chef-mdi/src/moc \
               -I$(QT_INC) \
               -I../../widget_toolkit/chefplot/src/ui \
               -I../../widget_toolkit/chefplot/include

#INCS_PYTHON =  -I$(PYTHON_DIR)/include/python$(PYTHON_VER)
#INCS_PYTHON  =  -I$(PYTHON_DIR)/include


INCS_FNAL      = -I$(FNALROOT)/include/gms \
                 -I$(FNALROOT)/include/bmlfactory \
                 -I$(FNALROOT)/include/beamline \
                 -I$(FNALROOT)/include/physics_toolkit \
                 -I$(FNALROOT)/include/mxyzptlk \
                 -I$(FNALROOT)/include/basic_toolkit 

INCS_BOOST   =  -I$(BOOST_INC) 

INCS = $(INCS_LOCAL) $(INCS_PYTHON) $(INCS_BOOST) $(INCS_FNAL) -I/local/include -I$(GLIB_INC_DIR) -I$(GLIBCONFIG_INC_DIR)

OBJS_BEAMLINE = \
       py-beamline-module.o \
       py-aperture.o \
       py-kick.o \
       py-bblens.o\
       py-bmlnelmnt.o \
       py-drift.o \
       py-sbend.o \
       py-cf_sbend.o \
       py-cf_rbend.o \
       py-lambertson.o \
       py-mover.o \
       py-marker.o \
       py-monitor.o \
       py-quadrupole.o \
       py-octupole.o \
       py-sextupole.o \
       py-septum.o \
       py-sector.o \
       py-slot.o \
       py-rbend.o \
       py-rfcavity.o \
       py-thinpoles.o \
       py-decapole.o \
       py-particle.o \
       py-beamlineiterator.o \
       py-lattfunc.o \

OBJS_MXYZPTLK = \
       py-mxyzptlk-module.o \
       py-jet.o \
       py-jetvector.o \
       py-mapping.o \
       py-coord.o 

OBJS_FACTORY = \
       py-bmlfactory-module.o \
       bmlfactorykludge.o

OBJS_BASICTK = \
       py-basictoolkit-module.o \
       py-barnacle.o \
       py-vector.o


OBJS_PHYSICSTK = \
       py-physicstoolkit-module.o \
       py-lattfuncsage.o\
       py-beamlinecontext.o

OBJS_CHEFPLOT = \
       py-chefplot-module.o \
       py-chefplotmain.o \
       py-chefcurve.o \
       py-chefplotdata.o

OBJS_CHEF = \
       py-chef-module.o \



all: static 
#all: static shared

shared: beamline.so mxyzptlk.so bmlfactory.so basic_toolkit.so physics_toolkit.so chef.so chefplot.so

static: libpybeamline.a libpymxyzptlk.a libpybmlfactory.a libpybasic_toolkit.a libpyphysics_toolkit.a libpychef.a libpychefplot.a

beamline.so: $(OBJS_BEAMLINE)
	g++  $(LDFLAGS) -o ../lib/beamline.so $(OBJS_BEAMLINE)  -L$(FNALROOT)/lib -lphysics_toolkit -lbeamline -lmxyzptlk -lbasic_toolkit -L$(BOOST_LIB) -lboost_python-gcc-mt-$(BOOST_VER) -Wl,--rpath=$(FNALROOT)/lib -Wl,--rpath=$(BOOST_LIB) 

basic_toolkit.so: $(OBJS_BASICTK)
	g++ $(LDFLAGS)  -o ../lib/basic_toolkit.so $(OBJS_BASICTK) -L$(FNALROOT)/lib  -lbasic_toolkit  -L$(BOOST_LIB) -lbasic_toolkit -lboost_python-gcc-mt-$(BOOST_VER) -Wl,--rpath=$(FNALROOT)/lib -Wl,--rpath=$(BOOST_LIB)

physics_toolkit.so: $(OBJS_PHYSICSTK)
	g++  $(LDFLAGS) -o ../lib/physics_toolkit.so $(OBJS_PHYSICSTK) -L$(FNALROOT)/lib -lphysics_toolkit -lbeamline -lmxyzptlk -lbasic_toolkit  -L$(BOOST_LIB) -lboost_python-gcc-mt-$(BOOST_VER) -Wl,--rpath=$(FNALROOT)/lib -Wl,--rpath=$(BOOST_LIB) 

mxyzptlk.so: $(OBJS_MXYZPTLK)
	g++  $(LDFLAGS) -o ../lib/mxyzptlk.so $(OBJS_MXYZPTLK) -L$(FNALROOT)/lib -lmxyzptlk -lbasic_toolkit  -L$(BOOST_LIB) -lboost_python-gcc-mt-$(BOOST_VER) -Wl,--rpath=$(FNALROOT)/lib -Wl,--rpath=$(BOOST_LIB)

bmlfactory.so: $(OBJS_FACTORY)
	g++ $(LDFLAGS)  -o ../lib/bmlfactory.so $(OBJS_FACTORY) -L$(FNALROOT)/lib -lbmlfactory -lbeamline -lmxyzptlk -lbasic_toolkit   -lglib-2.0 -L$(BOOST_LIB) -lboost_python-gcc-mt-$(BOOST_VER) -Wl,--rpath=$(FNALROOT)/lib -Wl,--rpath=$(BOOST_LIB) 

chefplot.so: $(OBJS_CHEFPLOT)
	g++ $(LDFLAGS)  -o ../lib/chefplot.so $(OBJS_CHEFPLOT) -L$(FNALROOT)/lib -lwidget_toolkit -L$(QTDIR)/lib -lqt-mt -L$(FNALROOT)/lib -lwidget_toolkit -lbmlfactory -lphysics_toolkit -lbeamline -lmxyzptlk -lbasic_toolkit  -L$(BOOST_LIB) -lboost_python-gcc-mt-$(BOOST_VER) -Wl,--rpath=$(FNALROOT)/lib -Wl,--rpath=$(BOOST_LIB) -Wl,--rpath=$(QTDIR)/lib

chef.so: $(OBJS_CHEF)
	g++ $(LDFLAGS)  -o ../lib/chef.so $(OBJS_CHEF) -L$(FNALROOT)/lib -lwidget_toolkit -L$(QTDIR)/lib -lqt-mt -L$(FNALROOT)/lib -lbmlfactory -lphysics_toolkit -lbeamline -lmxyzptlk -lbasic_toolkit  -L$(BOOST_LIB) -lboost_python-gcc-mt-$(BOOST_VER) -Wl,--rpath=$(FNALROOT)/lib -Wl,--rpath=$(BOOST_LIB) -Wl,--rpath=$(QTDIR)/lib


libpybeamline.a: $(OBJS_BEAMLINE)
	ar -rv ../lib/libpybeamline.a $(OBJS_BEAMLINE)

libpybasic_toolkit.a: $(OBJS_BASICTK)
	ar -rv ../lib/libpybasic_toolkit.a $(OBJS_BASICTK)

libpyphysics_toolkit.a: $(OBJS_PHYSICSTK)
	ar -rv ../lib/libpyphysics_toolkit.a $(OBJS_PHYSICSTK)

libpymxyzptlk.a: $(OBJS_MXYZPTLK)
	ar -rv ../lib/libpymxyzptlk.a $(OBJS_MXYZPTLK)

libpybmlfactory.a: $(OBJS_FACTORY)
	ar -rv ../lib/libpybmlfactory.a $(OBJS_FACTORY)

libpychefplot.a: $(OBJS_CHEFPLOT)
	ar -rv ../lib/libpychefplot.a $(OBJS_CHEFPLOT)

libpychef.a: $(OBJS_CHEF)
	ar -rv ../lib/libpychef.a $(OBJS_CHEF)


.cpp.o: 
	g++ $(INCS) -I$(PYTHON_DIR)/include/python$(PYTHON_VER) $(CXXFLAGS) -c $<

.cc.o: 
	g++  $(INCS) -I$(PYTHON_DIR)/include/python$(PYTHON_VER) $(CXXFLAGS) -c $<

unixtime.o: unixtime.cc 
	g++   -c unixtime.cc 

structest.so: structest.cc
	g++   $(INCS_BOOST) $(INCS_PYTHON) -Wl,--export-dynamic -pthread -shared -o structest.so structest.cc  -L$(PYTHON_DIR)/lib/python$(PYTHON_VER)/config -lpython$(PYTHON_VER)  $(BOOST_LIB)/libboost_python-gcc-$(BOOST_VER).a  -lpthread -Wl,-rpath,/usr/local/lib

clean:			
	\rm ../lib/*.so ../lib/*.a *.o
install: ../lib/libpybasic_toolkit.a ../lib/libpybeamline.a ../lib/libpybmlfactory.a ../lib/libpymxyzptlk.a ../lib/libpyphysics_toolkit.a  ../lib/libpychef.a
	install  ../lib/libpybasic_toolkit.a ../lib/libpybeamline.a ../lib/libpybmlfactory.a ../lib/libpymxyzptlk.a ../lib/libpyphysics_toolkit.a ../lib/libpychef.a ../lib/libpychefplot.a $(INSTALL_DIR)/lib


tevtest-dynamic: tevtest.o
	g++ -o tevtest-dynamic tevtest.o  ./libpychef.a ./libpybmlfactory.a ./libpyphysics_toolkit.a ./libpybeamline.a ./libpybasic_toolkit.a -L$(FNALROOT)/lib -lbmlfactory -lphysics_toolkit -lbeamline -lmxyzptlk -lbasic_toolkit  -lglib-2.0 -Wl,--rpath=$(FNALROOT)/lib 

tevtest-static:  tevtest.o
	g++ -o tevtest-static tevtest.o  -L$(FNALROOT)/lib -lbmlfactory -lphysics_toolkit -lbeamline -lmxyzptlk -lbasic_toolkit  -lglib-2.0 -Wl,--rpath=$(FNALROOT)/lib 



