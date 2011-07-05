file ( MAKE_DIRECTORY "${CHEF_SOURCE_DIR}/include/parsers/xsif" )

file(GLOB HEADERS "${CHEF_SOURCE_DIR}/parsers/xsif/include/*.h")
file ( COPY ${HEADERS}
       DESTINATION "${CHEF_SOURCE_DIR}/include/parsers/xsif" )
install(FILES ${HEADERS} DESTINATION include/parsers/xsif)

file ( MAKE_DIRECTORY "${CHEF_SOURCE_DIR}/include/parsers/xsif/tree-container" )
file(GLOB TREE_HEADERS
          "${CHEF_SOURCE_DIR}/parsers/xsif/include/tree-container/*.h")
file(GLOB TREE_INLINE
          "${CHEF_SOURCE_DIR}/parsers/xsif/include/tree-container/*.inl")
file ( COPY ${TREE_HEADERS} ${TREE_INLINE}
       DESTINATION "${CHEF_SOURCE_DIR}/include/parsers/xsif/tree-container" )
install(FILES ${TREE_HEADERS} ${TREE_INLINE}
        DESTINATION include/parsers/xsif/tree-container)

include_directories(${CHEF_SOURCE_DIR}/include/parsers/xsif)

add_subdirectory(src)
