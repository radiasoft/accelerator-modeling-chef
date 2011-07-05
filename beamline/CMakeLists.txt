file(GLOB HEADERS "${CHEF_SOURCE_DIR}/beamline/include/*.h")
file(GLOB TEMPLATES "${CHEF_SOURCE_DIR}/beamline/include/*.tcc")
file ( MAKE_DIRECTORY "${CHEF_SOURCE_DIR}/include/beamline" )
file ( COPY ${HEADERS} ${TEMPLATES} DESTINATION
            "${CHEF_SOURCE_DIR}/include/beamline" )
install(FILES ${HEADERS} ${TEMPLATES} DESTINATION include/beamline)

add_subdirectory(src)
