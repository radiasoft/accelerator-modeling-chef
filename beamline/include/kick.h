/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      kick.h
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
****** REVISION HISTORY
******
****** Mar 2007:          ostiguy@fnal.gov
****** - use covariant return types
****** - support for reference counted elements
****** Dec  2007:          ostiguy@fnal.gov
****** - new typesafe propagators
****** May 2008 ostiguy@fnal
****** - proper, explicit assignment operator
****** - propagator moved (back) to base class
****** - no assumption about internal structure
*****
**************************************************************************
*************************************************************************/

#ifndef KICK_H
#define KICK_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class BmlVisitor;
class ConstBmlVisitor;

class hkick;
typedef boost::shared_ptr<hkick>        HKickPtr;
typedef boost::shared_ptr<hkick const>  ConstHKickPtr;

class vkick;
typedef boost::shared_ptr<vkick>        VKickPtr;
typedef boost::shared_ptr<vkick const>  ConstVKickPtr;

class kick;
typedef boost::shared_ptr<kick>        KickPtr;
typedef boost::shared_ptr<kick const>  ConstKickPtr;

class DLLEXPORT hkick : public bmlnElmnt {

  class Propagator;

public:


  hkick();
  hkick( std::string const& name);                                         // name; assumes zero kick
  hkick( std::string const& name,                 double const& kick);     // kick size in radians
  hkick( std::string const& name,  double const& length, double const& kick);     // kick size in radians
  hkick( hkick const& );

  hkick* Clone() const { return new hkick( *this ); }

  hkick& operator=( hkick const& rhs);

 ~hkick();

  const char* Type()       const;
  bool        isMagnet()   const;

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

};



class DLLEXPORT vkick : public bmlnElmnt {

  class Propagator;

public:

  vkick();                                                             // Assumes zero kick
  vkick( std::string const& );                                                // name; assumes zero kick
  vkick( std::string const&  name, double const& kick);                       // kick size in radians
  vkick( std::string const&,double const& length, double const& kick );       // kick size in radians
  vkick( vkick const& );

  vkick* Clone() const { return new vkick( *this ); }

  vkick& operator=( vkick const& rhs);

 ~vkick();

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type()       const;
  bool        isMagnet()   const;

};


class DLLEXPORT kick : public bmlnElmnt {

  class Propagator;

public:

  kick();
  kick( std::string const& name );
  kick( std::string const& name,                         double const& horizontal_kick, double const& vertical_kick);
  kick( std::string const& name,   double const& length, double const& horizontal_kick, double const& vertical_kick);

  kick( kick const& );

  kick* Clone() const { return new kick( *this ); }

  kick& operator=( kick const& rhs);

 ~kick();

  void accept(BmlVisitor& v);
  void accept(ConstBmlVisitor& v) const;

  double const& getHorStrength() const;
  double        getVerStrength() const;

  void setHorStrength( double const& value);  
  void setVerStrength( double const& value);   

  const char* Type()       const;
  bool        isMagnet()   const;

private:

  double vh_ratio_;

  std::istream& readFrom(std::istream&);
  std::ostream& writeTo(std::ostream&);

 };

#endif    // KICK_H
