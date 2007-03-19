/*************************************************************************
**************************************************************************
**************************************************************************
******
******  PHYSICS TOOLKIT: Library of utilites and Sage classes
******             which facilitate calculations with the
******             BEAMLINE class library.
******
******  File:      BmlUtil.cc
******             Source code for Beamline utility methods.
******  Version:   2.0
******
******  Copyright (c) 2003  Universities Research Association, Inc.
******                All Rights Reserved
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
****** REVISION HISTORY
******
****** Mar 2007        ostiguy@fnal.gov
******
****** - use new-style STL compatible beamline iterators
****** - support for reference counted elements/beamlines
****** - use C++ based RTTI rather than string comparisons
****** - Initialization optimizations
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/TMatrix.h>
#include <mxyzptlk/Mapping.h>
#include <beamline/beamline_elements.h>
#include <beamline/BmlPtr.h> 

#include <physics_toolkit/BmlUtil.h>

using namespace std;
using namespace boost;

ostream* BmlUtil::errorStreamPtr_  = &cerr;
ostream* BmlUtil::outputStreamPtr_ = &cout;

const double BmlUtil::mlt1   = 1.0e-6;
const int    BmlUtil::PSDERR = 1;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlUtil::setErrorStream( ostream& w )
{
  errorStreamPtr_ = &w;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlUtil::setOutputStream( ostream& w )
{
  outputStreamPtr_ = &w;
}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BmlUtil::isKnown( const bmlnElmnt* x )
{
  return BmlUtil::isKnown( *x );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BmlUtil::isKnown( bmlnElmnt const& x )
{
  if ( typeid(x) == typeid(drift)         )   { return true; }
  if ( typeid(x) == typeid(marker)        )   { return true; }
  if ( typeid(x) == typeid(sbend)         )   { return true; }
  if ( typeid(x) == typeid(rbend)         )   { return true; }
  if ( typeid(x) == typeid(thinQuad)      )   { return true; }
  if ( typeid(x) == typeid(quadrupole)    )   { return true; }
  if ( typeid(x) == typeid(Slot)          )   { return true; }
  if ( typeid(x) == typeid(srot)          )   { return true; }
  if ( typeid(x) == typeid(BBLens)        )   { return true; }
  if ( typeid(x) == typeid(beamline)      )   { return true; }
  if ( typeid(x) == typeid(CF_rbend)      )   { return true; }
  if ( typeid(x) == typeid(CF_sbend)      )   { return true; }
  if ( typeid(x) == typeid(combinedFunction) ){ return true; }
  if ( typeid(x) == typeid(thinDecapole)  )   { return true; }
  //if ( typeid(x) == typeid(ElSeparator)   )   { return true; }
  if ( typeid(x) == typeid(hkick)         )   { return true; }
  if ( typeid(x) == typeid(vkick)         )   { return true; }
  if ( typeid(x) == typeid(kick)          )   { return true; }
  if ( typeid(x) == typeid(thinLamb)      )   { return true; }
  if ( typeid(x) == typeid(monitor)       )   { return true; }
  if ( typeid(x) == typeid(hmonitor)      )   { return true; }
  if ( typeid(x) == typeid(vmonitor)      )   { return true; }
  if ( typeid(x) == typeid(octupole)      )   { return true; }
  if ( typeid(x) == typeid(thinOctupole)  )   { return true; }
  if ( typeid(x) == typeid(Pinger)        )   { return true; }
  if ( typeid(x) == typeid(HPinger)       )   { return true; }
  if ( typeid(x) == typeid(VPinger)       )   { return true; }
  if ( typeid(x) == typeid(rfcavity)      )   { return true; }
  if ( typeid(x) == typeid(thinrfcavity)  )   { return true; }
  if ( typeid(x) == typeid(sector)        )   { return true; }
  if ( typeid(x) == typeid(thinSeptum)    )   { return true; }
  if ( typeid(x) == typeid(sextupole)     )   { return true; }
  if ( typeid(x) == typeid(thinSextupole) )   { return true; }
  if ( typeid(x) == typeid(thin12pole)    )   { return true; }
  if ( typeid(x) == typeid(thin14pole)    )   { return true; }
  if ( typeid(x) == typeid(thin16pole)    )   { return true; }
  if ( typeid(x) == typeid(thin18pole)    )   { return true; }
  if ( typeid(x) == typeid(thinMultipole) )   { return true; }

  return false;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlUtil::writeAsTransport( Mapping const& map )
{

  int d = map.Dim();
  int e;
  IntArray exps(d);

  Jet cmp;

  for( int i = 0; i < d; i++ ) {
    cmp = map(i);

    for ( Jet::iterator it = cmp.begin(); it != cmp.end() ; ++it ) { 
     *outputStreamPtr_ << "M_{ " << (i+1);
      exps = it->exponents();
      for( int j=0;  j<d; ++j) {
        e = exps(j);
        if( 0 != e ) {
          for( int k = 0; k < e; k++ ) {
            *outputStreamPtr_ << " " << (j+1);
	  }
        }        
      }
      *outputStreamPtr_ << " } = " << it->coefficient() << endl;
    }
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BmlUtil::isSpace( bmlnElmnt const&  x )
{
  if( !isKnown( x ) ) { 
    ostringstream uic;
    uic << "The type " << ( x.Type()) << " is not recognized.";
    throw( GenericException( __FILE__, __LINE__,
           "bool BmlUtil::isSpace( const bmlnElmnt* x )",
           uic.str().c_str() ) );
    
  }

  return (    ( 0 == strcasecmp( "drift",  x.Type() ) )
           || ( 0 == strcasecmp( "marker", x.Type() ) ) 
           || ( 0 == strcasecmp( "Slot",   x.Type() ) )
           || ( 0 == strcasecmp( "srot",   x.Type() ) ) 
         );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void BmlUtil::normalize( MatrixC& B )
{
  // This code is lifted from emittanceDilution.cc,
  // which lifted it from normalForm.cc

  const std::complex<double> complex_0(0.0,0.0);
  const std::complex<double> mi(0.,-1.);

  // Normalizing the linear normal form coordinates

  MatrixD  J( "J", 6 );
  MatrixC  Nx = ( B.transpose() * J * B * J ) * mi;

  for( int i = 0; i < 6; i++ ) {
   Nx( i, i ) = 1.0 / sqrt( abs( Nx(i,i) ) );
   if( abs( ( (std::complex<double> ) 1.0 ) - Nx(i,i) ) < 1.0e-10 ) Nx(i,i) = 1.0;

       /* CAUTION */   for( int j = 0; j < 6; j++ ) {
       /* CAUTION */    if( j == i ) continue;
       /* CAUTION */    else if( abs( Nx(i,j) ) > BmlUtil::mlt1 ) {
       /* CAUTION */          ostringstream uic;
       /* CAUTION */          uic << "Nondiagonal element in BJB^TJ: abs( Nx( "
                                  << i << ", " << j << " ) ) = "
                                  << std::abs(Nx(i,j));
       /* CAUTION */          throw( GenericException( __FILE__, __LINE__,
       /* CAUTION */                 "void BmlUtil::normalize( MatrixC& B )",
       /* CAUTION */                 uic.str().c_str() ) );
       /* CAUTION */    }
       /* CAUTION */    else Nx(i,j) = complex_0;
       /* CAUTION */   }
  }

  B = B*Nx;

  // Try to get the phase correct ...
  std::complex<double>  m0, cm0, m1, cm1;
  m0  = B(0,0)/abs(B(0,0));
  cm0 = conj(m0);
  m1  = B(1,1)/abs(B(1,1));
  cm1 = conj(m1);

  for( int i = 0; i < 6; i++ ) {
    B(i,0) *= cm0;
    B(i,3) *= m0;
    B(i,1) *= cm1;
    B(i,4) *= m1;
  }
  if( imag(B(3,0)) > 0.0 ) {
    for( int i=0; i<6; ++i) {
      m0 = B(i,0);
      B(i,0) = B(i,3);
      B(i,3) = m0;
    }
  }
  if( imag(B(4,1)) > 0.0 ) {
    for( int i=0; i < 6; ++i ) {
      m0 = B(i,1);
      B(i,1) = B(i,4);
      B(i,4) = m0;
    }
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int BmlUtil::makeCovariance( CovarianceSage::Info& w, Particle const& prtn,
                             double eps_1, double eps_2, double eps_3 )
{
  // Some of this code has been lifted from the function
  //   MatrixD BeamlineContext::equilibriumCovariance( double eps_1, double eps_2 )
  //   in the file BeamlineContext.cc.   - Leo Michelotti (October 29, 2004)

  static bool firstTime = true;
  const std::complex<double>  i( 0., 1. );

  if( firstTime ) {
    *errorStreamPtr_ << "\n*** WARNING ***"
                        "\n*** WARNING *** File: " << __FILE__
                     <<                  " Line: " << __LINE__
                     << "\n*** WARNING *** BmlUtil::makeCovariance"
                        "\n*** WARNING *** Current version of this function ignores"
                        "\n*** WARNING ***   the third \"emittance\" argument."
                        "\n*** WARNING ***"
                     << endl;
    firstTime = false;
  }

  // eps_1 and eps_2 are two "invariant emittances"
  //   in units of pi mm-mr. In this version, eps_3 is currently ignored.
  // I assume that eps_1 is "mostly horizontal" and eps_2 is
  //   "mostly vertical."

  // Convert to action, in meters (crazy units):
  //   I1 and I2 are expectation values of action coordinates.
  const double mm_mr = 1.0e-6;
  const double betaGamma = prtn.ReferenceBeta() * prtn.ReferenceGamma();
  double I1 = ( std::abs( eps_1 )/betaGamma ) * mm_mr / 2.0;
  double I2 = ( std::abs( eps_2 )/betaGamma ) * mm_mr / 2.0;

  int x   = Particle::xIndex();
  int y   = Particle::yIndex();
  int cdt = Particle::cdtIndex();    // not yet used
  int xp  = Particle::npxIndex();
  int yp  = Particle::npyIndex();
  int dpp = Particle::ndpIndex();    // not yet used

  int n = Particle::PSD;
  if( 6 != n ) {
    *errorStreamPtr_ << "\n*** WARNING ***"
                        "\n*** WARNING *** File: " << __FILE__
                     <<                  " Line: " << __LINE__
                     << "\n*** WARNING *** BmlUtil::makeCovariance"
                        "\n*** WARNING *** Current version of this function"
                        "\n*** WARNING ***   assumes phase space of dimension 6."
                        "\n*** WARNING ***   You are using " << Particle::PSD << "."
                        "\n*** WARNING ***"
                     << endl;
    return BmlUtil::PSDERR;
  }

  MatrixD aa(n,n);
  aa(x,x)     = I1;
  aa(xp,xp)   = I1;
  aa(y,y)     = I2;
  aa(yp,yp)   = I2;

  // Construct matrix of eigenvectors
  MatrixC E("I",n);
  E(x , 0) = sqrt( w.beta.hor / 2.0 );
  E(xp, 0) = - ( i + w.alpha.hor )/sqrt( 2.0*w.beta.hor );
  E(y , 1) = sqrt( w.beta.ver / 2.0 );
  E(yp, 1) = - ( i + w.alpha.ver )/sqrt( 2.0*w.beta.ver );

  for( int j = 0; j < 2; j++ ) {
    int m = j + (n/2);
    for( int k = 0; k < n; k++ ) {
      E(k,m) = conj(E(k,j));
    }
  }

  // This line is almost certainly not necessary.
  //   The matrix E already, has correct normalization,
  //   at least, as I normalize on Oct.29, 2004.
  //   But that may change. Who knows? So I'll retain the line.

  BmlUtil::normalize( E );

  // Finally, the actual calculation: one line ...
  w.covariance = real(E*aa*E.dagger());

  return 0;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int BmlUtil::makeCovariance( CovarianceSage::Info* wPtr,
                             const Particle& prtn,
                             double eps_1, double eps_2, double eps_3 )
{
  return BmlUtil::makeCovariance( *wPtr, prtn, eps_1, eps_2, eps_3 );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BmlPtr    BmlUtil::cloneLineAndInsert( double                     percent,
                                       std::list<ElmPtr>&         insertions,
                                       std::list<ElmPtr>&         targets,
                                       ConstBmlPtr                linePtr )
{

  if( !linePtr ) { return BmlPtr(); }

  if( 0 == insertions.size() || 0 == targets.size() ) {
    return  BmlPtr( linePtr->Clone() );
  }

  if( percent < 0.0 ) { percent = 0.0; }
  if( percent > 1.0 ) { percent = 1.0; }

  bool upstream   = (0.0 == percent);
  bool downstream = (1.0 == percent);


  // Begin ...

  BmlPtr ret( new beamline( linePtr->Name().c_str() ) );

  ElmPtr spa;
  ElmPtr spb;


  beamline::const_iterator it;

  for ( it  = linePtr->begin(); it != linePtr->end(); ++it) {

    if( insertions.empty() || targets.empty() ) break; 

    ElmPtr ins = insertions.front(); insertions.erase( insertions.begin() );
    ElmPtr trg = targets.front();       targets.erase(    targets.begin() );


    if( typeid(**it) == typeid(beamline) ) {

      insertions.push_front(ins);
      targets.push_front(trg);
      ret->append( cloneLineAndInsert ( percent, insertions, targets,  dynamic_pointer_cast<const beamline>(*it) ) );
    }

    else {
      // Here's where the real work is done.

      if( trg == (*it) ) {
        if( upstream ) {
          ret->append( ElmPtr( ins->Clone() ) );
          ret->append( ElmPtr( (*it)->Clone() )  );
        }
        else if( downstream ) {
          ret->append( ElmPtr( (*it)->Clone() )  );
          ret->append( ElmPtr( ins->Clone() ) );
        }
        else {
          (*it)->Split( percent, spa, spb ); 
          ret->append( ElmPtr(spa->Clone()) );
          ret->append( ElmPtr(ins->Clone()) );
          ret->append( ElmPtr(spb->Clone()) );
        }
      }

      else {
        ret->append( ElmPtr( (*it)->Clone()) );
        insertions.push_front( ins );
        targets.push_front( trg );
      }
    }
  }
  // If there are elements left over, handle them.

  for (  ; it != linePtr->end(); ++it ) {
    ret->append( ElmPtr( (*it)->Clone() ) );
  }

  // Finished ...

  ret->setEnergy( linePtr->Energy() );
  return ret;
  }



