file(GLOB HEADERS "${CHEF_SOURCE_DIR}/python-bindings/include/*.h")

install(FILES ${HEADERS} DESTINATION include/python-bindings)

add_subdirectory(src)
