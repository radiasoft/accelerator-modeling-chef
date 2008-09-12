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
  double       xOffset; // x offset in m
  double       yOffset; // y offset in m
  double       roll;    // roll angle in radians

  alignmentData();
  alignmentData(double const&, double const&, double const& );
  alignmentData( alignmentData const&);
  ~alignmentData();

  alignmentData&  operator=( alignmentData const&);
  bool            operator==( alignmentData const&) const;
};

//---------------------------------------------------------------------------------

class DLLEXPORT alignment {

public:

  alignment();
  alignment( double const& xOffset,
             double const& yOffset,
             double const& roll ); // roll = roll
  alignment(alignment     const&);
  alignment(alignmentData const&);
 ~alignment();

  bool isNull() const;
  bool operator==( alignment const& ) const;

  alignment& operator=(alignment const&);

  void                 setAlignment(alignmentData const&);
  alignmentData const& getAlignment() const;

  double const& x_offset() { return adata_.xOffset; }
  double const& y_offset() { return adata_.yOffset; }
  double const& roll()     { return adata_.roll;    }
  double const& cos_roll() { return cosRoll_; }
  double const& sin_roll() { return sinRoll_; }

  friend std::ostream& operator<<(std::ostream&, alignment&);

private:

  alignmentData adata_;    

  double        cosRoll_;         
  double        sinRoll_;         

};


#endif // ALIGNMENT_H
