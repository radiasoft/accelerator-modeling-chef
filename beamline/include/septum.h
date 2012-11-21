/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      septum.h
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
******                All Rights Reserved                             
******                                                                
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
******  REVISION HISTORY
******  Mar 2007           ostiguy@fnal.gov
****** - use covariant return types
****** - support for reference counted elements
******
******  Dec 2007           ostiguy@fnal.gov
****** - new typesafe propagator scheme
******
******  Sep 2012           cspark@fnal.gov
****** - new class septum added for finite length
**************************************************************************
*************************************************************************/

#ifndef SEPTUM_H
#define SEPTUM_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class BmlVisitor;
class ConstBmlVisitor;

class septum;
class thinSeptum;

typedef boost::shared_ptr<septum>                     SeptumPtr;
typedef boost::shared_ptr<septum const>          ConstSeptumPtr;
typedef boost::shared_ptr<thinSeptum>             ThinSeptumPtr;
typedef boost::shared_ptr<thinSeptum const>  ConstThinSeptumPtr;

class DLLEXPORT septum : public bmlnElmnt
{

  class Propagator;

public:

  typedef boost::shared_ptr<BasePropagator<septum> > PropagatorPtr;

  septum( char const* name, double const& length );

  septum( char const* name, double const& length, double const& voltage, double const& g );

  septum( char const* name, double const& length, double const& voltage, double const& g, double const& xw );

  septum( char const* name, double const& length, double const& voltage, double const& g, double const& xw, double const& ww );

  septum( septum const& );

  septum* Clone() const { return new septum( *this ); }

  septum& operator=( septum const& rhs);

 ~septum();

  void setVoltage( double const& x );
  void setGap( double const& x );
  void setWire( double const& x);
  void setWireWidth( double const& x );
  void setReportNumber( int const& );

  double const& getVoltage()     { return voltage_;     }
  double const& getGap()         { return gap_;         }
  double const& getWireX()       { return xWire_;       }
  double const& getWireWidth()   { return wireWidth_;   }

  void localPropagate(         Particle&  p );
  void localPropagate(      JetParticle&  p );
  void localPropagate(    ParticleBunch&  p );
  void localPropagate( JetParticleBunch&  p );

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  bool    isMagnet() const;
  const char* Type() const;

  int numberKicked_;      // ??? TODO: SHOULD BE PRIVATE ???
  int numberBadHits_;     // ??? TODO: SHOULD BE PRIVATE ???
  int numberBackHits_;    // ??? TODO: SHOULD BE PRIVATE ???
  int numberOutGap_;      // ??? TODO: SHOULD BE PRIVATE ???
  int turnNumber_;        // ??? TODO: SHOULD BE PRIVATE ???
  int reportNumber_;      // ??? TODO: SHOULD BE PRIVATE ???

private:

  septum();               // default constructor forbidden

  //double length_;         // length of septum element
  double voltage_;        // voltage of septum wire/foil in kV
  double gap_;            // gap between wire and anode in meters
  double xWire_;          // position of wire septum in meters
  double wireWidth_;      // width of wire in meters

  PropagatorPtr  propagator_;

  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

};

class DLLEXPORT thinSeptum : public bmlnElmnt
{

  class Propagator; 

public:

  typedef boost::shared_ptr<BasePropagator<thinSeptum> > PropagatorPtr;   

  thinSeptum( char const*  name );
  
  thinSeptum( char const*  name,
	    double const& sPos,    // kick in strength in radians for x > xWire 
	    double const& sNeg,    // kick in strength in radians for x < xWire 
	    double const& x );     // position of wire septum in meters
   
  thinSeptum( double const& sPos,    
	    double const& sNeg,    
	    double const& x ); 
  
  thinSeptum( thinSeptum const& );

  thinSeptum* Clone() const { return new thinSeptum( *this ); }

  thinSeptum& operator=( thinSeptum const& rhs);

 ~thinSeptum();
  
  void setStrengths( double const& sPos, double const& sNeg); 
  void setWire( double const& x); 
  void setWireWidth( double const& x );
  void setGap( double const& x );
  void setReportNumber( int const& );

  double const& getPosStrength() { return strengthPos_; }
  double const& getNegStrength() { return strengthNeg_; }
  double const& getWireX()       { return xWire_;       }
  double const& getWireWidth()   { return wireWidth_;   }
  double const& getGap()         { return gap_;         }
  
  void localPropagate(         Particle&  p );
  void localPropagate(      JetParticle&  p );
  void localPropagate(    ParticleBunch&  p );
  void localPropagate( JetParticleBunch&  p );

  void accept( BmlVisitor& v ); 
  void accept( ConstBmlVisitor& v ) const; 

  bool    isMagnet() const;
  const char* Type() const;

  int numberKicked_;      // ??? TODO: SHOULD BE PRIVATE ???
  int numberBadHits_;     // ??? TODO: SHOULD BE PRIVATE ???
  int numberBackHits_;    // ??? TODO: SHOULD BE PRIVATE ???
  int numberOutGap_;      // ??? TODO: SHOULD BE PRIVATE ???
  int turnNumber_;        // ??? TODO: SHOULD BE PRIVATE ???
  int reportNumber_;      // ??? TODO: SHOULD BE PRIVATE ???

private:
 
  thinSeptum();           // default constructor forbidden

  double strengthPos_;    // kick in strength in radians for x > xWire
  double strengthNeg_;	  // kick in strength in radians for x < xWire
  double xWire_;	  // position of wire septum in meters
  double wireWidth_;      // width of wire in meters
  double gap_;            // gap between wire and anode in meters

  PropagatorPtr  propagator_; 
  
  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

};
 
#endif // SEPTUM_H
