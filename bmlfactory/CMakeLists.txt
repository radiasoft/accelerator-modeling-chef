set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPRIVATE_ALLOCATE=allocate_block__" )
set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPRIVATE_DEALLOCATE=deallocate__" )

file(GLOB HEADERS "${CHEF_SOURCE_DIR}/bmlfactory/include/*.h")
install(FILES ${HEADERS} DESTINATION include/bmlfactory)

add_subdirectory(src)
