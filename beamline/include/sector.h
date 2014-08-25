/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      sector.h
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
******                                                                
******    REVISION HISTORY
******
****** Mar 2007            ostiguy@fnal.gov
****** - use covariant return types
****** - support for reference counted elements
****** Dec 2007            ostiguy@fnal.gov
****** - new typesafe propagator scheme
******
**************************************************************************
*************************************************************************/


#ifndef SECTOR_H
#define SECTOR_H

#include <vector>
#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>

class sector;
class BmlVisitor;
class ConstBmlVisitor;

typedef boost::shared_ptr<sector>       SectorPtr;
typedef boost::shared_ptr<sector const> ConstSectorPtr;


class DLLEXPORT sector : public bmlnElmnt {

  class Propagator;
 
public:

  typedef boost::shared_ptr<BasePropagator<sector> > PropagatorPtr;   

  sector( const char* = 0, double const& length = 0.0, char mapType= 1);

  sector( const char*, std::vector<double> const& betaH,  std::vector<double> const& alphaH,  std::vector<double> const& psiH,
                       std::vector<double> const& betaV,  std::vector<double> const& alphaV,  std::vector<double> const& psiV,  double const& length );


  sector( const char*, Mapping const&,  double const& length, char mapType=1 );

  sector( sector const& );

  sector* Clone() const { return new sector( *this ); }

 ~sector();

  void Split( double const&, ElmPtr&, ElmPtr& ) const;

  Mapping const& getMap()    const;
  Matrix         getMatrix() const;

  void localPropagate(         Particle&  p );   
  void localPropagate(      JetParticle&  p );   
  void localPropagate(    ParticleBunch&  b );   
  void localPropagate( JetParticleBunch&  b );   

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  void setFrequency( double (*)( double const& ) );
  void setFrequency( Jet    (*)( Jet    const& ) );
 
 void setLength( double const& );

  const char* Type()     const;
 
  bool        isMagnet() const;
  bool        isMatrix() const; 

  void usePropagator( PropagatorPtr& );

private:

  double (*DeltaT)    ( double const& );
  Jet    (*JetDeltaT) ( Jet    const& );

  std::ostream& writeTo ( std::ostream& );
  std::istream& readFrom( std::istream& );

  char                 mapType_;   // 0 => use matrix  != 0 => use general mapping         
  Mapping              myMap_;
  std::vector<double>  betaH_;     // 0 = entry;  1 = exit
  std::vector<double>  alphaH_;    
  double               deltaPsiH_;
  std::vector<double>  betaV_;     
  std::vector<double>  alphaV_;    
  double               deltaPsiV_;
  MatrixD              mapMatrix_;
 
  PropagatorPtr        propagator_;
};


#endif // SECTOR_H
