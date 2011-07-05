file(GLOB HEADERS "${CHEF_SOURCE_DIR}/gms/include/*.h")
file ( MAKE_DIRECTORY "${CHEF_SOURCE_DIR}/include/gms" )
file ( COPY ${HEADERS} DESTINATION
            "${CHEF_SOURCE_DIR}/include/gms" )
install(FILES ${HEADERS} DESTINATION include/gms)
