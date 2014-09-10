include_directories(${GLIB_INC} ${GLIBCONFIG_INC} ${PYTHON_INC} ${NUMPY_INC})
## We need to add the location of the xsif parser generated header files
include_directories(${CHEF_SOURCE_DIR}/parsers/xsif/src)

set(CHEF_LIBRARIES basic_toolkit mxyzptlk beamline physics_toolkit)
if (BUILD_PARSER_MODULES)
    set(CHEF_LIBRARIES ${CHEF_LIBRARIES} bmlfactory)
endif()

set(SRCS1 py-basictoolkit-module.cpp py-barnacle.cpp
          py-vector.cpp  py-matrix.cpp)
add_python_extension(pybasic_toolkit SHARED ${SRCS1})
target_link_libraries(pybasic_toolkit ${CHEF_LIBRARIES} ${Boost_LIBRARIES} ${PYTHON_LIBRARY})

set(SRCS2 py-mxyzptlk-module.cpp py-jetenv.cpp py-jet.cpp py-jetvector.cpp
          py-mapping.cpp py-coord.cpp)
add_python_extension(pymxyzptlk SHARED ${SRCS2})
target_link_libraries(pymxyzptlk ${CHEF_LIBRARIES} ${Boost_LIBRARIES} ${PYTHON_LIBRARY})

set(SRCS3 py-beamline-module.cpp py-aperture.cpp py-kick.cpp
          py-bblens.cpp py-bmlnelmnt.cpp py-beamline.cpp py-drift.cpp
          py-sbend.cpp py-cf_sbend.cpp py-cf_rbend.cpp py-lambertson.cpp
          py-mover.cpp py-marker.cpp py-monitor.cpp py-quadrupole.cpp
          py-octupole.cpp py-sextupole.cpp py-septum.cpp py-sector.cpp
          py-slot.cpp py-rbend.cpp py-rfcavity.cpp py-thinpoles.cpp
          py-decapole.cpp py-particle.cpp py-jetparticle.cpp py-lattfunc.cpp
          py-srot.cpp py-circuit.cpp py-fcircuit.cpp
          py-icircuit.cpp py-refregvisitor.cpp py-bmlvisitor.cpp
          py-yoshidapropagator.cpp py-nonlinearlens.cpp)
add_python_extension(pybeamline SHARED ${SRCS3})
target_link_libraries(pybeamline ${CHEF_LIBRARIES} ${Boost_LIBRARIES} ${PYTHON_LIBRARY})

if (BUILD_PARSER_MODULES)
  set(SRCS4 py-bmlfactory-module.cpp)
  add_python_extension(pybmlfactory SHARED ${SRCS4})
  target_link_libraries(pybmlfactory ${CHEF_LIBRARIES} ${Boost_LIBRARIES} ${PYTHON_LIBRARY})
endif ()

set(SRCS5 py-physicstoolkit-module.cpp
          py-lattfuncsage.cpp py-beamlinecontext.cpp py-normalformsage.cpp)
add_python_extension(pyphysics_toolkit SHARED ${SRCS5})
target_link_libraries(pyphysics_toolkit ${CHEF_LIBRARIES} ${Boost_LIBRARIES} ${PYTHON_LIBRARY})

install(TARGETS pybasic_toolkit DESTINATION lib)
install(FILES basic_toolkit.py DESTINATION lib)
install(TARGETS pymxyzptlk DESTINATION lib)
install(FILES mxyzptlk.py DESTINATION lib)
install(TARGETS pybeamline DESTINATION lib)
install(FILES beamline.py DESTINATION lib)
if (BUILD_PARSER_MODULES)
  install(TARGETS pybmlfactory DESTINATION lib)
  install(FILES bmlfactory.py DESTINATION lib)
endif()
install(TARGETS pyphysics_toolkit DESTINATION lib)
install(FILES physics_toolkit.py DESTINATION lib)
