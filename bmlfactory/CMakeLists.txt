file(GLOB HEADERS "${CHEF_SOURCE_DIR}/bmlfactory/include/*.h")
install(FILES ${HEADERS} DESTINATION include/bmlfactory)
file ( MAKE_DIRECTORY "${CHEF_SOURCE_DIR}/include/bmlfactory" )
file ( COPY ${HEADERS} DESTINATION
            "${CHEF_SOURCE_DIR}/include/bmlfactory" )

add_subdirectory(src)
