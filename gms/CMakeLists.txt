file(GLOB HEADERS "${CHEF_SOURCE_DIR}/gms/include/*.h")

install(FILES ${HEADERS} DESTINATION include/gms)
