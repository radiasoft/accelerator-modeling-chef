/*
 *  FindCovariance & TestCovariance
 *  
 *  Utility to find initial covariance matrix from 
 *  a sequence of width measurements.  Version 2.0
 *  enforces symmetry and decoupling as constraints.
 *  
 *  Version 3.0
 *  June 23, 1995
 *  
 *  Leo Michelotti
 *  
 */

#include "beamline.h"
#include "FindCovariance.h"

const int columns = 3;

MatrixD FindCovariance( const beamline& line,       
                        const slist&    sampleSites,
			      JetProton jp ) {
//******************************************************************
// This routine will calculate the covariance matrix from a sequence
// of width measurements.
// This routine expects:
//  const beamline& line       :  beamline in question.
//  const slist&    sampleSites:  an slist of bmlnElmnts where profile
//                                data has been attached as a
//                                Barnacle.
//        JetProton jp         :  A suitable particle to propagate.
//******************************************************************

  dlist_iterator NextElement( (dlist&) line );
  slist_iterator NextSample (          sampleSites );
  bmlnElmnt* pbe_line;
  bmlnElmnt* pbe_sample;
  double zero [] = { 0., 0., 0., 0., 0., 0. };

  int rows = sampleSites.size();
//  int rows = 12;  Why was this fixed to 12?

  double  xsq, ysq;

  MatrixD C( 6, 6 );
  MatrixD M;
  MatrixD AH( rows, columns );
  MatrixD AV( rows, columns );
  MatrixD yH( rows, 1 ), xH( columns, 1 );
  MatrixD yV( rows, 1 ), xV( columns, 1 );
  Mapping map;
  int count = 0;
  jp.setState( zero );

  if( pbe_sample = (bmlnElmnt*) NextSample() ) {

    // Load the measurement matrix  ..................

    while( ( pbe_sample                            ) && 
           ( pbe_line = (bmlnElmnt*) NextElement() ) 
         ) {

      pbe_line->propagate( jp );
      
      if( pbe_line == pbe_sample ) {
	jp.getState(map);
        M = map.Jacobian();
      
        xsq = ( (WIREData*) pbe_line->dataHook.find("WIREData") )->hsigma;
        yH( count, 0 ) = xsq*xsq;
        AH( count, 0 ) =       M( 0, 0 ) * M( 0, 0 );
        AH( count, 1 ) = 2.0 * M( 0, 0 ) * M( 0, 3 );
        AH( count, 2 ) =       M( 0, 3 ) * M( 0, 3 );
        
        ysq = ( (WIREData*) pbe_line->dataHook.find("WIREData") )->vsigma;
        yV( count, 0 ) = ysq*ysq;
        AV( count, 0 ) =       M( 1, 1 ) * M( 1, 1 );
        AV( count, 1 ) = 2.0 * M( 1, 1 ) * M( 1, 4 );
        AV( count, 2 ) =       M( 1, 4 ) * M( 1, 4 );

        count++;
      
        pbe_sample = (bmlnElmnt*) NextSample();
      } 
    }

    // A little check .........................................

    if( count != rows ) {
      cerr << "*** ERROR ***                                  \n"
              "*** ERROR *** FindCovariance                   \n"
              "*** ERROR *** Not all data samples were found. \n"
              "*** ERROR ***                                  \n"
           << endl;
      exit(1);
    }

    // Solve the equations and return the answer ..............

    xH = ( AH.transpose()*AH ).inverse() * AH.transpose() * yH;
    xV = ( AV.transpose()*AV ).inverse() * AV.transpose() * yV;

    C( 0, 0 )             = xH( 0, 0 );
    C( 0, 3 ) = C( 3, 0 ) = xH( 1, 0 );
    C( 3, 3 )             = xH( 2, 0 );

    C( 1, 1 )             = xV( 0, 0 );
    C( 1, 4 ) = C( 4, 1 ) = xV( 1, 0 );
    C( 4, 4 )             = xV( 2, 0 );

    return C;
  }

  else {
    cerr << "*** ERROR ***                            \n"
         << "*** ERROR *** FindCovariance             \n"
         << "*** ERROR *** Empty sampling list        \n"
         << "*** ERROR ***                            \n"
         << endl;
    exit(1);
    return C;
  }
}

void TestCovariance( const beamline& line,
                     const slist&    sampleSites,
                     JetProton       jp,
                     const MatrixD&  C ) {
  dlist_iterator NextElement( (dlist&) line );
  slist_iterator NextSample (          sampleSites );
  bmlnElmnt* pbe_line;
  bmlnElmnt* pbe_sample;
  double zero [] = { 0., 0., 0., 0., 0., 0. };
  double xsq, ysq;
  MatrixD M, Cov;
  Mapping map;

  jp.setState( zero );

  if( pbe_sample = (bmlnElmnt*) NextSample() ) {
    while( ( pbe_sample                            ) && 
           ( pbe_line = (bmlnElmnt*) NextElement() ) 
         ) {

      pbe_line->propagate( jp );

      if( pbe_line == pbe_sample ) {
	jp.getState(map);
        M = map.Jacobian();
        Cov = M*C*M.transpose();
      
        xsq = ( (WIREData*) pbe_line->dataHook.find("WIREData") )->hsigma;
        cout << "At element : " << pbe_line->Name()
             << "  Data = "   << xsq*xsq
             << "  Theory = " << Cov( 0, 0 )
             << "  Horizontal"
             << endl;
        
        ysq = ( (WIREData*) pbe_line->dataHook.find("WIREData") )->vsigma;
        cout << "At element : " << pbe_line->Name()
             << "  Data = "   << ysq*ysq
             << "  Theory = " << Cov( 1, 1 )
             << "  Vertical"
             << endl;
        
        pbe_sample = (bmlnElmnt*) NextSample();
      } 
    }
  }

  else {
    cerr << "*** ERROR ***                            \n"
         << "*** ERROR *** TestCovariance             \n"
         << "*** ERROR *** Empty sampling list        \n"
         << "*** ERROR ***                            \n"
         << endl;
    if( 1 + 1 == 2 ) exit(1);
  }

}

