/*************************************************************************
**************************************************************************
**************************************************************************
******
******  PHYSICS TOOLKIT: Library of utilites and Sage classes
******             which facilitate calculations with the
******             BEAMLINE class library.
******  Version:   2.0
******
******  File:      BmlUtil.cc
******             Source code for Beamline utility methods.
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
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "GenericException.h"
#include "BmlUtil.h"
#include "TMatrix.h"
#include "Mapping.h"

using FNAL::Complex;
using namespace std;

ostream* BmlUtil::_errorStreamPtr  = &cerr;
ostream* BmlUtil::_outputStreamPtr = &cout;

const double BmlUtil::mlt1   = 1.0e-6;
const int    BmlUtil::PSDERR = 1;


void BmlUtil::setErrorStream( ostream& w )
{
  _errorStreamPtr = &w;
}


void BmlUtil::setOutputStream( ostream& w )
{
  _outputStreamPtr = &w;
}



bool BmlUtil::isKnown( const bmlnElmnt* x )
{
  return BmlUtil::isKnown( *x );
}


bool BmlUtil::isKnown( const bmlnElmnt& x )
{
  if( 0 == strcmp( "drift", x.Type() ))           { return true; }
  if( 0 == strcmp( "marker", x.Type() ))          { return true; }
  if( 0 == strcmp( "sbend", x.Type() ))           { return true; }
  if( 0 == strcmp( "rbend", x.Type() ))           { return true; }
  if( 0 == strcmp( "thinQuad", x.Type() ))        { return true; }
  if( 0 == strcmp( "quadrupole", x.Type() ))      { return true; }
  if( 0 == strcmp( "Slot", x.Type() ))            { return true; }
  if( 0 == strcmp( "srot", x.Type() ))            { return true; }
  if( 0 == strcmp( "BBLens", x.Type() ))          { return true; }
  if( 0 == strcmp( "beamline", x.Type() ))        { return true; }
  if( 0 == strcmp( "CF_rbend", x.Type() ))        { return true; }
  if( 0 == strcmp( "CF_sbend", x.Type() ))        { return true; }
  if( 0 == strcmp( "combinedFunction", x.Type() )){ return true; }
  if( 0 == strcmp( "thinDecapole", x.Type() ))    { return true; }
  if( 0 == strcmp( "ElSeparator", x.Type() ))     { return true; }
  if( 0 == strcmp( "JetQuadrupole", x.Type() ))   { return true; }
  if( 0 == strcmp( "JetthinQuad", x.Type() ))     { return true; }
  if( 0 == strcmp( "hkick", x.Type() ))           { return true; }
  if( 0 == strcmp( "vkick", x.Type() ))           { return true; }
  if( 0 == strcmp( "kick", x.Type() ))            { return true; }
  if( 0 == strcmp( "thinLamb", x.Type() ))        { return true; }
  if( 0 == strcmp( "monitor", x.Type() ))         { return true; }
  if( 0 == strcmp( "hmonitor", x.Type() ))        { return true; }
  if( 0 == strcmp( "vmonitor", x.Type() ))        { return true; }
  if( 0 == strcmp( "octupole", x.Type() ))        { return true; }
  if( 0 == strcmp( "thinOctupole", x.Type() ))    { return true; }
  if( 0 == strcmp( "Pinger", x.Type() ))          { return true; }
  if( 0 == strcmp( "HPinger", x.Type() ))         { return true; }
  if( 0 == strcmp( "VPinger", x.Type() ))         { return true; }
  if( 0 == strcmp( "rfcavity", x.Type() ))        { return true; }
  if( 0 == strcmp( "thinrfcavity", x.Type() ))    { return true; }
  if( 0 == strcmp( "sector", x.Type() ))          { return true; }
  if( 0 == strcmp( "thinSeptum", x.Type() ))      { return true; }
  if( 0 == strcmp( "sextupole", x.Type() ))       { return true; }
  if( 0 == strcmp( "thinSextupole", x.Type() ))   { return true; }
  if( 0 == strcmp( "JetthinSext", x.Type() ))     { return true; }
  if( 0 == strcmp( "thin12pole", x.Type() ))      { return true; }
  if( 0 == strcmp( "thin14pole", x.Type() ))      { return true; }
  if( 0 == strcmp( "thin16pole", x.Type() ))      { return true; }
  if( 0 == strcmp( "thin18pole", x.Type() ))      { return true; }
  if( 0 == strcmp( "thinMultipole", x.Type() ))   { return true; }

  return false;
}


void BmlUtil::writeAsTransport( const Mapping* mapPtr )
{
  BmlUtil::writeAsTransport( *mapPtr );
}


void BmlUtil::writeAsTransport( const Mapping& map )
{
  Jet cmp;
  const JLterm* termPtr;
  int d = map.Dim();
  int e;
  IntArray exps(d);
  for( int i = 0; i < d; i++ ) {
    cmp = map(i);
    cmp.resetConstIterator();
    termPtr = cmp.stepConstIteratorPtr();
    while( 0 != termPtr ) {
      *_outputStreamPtr << "M_{ " << (i+1);
      exps = termPtr->exponents();
      for( int j = 0; j < d; j++ ) {
        e = exps(j);
        if( 0 != e ) {
          for( int k = 0; k < e; k++ ) {
            *_outputStreamPtr << " " << (j+1);
	  }
        }        
      }
      *_outputStreamPtr << " } = " << termPtr->coefficient() << endl;
      termPtr = cmp.stepConstIteratorPtr();
    }
  }
}


bool BmlUtil::isSpace( const bmlnElmnt* x )
{
  if( !BmlUtil::isKnown( x ) ) { 
    ostringstream uic;
    uic << "The type " << (x->Type()) << " is not recognized.";
    throw( GenericException( __FILE__, __LINE__,
           "bool BmlUtil::isSpace( const bmlnElmnt* x )",
           uic.str().c_str() ) );
    
  }

  return (    ( 0 == strcasecmp( "drift",  x->Type() ) )
           || ( 0 == strcasecmp( "marker", x->Type() ) ) 
           || ( 0 == strcasecmp( "Slot",   x->Type() ) )
           || ( 0 == strcasecmp( "srot",   x->Type() ) ) 
         );
}


bool BmlUtil::isSpace( const bmlnElmnt& x )
{
  return BmlUtil::isSpace( &x );
}



void BmlUtil::normalize( MatrixC& B )
{
  // This code is lifted from emittanceDilution.cc,
  // which lifted it from normalForm.cc

  int i, j;

  // Normalizing the linear normal form coordinates
  Complex  mi( 0., -1. );
  MatrixD  J( "J", 6 );
  MatrixC  Nx;
  Nx = ( B.transpose() * J * B * J ) * mi;

  for( i = 0; i < 6; i++ ) {
   Nx( i, i ) = 1.0 / sqrt( abs( Nx(i,i) ) );
   if( abs( ( (Complex) 1.0 ) - Nx(i,i) ) < 1.0e-10 ) Nx(i,i) = 1.0;

       /* CAUTION */   for( j = 0; j < 6; j++ ) {
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
  Complex m0, cm0, m1, cm1;
  m0  = B(0,0)/abs(B(0,0));
  cm0 = conj(m0);
  m1  = B(1,1)/abs(B(1,1));
  cm1 = conj(m1);
  for( i = 0; i < 6; i++ ) {
    B(i,0) *= cm0;
    B(i,3) *= m0;
    B(i,1) *= cm1;
    B(i,4) *= m1;
  }
  if( imag(B(3,0)) > 0.0 ) {
    for( i = 0; i < 6; i++ ) {
      m0 = B(i,0);
      B(i,0) = B(i,3);
      B(i,3) = m0;
    }
  }
  if( imag(B(4,1)) > 0.0 ) {
    for( i = 0; i < 6; i++ ) {
      m0 = B(i,1);
      B(i,1) = B(i,4);
      B(i,4) = m0;
    }
  }
}


// ============================================================== //

int BmlUtil::makeCovariance( CovarianceSage::Info& w,
                             const Particle& prtn,
                             double eps_1, double eps_2, double eps_3 )
{
  // Some of this code has been lifted from the function
  //   MatrixD BeamlineContext::equilibriumCovariance( double eps_1, double eps_2 )
  //   in the file BeamlineContext.cc.   - Leo Michelotti (October 29, 2004)

  static bool firstTime = true;
  static const FNAL::Complex i( 0., 1. );

  if( firstTime ) {
    *_errorStreamPtr << "\n*** WARNING ***"
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

  int x   = Particle::_x();
  int y   = Particle::_y();
  int cdt = Particle::_cdt();    // not yet used
  int xp  = Particle::_xp();
  int yp  = Particle::_yp();
  int dpp = Particle::_dpop();   // not yet used

  int n = Particle::PSD;
  if( 6 != n ) {
    *_errorStreamPtr << "\n*** WARNING ***"
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

  // DGN: Begin diagnostic section
  // cout << "DGN:   BmlUtil::normalize( E );" << endl;
  // MatrixC W(E); // DGN:
  // DGN: End   diagnostic section

  BmlUtil::normalize( E );

  // DGN: Begin diagnostic section
  // cout << "DGN: Change in argument = " << endl;
  // cout << (E - W) << endl;
  // DGN: End   diagnostic section

  // Finally, the actual calculation: one line ...
  w.covariance = real(E*aa*E.dagger());

  return 0;
}


int BmlUtil::makeCovariance( CovarianceSage::Info* wPtr,
                             const Particle& prtn,
                             double eps_1, double eps_2, double eps_3 )
{
  return BmlUtil::makeCovariance( *wPtr, prtn, eps_1, eps_2, eps_3 );
}


beamline* BmlUtil::cloneLineAndInsert( double          percent,
                                       BmlPtrList&     insertions,
                                       BmlPtrList&     targets,
                                       const beamline* linePtr )
{
  // Test arguments
  if( !linePtr ) { return 0; }

  if( 0 == insertions.size() || 0 == targets.size() ) {
    return ((beamline*) (linePtr->Clone()));
  }

  if( percent < 0.0 ) { percent = 0.0; }
  if( percent > 1.0 ) { percent = 1.0; }
  bool upstream   = (0.0 == percent);
  bool downstream = (1.0 == percent);


  // Begin ...
  beamline* ret = new beamline( linePtr->Name() );
  BeamlineIterator bli( linePtr );
  bmlnElmnt* q   = 0;
  bmlnElmnt* ins = 0;
  bmlnElmnt* trg = 0;
  bmlnElmnt* spa = 0;
  bmlnElmnt* spb = 0;


  while( 0 != ( q = bli++ ) ) {
    ins = insertions.get();
    trg = targets.get();
    if( (0 == ins) || (0 == trg) ) { break; }

    if( 0 == strcmp( "beamline", q->Type() ) ) {
      insertions.insert(ins);
      targets.insert(trg);
      ret->append( BmlUtil::cloneLineAndInsert
                   ( percent, insertions, targets,
                     dynamic_cast<const beamline*>(q) ) );
    }

    else {
      // Here's where the real work is done.
      if( trg == q ) {
        if( upstream ) {
          ret->append( ins );
          ret->append( q->Clone() );
        }
        else if( downstream ) {
          ret->append( q->Clone() );
          ret->append( ins );
        }
        else {
          q->Split( percent, &spa, &spb );
          ret->append( spa );
          ret->append( ins );
          ret->append( spb );
        }
      }

      else {
        ret->append(q->Clone());
        insertions.insert(ins);
        targets.insert(trg);
      }
    }
  }


  // If there are elements left over, handle them.
  while(q) {
    ret->append(q->Clone());
    q = bli++;
  }

  // Finished ...
  ret->setEnergy( linePtr->Energy() );
  return ret;
}



