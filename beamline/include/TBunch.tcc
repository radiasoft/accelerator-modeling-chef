/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Beamline:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      TBunch.tcc
******                                                                
******  Copyright Fermi Research Alliance / Fermilab    
******            All Rights Reserved                             
*****
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws.
******                                                                
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov
******
******                                                                
**************************************************************************
*************************************************************************/

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/MathConstants.h>
#include <boost/bind.hpp>

#include <cmath>
#include <ostream>
#include <iomanip>

#include<boost/random.hpp>

class Particle;
class JetParticle;

using namespace std;

using FNAL::pcout;
using FNAL::pcerr;

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
std::ostream& operator<<( std::ostream &os, TBunch<Particle_t> const& bunch) 
{

  using std::setw;

  os << bunch.size() << endl;

  for ( typename TBunch<Particle_t>::const_iterator it = bunch.begin(); it != bunch.end(); ++it )
  {
    Vector const state = it->State(); 
    os << setw(16) << it->ReferenceEnergy() << "  ";
    for (int i=0; i<6; ++i )
      os << setw(16) << state[i] << "  ";
    os << std::endl;
  }
  return os;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template <typename Particle_t>
std::istream& operator >>( std::istream &is, TBunch<Particle_t>& bunch) 
{
  using std::setw;
  Vector state(6);
  double reference_energy = 0.0; 
  int counter = 0;
  int nsize   = 0;
  is >> nsize;  

  while ( (!is.eof()) && (counter++ < nsize) ) { 
    is >> reference_energy;  
    for (int i=0; i<6; ++i) {
       is >> state[i]; 
    };     
    /// bunch.append( ParticlePtr(bunch.makeParticle(reference_energy,  state))); /// Particle is an abstract class !
  }
  return is;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
TBunch<Particle_t>::TBunch(Particle_t const& p, int nparticles, double const& intensity)
: reference_(  p.Clone() ), intensity_( intensity ), bunch_(),
  pool_( sizeof(Particle) , max( 128, (nparticles*110/100) ))  
{

 //--------------------------------------------------------------------------------
 // For performance, particles objects need to be allocated as contiguously 
 // as possible in memory. This is accomplished by allocating particles 
 // from a dedicated pool with a 10% reserve for more particles. 
 // Note that if new particles are appended, the pool capacity will automatically 
 // grow when reserve is exhausted.
 // The argument of the Particle Clone() function is the address used by 
 // (placement) new when instantiating the cloned particle.
 // 
 // Note: space for a minimum of 128 particles is allways allocated.  
 //---------------------------------------------------------------------------------
  
  for (int i =0; i< nparticles; ++i )
  {
    bunch_.push_back(  p.Clone(pool_.malloc()) ); 
  }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
TBunch<Particle_t>::~TBunch<Particle_t>() 
{
  delete reference_;

  for ( iterator it = begin();          it != end();          ++it ) { it->dtor(); }
  for ( iterator it = removed_.begin(); it != removed_.end(); ++it ) { it->dtor(); }

}



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
typename TBunch<Particle_t>::iterator   TBunch<Particle_t>::begin()
{
  return bunch_.begin();
}
 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
typename TBunch<Particle_t>::iterator  TBunch<Particle_t>::end()
{
  return bunch_.end();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
typename TBunch<Particle_t>::const_iterator   TBunch<Particle_t>::begin() const
{
  return bunch_.begin();
}
 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
typename TBunch<Particle_t>:: const_iterator  TBunch<Particle_t>::end() const
{
  return bunch_.end();
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
typename TBunch<Particle_t>::const_iterator   TBunch<Particle_t>::removed_begin() const
{
  return removed_.begin();
}
 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
typename TBunch<Particle_t>:: const_iterator  TBunch<Particle_t>::removed_end() const
{
  return removed_.end();
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
typename TBunch<Particle_t>::reverse_iterator     TBunch<Particle_t>::rbegin()
{
  return bunch_.rbegin();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
typename TBunch<Particle_t>::const_reverse_iterator    TBunch<Particle_t>:: rbegin()  const
{
  return bunch_.rbegin();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
typename TBunch<Particle_t>::reverse_iterator          TBunch<Particle_t>:: rend()
{
  return bunch_.rend();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
typename TBunch<Particle_t>::const_reverse_iterator     TBunch<Particle_t>::rend()    const
{
  return bunch_.rend();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
typename TBunch<Particle_t>::const_reverse_iterator     TBunch<Particle_t>::removed_rbegin()  const
{
  return removed_.rbegin();
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
typename TBunch<Particle_t>::const_reverse_iterator     TBunch<Particle_t>::removed_rend()    const
{
  return removed_.rend();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
void TBunch<Particle_t>::clear()
{
 
  for ( iterator it = begin();          it != end();          ++it ) { it->dtor(); }
  for ( iterator it = removed_.begin(); it != removed_.end(); ++it ) { it->dtor(); }

  bunch_.clear();
  removed_.clear();

  //-------------------------------------------------------
  // The following is a workaround for the bug described 
  // at http://svn.boost.org/trac/boost/ticket/284
  // Apparently, the initial pool size does not get reset 
  // as it should when the pool memory is released. 
  // The fix will be in
  // boost release 1.35 - JFO
  //-------------------------------------------------------

  pool_.purge_memory();  
  pool_.set_next_size(1024);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
void TBunch<Particle_t>::append( Particle_t const& x )
{
  bunch_.push_back(  x.Clone(pool_.malloc()) ); 
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
double  TBunch<Particle_t>::Intensity() const 
{      
  return intensity_* (1.0 - (double(removed_size()) / double( size()) )); 
} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
void TBunch<Particle_t>::setIntensity( double const& value)
{      
  intensity_ = value * double( size() ) / double( size() - removed_size() );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
int TBunch<Particle_t>::size() const
{
  return bunch_.size();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
int TBunch<Particle_t>::removed_size() const
{
  return removed_.size();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
bool TBunch<Particle_t>::empty() const
{
  return bunch_.empty();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
void TBunch<Particle_t>::setReferenceParticle ( Particle_t const& p)
{
  if (reference_) delete reference_;
  reference_ = p.Clone(pool_.malloc());    // use Clone() to preserve dynamic type

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
Particle_t const& TBunch<Particle_t>::getReferenceParticle () const
{
  return *reference_;
}



