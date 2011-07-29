set(SRCS Barnacle.cc erf.cc iosetup.cc OutputFormat.cc TML.cc 
         binomcoeff_recursive.c FFTFunctor.cc menu.cc parsef.c
         TML_SVD.cc cg.c Frame.cc MLPtr.cc RandomOrthogonal.cc
         TVector.cc ColorWheel.cc gaussrand.cc mod2pi.c rg.c
         ConvolutionFunctor.cc gcd.c mygaussj.cc SVDFit.cc
         Distribution.cc GenericException.cc nearint.c TemplateInstantiations.cc
         d_sign.c IntArray.cc nexcom.cc TMatrix.cc)

add_library(basic_toolkit ${SRCS})
target_link_libraries(basic_toolkit ${FFTW3_LIBRARIES})

install(TARGETS basic_toolkit DESTINATION lib)
