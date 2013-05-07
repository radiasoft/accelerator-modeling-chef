#####################################################################
#
# CMakeLists.txt for chef
#
#####################################################################

project(CHEF)

# Required version
cmake_minimum_required (VERSION 2.8)
set(CMAKE_MODULE_PATH "${CHEF_SOURCE_DIR}/CMake")

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-fpermissive -Wno-deprecated -DBOOST_PYTHON_NO_PY_SIGNATURES")

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

##
# Find necessary packages
##

# python
find_package(PythonInterp REQUIRED)
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_PATH})
message(STATUS "PYTHON_INCLUDE_PATH = ${PYTHON_INCLUDE_PATH}")

include(${CHEF_SOURCE_DIR}/CMake/AddPythonExtension.cmake)

# boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)
find_package(Boost 1.34.0
             COMPONENTS python regex unit_test_framework serialization system REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
message(STATUS "Boost_INCLUDE_DIRS = ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARY_DIRS = ${Boost_LIBRARY_DIRS}")
message(STATUS "Boost_LIBRARIES = ${Boost_LIBRARIES}")

# fftw3
find_package(FFTW3 REQUIRED)
include_directories(${FFTW3_INCLUDE_DIR})
set(PARALLEL_FFTW_LIBRARIES "${FFTW3_MPI_LIBRARIES} ${FFTW3_LIBRARIES}") #jfa
message(STATUS "FFTW3_INCLUDE_DIR = ${FFTW3_INCLUDE_DIR}")
message(STATUS "FFTW3_MPI_LIBRARIES = ${FFTW3_MPI_LIBRARIES}")
message(STATUS "FFTW3_LIBRARIES = ${FFTW3_LIBRARIES}")

message(STATUS "CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")

# glib
include(FindPkgConfig)
pkg_check_modules(GLIB2 glib-2.0)
include_directories(${GLIB2_INCLUDE_DIRS})

include_directories(BEFORE "${CHEF_SOURCE_DIR}/include")

# Set defaults for building parsers (build parsers by default).
if (NOT DEFINED BUILD_PARSER_MODULES)
  if (NOT $ENV{BUILD_PARSER_MODULES} STREQUAL "")
    set (BUILD_PARSER_MODULES $ENV{BUILD_PARSER_MODULES})
  else ()
    set (BUILD_PARSER_MODULES TRUE)
  endif ()
endif ()

if (NOT BUILD_PARSER_MODULES)
  message (STATUS "Not building parser modules")
endif ()

# Set defaults for building python bindings (build bindings by default).
if (NOT DEFINED BUILD_PYTHON_BINDINGS)
  if (NOT $ENV{BUILD_PYTHON_BINDINGS} STREQUAL "")
    set (BUILD_PYTHON_BINDINGS $ENV{BUILD_PYTHON_BINDINGS})
  else ()
    set (BUILD_PYTHON_BINDINGS TRUE)
  endif ()
endif ()

if (NOT BUILD_PYTHON_BINDINGS)
  message (STATUS "Not building python bindings")
endif ()

if (BUILD_PYTHON_BINDINGS)
    # numpy
    find_package(NUMPY REQUIRED)
    include_directories(${NUMPY_INCLUDE_DIR})
endif()

# Are we building shared or static (shared is the default)?
if (NOT DEFINED BUILD_SHARED_LIBS)
  if (NOT $ENV{BUILD_SHARED_LIBS} STREQUAL "")
    set (BUILD_SHARED_LIBS $ENV{BUILD_SHARED_LIBS})
  else ()
    set (BUILD_SHARED_LIBS TRUE)
  endif ()
endif ()

if (BUILD_SHARED_LIBS)
  message (STATUS "Building shared libraries")
else ()
  message (STATUS "Building static libraries")
endif ()

# We should probably warn someone trying to build static libraries along with
#  the parsers since it won't work with python. The parser libaries will still
#  be build shared
if ( (NOT BUILD_SHARED_LIBS) AND BUILD_PYTHON_BINDINGS )
  message(WARNING "Static library build selected, ignored for parser modules")
endif ()

##
# create links for include subdirectories
##
add_custom_target(
        create_includedir ALL
        COMMAND ./create_includedir.sh
        COMMENT "running create_includedir.sh script"
)

##
# Install chef-config.sh
##
# We need to set some variables here so that the substitution goes smoothly.
# NB: We could use cmake variables inside of chef-config.sh.in, however,
#     that would break an autotools build
set ( prefix "${CMAKE_INSTALL_PREFIX}" )
set ( exec_prefix "${prefix}/bin" )
set ( includedir "${prefix}/include" )
set ( libdir "${prefix}/lib" )
configure_file(${CHEF_SOURCE_DIR}/chef-config.sh.in
               ${CHEF_BINARY_DIR}/chef-config.sh)
install(FILES ${CHEF_BINARY_DIR}/chef-config.sh DESTINATION bin
        PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE
                    GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
unset ( prefix )
unset ( exec_prefix )
unset ( includedir )
unset ( libdir )

##
# Add subdirectories
##
add_subdirectory(basic_toolkit)
add_subdirectory(beamline)
if (BUILD_PARSER_MODULES)
  add_subdirectory(bmlfactory)
  add_subdirectory(parsers)
endif ()
add_subdirectory(integrator)
add_subdirectory(mxyzptlk)
add_subdirectory(physics_toolkit)
add_subdirectory(gms)
if (BUILD_PYTHON_BINDINGS)
  add_subdirectory(python-bindings)
endif ()
