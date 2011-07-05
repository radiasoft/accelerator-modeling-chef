set(SRCS AdjustPosition.cc        ConvertToTunes.cc     FPSolver.cc            QBpropVisitor.cc
	 AlignVisitor.cc          CovarianceSage.cc     JacobianVisitor.cc     QuadEliminator.cc
	 AzimuthVisitor.cc        DispersionSage.cc     JetParticleVisitor.cc  Sage.cc
	 BeamlineContext.cc       DriftConverter.cc     LattFuncSage.cc        SurveyMatcher.cc
	 BeamlineSpitout.cc       DriftEliminator.cc    LayoutVisitor.cc       TemplateInstantiations.cc
	 beamline_twiss.cc        DriftsToSlots.cc      LBSage.cc              TransitionVisitor.cc
	 BendEliminator.cc        EdwardsTeng.cc        MatrixCalcVisitor.cc   TuneAdjuster.cc
	 BmlUtil.cc               EdwardsTengSage.cc    ModifierVisitor.cc
	 ChromaticityAdjuster.cc  ElementTagVisitor.cc  normalForm.cc
	 ClosedOrbitSage.cc       FindCovariance.cc     ParticleVisitor.cc
)

add_library(physics_toolkit SHARED ${SRCS})

install(TARGETS physics_toolkit DESTINATION lib)
