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

#include <basic_toolkit/Barnacle.h>
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
#include <basic_toolkit/ConvolutionFunctor.h>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/random.hpp>
#include <algorithm>
#include <string>
#include <vector>
#include <map>

template class TBunch<Particle>;
template class list<ElmPtr>;

template class boost::shared_ptr<CF_rbend>; 
template class boost::shared_ptr<CF_sbend>; 
template class boost::shared_ptr<sbend>; 
template class boost::shared_ptr<Slot>; 

template boost::shared_ptr<Slot>             boost::dynamic_pointer_cast<Slot,            bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<CF_rbend>         boost::dynamic_pointer_cast<CF_rbend,        bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<thinrfcavity>     boost::dynamic_pointer_cast<thinrfcavity,    bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<combinedFunction> boost::static_pointer_cast<combinedFunction, bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<thinOctupole>     boost::dynamic_pointer_cast<thinOctupole,    bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<rfcavity>         boost::dynamic_pointer_cast<rfcavity,        bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<sbend>            boost::dynamic_pointer_cast<sbend,           bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<CF_sbend>         boost::dynamic_pointer_cast<CF_sbend,        bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);
template boost::shared_ptr<rbend>            boost::dynamic_pointer_cast<rbend,           bmlnElmnt>(boost::shared_ptr<bmlnElmnt> const&);

template WakeKickPropagator* boost::addressof<WakeKickPropagator>(WakeKickPropagator&);
template NameIs*             boost::addressof<NameIs>(NameIs&);
template TypeIs*             boost::addressof<TypeIs>(TypeIs&);
template LengthIs*           boost::addressof<LengthIs>(LengthIs&);
template LengthLess*         boost::addressof<LengthLess>(LengthLess&);
template StrengthIs*         boost::addressof<StrengthIs>(StrengthIs&);
template LengthMore*         boost::addressof<LengthMore>(LengthMore&);
template StrengthMore*       boost::addressof<StrengthMore>(StrengthMore&);
template StrengthLess*       boost::addressof<StrengthLess>(StrengthLess&);

template class boost::function< void( ParticleBunch& ) >;
template LattFunc boost::any_cast<LattFunc>(boost::any&);
template LattRing boost::any_cast<LattRing>(boost::any&);

template std::ostream& boost::operator<< (std::ostream&, boost::shared_ptr<beamline> const&);

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

namespace { 
  
  // we force instantiation of an anomymous function objects. In that case, this turns out to be easier that attempting to 
  // instantiatiate the right templates ...
   
  boost::function<double(int) > junk1 = boost::bind<double>(  ShortRangeTWakeFunction(), _1, 0.0, 0.0 );
  boost::function<double(int) > junk2 = boost::bind<double>(  ShortRangeLWakeFunction(), _1, 0.0, 0.0 );

}

template class std::vector<boost::shared_ptr<rfcavity> >;
template class std::vector<boost::shared_ptr<thinrfcavity> >;

template class boost::random::mersenne_twister<unsigned int, 32, 624, 397, 31, 2567483615u, 11, 7, 2636928640u, 15, 4022730752u, 18, 3346425566u>;

//typedef  boost::mt19937 rnd_mt19937;
//template class boost::variate_generator<boost::mt19937&, boost::normal_distribution<> >;

template class boost::uniform_real<>;
template class boost::normal_distribution<>;
template class boost::variate_generator<boost::rand48&,  boost::uniform_real<>        >;



template std::vector<double>::iterator std::transform( std::vector<double>::iterator,  std::vector<double>::iterator,  std::vector<double>::iterator, 
                              BOOST_TYPEOF( std::bind2nd( multiplies<double>(), 1.0 )) );

typedef  boost::function<void(bmlnElmnt&,    Particle&)   >    particle_propagator_t;
typedef  boost::function<void(bmlnElmnt&, JetParticle&)   > jetparticle_propagator_t;
typedef  boost::function<void(bmlnElmnt&, ParticleBunch&) >       bunch_propagator_t;

template class  std::map<int, particle_propagator_t>;
template class  std::map<int, jetparticle_propagator_t>; 

template class  std::map<char const*, boost::any>;

template WakeKick::WakeKick( char const* name, boost::function< void( ParticleBunch& ) > );      

template class boost::detail::sp_counted_impl_p<LCavityUpstream>;
template class boost::detail::sp_counted_impl_p<LCavityDnstream>;
template class boost::detail::sp_counted_impl_p<WakeKick>;


#endif //BEAMLINE_EXPLICIT_TEMPLATES
