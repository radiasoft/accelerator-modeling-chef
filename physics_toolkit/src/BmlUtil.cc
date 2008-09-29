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

#include <physics_toolkit/BmlUtil.h>

#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/TMatrix.h>
#include <mxyzptlk/Mapping.h>
#include <beamline/beamline_elements.h>
#include <beamline/Particle.h>
#include <beamline/beamline.h> 


using namespace std;

namespace {
 Particle::PhaseSpaceIndex const& i_x     = Particle::i_x;
 Particle::PhaseSpaceIndex const& i_npx   = Particle::i_npx;
 Particle::PhaseSpaceIndex const& i_y     = Particle::i_y;
 Particle::PhaseSpaceIndex const& i_npy   = Particle::i_npy;
 Particle::PhaseSpaceIndex const& i_cdt   = Particle::i_cdt;
 Particle::PhaseSpaceIndex const& i_ndp   = Particle::i_ndp;
}

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

void BmlUtil::writeAsTransport( Mapping const& map )
{

#if 0
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
#endif
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BmlUtil::isSpace( bmlnElmnt const&  x )
{
  return (    ( 0 == strcasecmp( "drift",  x.Type() ) )
           || ( 0 == strcasecmp( "marker", x.Type() ) ) 
           || ( 0 == strcasecmp( "Slot",   x.Type() ) )
           || ( 0 == strcasecmp( "srot",   x.Type() ) ) 
         );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int BmlUtil::makeCovariance( CSLattFuncs& w, Particle const& prtn,
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

  aa[i_x  ][ i_x   ]   = I1;
  aa[i_npx][ i_npx ]   = I1;
  aa[i_y  ][ i_y   ]   = I2;
  aa[i_npy][ i_npy ]   = I2;

  // Construct matrix of eigenvectors

  MatrixC E = MatrixC::Imatrix(n);
 
  CVLattFuncs lf(w);
 
  E[ i_x   ][0] =   sqrt( lf.beta.hor / 2.0 );
  E[ i_npx ][0] = - ( i + lf.alpha.hor )/sqrt( 2.0*lf.beta.hor );
  E[ i_y   ][1] =   sqrt( lf.beta.ver / 2.0 );
  E[ i_npy ][1] = - ( i + lf.alpha.ver )/sqrt( 2.0*lf.beta.ver );

  for( int j=0; j < 2; ++j) {
    int m = j + (n/2);
    for( int k=0; k < n; ++k) {
      E[k][m] = conj(E[k][j]);
    }
  }

  // This line is almost certainly not necessary.
  //   The matrix E already, has correct normalization,
  //   at least, as I normalize on Oct.29, 2004.
  //   But that may change. Who knows? So I'll retain the line.

  E = toNormalForm( E, Matrix::j_ordering); 

  // Finally, the actual calculation: one line ...

  //***************** FIXME !!!! 

  // w.covariance = real(E*aa*E.dagger());

  return 0;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BmlPtr    BmlUtil::cloneLineAndInsert( double                     percent,
                                       std::list<ElmPtr>&         insertions,
                                       std::list<ElmPtr>&         targets,
                                       beamline const&            bml )
{

  if( insertions.empty() || targets.empty() ) {
    return  BmlPtr( bml.Clone() );
  }

  if( percent < 0.0 ) { percent = 0.0; }
  if( percent > 1.0 ) { percent = 1.0; }

  bool upstream   = (0.0 == percent);
  bool downstream = (1.0 == percent);


  // Begin ...

  BmlPtr ret( new beamline( bml.Name() ) );

  ElmPtr spa;
  ElmPtr spb;


  beamline::const_iterator it;

  for ( it  = bml.begin(); it != bml.end(); ++it) {

    if( insertions.empty() || targets.empty() ) break; 

    if( (*it)->isBeamline() ) {
      ret->append( cloneLineAndInsert ( percent, insertions, targets,  dynamic_cast<beamline const&>(**it) ) );
    }

    else {

      // Here's where the real work is done.

      ElmPtr ins = insertions.front(); insertions.pop_front();
      ElmPtr trg = targets.front();       targets.pop_front();

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

          std::pair<ElmPtr,ElmPtr> elms = (*it)->split( percent); 
          ret->append( ElmPtr(elms.first->Clone()) );
          ret->append( ElmPtr(ins->Clone()) );
          ret->append( ElmPtr(elms.second->Clone()) );
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

  for (  ; it != bml.end(); ++it ) {
    ret->append( ElmPtr( (*it)->Clone() ) );
  }

  // Finished ...

  ret->setMomentum( bml.Momentum() );
  return ret;
  }



