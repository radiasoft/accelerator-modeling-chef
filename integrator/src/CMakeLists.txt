set(SRCS AkimaSplinterpoland.cc  Linterpoland.cc  RungeKutta.cc  Splinterpoland.cc
	 Integrator.cc           Riemann.cc       Simpson.cc     Symplectic.cc
)

add_library(integrator ${SRCS})

install(TARGETS integrator DESTINATION lib)
