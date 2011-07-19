file(GLOB HEADERS "${CHEF_SOURCE_DIR}/physics_toolkit/include/*.h")
file(GLOB TEMPLATES "${CHEF_SOURCE_DIR}/physics_toolkit/include/*.tcc")

install(FILES ${HEADERS} ${TEMPLATES} DESTINATION include/physics_toolkit)

add_subdirectory(src)
