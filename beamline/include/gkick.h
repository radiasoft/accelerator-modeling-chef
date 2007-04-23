/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      gkick.h
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
******                                                                
**************************************************************************
*************************************************************************/

#ifndef GKICK_H
#define GKICK_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class BmlVisitor;
class ConstBmlVisitor;


class gkick;

typedef boost::shared_ptr<gkick>        GKickPtr;
typedef boost::shared_ptr<gkick const>  ConstGKickPtr;


class DLLEXPORT gkick : public bmlnElmnt {

public:

  gkick();

  gkick( gkick const& x);

  gkick* Clone() const { return new gkick( *this ); }

  gkick& operator=( gkick const& rhs);

 ~gkick();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  const char* Type()       const;
  bool        isMagnet()   const;

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  void    set_dx( double const& );
  void   set_dxp( double const& );
  void    set_dy( double const& );
  void   set_dyp( double const& );
  void    set_dl( double const& );
  void    set_dp( double const& );
  void set_angle( double const& );
  void    set_dz( double const& );
  void     set_v( double const& );
  void     set_t( double const& );

  void   setStrength( double const& );


 private:

  double dx_ ;    // x  translation.
  double dxp_;    // x' translation.
  double dy_ ;    // y  translation.
  double dyp_;    // y' translation.
  double dl_ ;    // path length change.
  double dp_ ;    // dp/p change
  double angle_;  // rotation angle of coordinates about the longitudinal axis.
  double dz_;     // is the longitudinal displacement.
  double v_;      // is the  extrance-exit parameter  of the  kick.  
  double t_;      // is the momentum dependence  parameter.
};

#endif    // GKICK_H
