file(GLOB HEADERS "${CHEF_SOURCE_DIR}/integrator/include/*.h")
file(GLOB TEMPLATES "${CHEF_SOURCE_DIR}/integrator/include/*.tcc")
file ( MAKE_DIRECTORY "${CHEF_SOURCE_DIR}/include/integrator" )
file ( COPY ${HEADERS} ${TEMPLATES} DESTINATION
            "${CHEF_SOURCE_DIR}/include/integrator" )
install(FILES ${HEADERS} ${TEMPLATES} DESTINATION include/integrator)

add_subdirectory(src)
