/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and
******             synchrotrons.
******
******  File:      Aperture.cc
******
******             Implemented classes:
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

#include <ieeefp.h>

#include <basic_toolkit/MathConstants.h>
#include <beamline/BmlVisitor.h>
#include <beamline/Particle.h>
#include <beamline/ParticleBunch.h>

#include <beamline/Aperture.h>


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//
//   Extraction criterion
// 

namespace {

bool isGone( Particle const& p )
{
  for( int i = 0; i < 6; ++i ) {
    if( 0 == finite( p.State()[i] ) ) {
      #ifdef DIAGNOSTICS_ON
      cout << "DGN: Will remove particle: infinite state detected: "
           << p.State()
           << " : with tag: " << p.getTag()
           << endl;
      #endif
      return true;
    }
  }

  if( std::string::npos != (p.getTag()).find("KILL") ) {
    #ifdef DIAGNOSTICS_ON
    cout << "DGN: Will remove particle with tag: "
         << p.getTag() 
         << ": " << p.State()
         << endl;
    #endif
    return true;
  }

  return false;
}

// --------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
// --------------------------------------------------------------------------------

#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))
// 
// Nothing needs to be done; there are no templates.
// 
#endif

} // End: anonymous namespace



// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// 
//   class ThinAperture
// 


ThinAperture::ThinAperture( char const* nm )
: bmlnElmnt( nm, 0.0, 0.0 ), killedList()
{
}


ThinAperture::ThinAperture( ThinAperture const& x )
: bmlnElmnt(x), killedList(x.killedList)
{
}


ThinAperture::~ThinAperture()
{
}


bool ThinAperture::isMagnet() const
{ 
  return false;
}


void ThinAperture::accept( BmlVisitor& v )
{ 
  v.visit( *this );
}


void ThinAperture::accept( ConstBmlVisitor& v ) const
{ 
  v.visit( *this );
}


void ThinAperture::localPropagate( Particle& p )
{
  if( ! contains( p ) ) {
    p.setTag(   p.getTag() 
              + std::string(" / KILL: OUTSIDE APERTURE ") 
              + std::string( Name() )
              + std::string(" / ")
            );
    killedList.push_back( ConstParticlePtr( p.Clone() ) );
    // WARNING: at the single particle level, the particle will continue
    // WARNING: to propagate and could, in principle, be killed by
    // WARNING: more than one aperture.
  }
}


void ThinAperture::localPropagate( ParticleBunch& bunch )
{
  // Tag particles to be removed ...
  // -------------------------------
  for (  ParticleBunch::iterator it = bunch.begin(); 
         it != bunch.end(); 
         ++it )
  {  
    if( ! contains( (*it) ) ) {
      (*it).setTag(   (*it).getTag() 
                    + std::string(" / KILL: OUTSIDE APERTURE ") 
                    + std::string( Name() )
                    + std::string(" / ")
                  );

      killedList.push_back( ConstParticlePtr( (*it).Clone() ) );
    }
  }

  // ... then remove them.
  // ---------------------
  bunch.remove( ::isGone );
}


void ThinAperture::localPropagate( JetParticle& )
{
  // Do nothing.
}


void ThinAperture::localPropagate( JetParticleBunch& )
{
  // Do nothing.
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// 
//   class ThinStarAperture
// 


ThinStarAperture::ThinStarAperture( char const* nm )
: ThinAperture(nm)
{
}


ThinStarAperture::ThinStarAperture( char const* nm, 
                                    std::vector<std::pair<double, double> > const& vs /*[m]*/ )
: ThinAperture(nm)
{
  if( vs.size() < 3 ) {
   ostringstream uic;
   uic  << "Must have at least 3 vertices.  You submitted " << vs.size();
   throw( GenericException( __FILE__, __LINE__, 
          "ThinStarAperture::ThinStarAperture(...)",
          uic.str().c_str() ) );
  }

  for( std::vector<std::pair<double, double> >::const_iterator it = vs.begin();
       it != vs.end();
       ++it ) 
  {
    vertices_.push_back( std::complex<double>( (*it).first, (*it).second ) );
  }
}


ThinStarAperture::ThinStarAperture( ThinStarAperture const& x )
: ThinAperture(x),
  vertices_(x.vertices_)
{
  #if 0
  for( std::vector<std::complex<double> >::const_iterator it = x.vertices_.begin();
       it != x.vertices_.end();
       ++it ) {
    vertices_.push_back( (*it) );
  }
  #endif
}


ThinStarAperture* ThinStarAperture::Clone() const
{
  return new ThinStarAperture( *this ); 
}


ThinStarAperture::~ThinStarAperture()
{
}


const char* ThinStarAperture::Type() const
{
  return "ThinStarAperture";
}


bool ThinStarAperture::contains( Particle const& prt ) const
{
  std::complex<double> u( prt.get_x(), prt.get_y() );

  int    i   = 0;
  int    sss = vertices_.size();
  double sum = 0.0;

  while( i < sss ) {
    int j = i+1;
    if( sss == j ) { j = 0; }

    std::complex<double> v( vertices_[i] );
    std::complex<double> w( vertices_[j] );

    double theta = arg( (w-u)*conj(v-u) );

    sum += theta;
    // DEBUG: cout << theta << " -> " << sum << endl;
    ++i;
  }

  return (    ( std::abs( sum - M_TWOPI ) < 1.0e-12 ) 
           || ( std::abs( sum + M_TWOPI ) < 1.0e-12 ) );
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// 
//   class ThinEllipticalAperture
// 


ThinEllipticalAperture::ThinEllipticalAperture( char const* nm, 
                                                double const& x_center,  // [m]
                                                double const& y_center,
                                                double const& width,     // {m]
                                                double const& height,
                                                double const& angle   )  // [rad]
: ThinAperture(nm),
  cx_(x_center),
  cy_(y_center),
  dx_(std::abs(width)/2.0),
  dy_(std::abs(height)/2.0),
  cs_(cos(angle)),
  sn_(sin(angle))
{
}


ThinEllipticalAperture::ThinEllipticalAperture( ThinEllipticalAperture const& x )
: ThinAperture( x ),
  cx_(x.cx_),
  cy_(x.cy_),
  dx_(x.dx_),
  dy_(x.dy_),
  cs_(x.cs_),
  sn_(x.sn_)
{
}


ThinEllipticalAperture* ThinEllipticalAperture::Clone() const
{
  return new ThinEllipticalAperture( *this ); 
}


ThinEllipticalAperture::~ThinEllipticalAperture()
{
}


const char* ThinEllipticalAperture::Type() const
{
  return "ThinEllipticalAperture";
}


bool ThinEllipticalAperture::contains( Particle const& prt ) const
{
  double t;
  double x = prt.get_x() - cx_;
  double y = prt.get_y() - cy_;

  t = ( cs_*x + sn_*y )/dx_;
  y = ( cs_*y - sn_*x )/dy_;
  x = t;

  return ( ( x*x + y*y ) < 1.0 );
}



// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// 
//   class ThinRectangularAperture
// 

ThinRectangularAperture::ThinRectangularAperture( char const* nm, 
                                                double const& x_center,  // [m]
                                                double const& y_center,
                                                double const& width,     // {m]
                                                double const& height,
                                                double const& angle   )  // [rad]
: ThinAperture(nm),
  cx_(x_center),
  cy_(y_center),
  dx_(std::abs(width)/2.0),
  dy_(std::abs(height)/2.0),
  cs_(cos(angle)),
  sn_(sin(angle))
{
}


ThinRectangularAperture::ThinRectangularAperture( ThinRectangularAperture const& x )
: ThinAperture( x ),
  cx_(x.cx_),
  cy_(x.cy_),
  dx_(x.dx_),
  dy_(x.dy_),
  cs_(x.cs_),
  sn_(x.sn_)
{
}


ThinRectangularAperture* ThinRectangularAperture::Clone() const
{
  return new ThinRectangularAperture( *this ); 
}


ThinRectangularAperture::~ThinRectangularAperture()
{
}


const char* ThinRectangularAperture::Type() const
{
  return "ThinRectangularAperture";
}


bool ThinRectangularAperture::contains( Particle const& prt ) const
{
  double t;
  double x = prt.get_x() - cx_;
  double y = prt.get_y() - cy_;

  t = cs_*x + sn_*y;
  y = cs_*y - sn_*x;
  x = t;

  return ( (std::abs(x) < dx_) && (std::abs(y) < dy_) );
}
