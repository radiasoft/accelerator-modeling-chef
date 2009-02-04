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
#include <map>
#include <utility>
#include <algorithm>

#include <beamline/beamline.h>
#include <beamline/beamline_elements.h>

#include <beamline/BBLens.tcc>
#include <beamline/bmlnElmnt.tcc>

#include <beamline/BendPropagators.h>
#include <beamline/EdgePropagators.h>
#include <beamline/TBunch.h>
#include <beamline/TBunch.tcc>
#include <beamline/JetParticle.h>
#include <beamline/Particle.h>
#include <beamline/WakeKickPropagator.h>
#include <beamline/WakeFunctions.h>
#include <beamline/BeamlineExpressionTree.h>
#include <beamline/LinacCavityParts.h>
#include <beamline/BeamlineExpressionTree.h>
#include <beamline/BeamlinePropagators.h>
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
#include <beamline/ThinMultipolePropagators.h>
#include <beamline/PropagatorFactory.h>
#include <beamline/BunchProjector.h>

#include <beamline/AlignmentDecorator.h>
#include <beamline/ApertureDecorator.h>


#include <basic_toolkit/ConvolutionFunctor.h>
#include <basic_toolkit/ConvolutionFunctorImpl.h>
#include <basic_toolkit/ConvolutionFunctor.tcc>
#include <basic_toolkit/ConvolutionFunctorImpl.tcc>

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

template 
void bmlnElmnt::enterLocalFrame( Particle& p ) const;

template 
void bmlnElmnt::enterLocalFrame( JetParticle& p ) const;

template 
void bmlnElmnt::leaveLocalFrame( Particle& p ) const;

template 
void bmlnElmnt::leaveLocalFrame( JetParticle& p ) const;


template boost::shared_ptr<thinSextupole const> boost::dynamic_pointer_cast<thinSextupole const, bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<thinQuad      const> boost::dynamic_pointer_cast<thinQuad      const, bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<thinOctupole  const> boost::dynamic_pointer_cast<thinOctupole  const, bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<WakeKick>         boost::dynamic_pointer_cast<WakeKick,         bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
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

template boost::shared_ptr<LinacCavity::Propagator> boost::dynamic_pointer_cast<LinacCavity::Propagator, BasePropagator>(boost::shared_ptr<BasePropagator> const&);
template boost::shared_ptr<BasePropagator>::shared_ptr( BasePropagator * p );

//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------
#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void TBunch<Particle>::sort<LWakeOrder>( LWakeOrder );
// template void TBunch<JetParticle>::sort<LWakeOrder>( LWakeOrder ); // disabled for now ??? FIX ME ???

#endif
//----------------------------------------------------------------------------------

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

template LattRing  boost::any_cast<LattRing>(boost::any&);
template LattRing* boost::any_cast<LattRing>(boost::any*);

template std::ostream& boost::operator<< (std::ostream&, boost::shared_ptr<beamline> const&);

namespace {
 boost::function<double(int)> ft = boost::bind<double>( ShortRangeTWakeFunction(), _1, 0.0, 0.0 );
 boost::function<double(int)> fl = boost::bind<double>( ShortRangeLWakeFunction(), _1, 0.0, 0.0 );
}

template ConvolutionFunctor<double>::ConvolutionFunctor( int, BOOST_TYPEOF(  boost::bind<double>( ShortRangeLWakeFunction(),  _1,  0.0,  0.0 )), bool);
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


template class boost::shared_ptr<bmlnElmnt>;
template       boost::shared_ptr<bmlnElmnt>::shared_ptr( bmlnElmnt*);

template class boost::shared_ptr<beamline>;
template       boost::shared_ptr<beamline>::shared_ptr( beamline* );


// All ElmPtr, in alphabetical order 

template       boost::shared_ptr<Bend>::shared_ptr( Bend* );

template       boost::shared_ptr<CF_rbend>::shared_ptr( CF_rbend* );

template       boost::shared_ptr<CF_sbend>::shared_ptr( CF_sbend* );

template       boost::shared_ptr<drift>::shared_ptr( drift* );

template       boost::shared_ptr<Edge>::shared_ptr( Edge* );

template       boost::shared_ptr<hkick>::shared_ptr( hkick* );

template       boost::shared_ptr<gkick>::shared_ptr( gkick* );

template       boost::shared_ptr<kick>::shared_ptr(kick *);

template       boost::shared_ptr<LCavityDnstream>::shared_ptr( LCavityDnstream* );

template       boost::shared_ptr<LCavityUpstream>::shared_ptr( LCavityUpstream* );

template       boost::shared_ptr<LinacCavity>::shared_ptr( LinacCavity* );

template       boost::shared_ptr<marker>::shared_ptr(marker *);

template       boost::shared_ptr<octupole>::shared_ptr( octupole* );

template       boost::shared_ptr<quadrupole>::shared_ptr( quadrupole* );

template       boost::shared_ptr<sbend>::shared_ptr(sbend* );

template       boost::shared_ptr<Slot>::shared_ptr( Slot* );

template       boost::shared_ptr<rbend>::shared_ptr( rbend*);

template       boost::shared_ptr<sector>::shared_ptr( sector*);

template       boost::shared_ptr<sextupole>::shared_ptr( sextupole* );

template       boost::shared_ptr<Solenoid>::shared_ptr( Solenoid* );

template       boost::shared_ptr<thinDecapole>::shared_ptr( thinDecapole* );

template       boost::shared_ptr<thinQuad>::shared_ptr( thinQuad* );

template       boost::shared_ptr<thinrfcavity>::shared_ptr( thinrfcavity*);

template       boost::shared_ptr<thinMultipole>::shared_ptr( thinMultipole* );

template       boost::shared_ptr<thinOctupole>::shared_ptr( thinOctupole* );

template       boost::shared_ptr<thinSextupole>::shared_ptr( thinSextupole* );

template       boost::shared_ptr<vkick>::shared_ptr( vkick* );

template       boost::shared_ptr<WakeKick>::shared_ptr( WakeKick* );

template       boost::shared_ptr<ApertureDecorator>::shared_ptr( ApertureDecorator * );
template       boost::shared_ptr<AlignmentDecorator>::shared_ptr( AlignmentDecorator * );
template       boost::shared_ptr<PropagatorDecorator>::shared_ptr( PropagatorDecorator * );

//---------------------------------------------
// BasePropagators
//--------------------------------------------

template boost::shared_ptr<beamline::Propagator       >::shared_ptr( beamline::Propagator*        );
template boost::shared_ptr<Slot::Propagator           >::shared_ptr( Slot::Propagator*            );
template boost::shared_ptr<kick::Propagator           >::shared_ptr( kick::Propagator*            );
template boost::shared_ptr<sector::Propagator         >::shared_ptr( sector::Propagator*          );
template boost::shared_ptr<vkick::Propagator          >::shared_ptr( vkick::Propagator*           );
template boost::shared_ptr<hmonitor::Propagator       >::shared_ptr( hmonitor::Propagator*        );
template boost::shared_ptr<vmonitor::Propagator       >::shared_ptr( vmonitor::Propagator*        );
template boost::shared_ptr<marker::Propagator         >::shared_ptr( marker::Propagator*          );
template boost::shared_ptr<hkick::Propagator          >::shared_ptr( hkick::Propagator*           );
template boost::shared_ptr<monitor::Propagator        >::shared_ptr( monitor::Propagator*         );
template boost::shared_ptr<gkick::Propagator          >::shared_ptr( gkick::Propagator*           );
template boost::shared_ptr<thinSeptum::Propagator     >::shared_ptr( thinSeptum::Propagator*      );
template boost::shared_ptr<ThinPole::Propagator       >::shared_ptr( ThinPole::Propagator*        );
template boost::shared_ptr<Pinger::Propagator         >::shared_ptr( Pinger::Propagator*          );
template boost::shared_ptr<thinrfcavity::Propagator   >::shared_ptr( thinrfcavity::Propagator*    );
template boost::shared_ptr<LinacCavity::Propagator    >::shared_ptr( LinacCavity::Propagator*     );
template boost::shared_ptr<srot::Propagator           >::shared_ptr( srot::Propagator*            );
template boost::shared_ptr<rfcavity::Propagator       >::shared_ptr( rfcavity::Propagator*        );
template boost::shared_ptr<LCavityDnstream::Propagator>::shared_ptr( LCavityDnstream::Propagator* );
template boost::shared_ptr<LCavityUpstream::Propagator>::shared_ptr( LCavityUpstream::Propagator* );
template boost::shared_ptr<BBLens::Propagator         >::shared_ptr( BBLens::Propagator*          );
template boost::shared_ptr<quadrupole::Propagator     >::shared_ptr( quadrupole::Propagator*      );
template boost::shared_ptr<quadrupole::MADPropagator  >::shared_ptr( quadrupole::MADPropagator*   );
template boost::shared_ptr<sbend::Propagator          >::shared_ptr( sbend::Propagator*           );
template boost::shared_ptr<sbend::MADPropagator       >::shared_ptr( sbend::MADPropagator*        );
template boost::shared_ptr<thinDecapole::Propagator   >::shared_ptr( thinDecapole::Propagator*    );
template boost::shared_ptr<Edge::Propagator           >::shared_ptr( Edge::Propagator*            );
template boost::shared_ptr<octupole::Propagator       >::shared_ptr( octupole::Propagator*        );
template boost::shared_ptr<rbend::Propagator          >::shared_ptr( rbend::Propagator*           );
template boost::shared_ptr<sextupole::Propagator      >::shared_ptr( sextupole::Propagator*       );
template boost::shared_ptr<drift::Propagator          >::shared_ptr( drift::Propagator*           );
template boost::shared_ptr< thinSextupole::Propagator >::shared_ptr( thinSextupole::Propagator*   );
template boost::shared_ptr<thinQuad::Propagator       >::shared_ptr( thinQuad::Propagator*        );
template boost::shared_ptr<thinMultipole::Propagator  >::shared_ptr( thinMultipole::Propagator*   );
template boost::shared_ptr<thinOctupole::Propagator   >::shared_ptr( thinOctupole::Propagator*    );
template boost::shared_ptr<Bend::Propagator           >::shared_ptr( Bend::Propagator*            );
template boost::shared_ptr<CF_sbend::Propagator       >::shared_ptr( CF_sbend::Propagator*        );
template boost::shared_ptr<CF_rbend::Propagator       >::shared_ptr( CF_rbend::Propagator*        );
template boost::shared_ptr<Solenoid::Propagator       >::shared_ptr( Solenoid::Propagator*        );
template boost::shared_ptr<WakeKick::Propagator       >::shared_ptr( WakeKick::Propagator*        );

template class std::map<int, std::complex<double> >;

namespace {
  std::list<std::pair<int, std::complex<double> > > dummy_list;
  std::list<std::pair<boost::shared_ptr<bmlnElmnt>, double> > second_dummy_list;
}

template        EVector_t<double>::Type BBLens::NormalizedEField(        double const&,       double const&) const;
template EVector_t<TJet<double> >::Type BBLens::NormalizedEField(  TJet<double> const&, TJet<double> const&) const;

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




//template double   boost::any_cast<double>(boost::any const&);

template boost::any::any( double const& );
template double   boost::any_cast<double>( boost::any& );


// for PropagatorFactory


typedef  boost::function<void(bmlnElmnt&,    Particle&)   >    particle_propagator_t;
typedef  boost::function<void(bmlnElmnt&, JetParticle&)   > jetparticle_propagator_t;
typedef  boost::function<void(bmlnElmnt&, ParticleBunch&) >       bunch_propagator_t;

std::map<int, particle_propagator_t>       particle_propagators_map_;
std::map<int, jetparticle_propagator_t> jetparticle_propagators_map_;

template class boost::function2<void,bmlnElmnt&, Particle& >;
template class boost::function2<void,bmlnElmnt&, JetParticle& >;

//------------------------------------------------------
// iterators 
//------------------------------------------------------
 
template       class       beamline::iter<ElmPtr>;     
template       class       beamline::iter<ElmPtr const>;     
   

template       class       beamline::reverse_iter<ElmPtr>;     
template       class       beamline::reverse_iter<ElmPtr const>;     

template       class       beamline::pre_order_iter<ElmPtr>;     
template       class       beamline::pre_order_iter<ElmPtr const>;     

template       class       beamline::reverse_pre_order_iter<ElmPtr>;
template       class       beamline::reverse_pre_order_iter<ElmPtr const>;


template       class       beamline::post_order_iter<ElmPtr>;     
template       class       beamline::post_order_iter<ElmPtr const>;     


template       class       beamline::reverse_post_order_iter<ElmPtr>;
template       class       beamline::reverse_post_order_iter<ElmPtr const>;


template       class       beamline::deep_iter<ElmPtr>;
template       class       beamline::deep_iter<ElmPtr const>;


template       class       beamline::reverse_deep_iter<ElmPtr>;
template       class       beamline::reverse_deep_iter<ElmPtr const>;

template class boost::function1<double, TVector<double> const&,          std::allocator<void> >;
template class boost::function1<TVector<double>, TVector<double> const&, std::allocator<void> >;
template class boost::function1<Matrix, TVector<double> const&,          std::allocator<void> >;

template class boost::function0<double,  std::allocator<void> >; 

namespace { 
 double OrbitLength( ElmPtr elm, Particle& p) { return elm->OrbitLength(p); } 
 double Length( ElmPtr elm)                   { return elm->Length();       } 
 Proton particle(100); 
 Particle const& p = particle;
 boost::function<double(boost::shared_ptr<bmlnElmnt> )> tmp1 = boost::bind<double>( ::OrbitLength, _1, p );  
  // boost::function<double(boost::shared_ptr<bmlnElmnt> )> tmp2( &::Length );  
}


template 
class boost::function1<double, boost::shared_ptr<bmlnElmnt>, std::allocator<void> >;

template 
boost::any::any( bool const &);

template 
bool boost::any_cast(any &);

template 
bool boost::any_cast(const any &);

template
const bool* boost::any_cast(const any *);

template
bool * boost::any_cast(any *);


template
class std::vector<void*, std::allocator<void*> >;

typedef std::vector<void*, std::allocator<void*> >::iterator viter; 

template
void std::vector<void*, std::allocator<void*> >::insert( viter, viter, viter);



#endif //BEAMLINE_EXPLICIT_TEMPLATES
