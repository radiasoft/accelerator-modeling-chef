set(SRCS alignment.cc                    gkick.cc                        read_istream.cc
	 Aperture.cc                     GkickPropagators.cc             RefRegVisitor.cc
	 BBLens.cc                       ICircuit.cc                     rfcavity.cc
	 BBLensPropagators.cc            JetParticle.cc                  RFCavityPropagators.cc
	 beamline.cc                     kick.cc                         sbend.cc
	 BeamlineExpressionTree.cc       KickPropagators.cc              SBendPropagators.cc
	 Bend.cc                         lambertson.cc                   sector.cc
	 BendPropagators.cc              LambertsonPropagators.cc        SectorPropagators.cc
	 BipolarCircuit.cc               LinacCavity.cc                  septum.cc
	 bmlnElmnt.cc                    LinacCavityParts.cc             SeptumPropagators.cc
	 BmlVisitor.cc                   LinacCavityPartsPropagators.cc  sextupole.cc
	 BunchProjector.cc               LinacCavityPropagators.cc       SextupolePropagators.cc
	 CF_rbend.cc                     marker.cc                       Slot.cc
	 CF_rbendPropagators.cc          MarkerPropagators.cc            SlotPropagators.cc
	 CF_sbend.cc                     monitor.cc                      Solenoid.cc
	 CF_sbendPropagators.cc          CF_sbend_MADPropagator.cc
	 MonitorPropagators.cc           SolenoidPropagators.cc
	 circuit.cc                      mover.cc                        srot.cc
	 combinedFunction.cc             octupole.cc                     SRotPropagators.cc
	 CombinedFunctionPropagators.cc  OctupolePropagators.cc          TBunch.cc
	 decapole.cc                     Particle.cc                     TemplateInstantiations.cc
	 DecapolePropagators.cc          pinger.cc                       thinMultipole.cc
	 drift.cc                        PingerPropagators.cc            ThinMultipolesPropagators.cc
	 DriftPropagators.cc             PropagatorFactory.cc            thinpoles.cc
	 Edge.cc                         quadrupole.cc                   ThinPolesPropagators.cc
	 EdgePropagators.cc              QuadrupolePropagators.cc        WakeFunctions.cc
	 FCircuit.cc                     rbend.cc                        WakeKick.cc
	 FramePusher.cc                  RBendPropagators.cc             WakeKickPropagators.cc
	 YoshidaPropagator.cc            nonLinearLens.cc
	 NonLinearLensPropagators.cc
)

add_library(beamline ${SRCS})
target_link_libraries(beamline basic_toolkit mxyzptlk ${Boost_SYSTEM_LIBRARY} ${FFTW3_LIBRARIES})

install(TARGETS beamline DESTINATION lib)
