/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      thinpoles.h
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
******
******  Mar 2007 ostiguy@fnal.gov 
******  - added support for reference counted elements
******  Dec 2007 ostiguy@fnal.gov
******  - streamlined constructors      
******  - common base class 
******  - new type-safe propagator scheme    
******  May 2008 ostiguy@fnal
******  - proper, explicit assignment operator
******  - propagator moved (back) to base class
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


class DLLEXPORT ThinPole : public bmlnElmnt {

protected:

  class Propagator;

public:

  ThinPole( int pole );
  ThinPole( std::string const& name, double const& integrated_strength, int pole);
  ThinPole( ThinPole const& );

  ThinPole* Clone() const = 0;

 ~ThinPole();

  ThinPole& operator=( ThinPole const& rhs);

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type() const;
  bool isMagnet() const;

  int getPole() const; 

 protected:
 
  int           pole_; 
};


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

class DLLEXPORT thin2pole : public ThinPole {

public:

  thin2pole();
  thin2pole( std::string const& name, double const& integrated_strength);
  thin2pole( thin2pole const& );

  thin2pole* Clone() const { return new thin2pole( *this ); }

 ~thin2pole();

  thin2pole& operator=( thin2pole const& rhs);

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type() const;

};


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


class DLLEXPORT thin12pole : public ThinPole {

public:

  thin12pole();
  thin12pole( std::string const& name, double const& strength);
  thin12pole( thin12pole const& );
  thin12pole* Clone() const { return new thin12pole( *this ); }

 ~thin12pole();

  thin12pole& operator=( thin12pole const& rhs);

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type() const;

} ;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

class DLLEXPORT thin14pole : public ThinPole {

public:

  thin14pole();
  thin14pole( std::string const& name, double const& strength);
  thin14pole( thin14pole const& );
  thin14pole* Clone() const { return new thin14pole( *this ); }

 ~thin14pole();

  thin14pole& operator=( thin14pole const& rhs);

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type() const;

} ;


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

class DLLEXPORT thin16pole : public ThinPole {

public:

  thin16pole();

  thin16pole( std::string const& name, double const& strength );
  thin16pole( thin16pole const & );
  thin16pole* Clone() const { return new thin16pole( *this ); }
 ~thin16pole();

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type() const;

} ;

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


class DLLEXPORT thin18pole : public ThinPole {

public:

  thin18pole();
  thin18pole( std::string const& name, double const&  strength );
  thin18pole( thin18pole const& );
  thin18pole* Clone() const { return new thin18pole( *this ); }

 ~thin18pole();

  thin18pole& operator=( thin18pole const& rhs);

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type() const;

};


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


#endif // THINPOLES_H
