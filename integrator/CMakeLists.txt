file(GLOB HEADERS "${CHEF_SOURCE_DIR}/integrator/include/*.h")
file(GLOB TEMPLATES "${CHEF_SOURCE_DIR}/integrator/include/*.tcc")

install(FILES ${HEADERS} ${TEMPLATES} DESTINATION include/integrator)

add_subdirectory(src)
