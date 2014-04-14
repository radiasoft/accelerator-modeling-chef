set(SRCS AkimaSplinterpoland.cc  Linterpoland.cc  RungeKutta.cc  Splinterpoland.cc
	 Integrator.cc           Riemann.cc       Simpson.cc     Symplectic.cc
)

add_library(integrator ${SRCS})
target_link_libraries(integrator basic_toolkit ${Boost_SYSTEM_LIBRARY})

install(TARGETS integrator DESTINATION lib)
