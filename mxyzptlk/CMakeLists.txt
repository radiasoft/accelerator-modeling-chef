file(GLOB HEADERS "${CHEF_SOURCE_DIR}/mxyzptlk/include/*.h")
file(GLOB TEMPLATES "${CHEF_SOURCE_DIR}/mxyzptlk/include/*.tcc")

install(FILES ${HEADERS} ${TEMPLATES} DESTINATION include/mxyzptlk)

add_subdirectory(src)
