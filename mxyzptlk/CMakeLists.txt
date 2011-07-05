file(GLOB HEADERS "${CHEF_SOURCE_DIR}/mxyzptlk/include/*.h")
file(GLOB TEMPLATES "${CHEF_SOURCE_DIR}/mxyzptlk/include/*.tcc")
file ( MAKE_DIRECTORY "${CHEF_SOURCE_DIR}/include/mxyzptlk" )
file ( COPY ${HEADERS} ${TEMPLATES} DESTINATION
            "${CHEF_SOURCE_DIR}/include/mxyzptlk" )
install(FILES ${HEADERS} ${TEMPLATES} DESTINATION include/mxyzptlk)

add_subdirectory(src)
