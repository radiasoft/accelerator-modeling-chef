/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Beamline:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      ParticleBunch.cc
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

#include <beamline/ParticleBunch.h>

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/MathConstants.h>

#include <cmath>
#include <ostream>
#include <iomanip>

#include<boost/random.hpp>

using namespace std;

using FNAL::pcout;
using FNAL::pcerr;


namespace {

typedef  boost::rand48    base_generator_type;

base_generator_type generator1((int32_t) 117u);
base_generator_type generator2((int32_t) 137u);

boost::variate_generator<base_generator_type&, boost::uniform_real<> > srnd(generator1,   boost::uniform_real<>(0.0, 1.0 )   );
boost::variate_generator<base_generator_type&, boost::uniform_real<> > trnd(generator2,   boost::uniform_real<>(0.0, 2*M_PI) );

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::ostream& operator<<( std::ostream &os, ParticleBunch const& bunch) 
{

  using std::setw;

  os << bunch.size() << endl;

  for (ParticleBunch::const_iterator it = bunch.begin(); it != bunch.end(); ++it )
  {
    const Vector state = it->State(); 
    os << setw(16) << it->ReferenceEnergy() << "  ";
    for (int i=0; i<6; ++i )
      os << setw(16) << state[i] << "  ";
    os << std::endl;
  }
  return os;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


std::istream& operator >>( std::istream &is, ParticleBunch& bunch) 
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

ParticleBunch::ParticleBunch(Particle const& p, int nparticles, double const& population)
:
  reference_(  p.Clone() ), 
  population_(population ), 
  bunch_(),
  pool_( sizeof(Particle) , max( 128, (nparticles*110/100) ))  

{

 //--------------------------------------------------------------------------------
 // For performance, particles objects need to be allocated as contiguously 
 // as possible in memory. This is accomplished by allocating particles 
 // from a dedicated pool with a 10% reserve for more particles. 
 // Note that if new particles are appended, the pool will automatically grow if the
 // reserve is exhausted.
 // The argument of the Particle Clone() function is the address used by 
 // (placement) new when instantiating the cloned particle.
 // 
 // Note: space for a minimum of 128 particles is allways allocated.  
 //---------------------------------------------------------------------------------
  
  for (int i =0; i< nparticles; ++i )
  {
    bunch_.push_back(  p.Clone( pool_.malloc() ) ); 
  }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ParticleBunch::~ParticleBunch() 
{
  delete reference_;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ParticleBunch::iterator   ParticleBunch::begin()
{
  return bunch_.begin();
}
 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ParticleBunch:: iterator  ParticleBunch::end()
{
  return bunch_.end();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ParticleBunch::const_iterator   ParticleBunch::begin() const
{
  return bunch_.begin();
}
 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ParticleBunch:: const_iterator  ParticleBunch::end() const
{
  return bunch_.end();
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ParticleBunch::append( Particle const& x )
{

  bunch_.push_back(  x.Clone( pool_.malloc() ) ); 

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//std::vector<ParticlePtr> ParticleBunch::remove( Discriminator& dsc )
//{}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double ParticleBunch::Population() const
{
  return population_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ParticleBunch::clear()
{
  clear();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int ParticleBunch::size() const
{
  return bunch_.size();
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool ParticleBunch::empty() const
{
  return bunch_.empty();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  ParticleBunch::populateGaussian( PhaseSpaceProjection psid, double sigma_x, double sigma_px, double r_12 )
{

  int M = 10; //  Strictly speaking, the limit M --> infty is the Gaussian, but 30 is more than good enough
              //  Cutoff will be at sqrt(10.5) = 3.2 sigma
    
  double x_lim  = sqrt(M + 0.5) *  sigma_x;
  double px_lim = sqrt(M + 0.5) *  sigma_px;
 
  populateBinomial ( psid,  M,  x_lim,  px_lim, r_12 ); 


}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  ParticleBunch::populateWaterBag ( PhaseSpaceProjection psid, double x_rms, double px_rms,  double r_12 )
{

  // Water Bag = uniform in phase space 

 double M = 1.0;

 double x_lim  = sqrt(M + 0.5) *  x_rms;
 double px_lim = sqrt(M + 0.5) *  px_rms;

 populateBinomial ( psid,  M,  x_lim,  px_lim, r_12 ); 

}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  ParticleBunch::populateParabolic( PhaseSpaceProjection psid, double x_rms, double px_rms,  double r_12 )
{

  double M = 2.0; 

  double x_lim  = sqrt(M + 0.5) *  x_rms;
  double px_lim = sqrt(M + 0.5) *  px_rms;

  populateBinomial ( psid,  M,  x_lim,  px_lim, r_12 ); 



}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  ParticleBunch::populateKV       ( PhaseSpaceProjection psid, double x_rms, double px_rms,  double r_12 )
{
  // KV = uniform projection  (KV in 2N phase space ==> uniform projection in N (momentum or coordinates) subspace    
  
  double M = 1.0e-9; // Stricly speaking, KV is the limit  M-->0, but this is pretty good. 
  populateBinomial ( psid,  M,  x_rms,  px_rms, r_12 ); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ParticleBunch::populateBinomial ( PhaseSpaceProjection psid, double M, double x_lim, double px_lim, double r_12)
{

 double chi    = asin(r_12);

 double a     = 0.0;
 double alpha = 0.0;
 double u     = 0.0;
 double v     = 0.0;
 double x     = 0.0;
 double px    = 0.0;
  
 std::vector<std::pair<double,double> > phase_space;

 

 for ( iterator it  = bunch_.begin(); it != bunch_.end(); ++it ) {

    a     = sqrt(1.0 - pow( srnd(), 1.0/M ));

    alpha = trnd();

    u     = a*cos(alpha);
    v     = a*sin(alpha);

    Vector& state = it->getState(); 
 
    state[0+psid]    += x_lim*u;
    state[3+psid]    += px_lim*(u*sin(chi) + v*cos(chi)); 

 }

 return;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ParticleBunch::setReferenceParticle ( Particle const& p)
{

  if (reference_) delete reference_;

  reference_ = p.Clone();       // use Clone() to preserve dynamic type

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Particle const& ParticleBunch::getReferenceParticle () const
{
  return *reference_;
}

