lib_LTLIBRARIES		= libpybasic_toolkit.la  libpymxyzptlk.la libpybeamline.la libpybmlfactory.la libpyphysics_toolkit.la

libpybasic_toolkit_la_SOURCES     =   py-basictoolkit-module.cpp py-barnacle.cpp  py-vector.cpp  py-matrix.cpp

libpymxyzptlk_la_SOURCES          =   py-mxyzptlk-module.cpp py-jetenv.cpp  py-jet.cpp py-jetvector.cpp py-mapping.cpp py-coord.cpp 

libpybeamline_la_SOURCES          =   py-beamline-module.cpp py-aperture.cpp py-kick.cpp py-bblens.cpp py-bmlnelmnt.cpp py-drift.cpp py-sbend.cpp \
                                      py-cf_sbend.cpp py-cf_rbend.cpp py-lambertson.cpp py-mover.cpp py-marker.cpp py-monitor.cpp \
                                      py-quadrupole.cpp py-octupole.cpp py-sextupole.cpp py-septum.cpp py-sector.cpp py-slot.cpp \
                                      py-rbend.cpp py-rfcavity.cpp py-thinpoles.cpp py-decapole.cpp py-particle.cpp py-beamlineiterator.cpp \
                                      py-lattfunc.cpp 

libpybmlfactory_la_SOURCES        =   py-bmlfactory-module.cpp bmlfactorykludge.cpp

libpyphysics_toolkit_la_SOURCES   =   py-physicstoolkit-module.cpp py-lattfuncsage.cpp py-beamlinecontext.cpp

### the following depend on CHEF 
# libpychefplot_la_SOURCES        =   py-chefplot-module.cpp py-chefplotmain.cpp py-chefcurve.cpp py-chefplotdata.cpp

# libpychef_la_SOURCES            =   py-chef-module.cpp 



CPPFLAGS		          =      -I$(top_srcdir)/../basic_toolkit/include  -I$(top_srcdir)/../mxyzptlk/include  -I$(top_srcdir)/../beamline/include -I$(top_srcdir)/../bmlfactory/include -I$(top_srcdir)/../physics_toolkit/include -I$(BOOST_INC) -I$(top_srcdir)/include -I$(top_srcdir)/../gms/include -I$(GLIB_INC) -I$(GLIBCONFIG_INC) -I$(PYTHON_INC)
