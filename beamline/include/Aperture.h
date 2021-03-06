/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and
******             synchrotrons.
******
******  File:      Aperture.h
******
******             Defined classes:
******             - class ThinRectangularAperture
******             - class ThinEllipticalAperture
******             - class ThinStarAperture
******
******  Author:    Leo Michelotti
******             Phone: (630) 840 4956
******             Email: michelotti@fnal.gov
******
******  Copyright (c) 2010 Universities Research Association, Inc.
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
******  ----------------
******  REVISION HISTORY
******  ----------------
******  April, 2010            michelotti@fnal.gov
******  - original version
******    : still a prototype; under development.
******    : three shapes recognized: rectangle, elliptic, and "star,"
******      specified by an arbitrary set of vertices.
******    : only removes particles from bunch.
******
**************************************************************************
*************************************************************************/


#ifndef APERTURE_H
#define APERTURE_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class Particle;
class JetParticle;
class BmlVisitor;
class ConstBmlVisitor;


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// NOTE: These classes and typedefs are predefined here
// for consistency with other header files.

class ThinAperture;
class ThinStarAperture;
class ThinRectangularAperture;
class ThinEllipticalAperture;

typedef boost::shared_ptr<ThinAperture>             ThinAperturePtr;
typedef boost::shared_ptr<ThinStarAperture>         ThinStarAperturePtr;
typedef boost::shared_ptr<ThinRectangularAperture>  ThinRectangularAperturePtr;
typedef boost::shared_ptr<ThinEllipticalAperture>   ThinEllipticalAperturePtr;


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

class DLLEXPORT ThinAperture : public bmlnElmnt
{
  public:
    ThinAperture( char const* name = "" );
    ThinAperture( ThinAperture const& );
    virtual ~ThinAperture();

  public:
    // Necessary functions; virtual in bmlnElmnt
    // -----------------------------------------
    virtual ThinAperture* Clone() const = 0;

    void accept( BmlVisitor& );
    void accept( ConstBmlVisitor& ) const;

    void localPropagate( Particle& );
    void localPropagate( ParticleBunch& );
    void localPropagate( JetParticle& );
    void localPropagate( JetParticleBunch& );

    bool isMagnet() const;

    virtual const char* Type()  const = 0;
    // -----------------------------------------

  public:
    virtual bool contains( Particle const& ) const = 0;

    std::vector<ConstParticlePtr> killedList;
};


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

class DLLEXPORT ThinStarAperture : public ThinAperture
{
  public:
    ThinStarAperture( char const* name = "" );
    ThinStarAperture( char const* name,
                      std::vector<std::pair<double, double> > const& vertices /*[m]*/ );
    ThinStarAperture( ThinStarAperture const& );
    ~ThinStarAperture();

  public:
    ThinStarAperture* Clone() const;
    const char*       Type()  const;

  public:
    bool contains( Particle const& ) const;

  private:
    std::vector<std::complex<double> > vertices_;
};


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

class DLLEXPORT ThinRectangularAperture : public ThinAperture
{
  public:
    ThinRectangularAperture( char const* name        = "" , 
                             double const& x_center  = 0.0,       // [m]
                             double const& y_center  = 0.0,
                             double const& width     = 0.05,      // {m]
                             double const& height    = 0.05,
                             double const& angle     = 0.0    );  // [rad]
    ThinRectangularAperture( ThinRectangularAperture const& );
    ~ThinRectangularAperture();

  public:
    ThinRectangularAperture* Clone() const;
    const char*              Type()  const;

  public:
    bool contains( Particle const& ) const;

  private:
    double cx_, cy_, dx_, dy_;
    double cs_, sn_;
};


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

class DLLEXPORT ThinEllipticalAperture : public ThinAperture
{
  public:
    ThinEllipticalAperture( char const* name = "" , 
                            double const& x_center  = 0.0,       // [m]
                            double const& y_center  = 0.0,
                            double const& width     = 0.05,      // {m]
                            double const& height    = 0.05,
                            double const& angle     = 0.0    );  // [rad]
    ThinEllipticalAperture( ThinEllipticalAperture const& );
    ~ThinEllipticalAperture();

  public:
    ThinEllipticalAperture* Clone() const;
    const char*           Type()  const;

  public:
    bool contains( Particle const& ) const;

  private:
    double cx_, cy_, dx_, dy_;
    double cs_, sn_;
};


#endif // APERTURE_H
