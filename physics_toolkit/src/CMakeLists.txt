set(SRCS AdjustPosition.cc
         AlignVisitor.cc
         AzimuthVisitor.cc
         BeamlineContext.cc
         BeamlineSpitout.cc
         beamline_twiss.cc
         BendEliminator.cc
         BmlUtil.cc
         ChromaticityAdjuster.cc
         ClosedOrbitSage.cc
         ConvertToTunes.cc
         CovarianceSage.cc
         DispersionSage.cc
         DriftConverter.cc
         DriftEliminator.cc
         DriftsToSlots.cc
         EdwardsTeng.cc
         EdwardsTengSage.cc
         ElementTagVisitor.cc
         FindCovariance.cc
         FPSolver.cc
         JacobianVisitor.cc
         JetParticleVisitor.cc
         LattFuncSage.cc
         LayoutVisitor.cc
         LBSage.cc
         MatrixCalcVisitor.cc
         ModifierVisitor.cc
         normalForm.cc
         normalFormSage.cc
         ParticleVisitor.cc
         QBpropVisitor.cc
         QuadEliminator.cc
         Sage.cc
         SurveyMatcher.cc
         TemplateInstantiations.cc
         TransitionVisitor.cc
         TuneAdjuster.cc
)

add_library(physics_toolkit ${SRCS})
target_link_libraries(physics_toolkit beamline basic_toolkit mxyzptlk ${Boost_SYSTEM_LIBRARY})

install(TARGETS physics_toolkit DESTINATION lib)
