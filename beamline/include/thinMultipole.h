/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      thinMultipole.h
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
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov                         
******                                           
****** REVISION HISTORY
****** May 2008 ostiguy@fnal
******  - proper, explicit assignment operator
******  - propagator moved (back) to base class
******  - added pole iterators
**************************************************************************
*************************************************************************/

#ifndef THINMULTIPOLE_H
#define THINMULTIPOLE_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>
#include <map>

class BmlVisitor;
class ConstBmlVisitor;

class thinMultipole;

typedef boost::shared_ptr<thinMultipole>            ThinMultipolePtr;
typedef boost::shared_ptr<thinMultipole const> ConstThinMultipolePtr;

class DLLEXPORT thinMultipole : public bmlnElmnt {

  class Propagator;
 
  typedef std::map<int, std::complex<double> >::const_iterator const_iterator; 

public:

  thinMultipole(); 
  thinMultipole( std::string const& name ); 
  thinMultipole( thinMultipole const& );

  thinMultipole* Clone() const { return new thinMultipole( *this ); }

 ~thinMultipole();

  thinMultipole& operator=(thinMultipole const& rhs);

  void                      setPole(int n, std::complex<double> const&  coeff);
  
  const_iterator begin() const;
  const_iterator   end() const;

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type() const;

  bool isMagnet()     const;
  bool isThin()       const;
  bool isPassive()    const;
  bool isDriftSpace() const;

  private:
  
   std::map<int, std::complex<double> > poles_;

} ;

#endif // THINMULTIPOLE_H
