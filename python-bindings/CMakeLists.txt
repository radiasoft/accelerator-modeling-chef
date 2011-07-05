file(GLOB HEADERS "${CHEF_SOURCE_DIR}/python-bindings/include/*.h")
file ( MAKE_DIRECTORY "${CHEF_SOURCE_DIR}/include/python-bindings" )
file ( COPY ${HEADERS} DESTINATION
            "${CHEF_SOURCE_DIR}/include/python-bindings" )
install(FILES ${HEADERS} DESTINATION include/python-bindings)

add_subdirectory(src)
