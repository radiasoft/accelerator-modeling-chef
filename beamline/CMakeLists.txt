file(GLOB HEADERS "${CHEF_SOURCE_DIR}/beamline/include/*.h")
file(GLOB TEMPLATES "${CHEF_SOURCE_DIR}/beamline/include/*.tcc")

install(FILES ${HEADERS} ${TEMPLATES} DESTINATION include/beamline)

add_subdirectory(src)
