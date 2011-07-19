#####################################################################
#
# CMakeLists.txt for chef 
#
#####################################################################

project(CHEF) 

# Required version
cmake_minimum_required (VERSION 2.8)
set(CMAKE_MODULE_PATH "${CHEF_SOURCE_DIR}/CMake")

add_definitions(-g -O2 -pipe -fpermissive -Wno-deprecated
                -DBOOST_PYTHON_NO_PY_SIGNATURES) #jfa (1st line) and goldy
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

##
# Find necessary packages
##

# python
find_package(PythonInterp REQUIRED)
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_PATH})
message(STATUS "PYTHON_INCLUDE_PATH = ${PYTHON_INCLUDE_PATH}")

# boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)
find_package(Boost 1.34.0
             COMPONENTS python regex unit_test_framework serialization REQUIRED)
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

include_directories("${CHEF_SOURCE_DIR}/include")

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
configure_file(${CHEF_SOURCE_DIR}/chef-config.sh.in
               ${CHEF_BINARY_DIR}/chef-config.sh)
install(FILES ${CHEF_BINARY_DIR}/chef-config.sh DESTINATION bin
        PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE
                    GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

##
# Add subdirectories
##
add_subdirectory(basic_toolkit)
add_subdirectory(beamline)
add_subdirectory(bmlfactory)
add_subdirectory(parsers)
add_subdirectory(integrator)
add_subdirectory(mxyzptlk)
add_subdirectory(physics_toolkit)
add_subdirectory(gms)
add_subdirectory(python-bindings)
