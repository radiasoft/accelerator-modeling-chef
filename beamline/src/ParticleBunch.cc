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
******  Copyright (c) 1991 Universities Research Association, Inc.    
******                All Rights Reserved                             
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
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <ParticleBunch.h>

#include <math.h>
#include <MathConstants.h>

using namespace std;


ParticleBunch::Discriminator::Discriminator( const ParticleBunch* x )
: _within(x)
{

}


ParticleBunch::Discriminator::Discriminator( const Discriminator& x )
: _within( x._within )
{
}


ParticleBunch::ParticleBunch()
{

}


ParticleBunch::~ParticleBunch() 
{
 
 // No need to explicitly invoke delete on a shared_ptr.  
 // delete( shared_ptr ) is called when the stl container is deleted.

}


void ParticleBunch::append( const Particle& x )
{

  _bag.push_back( ParticlePtr( x.Clone()) ); 

}


void ParticleBunch::append( ParticlePtr p )
{
  _bag.push_back( p ); 
} 


list<ParticlePtr> ParticleBunch::remove( Discriminator& dsc )
{
  //list<Particle*>::iterator collector 
  //    = remove_if ( _bag.begin(), _bag.end(), dsc );

  // This is kludged, until I can figure out how
  // to make std algorithms do what I want them to do.
  // This really seems horribly inefficient
  // compared to using a dlist_traversor

  list<ParticlePtr> marked;
  list<ParticlePtr>::iterator cur;
  for( cur  = _bag.begin();
       cur != _bag.end();
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
      = _bag.erase( std::remove( _bag.begin(), _bag.end(), *cur ), _bag.end() );
  }
  
  return marked;
}


void ParticleBunch::empty()
{
  this->clear();
}


void ParticleBunch::clear()
{
  if( !_bag.empty() ) { _bag.clear(); }
}


int ParticleBunch::size() const
{
  return _bag.size();
}


bool ParticleBunch::isEmpty() const
{
  return _bag.empty();
}


// FAILS: list<Particle*> ParticleBunch::remove( Discriminator& dsc )
// FAILS: {
// FAILS:   list<Particle*>::iterator new_end
// FAILS:     = remove_if( _bag.begin(), _bag.end(), dsc );
// FAILS:   // Contrary to its name, remove_if actually removes nothing.
// FAILS: 
// FAILS:   list<Particle*> ret;
// FAILS:   list<Particle*>::iterator cur;
// FAILS:   for( cur  = new_end; cur != _bag.end(); cur++ ) {
// FAILS:     ret.push_back( *cur ); 
// FAILS:   }
// FAILS:   _bag.erase( new_end, _bag.end() );
// FAILS: 
// FAILS:   return ret;
// FAILS: }



ParticleBunch::Iterator::Iterator( ParticleBunch& x )
{
  _cur = x._bag.begin();
  _ref = &x;
}


ParticleBunch::Iterator::Iterator( const Iterator& )
{
  throw( Particle::GenericException( __FILE__, __LINE__, 
         "ParticleBunch::Iterator::Iterator( const Iterator& )", 
         "Copy constructor may not be called for this object." ) );
}


ParticleBunch::Iterator::~Iterator()
{
}




const Particle* ParticleBunch::Iterator::next()
{

  if( _cur == _ref->_bag.end() ) { 
    return 0;
  }
  else {
    Particle* ptr = (*_cur).get();
    _cur++;
    return ptr; 
  }
  

}


void ParticleBunch::Iterator::reset()
{
  _cur = _ref->_bag.begin();
}



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



Particle* ProtonBunch::makeParticle( double energy, double* state )
{

  return new Proton( energy, state );

}


std::ostream& 
operator<<( std::ostream &os, const ParticleBunch& bunch) {

  // Note: ParticleBunch& bunch should be declared const ParticleBunch& bunch
  // Unfortunately,  const declarations are not consistent  

  using std::setw;

  double state[6];

  os << bunch.size() << endl;
 
  ParticleBunch::Iterator it( const_cast<ParticleBunch&>(bunch) );  

  while ( const Particle* p  = it.next()  ) {  

    const_cast<Particle*>(p)->getState( state ); // work around broken getState() signature.
    
    os << setw(16) << p->ReferenceEnergy() << "  ";

    for (int i=0; i<6; ++i )
      os << setw(16) << state[i] << "  ";
 
    os << std::endl;
  }

  return os;

}

//....................................................................................................


std::istream& 
operator >>( std::istream &is, ParticleBunch& bunch) {

  using std::setw;

    double state[6];
  double reference_energy = 0.0; 


  int counter = 0;
  int nsize   = 0;

  is >> nsize;  

  while ( (!is.eof()) && (counter++ < nsize) ) { 

    is >> reference_energy;  

    for (int i=0; i<6; ++i) {
       is >> state[i]; 
    };     
    
    bunch.append( ParticlePtr(bunch.makeParticle(reference_energy,  state)));
    
  }

  return is;

}





/*** OLD CODE ***

ProtonBunch::ProtonBunch(int nm, double energy, double* widths,
			 Distribution& distrib,
                         BunchPredicate* pBunchPredicate ): ParticleBunch() 
{
 Proton* p;
 double x[ BMLN_dynDim ];

 int bProtonAllocated = false;

 while( iNOfTracked_ < nm ) {
   
   if ( !bProtonAllocated ) {  
     p = new Proton(energy);
   }
   x[0] = distrib.getValue()*widths[0];
   x[1] = distrib.getValue()*widths[1];
   x[2] = distrib.getValue()*widths[2];
   x[3] = distrib.getValue()*widths[3];
   x[4] = distrib.getValue()*widths[4];
   x[5] = distrib.getValue()*widths[5];
   p->setState( x );
   
   if (( pBunchPredicate != 0 ) && ( (*pBunchPredicate)( *p ) == 0 )) {
     bProtonAllocated = true;
   } else {
     append( p );
     bProtonAllocated = false;
     iNOfTracked_++;
   }
   iNOfTotal_++;
 }
}

ProtonBunch::ProtonBunch(int nm, double energy, double* widths, double* offsets,
                         Distribution& distrib,
                         BunchPredicate* pBunchPredicate ): ParticleBunch() {
 Proton* p;
 double x[ BMLN_dynDim ];

 while( iNOfTracked_ < nm ) {
   
   p = new Proton(energy);
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
}

void ProtonBunch::recreate( int nm, double energy, double* widths,
			    Distribution& distrib,
                            BunchPredicate* pBunchPredicate ) {
 Proton* p;
 double x[ BMLN_dynDim ];
				// First get rid of the old particles
 slist_iterator getNext( *(slist*) this );
 while( (p = (Proton*) getNext()) )delete p;
 clear();

 iNOfTracked_ = 0;
 iNOfTotal_   = 0;

 while( iNOfTracked_ < nm ) {
   
   p = new Proton(energy);
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
}

void ProtonBunch::recreate(int nm, double energy, double* widths, double* offsets,
                           Distribution& distrib, BunchPredicate* pBunchPredicate ) {
 Proton* p;
 double x[ BMLN_dynDim ];

				// First get rid of the old particles
 slist_iterator getNext( *(slist*) this );
 while( (p = (Proton*) getNext()) ) delete p;
 clear();

 iNOfTracked_ = 0;
 iNOfTotal_   = 0;
 
 while( iNOfTracked_ < nm ) {
   
   p = new Proton(energy);
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
}

ProtonBunch::ProtonBunch( int, int nm, char t, 
                          double time_h, double energy_h ) : ParticleBunch() {
 int i;
 Proton* p;
 double x[ BMLN_dynDim ];
 x[0] = x[1] = x[3] = x[4] = 0.0;

 srand48(0);     // has to be before the loop

 double random, rad_fract, theta;
 for( i = 0; i < nm; i++ ) {
  p = new Proton;
  
  switch (t){
  case 'R':
    random =  lrand48()/MAX_RANDOM;
    rad_fract = sqrt(1. - pow(1. - random, 2./3.) );
    theta = M_TWOPI * lrand48()/MAX_RANDOM;

    x[2] = time_h * rad_fract * cos(theta);
    x[5] = energy_h * rad_fract * sin(theta);

    break;

  default: printf("No implementation yet for types other than R" );
    break;
  }

  p->setState( x );
  append( p );
 }
}

ElectronBunch::ElectronBunch() : ParticleBunch() {
}
 
ElectronBunch::ElectronBunch(int nm, double energy, double* widths,
                             Distribution& distrib,
                             BunchPredicate* pBunchPredicate ) : ParticleBunch() {
                               
 Electron* p;
 double x[ BMLN_dynDim ];

 while( iNOfTracked_ < nm ) {
   
   p = new Electron(energy);
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
}

ElectronBunch::ElectronBunch(int nm, double energy, double* widths,
			     double* offsets,Distribution& distrib,
                             BunchPredicate* pBunchPredicate ) : ParticleBunch() {
 Electron* p;
 double x[ BMLN_dynDim ];

 while( iNOfTracked_ < nm ) {
   
   p = new Electron(energy);
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
}

ElectronBunch::~ElectronBunch() {
}

void ElectronBunch::recreate(int nm, double energy, double* widths,
			     Distribution& distrib, BunchPredicate* pBunchPredicate ) {
 Electron* p;
 double x[ BMLN_dynDim ];
				// First get rid of the old particles
 slist_iterator getNext( *(slist*) this );
 while( (p = (Electron*) getNext()) ) delete p;

 clear();
 iNOfTracked_ = 0;
 iNOfTotal_   = 0;

 while( iNOfTracked_ < nm ) {
   
   p = new Electron(energy);
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
}

void ElectronBunch::recreate(int nm, double energy, double* widths, double* offsets,
                             Distribution& distrib, BunchPredicate* pBunchPredicate ) {
 Electron* p;
 double x[ BMLN_dynDim ];

				// First get rid of the old particles
 slist_iterator getNext( *(slist*) this );
 while( (p = (Electron*) getNext()) ) delete p;
 clear();

 iNOfTracked_ = 0;
 iNOfTotal_   = 0;

 while( iNOfTracked_ < nm ) {

   p = new Electron(energy);
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
}
*/
