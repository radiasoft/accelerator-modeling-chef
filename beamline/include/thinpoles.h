/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      thinpoles.h
******  Version:   2.1
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
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
******  REVISION HISTORY
******
******  Dec 2006 - Feb 2007  Jean-Francois Ostiguy
******                       ostiguy@fnal.gov
******
******  - Clone() using covariant return type
******  - adapted to new beamline container based on shared_ptr
******  - member signatures based on reference types.
******  Mar 2007 
******  - added support for reference counted elements
******                                           
**************************************************************************
*************************************************************************/

#ifndef THINPOLES_H
#define THINPOLES_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class BmlVisitor;
class ConstBmlVisitor;

class thin2pole;
class thin12pole;
class thin14pole;
class thin16pole;
class thin18pole;
class thinMultipole;

typedef boost::shared_ptr<thin2pole>                Thin2polePtr;
typedef boost::shared_ptr<thin2pole  const>    ConstThin2polePtr;

typedef boost::shared_ptr<thin12pole>              Thin12polePtr;
typedef boost::shared_ptr<thin12pole const>   ConstThin12polePtr;

typedef boost::shared_ptr<thin14pole>              Thin14polePtr;
typedef boost::shared_ptr<thin14pole const>   ConstThin14polePtr;

typedef boost::shared_ptr<thin16pole>              Thin16polePtr;
typedef boost::shared_ptr<thin16pole const>   ConstThin16polePtr;

typedef boost::shared_ptr<thin18pole>              Thin18polePtr;
typedef boost::shared_ptr<thin18pole const>   ConstThin18polePtr;

typedef boost::shared_ptr<thinMultipole>            ThinMultipolePtr;
typedef boost::shared_ptr<thinMultipole const> ConstThinMultipolePtr;


class DLLEXPORT thin2pole : public bmlnElmnt {

public:

  thin2pole();
  thin2pole( double const& /* strength = BL */ );
  thin2pole( const char*  /* name */,
             double const& /* strength */ );
  thin2pole( const thin2pole& );
  thin2pole* Clone() const { return new thin2pole( *this ); }
  virtual ~thin2pole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type() const;
  bool isMagnet() const;

} ;

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


class DLLEXPORT thin12pole : public bmlnElmnt
{
public:
  thin12pole();
  thin12pole( double const& /* strength */ );
  thin12pole( const char*  /* name */,
              double const& /* strength */ );
  thin12pole( const thin12pole& );
  thin12pole* Clone() const { return new thin12pole( *this ); }
  virtual ~thin12pole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type() const;
  bool isMagnet() const;

} ;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

class DLLEXPORT thin14pole : public bmlnElmnt
{
public:
  thin14pole();
  thin14pole( double const& /* strength */ );
  thin14pole( const char*  /* name */,
              double const& /* strength */ );
  thin14pole( const thin14pole& );
  thin14pole* Clone() const { return new thin14pole( *this ); }
  virtual ~thin14pole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type() const;
  bool isMagnet() const;

} ;


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

class DLLEXPORT thin16pole : public bmlnElmnt
{
public:
  thin16pole();
  thin16pole( double const& /* strength */ );
  thin16pole( const char*  /* name */,
              double const& /* strength */ );
  thin16pole( const thin16pole& );
  thin16pole* Clone() const { return new thin16pole( *this ); }
  virtual ~thin16pole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type() const;
  bool isMagnet() const;

} ;

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


class DLLEXPORT thin18pole : public bmlnElmnt
{
public:
  thin18pole();
  thin18pole( double const& /* strength */ );
  thin18pole( const char*  /* name */,
              double const& /* strength */ );
  thin18pole( const thin18pole& );
  thin18pole* Clone() const { return new thin18pole( *this ); }
  virtual ~thin18pole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type() const;
  bool isMagnet() const;

} ;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

class DLLEXPORT thinMultipole : public bmlnElmnt
{
public:
  thinMultipole();
  thinMultipole( double const& /* strength */ );
  thinMultipole( const char*  /* name */,
                double const& /* strength */ );
  thinMultipole( thinMultipole const& );
  thinMultipole* Clone() const { return new thinMultipole( *this ); }
  virtual ~thinMultipole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type() const;
  bool isMagnet() const;

} ;

#endif // THINPOLES_H
