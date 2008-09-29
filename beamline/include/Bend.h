/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
****** BEAMLINE:  C++ objects for design and analysis
******            of beamlines, storage rings, and   
******            synchrotrons.                      
******                                    
****** File:      Bend.h
******
******  Copyright (c) Fermi Alliance / Fermilab    
******                All Rights Reserved                             
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
******
****** May 2008 ostiguy@fnal.gov
****** - proper, explicit assignment operator
****** - propagator moved (back) to base class
****** - no assumption about internal structure
******
**************************************************************************
**************************************************************************
*************************************************************************/

#ifndef BEND_H
#define BEND_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class BmlVisitor;
class ConstBmlVisitor;

class Bend;

typedef boost::shared_ptr<Bend>            BendPtr;
typedef boost::shared_ptr<Bend const> ConstBendPtr;


class DLLEXPORT Bend : public bmlnElmnt {

  class Propagator;
  class bend_core_access;

  friend class  bend_core_access; 
   
public:

  enum BendType {type_sbend, type_rbend };

  Bend(  char const* name, double const& orbit_length_m, 
	                    double const& bfield_T, 
                            double const& bend_angle,
                            double const& usAngle,
	                    double const& dsAngle,
                            double const& usFaceAngle,
                            double const& dsFaceAngle, BendType bend_type );

  Bend(  Bend const& );

  Bend* Clone() const { return new Bend( *this ); }

 ~Bend();

  Bend& operator=(Bend const& rhs);

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type()  const;
  bool isMagnet()     const;
  bool isThin()       const;
  bool isPassive()    const;
  bool isDriftSpace() const;

  BendType getBendType() const;

private:

  BendType type_;           // the type of bend ( type_sbend or type_rbend) 
  double   angle_;          // [radians] bend angle

  double usAngle_;          // [radians] as defined in MAD for Bends.
  double dsAngle_;          

  double usFaceAngle_;      // [radians] entry (upstream) and exit (downstream) 
  double dsFaceAngle_;      // angles of the fiducial orbit referenced
                            // to the physical edge of the magnet. 

  std::ostream& writeTo(std::ostream&); 
  std::istream& readFrom(std::istream&); 

};

class Bend::bend_core_access {

 public:

  static double&                    get_angle( Bend& o )      { return o.angle_;       }
  static double&                    get_usAngle( Bend& o)     { return o.usAngle_;     } 
  static double&                    get_dsAngle( Bend& o)     { return o.dsAngle_;     }
  static double&                    get_usFaceAngle( Bend& o) { return o.usFaceAngle_; }
  static double&                    get_dsFaceAngle( Bend& o) { return o.dsFaceAngle_; }

}; 

#endif // BEND_H
