/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      TemplateInstantiations.cc
******                                                                
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
******                All Rights Reserved                             
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******  Author:    Jean-Francois Ostiguy                                     
******             Email: ostiguy@fnal.gov                         
****** 
**************************************************************************
**************************************************************************
*************************************************************************/
#ifdef BEAMLINE_EXPLICIT_TEMPLATES

#include <list>
#include <utility>

#include <basic_toolkit/Barnacle.h>
#include <beamline/beamline.h>
#include <beamline/drift.h>
#include <beamline/BendPropagators.h>
#include <beamline/EdgePropagators.h>
#include <beamline/quadrupole.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/TBunch.h>
#include <beamline/TBunch.tcc>
#include <beamline/Particle.h>
#include <beamline/lattFunc.h>
#include <beamline/WakeKick.h>
#include <beamline/WakeKickPropagator.h>
#include <beamline/beamline_elements.h>
#include <beamline/WakeFunctions.h>
#include <beamline/BeamlineExpressionTree.h>
#include <beamline/LinacCavityParts.h>
#include <beamline/BeamlineExpressionTree.h>
#include <beamline/DriftPropagators.h>
#include <beamline/QuadrupolePropagators.h>
#include <beamline/SextupolePropagators.h>
#include <beamline/DecapolePropagators.h>
#include <beamline/OctupolePropagators.h>
#include <beamline/SBendPropagators.h>
#include <beamline/RBendPropagators.h>
#include <beamline/CF_sbendPropagators.h>
#include <beamline/CF_rbendPropagators.h>
#include <beamline/SlotPropagators.h>
#include <beamline/MonitorPropagators.h>
#include <beamline/KickPropagators.h>
#include <beamline/SolenoidPropagators.h>
#include <beamline/SeptumPropagators.h>
#include <beamline/GkickPropagators.h>
#include <beamline/SectorPropagators.h>
#include <beamline/MarkerPropagators.h>
#include <beamline/ThinPolesPropagators.h>
#include <beamline/LinacCavityPropagators.h>
#include <beamline/LinacCavityPartsPropagators.h>
#include <beamline/RFCavityPropagators.h>
#include <beamline/SRotPropagators.h>
#include <beamline/BBLensPropagators.h>
#include <beamline/PingerPropagators.h>
#include <beamline/PropagatorFactory.h>

#include <beamline/quadrupole.h>
#include <beamline/sbend.h>
#include <beamline/decapole.h>
#include <beamline/Edge.h>
#include <beamline/octupole.h>
#include <beamline/rbend.h>
#include <beamline/sextupole.h>
#include <beamline/drift.h>
#include <beamline/Bend.h>
#include <beamline/BunchProjector.h>

#include <basic_toolkit/ConvolutionFunctor.h>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/random.hpp>
#include <algorithm>
#include <string>
#include <vector>
#include <map>

template class TBunch<Particle>;
template class TBunch<JetParticle>;

template class list<ElmPtr>;

template class boost::shared_ptr<CF_rbend>; 
template class boost::shared_ptr<CF_sbend>; 
template class boost::shared_ptr<sbend>; 
template class boost::shared_ptr<thinrfcavity>; 

template class BasePropagator<ThinPole>;
template class BasePropagator<Pinger>;
template class BasePropagator<thinMultipole>;

template boost::shared_ptr<beamline const>   boost::dynamic_pointer_cast<beamline const,   bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<thinQuad>         boost::dynamic_pointer_cast<thinQuad,         bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<thinSextupole>    boost::dynamic_pointer_cast<thinSextupole,    bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<beamline>         boost::dynamic_pointer_cast<beamline,         bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<Slot>             boost::dynamic_pointer_cast<Slot,             bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<CF_rbend>         boost::dynamic_pointer_cast<CF_rbend,         bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<thinrfcavity>     boost::dynamic_pointer_cast<thinrfcavity,     bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<combinedFunction> boost::dynamic_pointer_cast<combinedFunction, bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<thinOctupole>     boost::dynamic_pointer_cast<thinOctupole,     bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<rfcavity>         boost::dynamic_pointer_cast<rfcavity,         bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<sbend>            boost::dynamic_pointer_cast<sbend,            bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<CF_sbend>         boost::dynamic_pointer_cast<CF_sbend,         bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<rbend>            boost::dynamic_pointer_cast<rbend,            bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<LinacCavity>      boost::dynamic_pointer_cast<LinacCavity,      bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<LCavityUpstream>  boost::dynamic_pointer_cast<LCavityUpstream,  bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<LCavityDnstream>  boost::dynamic_pointer_cast<LCavityDnstream,  bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<beamline>         boost::static_pointer_cast<beamline,          bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<Slot>             boost::static_pointer_cast<Slot,              bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<CF_rbend>         boost::static_pointer_cast<CF_rbend,          bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<thinrfcavity>     boost::static_pointer_cast<thinrfcavity,      bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<combinedFunction> boost::static_pointer_cast<combinedFunction,  bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<thinOctupole>     boost::static_pointer_cast<thinOctupole,      bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<rfcavity>         boost::static_pointer_cast<rfcavity,          bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<sbend>            boost::static_pointer_cast<sbend,             bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<CF_sbend>         boost::static_pointer_cast<CF_sbend,          bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<rbend>            boost::static_pointer_cast<rbend,             bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<Bend>             boost::static_pointer_cast<Bend,              bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);


//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------
#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))
//
// not needed for now
//
#endif
//----------------------------------------------------------------------------------

template void TBunch<Particle>::sort<LWakeOrder>( LWakeOrder );
// TEMPORARILY REMOVED: template void TBunch<JetParticle>::sort<LWakeOrder>( LWakeOrder ); ??? FIX ME ???

template WakeKickPropagator * boost::addressof(WakeKickPropagator&);
template WakeKickPropagator const * boost::addressof(WakeKickPropagator const&);

template       NameIs const *       boost::addressof(            NameIs const& );
template       TypeIs const *       boost::addressof(            TypeIs const& );
template     LengthIs const *       boost::addressof(          LengthIs const& );
template   LengthLess const *       boost::addressof(        LengthLess const& );
template   StrengthIs const *       boost::addressof(        StrengthIs const& );
template   LengthMore const *       boost::addressof(        LengthMore const& );
template StrengthMore const *       boost::addressof(      StrengthMore const& );
template StrengthLess const *       boost::addressof(      StrengthLess const& );

template       NameIs *       boost::addressof(            NameIs& );
template       TypeIs *       boost::addressof(            TypeIs& );
template     LengthIs *       boost::addressof(          LengthIs& );
template   LengthLess *       boost::addressof(        LengthLess& );
template   StrengthIs *       boost::addressof(        StrengthIs& );
template   LengthMore *       boost::addressof(        LengthMore& );
template StrengthMore *       boost::addressof(      StrengthMore& );
template StrengthLess *       boost::addressof(      StrengthLess& );


//template class boost::function< void( ParticleBunch& ) >;

template LattFunc boost::any_cast<LattFunc>(boost::any&);
template LattRing boost::any_cast<LattRing>(boost::any&);

template std::ostream& boost::operator<< (std::ostream&, boost::shared_ptr<beamline> const&);

namespace {
 boost::function<double(int)> ft = boost::bind<double>( ShortRangeTWakeFunction(), _1, 0.0, 0.0 );
 boost::function<double(int)> fl = boost::bind<double>( ShortRangeLWakeFunction(), _1, 0.0, 0.0 );
}

template class ConvolutionFunctorImpl<double>;
template class ConvolutionFunctor<double>;

template ConvolutionFunctorImpl<double>::ConvolutionFunctorImpl( int, BOOST_TYPEOF(  boost::bind<double>( ShortRangeLWakeFunction(),  _1,  0.0,  0.0 )), bool);
template ConvolutionFunctor<double>::ConvolutionFunctor( int, BOOST_TYPEOF(  boost::bind<double>( ShortRangeLWakeFunction(),  _1,  0.0,  0.0 )), bool);

template ConvolutionFunctorImpl<double>::ConvolutionFunctorImpl( int, BOOST_TYPEOF(  boost::bind<double>( ShortRangeTWakeFunction(),  _1,  0.0,  0.0 )), bool);
template ConvolutionFunctor<double>::ConvolutionFunctor( int, BOOST_TYPEOF(  boost::bind<double>( ShortRangeTWakeFunction(),  _1,  0.0,  0.0 )), bool);


template class boost::function1<bool,   bmlnElmnt const*,   std::allocator<void> >;
template class boost::function1<bool,   bmlnElmnt const&,   std::allocator<void> >;
template class boost::function1<double, int,                std::allocator<void> >;
template class boost::function1<void,   TBunch<Particle>&,  std::allocator<void> >;
template class boost::function1<double, TJet<double> const&,std::allocator<void> >;
template class boost::function1<double, double const&,      std::allocator<void> >;

  // we force instantiation of an anomymous function objects. In that case, this turns out to be easier that attempting to 
  // instantiatiate the right templates ...
   
template class boost::random::mersenne_twister<unsigned int,  32, 624, 397, 31, 2567483615u,  11, 7, 2636928640u,  15, 4022730752u,  18, 3346425566u>;
template class boost::random::mersenne_twister<unsigned long, 32, 624, 397, 31, 2567483615ul, 11, 7, 2636928640ul, 15, 4022730752ul, 18, 3346425566ul>;

//typedef  boost::mt19937 rnd_mt19937;
//template class boost::variate_generator<boost::mt19937&, boost::normal_distribution<> >;

template class boost::uniform_real<>;
template class boost::normal_distribution<>;
template class boost::variate_generator<boost::rand48&,  boost::uniform_real<>        >;



template std::vector<double>::iterator std::transform( std::vector<double>::iterator,  std::vector<double>::iterator,  std::vector<double>::iterator, 
                              BOOST_TYPEOF( std::bind2nd( multiplies<double>(), 1.0 )) );


template class  std::map<std::string, boost::any>;

template WakeKick::WakeKick( char const* name, boost::function< void( ParticleBunch& ) > );      


template class boost::detail::sp_counted_impl_p<beamline::pre_order_iter<boost::shared_ptr<bmlnElmnt> > >;
template class boost::detail::sp_counted_impl_p<beamline::pre_order_iter<boost::shared_ptr<bmlnElmnt> const> >;
template class boost::detail::sp_counted_impl_p<Slot>;
template class boost::detail::sp_counted_impl_p<thinrfcavity>;
template class boost::detail::sp_counted_impl_p<LinacCavity>;
template class boost::detail::sp_counted_impl_p<LCavityUpstream>;
template class boost::detail::sp_counted_impl_p<LCavityDnstream>;
template class boost::detail::sp_counted_impl_p<WakeKick>;
template class boost::detail::sp_counted_impl_p<sector>;
template class boost::detail::sp_counted_impl_p<Bend>;
template class boost::detail::sp_counted_impl_p<Edge>;
template class boost::detail::sp_counted_impl_p<rbend>;
template class boost::detail::sp_counted_impl_p<sbend>;
template class boost::detail::sp_counted_impl_p<CF_sbend>;
template class boost::detail::sp_counted_impl_p<CF_rbend>;
template class boost::detail::sp_counted_impl_p<quadrupole>;
template class boost::detail::sp_counted_impl_p<thinQuad>;
template class boost::detail::sp_counted_impl_p<sextupole>;
template class boost::detail::sp_counted_impl_p<thinSextupole>;
template class boost::detail::sp_counted_impl_p<Solenoid>;
template class boost::detail::sp_counted_impl_p<marker>;
template class boost::detail::sp_counted_impl_p<octupole>;
template class boost::detail::sp_counted_impl_p<thinOctupole>;
template class boost::detail::sp_counted_impl_p<thinDecapole>;
template class boost::detail::sp_counted_impl_p<drift>;
template class boost::detail::sp_counted_impl_p<kick>;
template class boost::detail::sp_counted_impl_p<gkick>;
template class boost::detail::sp_counted_impl_p<hkick>;
template class boost::detail::sp_counted_impl_p<vkick>;

//---------------------------------------------
// BasePropagators
//--------------------------------------------

template class BasePropagator<drift>;
template class BasePropagator<quadrupole>;
template class BasePropagator<thinQuad>;
template class BasePropagator<sextupole>;
template class BasePropagator<thinSextupole>;
template class BasePropagator<octupole>;
template class BasePropagator<thinOctupole>;
template class BasePropagator<thinDecapole>;
template class BasePropagator<rbend>;
template class BasePropagator<sbend>;
template class BasePropagator<Bend>;
template class BasePropagator<Edge>;
template class BasePropagator<combinedFunction>;
template class BasePropagator<CF_rbend>;
template class BasePropagator<CF_sbend>;
template class BasePropagator<thinLamb>;
template class BasePropagator<kick>;
template class BasePropagator<hkick>;
template class BasePropagator<vkick>;
template class BasePropagator<gkick>;
template class BasePropagator<Solenoid>;
template class BasePropagator<marker>;
template class BasePropagator<Slot>;
template class BasePropagator<vmonitor>;
template class BasePropagator<hmonitor>;
template class BasePropagator<monitor>;
template class BasePropagator<thinSeptum>;
template class BasePropagator<sector>;
template class BasePropagator<srot>;
template class BasePropagator<LCavityDnstream>;
template class BasePropagator<LCavityUpstream>;
template class BasePropagator<rfcavity>;
template class BasePropagator<thinrfcavity>;
template class BasePropagator<LinacCavity>;
template class BasePropagator<BBLens>;

template class boost::detail::sp_counted_impl_p<Slot::Propagator>;
template class boost::detail::sp_counted_impl_p<kick::Propagator>;
template class boost::detail::sp_counted_impl_p<sector::Propagator>;
template class boost::detail::sp_counted_impl_p<vkick::Propagator>;
template class boost::detail::sp_counted_impl_p<hmonitor::Propagator>;
template class boost::detail::sp_counted_impl_p<vmonitor::Propagator>;
template class boost::detail::sp_counted_impl_p<marker::Propagator>;
template class boost::detail::sp_counted_impl_p<hkick::Propagator>;
template class boost::detail::sp_counted_impl_p<monitor::Propagator>;
template class boost::detail::sp_counted_impl_p<BasePropagator<sector> >;
template class boost::detail::sp_counted_impl_p<gkick::Propagator>;
template class boost::detail::sp_counted_impl_p<thinSeptum::Propagator>;
template class boost::detail::sp_counted_impl_p<ThinPole::Propagator>;
template class boost::detail::sp_counted_impl_p<Pinger::Propagator>;
template class boost::detail::sp_counted_impl_p<thinrfcavity::Propagator>;
template class boost::detail::sp_counted_impl_p<LinacCavity::Propagator>;
template class boost::detail::sp_counted_impl_p<srot::Propagator>;
template class boost::detail::sp_counted_impl_p<rfcavity::Propagator>;
template class boost::detail::sp_counted_impl_p<LCavityDnstream::Propagator>;
template class boost::detail::sp_counted_impl_p<LCavityUpstream::Propagator>;
template class boost::detail::sp_counted_impl_p<BBLens::Propagator>;
template class boost::detail::sp_counted_impl_p<quadrupole::Propagator>;
template class boost::detail::sp_counted_impl_p<sbend::Propagator>;
template class boost::detail::sp_counted_impl_p<thinDecapole::Propagator>;
template class boost::detail::sp_counted_impl_p<Edge::Propagator>;
template class boost::detail::sp_counted_impl_p<octupole::Propagator>;
template class boost::detail::sp_counted_impl_p<rbend::Propagator>;
template class boost::detail::sp_counted_impl_p<sextupole::Propagator>;
template class boost::detail::sp_counted_impl_p<drift::Propagator>;
template class boost::detail::sp_counted_impl_p<thinSextupole::Propagator>;
template class boost::detail::sp_counted_impl_p<thinQuad::Propagator>;
template class boost::detail::sp_counted_impl_p<thinOctupole::Propagator>;
template class boost::detail::sp_counted_impl_p<Bend::Propagator>;
template class boost::detail::sp_counted_impl_p<CF_sbend::Propagator>;
template class boost::detail::sp_counted_impl_p<CF_rbend::Propagator>;
template class boost::detail::sp_counted_impl_p<Solenoid::Propagator>;

template class boost::detail::sp_counted_impl_p<BasePropagator<Edge> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<thinOctupole> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<octupole> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<quadrupole> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<CF_sbend> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<CF_rbend> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<drift> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<rbend> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<thinQuad> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<Bend> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<sbend> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<sextupole> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<thinDecapole> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<thinSextupole> >; 
template class boost::detail::sp_counted_impl_p<BasePropagator<thinLamb> >; 
template class boost::detail::sp_counted_impl_p<BasePropagator<kick> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<hkick> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<vkick> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<gkick> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<Solenoid> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<marker> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<Slot> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<Pinger> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<hmonitor> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<vmonitor> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<monitor> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<srot> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<thinrfcavity> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<LinacCavity> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<rfcavity> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<LCavityDnstream> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<BBLens> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<LCavityUpstream> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<thinSeptum> >;
template class boost::detail::sp_counted_impl_p<BasePropagator<ThinPole> >;

namespace {
  std::list<std::pair<int, std::complex<double> > > dummy_list;
}

// for RefRegVisitor

template class std::vector<boost::shared_ptr<rfcavity> >;  
template class std::vector<boost::shared_ptr<thinrfcavity> >;  


template void std::fill  ( std::vector<boost::shared_ptr<rfcavity> >::iterator, 
                           std::vector<boost::shared_ptr<rfcavity> >::iterator,    boost::shared_ptr<rfcavity> const& ); 

template void std::fill  ( std::vector<boost::shared_ptr<thinrfcavity> >::iterator, 
                           std::vector<boost::shared_ptr<thinrfcavity> >::iterator, boost::shared_ptr<thinrfcavity> const& ); 

template std::vector<boost::shared_ptr<thinrfcavity> >::iterator 
         std::fill_n(  std::vector<boost::shared_ptr<thinrfcavity> >::iterator, unsigned int, boost::shared_ptr<thinrfcavity> const&);

template std::vector<boost::shared_ptr<rfcavity> >::iterator 
         std::fill_n(      std::vector<boost::shared_ptr<rfcavity> >::iterator, unsigned int,     boost::shared_ptr<rfcavity> const&);



template LattRing* boost::any_cast<LattRing>(boost::any*);
template LattFunc* boost::any_cast<LattFunc>(boost::any*);

// for PropagatorFactory


typedef  boost::function<void(bmlnElmnt&,    Particle&)   >    particle_propagator_t;
typedef  boost::function<void(bmlnElmnt&, JetParticle&)   > jetparticle_propagator_t;
typedef  boost::function<void(bmlnElmnt&, ParticleBunch&) >       bunch_propagator_t;

std::map<int, particle_propagator_t>       particle_propagators_map_;
std::map<int, jetparticle_propagator_t> jetparticle_propagators_map_;

template class boost::function2<void,bmlnElmnt&, Particle& >;
template class boost::function2<void,bmlnElmnt&, JetParticle& >;

#endif //BEAMLINE_EXPLICIT_TEMPLATES

