set(SRCS EnvPtr.cc   TJetEnvironmentInstantiations.cc  TJLInstantiations.cc
	 TemplateInstantiations.cc  TJetVector.cc      TJLterm.cc
	 TJet.cc                    TJL1.cc            TJLtermInstantiations.cc
	 TJetConversions.cc         TJL1Instantiations.cc   TLieOperator.cc
	 TJetEnvironment.cc         TJL.cc              TMapping.cc
)

add_library(mxyzptlk ${SRCS})
target_link_libraries(mxyzptlk basic_toolkit ${Boost_SYSTEM_LIBRARY})

install(TARGETS mxyzptlk DESTINATION lib)
