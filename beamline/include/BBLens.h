/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      BBLens.h
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
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
****** REVISION HISTORY:
******
****** Mar 2007         ostiguy@fnal.gov
****** - use covariant return type
****** - added support for reference counted smart pointers
****** - added proper  assigment operator  
**************************************************************************
*************************************************************************/
#ifndef BBLENS_H
#define BBLENS_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class BmlVisitor;
class ConstBmlVisitor;

template <typename T>
class TJetVector;

typedef TJetVector<double> JetVector;


class BBLens;

typedef boost::shared_ptr<BBLens>            BBLensPtr;
typedef boost::shared_ptr<BBLens const> ConstBBLensPtr;


class DLLEXPORT BBLens : public bmlnElmnt {

public:
  BBLens( const char* name=0, double const&  length = 1.0
             /* length [m]: the length of the bunch
                in its rest frame */,
          double const&        = 3.3e11
             /* strength : total number of proton
                charges in the bunch */,
          double const&        = 1000.0
             /* gamma of the bunch, in the lab frame */,
          const double* = 0
             /* pointer to an array containing
                three values for 6 X invariant emittance [m] / pi  */
        );
  BBLens( BBLens const& );

  BBLens* Clone() const { return new BBLens( *this ); }

 ~BBLens();

  BBLens& operator=( BBLens const&);


  char useRound;         // By default = 1
                         // If 1: then round beam approximation
                         // used when horizontal and vertical 
                         // sigmas approximately equal.

  void Kludge( double const&  /* num    */, 
               double const&  /* gamma  */, 
         const double* /* sigma  */ );
  void KludgeNum( double const& ); 
  void KludgeSigma( const double* );
  void AdjustSigma();

  Vector NormalizedEField( double const& x, double const& y );
             /* returns the "normalized" electric field
                in the rest frame of the bunch, in inverse
                meters.  To get the field [V/m], this must
                be multiplied by Q/(2 pi epsilon_o), where
                Q is the line density of charge [C/m] (in
                rest frame). */

  JetVector NormalizedEField( const Jet& x, const Jet& y );
             /* returns the "normalized" electric field
                in the rest frame of the bunch, in inverse
                meters.  To get the field [V/m], this must
                be multiplied by Q/(2 pi epsilon_o), where
                Q is the line density of charge [C/m] (in
                rest frame). */

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v );            
  void accept( ConstBmlVisitor& v ) const; 

  const char* Type()     const;
  bool        isMagnet() const;

  Vector Beta();
  void GetSigma( double* );

private:

  double emittance[3];   // One sigma (noninvariant) emittance / pi
  double gamma;          // Relativistic gamma
  double beta;           // Relativistic beta
  double num;            // Number of proton charges; if the bunch
                         // is negatively charged, then this number
                         // is negative.
  double sigma[3];       // This will depend on position in the 
                         // lattice.  The third component is zero
                         // for now.

};

#endif // BBLENS_H
