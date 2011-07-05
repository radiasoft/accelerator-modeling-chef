file(GLOB HEADERS "${CHEF_SOURCE_DIR}/physics_toolkit/include/*.h")
file(GLOB TEMPLATES "${CHEF_SOURCE_DIR}/physics_toolkit/include/*.tcc")
file ( MAKE_DIRECTORY "${CHEF_SOURCE_DIR}/include/physics_toolkit" )
file ( COPY ${HEADERS} ${TEMPLATES} DESTINATION
            "${CHEF_SOURCE_DIR}/include/physics_toolkit" )
install(FILES ${HEADERS} ${TEMPLATES} DESTINATION include/physics_toolkit)

add_subdirectory(src)
