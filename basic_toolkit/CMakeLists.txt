file(GLOB HEADERS "${CHEF_SOURCE_DIR}/basic_toolkit/include/*.h")
file(GLOB TEMPLATES "${CHEF_SOURCE_DIR}/basic_toolkit/include/*.tcc")

install(FILES ${HEADERS} ${TEMPLATES} DESTINATION include/basic_toolkit)

add_subdirectory(src)
