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
******                                                                
******                                                           
**************************************************************************
*************************************************************************/
#ifndef ALIGNMENT_H
#define ALIGNMENT_H

#include <basic_toolkit/globaldefs.h>
#include <basic_toolkit/VectorD.h>

class Particle;
class JetParticle;


template <typename T>
class  TJet;

template <typename T>
class  TJetVector;

template <typename T>
class  TMapping;

typedef TJet<double>       Jet;
typedef TJetVector<double> JetVector; 
typedef TMapping<double>   Mapping; 


struct DLLEXPORT alignmentData {
  double       xOffset;
  double       yOffset;
  double       tilt;
  alignmentData();
  alignmentData(const alignmentData&);
  ~alignmentData();
  alignmentData& operator=(const alignmentData&);
  bool operator==(const alignmentData&) const;
};


class DLLEXPORT alignment {
private:
  double       xOffset;         // offset in meters
  double       yOffset;         // offset in meters
  double       tilt;            // roll in radians
                                // we could put in pitch someday
  double       cosTilt;         // cos(tilt)
  double       sinTilt;         // sin(tilt)
public:
  alignment();
  alignment( double, // xOffset
             double, // yOffset
             double  // roll (=tilt)
           );
  alignment(const alignment&);
  alignment(const alignmentData&);
  ~alignment();

  bool isNull() const;
  bool operator==( const alignment& ) const;

  alignment& operator=(const alignment&);
                                // these methods will overwrite an array
                                // BMLM_dynDim in length
                                // I don\'t think that the Particle or
                                // JetParticle should be modified directly
  void    align(Particle const&, Vector&);
  void    align(Vector&,         Vector&);
  void misalign(Particle const&, Vector&);
  void    align(JetParticle&,    Mapping&);
  void    align(JetVector&,      Mapping&);

  void misalign(JetParticle&,    Mapping&);
  void misalign(Vector& );
  void    align(Vector& );
  void misalign(JetVector& );
  void    align(JetVector& );

  void          setAlignment(alignmentData const&);
  alignmentData getAlignment() const;

  double x_offset() { return xOffset; }
  double y_offset() { return yOffset; }
  double roll()     { return tilt;    }
  double cos_roll() { return cosTilt; }
  double sin_roll() { return sinTilt; }

  friend std::ostream& operator<<(std::ostream&, alignment&);
};


#endif // ALIGNMENT_H
