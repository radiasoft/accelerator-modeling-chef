/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      RFGap.h
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
****** SYNOPSIS: A generic RF gap, which includes focusing and 
******           transit time effects.  
******                                                        
****** REVISION HISTORY
****** 
**************************************************************************
*************************************************************************/

#ifndef RFGAP_H
#define RFGAP_H

#include <beamline/bmlnElmnt.h>
#include <basic_toolkit/MathConstants.h>

class BmlVisitor;
class ConstBmlVisitor;
class RefRegVisitor;

class RFGap;

typedef boost::shared_ptr<RFGap>             RFGapPtr;
typedef boost::shared_ptr<RFGapconst>   ConstRFGapPtr;


class RFGap : public bmlnElmnt {

  class Propagator;

public:

  RFGap( std::string const&  name ="" ); 
  RFGap( std::string const&  name,
                double const& MeV,
	        double const& phase,
	        double const& h=1.0,         
	        boost::function<double>(double)& Tf);

  RFGap( RFGap const& );
  RFGap* Clone() const { return new thinRFGap( *this ); }

 ~RFGap();

  RFGap& operator=( RFGap const& rhs);

  double const& frequency()       const; 
  double const& harmon()          const; 
  double const& phi()             const; 


  void                 setFrequency( double const& );
  void       setFrequencyRelativeTo( double const& );
  void                    setHarmon( double const& );
  void                       setPhi( double const& );  // radians

  char const*     Type()  const;

  bool        isMagnet()  const;
  bool          isThin()  const;
  bool       isPassive()  const;
  bool    isDriftSpace()  const;

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

private:

  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

  double f_;                     // RF frequency [Hz]
  double phis_;                  // synchronous phase
  double h_;                     // harmonic number  

};


#endif // RFGAP_H
