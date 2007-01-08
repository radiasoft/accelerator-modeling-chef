lib_LTLIBRARIES         = libpybasic_toolkit.la  libpymxyzptlk.la libpybeamline.la libpybmlfactory.la libpyphysics_toolkit.la libpurepybasic_toolkit.la  libpurepymxyzptlk.la libpurepybeamline.la libpurepybmlfactory.la libpurepyphysics_toolkit.la

libpybasic_toolkit_la_SOURCES     =   py-basictoolkit-module.cpp py-barnacle.cpp  py-vector.cpp  py-matrix.cpp py-slist.cpp 

libpymxyzptlk_la_SOURCES          =   py-mxyzptlk-module.cpp py-jetenv.cpp  py-jet.cpp py-jetvector.cpp py-mapping.cpp py-coord.cpp 

libpybeamline_la_SOURCES          =   py-beamline-module.cpp py-aperture.cpp py-kick.cpp py-bblens.cpp py-bmlnelmnt.cpp py-beamline.cpp py-drift.cpp py-sbend.cpp \
                                      py-cf_sbend.cpp py-cf_rbend.cpp py-lambertson.cpp py-mover.cpp py-marker.cpp py-monitor.cpp \
                                      py-quadrupole.cpp py-octupole.cpp py-sextupole.cpp py-septum.cpp py-sector.cpp py-slot.cpp \
                                      py-rbend.cpp py-rfcavity.cpp py-thinpoles.cpp py-decapole.cpp py-particle.cpp py-beamlineiterator.cpp \
                                      py-lattfunc.cpp py-insertionlist.cpp \
                                      py-refregvisitor.cpp py-bmlvisitor.cpp

libpybmlfactory_la_SOURCES        =   py-bmlfactory-module.cpp 

libpyphysics_toolkit_la_SOURCES   =   py-physicstoolkit-module.cpp py-lattfuncsage.cpp py-beamlinecontext.cpp

libpurepybasic_toolkit_la_SOURCES = $(libpybasic_toolkit_la_SOURCES)
libpurepymxyzptlk_la_SOURCES = $(libpymxyzptlk_la_SOURCES)
libpurepybeamline_la_SOURCES = $(libpybeamline_la_SOURCES)
libpurepybmlfactory_la_SOURCES = $(libpybmlfactory_la_SOURCES)
libpurepyphysics_toolkit_la_SOURCES = $(libpyphysics_toolkit_la_SOURCES)
### the following depend on CHEF 
# libpychefplot_la_SOURCES        =   py-chefplot-module.cpp py-chefplotmain.cpp py-chefcurve.cpp py-chefplotdata.cpp

# libpychef_la_SOURCES            =   py-chef-module.cpp 

libpurepybasic_toolkit_la_LIBADD = $(BOOST_PYTHON_LINK) \
    ../../physics_toolkit/src/libphysics_toolkit.la \
    ../../beamline/src/libbeamline.la \
    ../../bmlfactory/src/libbmlfactory.la \
    -lglib-2.0 \
    ../../mxyzptlk/src/libmxyzptlk.la \
    ../../basic_toolkit/src/libbasic_toolkit.la 

libpurepymxyzptlk_la_LIBADD = $(BOOST_PYTHON_LINK) \
    ../../physics_toolkit/src/libphysics_toolkit.la \
    ../../beamline/src/libbeamline.la \
    ../../bmlfactory/src/libbmlfactory.la \
    -lglib-2.0 \
    ../../mxyzptlk/src/libmxyzptlk.la \
    ../../basic_toolkit/src/libbasic_toolkit.la 

libpurepybeamline_la_LIBADD = $(BOOST_PYTHON_LINK) \
    ../../physics_toolkit/src/libphysics_toolkit.la \
    ../../beamline/src/libbeamline.la \
    ../../bmlfactory/src/libbmlfactory.la \
    -lglib-2.0 \
    ../../mxyzptlk/src/libmxyzptlk.la \
    ../../basic_toolkit/src/libbasic_toolkit.la

libpurepybmlfactory_la_LIBADD = $(BOOST_PYTHON_LINK) \
    ../../physics_toolkit/src/libphysics_toolkit.la \
    ../../beamline/src/libbeamline.la \
    ../../bmlfactory/src/libbmlfactory.la \
    -lglib-2.0 \
    ../../mxyzptlk/src/libmxyzptlk.la \
    ../../basic_toolkit/src/libbasic_toolkit.la

libpurepyphysics_toolkit_la_LIBADD = $(BOOST_PYTHON_LINK) \
    ../../physics_toolkit/src/libphysics_toolkit.la \
    ../../beamline/src/libbeamline.la \
    ../../bmlfactory/src/libbmlfactory.la \
    -lglib-2.0 \
    ../../mxyzptlk/src/libmxyzptlk.la \
    ../../basic_toolkit/src/libbasic_toolkit.la

CPPFLAGS                          =    -I$(BOOST_INC)  -I$(top_srcdir)/../include -I$(GLIB_INC) -I$(GLIBCONFIG_INC) -I$(PYTHON_INC)

install-exec-hook:
	for lib in basic_toolkit beamline bmlfactory mxyzptlk physics_toolkit ;\
            do \
            if test -f $(DESTDIR)$(libdir)/libpurepy$$lib.so ; then \
                rm -f $(DESTDIR)$(libdir)/$$lib.so ; \
                $(LN_S) $(DESTDIR)$(libdir)/libpurepy$$lib.so \
                        $(DESTDIR)$(libdir)/$$lib.so ; \
                echo "installed $(DESTDIR)$(libdir)/$$lib.so"; \
            else echo "python libs install error: no such file $(DESTDIR)$(libdir)/libpurepy$$lib.so" ; \
            fi \
            done

