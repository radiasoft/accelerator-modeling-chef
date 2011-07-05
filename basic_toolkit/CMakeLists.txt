file(GLOB HEADERS "${CHEF_SOURCE_DIR}/basic_toolkit/include/*.h")
file(GLOB TEMPLATES "${CHEF_SOURCE_DIR}/basic_toolkit/include/*.tcc")
file ( MAKE_DIRECTORY "${CHEF_SOURCE_DIR}/include/basic_toolkit" )
file ( COPY ${HEADERS} ${TEMPLATES} DESTINATION
            "${CHEF_SOURCE_DIR}/include/basic_toolkit" )
install(FILES ${HEADERS} ${TEMPLATES} DESTINATION include/basic_toolkit)

add_subdirectory(src)
