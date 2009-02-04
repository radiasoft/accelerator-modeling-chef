/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      Alignment.h
******
******                                                                
******  Copyright (c) Universities Research Association, Inc. / Fermilab     
******                All Rights Reserved                             
******
******  Author: ostiguy@fnal.gov
******          michelotti@fnal.gov
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
******  REVISION HISTORY                                                              
******                     
******                                        
**************************************************************************
*************************************************************************/
#ifndef ALIGNMENT_H
#define ALIGNMENT_H

#include <basic_toolkit/globaldefs.h>
#include <basic_toolkit/VectorD.h>
#include <mxyzptlk/JetFwd.h>
#include <mxyzptlk/MappingFwd.h>
#include <mxyzptlk/JetVectorFwd.h>


class DLLEXPORT Alignment {

public:

  Alignment();
  Alignment( double const& xOffset,
             double const& yOffset,
             double const& roll,
             double const& pitch ); 

  Alignment(Alignment  const&);
 ~Alignment();

  bool isNull() const;
  bool operator==( Alignment const& ) const;

  Alignment& operator=(Alignment const&);

  double const& xOffset()  const  { return xOffset_; }
  double const& yOffset()  const  { return yOffset_; }
  double const& roll()     const  { return roll_;    }
  double const& pitch()    const  { return pitch_;   }

  double setXOffset(double const& val) {  double old = xOffset_; xOffset_ = val; return old; }  
  double setYOffset(double const& val) {  double old = yOffset_; yOffset_ = val; return old; }   
  double    setRoll(double const& val) {  double old = roll_;      roll_  = val;  
                                          cosRoll_= cos(roll_); sinRoll_= sin(roll_); return old;}
 
  double const& cos_roll() const { return cosRoll_; }
  double const& sin_roll() const { return sinRoll_; } 

  friend std::ostream& operator<<(std::ostream&, Alignment&);

private:

  double        xOffset_;  // x offset in m
  double        yOffset_;  // y offset in m
  double           roll_;  // roll  angle in radians, [ arbitary                    ]
  double          pitch_;  // pitch angle in radians  [ assumed small i.e. << 1 rad ]

  double        cosRoll_;         
  double        sinRoll_;         

};


#endif // ALIGNMENT_H
