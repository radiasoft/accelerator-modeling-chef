/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Beamline:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      ParticleBunch.cc
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
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
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <beamline/ParticleBunch.h>

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/MathConstants.h>
#include <cmath>
#include <ostream>
#include <iomanip>

using namespace std;

using FNAL::pcout;
using FNAL::pcerr;


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//  Class ParticleBunch
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////

ParticleBunch::Discriminator::Discriminator( const ParticleBunch* x )
: _within(x)
{}


ParticleBunch::Discriminator::Discriminator( const Discriminator& x )
: _within( x._within )
{}


ParticleBunch::ParticleBunch()
{}


ParticleBunch::~ParticleBunch() 
{
 
 // No need to explicitly invoke delete on a shared_ptr.  
 // delete( shared_ptr ) is called when the stl container is deleted.

}


ParticleBunch::iterator   ParticleBunch::begin()
{
  return bunch_.begin();
}
 
ParticleBunch:: iterator  ParticleBunch::end()
{
  return bunch_.end();
}

ParticleBunch::const_iterator   ParticleBunch::begin() const
{
  return bunch_.begin();
}
 
ParticleBunch:: const_iterator  ParticleBunch::end() const
{
  return bunch_.end();
}


void ParticleBunch::append( const Particle& x )
{
  bunch_.push_back( ParticlePtr( x.Clone()) ); 
}


void ParticleBunch::append( ParticlePtr p )
{
  bunch_.push_back( p ); 
} 


list<ParticlePtr> ParticleBunch::remove( Discriminator& dsc )
{
  //list<Particle*>::iterator collector 
  //    = remove_if ( bunch_.begin(), bunch_.end(), dsc );

  // This is kludged, until I can figure out how
  // to make std algorithms do what I want them to do.
  // This really seems horribly inefficient
  // compared to using a dlist_traversor

  list<ParticlePtr> marked;
  list<ParticlePtr>::iterator cur;
  for( cur  = bunch_.begin();
       cur != bunch_.end();
       cur++                ) 
  {
    if( dsc( *cur ) ) { 
      marked.push_back( *cur ); 
    }
  }

  for( cur  = marked.begin();
       cur != marked.end();
       cur++                  )
  {
    list<ParticlePtr>::iterator new_end 
      = bunch_.erase( std::remove( bunch_.begin(), bunch_.end(), *cur ), bunch_.end() );
  }
  
  return marked;
}


void ParticleBunch::clear()
{
  clear();
}


int ParticleBunch::size() const
{
  return bunch_.size();
}


bool ParticleBunch::empty() const
{
  return bunch_.empty();
}


// FAILS: list<Particle*> ParticleBunch::remove( Discriminator& dsc )
// FAILS: {
// FAILS:   list<Particle*>::iterator new_end
// FAILS:     = remove_if( bunch_.begin(), bunch_.end(), dsc );
// FAILS:   // Contrary to its name, remove_if actually removes nothing.
// FAILS: 
// FAILS:   list<Particle*> ret;
// FAILS:   list<Particle*>::iterator cur;
// FAILS:   for( cur  = new_end; cur != bunch_.end(); cur++ ) {
// FAILS:     ret.push_back( *cur ); 
// FAILS:   }
// FAILS:   bunch_.erase( new_end, bunch_.end() );
// FAILS: 
// FAILS:   return ret;
// FAILS: }




std::ostream& operator<<( std::ostream &os, ParticleBunch const& bunch) 
{

  using std::setw;

  os << bunch.size() << endl;

  for (ParticleBunch::const_iterator it = bunch.begin(); it != bunch.end(); ++it )
  {
    const Vector state = (*it)->State(); 
    os << setw(16) << (*it)->ReferenceEnergy() << "  ";
    for (int i=0; i<6; ++i )
      os << setw(16) << state[i] << "  ";
    os << std::endl;
  }
  return os;
}


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


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//  Class ProtonBunch
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////

ProtonBunch::ProtonBunch() : ParticleBunch() 
{
}
 

ProtonBunch::~ProtonBunch()
{

}
 

void ProtonBunch::append( const Proton& p) 
{
  ParticleBunch::append(p);
}


void ProtonBunch::append( ProtonPtr p) 
{
  ParticleBunch::append(p);
}



Proton* ProtonBunch::makeParticle( double energy, Vector const& state )
{
  return new Proton( energy, state );
}


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//  Class PositronBunch
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////

PositronBunch::PositronBunch() : ParticleBunch() 
{
}
 

PositronBunch::PositronBunch(int nm, double energy, double* widths,
                             Distribution& distrib,
                             ParticleBunch::Discriminator* pBunchPredicate )
: ParticleBunch() 
{
 #if 0                               
 Positron* p;
 double x[ BMLN_dynDim ];

 while( iNOfTracked_ < nm ) {
   
   p = new Positron(energy);
   x[0] = distrib.getValue()*widths[0];
   x[1] = distrib.getValue()*widths[1];
   x[2] = distrib.getValue()*widths[2];
   x[3] = distrib.getValue()*widths[3];
   x[4] = distrib.getValue()*widths[4];
   x[5] = distrib.getValue()*widths[5];
   p->setState( x );
   
   if (( pBunchPredicate != 0 ) && ( (*pBunchPredicate)( *p ) == 0 )) {
     delete p;
   } else {
     append( p );
     iNOfTracked_++;
   }
   iNOfTotal_++;   
 }
 #endif
}


PositronBunch::PositronBunch(int nm, double energy, double* widths,
			     double* offsets,Distribution& distrib,
                             ParticleBunch::Discriminator* pBunchPredicate ) 
: ParticleBunch() 
{
 #if 0
 Positron* p;
 double x[ BMLN_dynDim ];

 while( iNOfTracked_ < nm ) {
   
   p = new Positron(energy);
   x[0] = distrib.getValue()*widths[0] + offsets[0];
   x[1] = distrib.getValue()*widths[1] + offsets[1];
   x[2] = distrib.getValue()*widths[2] + offsets[2];
   x[3] = distrib.getValue()*widths[3] + offsets[3];
   x[4] = distrib.getValue()*widths[4] + offsets[4];
   x[5] = distrib.getValue()*widths[5] + offsets[5];
   p->setState( x );
   
   if (( pBunchPredicate != 0 ) && ( (*pBunchPredicate)( *p ) == 0 )) {
     delete p;
   } else {
     append( p );
     iNOfTracked_++;
   }
   iNOfTotal_++;   
 }
 #endif
}


PositronBunch::~PositronBunch() 
{
}


void PositronBunch::recreate(int nm, double energy, double* widths,
			     Distribution& distrib, 
                             ParticleBunch::Discriminator* pBunchPredicate ) 
{
 #if 0
 Positron* p;
 double x[ BMLN_dynDim ];
				// First get rid of the old particles
 slist_iterator getNext( *(slist*) this );
 while( (p = (Positron*) getNext()) ) delete p;

 clear();
 iNOfTracked_ = 0;
 iNOfTotal_   = 0;

 while( iNOfTracked_ < nm ) {
   
   p = new Positron(energy);
   x[0] = distrib.getValue()*widths[0];
   x[1] = distrib.getValue()*widths[1];
   x[2] = distrib.getValue()*widths[2];
   x[3] = distrib.getValue()*widths[3];
   x[4] = distrib.getValue()*widths[4];
   x[5] = distrib.getValue()*widths[5];   
   p->setState( x );
   
   if (( pBunchPredicate != 0 ) && ( (*pBunchPredicate)( *p ) == 0 )) {
     delete p;
   } else {
     append( p );
     iNOfTracked_++;
   }
   iNOfTotal_++;   
 }
 #endif
}


void PositronBunch::recreate(int nm, double energy, double* widths, double* offsets,
                             Distribution& distrib, 
                             ParticleBunch::Discriminator* pBunchPredicate )
{
 #if 0
 Positron* p;
 double x[ BMLN_dynDim ];

				// First get rid of the old particles
 slist_iterator getNext( *(slist*) this );
 while( (p = (Positron*) getNext()) ) delete p;
 clear();

 iNOfTracked_ = 0;
 iNOfTotal_   = 0;

 while( iNOfTracked_ < nm ) {

   p = new Positron(energy);
   x[0] = distrib.getValue()*widths[0] + offsets[0];
   x[1] = distrib.getValue()*widths[1] + offsets[1];
   x[2] = distrib.getValue()*widths[2] + offsets[2];
   x[3] = distrib.getValue()*widths[3] + offsets[3];
   x[4] = distrib.getValue()*widths[4] + offsets[4];
   x[5] = distrib.getValue()*widths[5] + offsets[5];
   p->setState( x );
   
   if (( pBunchPredicate != 0 ) && ( (*pBunchPredicate)( *p ) == 0 )) {
     delete p;
   } else {
     append( p );
     iNOfTracked_++;
   }
   iNOfTotal_++;
 }
 #endif
}


void PositronBunch::append( const Positron& p) 
{
  ParticleBunch::append(p);
}


void PositronBunch::append( PositronPtr p) 
{
  ParticleBunch::append(p);
}



Positron* PositronBunch::makeParticle( double energy, Vector const& state )
{
  return new Positron( energy, state );
}


