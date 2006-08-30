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
******                                                                
**************************************************************************
*************************************************************************/

#ifndef THINPOLES_H
#define THINPOLES_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class DLLEXPORT thin2pole : public bmlnElmnt
{
public:
  thin2pole();
  thin2pole( double /* strength = BL */ );
  thin2pole( const char*  /* name */,
             double /* strength */ );
  thin2pole( bmlnElmntData& );
  thin2pole( const thin2pole& );
  ~thin2pole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThin2pole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThin2pole( this ); }

  const char* Type() const;
  virtual bool isType(const char* c) 
  { if ( strcmp(c, "thin2pole") != 0 ) return bmlnElmnt::isType(c); else return true; }
  bool isMagnet() const;

  bmlnElmnt* Clone() const { return new thin2pole( *this ); }
} ;


class DLLEXPORT thin12pole : public bmlnElmnt
{
public:
  thin12pole();
  thin12pole( double /* strength */ );
  thin12pole( const char*  /* name */,
              double /* strength */ );
  thin12pole( bmlnElmntData& );
  thin12pole( const thin12pole& );
  ~thin12pole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThin12pole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThin12pole( this ); }

  const char* Type() const;
  virtual bool isType(const char* c) 
  { if ( strcmp(c, "thin12pole") != 0 ) return bmlnElmnt::isType(c); else return true; }
  bool isMagnet() const;

  bmlnElmnt* Clone() const { return new thin12pole( *this ); }
} ;

class DLLEXPORT thin14pole : public bmlnElmnt
{
public:
  thin14pole();
  thin14pole( double /* strength */ );
  thin14pole( const char*  /* name */,
              double /* strength */ );
  thin14pole( bmlnElmntData& );
  thin14pole( const thin14pole& );
  ~thin14pole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThin14pole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThin14pole( this ); }

  const char* Type() const;
  virtual bool isType(const char* c) 
  { if ( strcmp(c, "thin14pole") != 0 ) return bmlnElmnt::isType(c); else return true; }
  bool isMagnet() const;

  bmlnElmnt* Clone() const { return new thin14pole( *this ); }
} ;

class DLLEXPORT thin16pole : public bmlnElmnt
{
public:
  thin16pole();
  thin16pole( double /* strength */ );
  thin16pole( const char*  /* name */,
              double /* strength */ );
  thin16pole( bmlnElmntData& );
  thin16pole( const thin16pole& );
  ~thin16pole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThin16pole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThin16pole( this ); }

  const char* Type() const;
  virtual bool isType(const char* c) 
  { if ( strcmp(c, "thin16pole") != 0 ) return bmlnElmnt::isType(c); else return true; }
  bool isMagnet() const;

  bmlnElmnt* Clone() const { return new thin16pole( *this ); }
} ;

class DLLEXPORT thin18pole : public bmlnElmnt
{
public:
  thin18pole();
  thin18pole( double /* strength */ );
  thin18pole( const char*  /* name */,
              double /* strength */ );
  thin18pole( bmlnElmntData& );
  thin18pole( const thin18pole& );
  ~thin18pole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThin18pole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThin18pole( this ); }

  const char* Type() const;
  virtual bool isType(const char* c) 
  { if ( strcmp(c, "thin18pole") != 0 ) return bmlnElmnt::isType(c); else return true; }
  bool isMagnet() const;

  bmlnElmnt* Clone() const { return new thin18pole( *this ); }
} ;

class DLLEXPORT thinMultipole : public bmlnElmnt
{
public:
  thinMultipole();
  thinMultipole( double /* strength */ );
  thinMultipole( const char*  /* name */,
                double /* strength */ );
  thinMultipole( bmlnElmntData& );
  thinMultipole( const thinMultipole& );
  ~thinMultipole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThinMultipole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThinMultipole( this ); }

  const char* Type() const;
  virtual bool isType(const char* c) 
  { if ( strcmp(c, "thinMultipole") != 0 ) return bmlnElmnt::isType(c); else return true; }
  bool isMagnet() const;

  bmlnElmnt* Clone() const { return new thinMultipole( *this ); }
} ;

#endif // THINPOLES_H
