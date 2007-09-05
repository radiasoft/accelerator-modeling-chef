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

//---------------------------------------------------------------------------------

class DLLEXPORT alignment {

public:

  alignment();
  alignment( double const& xOffset,
             double const& yOffset,
             double const& roll ); // roll = tilt
  alignment(alignment     const&);
  alignment(alignmentData const&);
 ~alignment();

  bool isNull() const;
  bool operator==( alignment const& ) const;

  alignment& operator=(alignment const&);
                                // these methods will overwrite an array
                                // BMLM_dynDim in length
                                // I don\'t think that the Particle or
                                // JetParticle should be modified directly

  // void    align(Particle    const&, Vector&  );
  // void    align(Vector      const&, Vector&  );
  // void misalign(Particle    const&, Vector&  );
  // void    align(JetParticle const&, Mapping& );
  // void    align(JetVector   const&, Mapping& );

  //void misalign(JetParticle const&, Mapping&);
  //void misalign(Vector&    );
  //void    align(Vector&    );
  //void misalign(JetVector& );
  //void    align(JetVector& );

  void          setAlignment(alignmentData const&);
  alignmentData getAlignment() const;

  double const& x_offset() { return xOffset_; }
  double const& y_offset() { return yOffset_; }
  double const& roll()     { return tilt_;    }
  double const& cos_roll() { return cosTilt_; }
  double const& sin_roll() { return sinTilt_; }

  friend std::ostream& operator<<(std::ostream&, alignment&);

private:

  double       xOffset_;         // offset in meters
  double       yOffset_;         // offset in meters
  double       tilt_;            // roll in radians
                                 // we could put in pitch someday
  double       cosTilt_;         // cos(tilt)
  double       sinTilt_;         // sin(tilt)

};


#endif // ALIGNMENT_H
