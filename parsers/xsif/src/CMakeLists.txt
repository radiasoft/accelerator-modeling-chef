find_package(BISON)
find_package(FLEX)

set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPRIVATE_ALLOCATE=allocate_block__" )
set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPRIVATE_DEALLOCATE=deallocate__" )

bison_target(XsifParser xsif_yacc.ypp
             ${CMAKE_CURRENT_BINARY_DIR}/XsifParser_ypp.cc)
flex_target(XsifScanner xsif_lex.l ${CMAKE_CURRENT_BINARY_DIR}/lex.xsif_yy.cc)
add_flex_bison_dependency(XsifScanner XsifParser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

include_directories(${Boost_INCLUDE_DIRS} ${FFTW3_INCLUDE_DIR})

set(SRCS Expression.cc XSIFFactory.cc XsifParserDriver.cc
         ${BISON_XsifParser_OUTPUTS} ${FLEX_XsifScanner_OUTPUTS}
)

add_library(xsifparser SHARED ${SRCS})

install(TARGETS xsifparser DESTINATION lib)
