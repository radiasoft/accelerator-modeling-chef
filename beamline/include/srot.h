/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      srot.h
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
****** Mar 2007          ostiguy@fnal.gov
****** - use covariant return type
****** - added support for reference-counted elements
****** Dec 2007          ostiguy@fnal.gov
****** - new typesafe propagator scheme
****** May 2008 ostiguy@fnal
****** - proper, explicit assignment operator
****** - propagator moved (back) to base class
******
**************************************************************************
*************************************************************************/
#ifndef SROT_H
#define SROT_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class BmlVisitor;
class ConstBmlVisitor;

class srot;

typedef boost::shared_ptr<srot>             SRotPtr;
typedef boost::shared_ptr<srot const>  ConstSRotPtr;


class DLLEXPORT srot : public bmlnElmnt {

  class Propagator;

public:

  srot();
  srot( std::string const& name);
  srot( std::string const& name, double const& strength); // rotation angle in radians 
  srot( srot const& );

  srot* Clone() const { return new srot( *this ); }

 ~srot();

  srot& operator=(srot const&);

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  bool        isMagnet() const;
  char const*     Type() const;

};

#endif // SROT_H
