include_directories(${GLIB_INC} ${GLIBCONFIG_INC} ${PYTHON_INC} ${NUMPY_INC})
## We need to add the location of the xsif parser generated header files
include_directories(${CHEF_SOURCE_DIR}/parsers/xsif/src)

set(SRCS1 py-basictoolkit-module.cpp py-barnacle.cpp
          py-vector.cpp  py-matrix.cpp)
add_library(pybasic_toolkit SHARED ${SRCS1})

set(SRCS2 py-mxyzptlk-module.cpp py-jetenv.cpp py-jet.cpp py-jetvector.cpp 
          py-mapping.cpp py-coord.cpp)
add_library(pymxyzptlk SHARED ${SRCS2})

set(SRCS3 py-beamline-module.cpp py-aperture.cpp py-kick.cpp
          py-bblens.cpp py-bmlnelmnt.cpp py-beamline.cpp py-drift.cpp
          py-sbend.cpp py-cf_sbend.cpp py-cf_rbend.cpp py-lambertson.cpp
          py-mover.cpp py-marker.cpp py-monitor.cpp py-quadrupole.cpp 
          py-octupole.cpp py-sextupole.cpp py-septum.cpp py-sector.cpp
          py-slot.cpp py-rbend.cpp py-rfcavity.cpp py-thinpoles.cpp
          py-decapole.cpp py-particle.cpp py-jetparticle.cpp py-lattfunc.cpp
          py-srot.cpp py-circuit.cpp py-fcircuit.cpp 
	 py-icircuit.cpp py-refregvisitor.cpp py-bmlvisitor.cpp)
add_library(pybeamline SHARED ${SRCS3})

if (BUILD_PARSER_MODULES)
  set(SRCS4 py-bmlfactory-module.cpp)
  add_library(pybmlfactory SHARED ${SRCS4})
endif ()

set(SRCS5 py-physicstoolkit-module.cpp
          py-lattfuncsage.cpp py-beamlinecontext.cpp)
add_library(pyphysics_toolkit SHARED ${SRCS5})

install(TARGETS pybasic_toolkit DESTINATION lib)
install(TARGETS pymxyzptlk DESTINATION lib)
install(TARGETS pybeamline DESTINATION lib)
if (BUILD_PARSER_MODULES)
  install(TARGETS pybmlfactory DESTINATION lib)
endif()
install(TARGETS pyphysics_toolkit DESTINATION lib)
