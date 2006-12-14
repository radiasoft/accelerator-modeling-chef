/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      FindCovariance.cc
******  Version:   2.0
******                                                                
******  Copyright (c) 2001  Universities Research Association, Inc.   
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
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

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
 * 
 *  Added some formatting to the output to change 
 *  its look.
 *
 *  Version 3.1
 *  October 29, 1998
 *
 *  Leo Michelotti
 *  
 */

#include <iomanip>

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>
#include <mxyzptlk/Mapping.h>
#include <beamline/beamline.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/BeamlineIterator.h>
#include <physics_toolkit/FindCovariance.h>

using namespace std;

MatrixD FindCovariance( const beamline&    line,       
                        const slist&       sampleSites,
                        const JetParticle& jp ) 
{
  //******************************************************************
  // This routine will calculate the covariance matrix from a sequence
  // of width measurements.
  // This routine expects:
  //  const beamline& line       :  beamline in question.
  //  const slist&    sampleSites:  an slist of bmlnElmnts where profile
  //                                data has been attached as a
  //                                Barnacle.
  //  const JetParticle& jp      :  A suitable particle to propagate.
  //******************************************************************

  const int columns = 3;

  BeamlineIterator bi( const_cast<beamline&>(line) );
  slist_iterator NextSample ( sampleSites );
  bmlnElmnt* pbe_line;
  bmlnElmnt* pbe_sample;
  Vector zero(6);

  int rows = sampleSites.size();
  double  xsq, ysq;

  MatrixD C( 6, 6 );
  MatrixD M;
  MatrixD AH( rows, columns );
  MatrixD AV( rows, columns );
  MatrixD yH( rows, 1 ), xH( columns, 1 );
  MatrixD yV( rows, 1 ), xV( columns, 1 );
  Mapping map;
  int count = 0;

  JetParticle* jpPtr = jp.Clone();
  jpPtr->setState( zero );

  if( pbe_sample = (bmlnElmnt*) NextSample() ) {

    // Load the measurement matrix  ..................

    while( ( pbe_sample ) && ( pbe_line = bi++ ) ) {

      pbe_line->propagate( *jpPtr );
      
      if( pbe_line == pbe_sample ) {
        map = jpPtr->getState();
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
      ostringstream uic;
      uic << "Found " 
          << count << " marked bmlnElmnts but expected " 
          << rows
          << endl;
      delete jpPtr; jpPtr = 0;
      throw( GenericException( __FILE__, __LINE__, 
             "MatrixD FindCovariance( const beamline& line,       ", 
             uic.str().c_str() ) );
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

    delete jpPtr; jpPtr = 0;
    return C;
  }
  else {
    delete jpPtr; jpPtr = 0;
    throw( GenericException( __FILE__, __LINE__, 
           "MatrixD FindCovariance( const beamline& line,       ", 
           "Empty sampling list." ) );
  }
}


void TestCovariance( const beamline&    line,
                     const slist&       sampleSites,
                     const JetParticle& jp,
                     const MatrixD&     C ) 
{
  BeamlineIterator NextElement( const_cast<beamline&>(line) );
  slist_iterator NextSample ( sampleSites );
  bmlnElmnt* pbe_line;
  bmlnElmnt* pbe_sample;
  Vector zero(6);
  double xsq, ysq;
  MatrixD M, Cov;
  Mapping map;

  JetParticle* jpPtr = jp.Clone();
  jpPtr->setState( zero );
  (*FNAL::pcout).setf( ios::fixed );

  if( pbe_sample = (bmlnElmnt*) NextSample() ) {
    while( ( pbe_sample                            ) && 
           ( pbe_line = NextElement++ ) 
         ) {

      pbe_line->propagate( *jpPtr );

      if( pbe_line == pbe_sample ) 
      {
        map = jpPtr->getState();
        M = map.Jacobian();
        Cov = M*C*M.transpose();
      
        xsq = ( (WIREData*) pbe_line->dataHook.find("WIREData") )->hsigma;
        (*FNAL::pcout) << setw(12) << pbe_line->Name()
             << "  Data = "   
             << setw(8) << setprecision(3) << 1000.0*xsq
             << "  Theory = " 
             << setw(8) << setprecision(3) << 1000.0*sqrt( Cov( 0, 0 ) )
             << "  [mm]  Horizontal"
             << endl;
        
        ysq = ( (WIREData*) pbe_line->dataHook.find("WIREData") )->vsigma;
        (*FNAL::pcout) << setw(12) << pbe_line->Name()
             << "  Data = "   
             << setw(8) << setprecision(3) << 1000.0*ysq
             << "  Theory = " 
             << setw(8) << setprecision(3) << 1000.0*sqrt( Cov( 1, 1 ) )
             << "  [mm]  Vertical"
             << endl;
        
        pbe_sample = (bmlnElmnt*) NextSample();
      } 
      else 
      {
        map = jpPtr->getState();
        M = map.Jacobian();
        Cov = M*C*M.transpose();
      
        (*FNAL::pcout) << setw(12) << pbe_line->Name()
             << "                 "
             << "  Theory = " 
             << setw(8) << setprecision(3) << 1000.0*sqrt( Cov( 0, 0 ) )
             << "  [mm]  Horizontal"
             << endl;
        
        (*FNAL::pcout) << setw(12) << pbe_line->Name()
             << "                 "
             << "  Theory = " 
             << setw(8) << setprecision(3) << 1000.0*sqrt( Cov( 1, 1 ) )
             << "  [mm]  Vertical"
             << endl;
      }
    }
  }

  else {
    throw( GenericException( __FILE__, __LINE__, 
           "void TestCovariance( const beamline& line,", 
           "Empty sampling list." ) );
  }

}

