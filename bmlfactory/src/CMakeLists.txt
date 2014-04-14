find_package(BISON)
find_package(FLEX)

set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPRIVATE_ALLOCATE=allocate_block__" )
set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPRIVATE_DEALLOCATE=deallocate__" )

bison_target(MadParser mad_yacc.y ${CMAKE_CURRENT_BINARY_DIR}/mad_yacc.c)
flex_target(MadScanner mad_lex.l ${CMAKE_CURRENT_BINARY_DIR}/mad_lex.c)
add_flex_bison_dependency(MadScanner MadParser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

include_directories(${GLIB_INC} ${GLIBCONFIG_INC})

set(SRCS beam_element.c  bmlfactory_exit.cc  const_table.c   
	 MAD8Factory.cc  ParserException.cc  beamel_table.c  
	 bml_table.c     expression.c        mad.c           
	 str_to_upper.c  beam_line.c         cfg_output.c        
	 fb_allocator.c  madparser.c         var_table.c
         bmlfactory.cc   comment_arr.c       fb.c            
	 matrix.c        yywrap.c            ${BISON_MadParser_OUTPUTS}
         ${FLEX_MadScanner_OUTPUTS}
)

add_library(bmlfactory ${SRCS})

install(TARGETS bmlfactory DESTINATION lib)
