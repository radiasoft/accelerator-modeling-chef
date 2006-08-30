/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File: SurveyMatcher.cc
******                                                                
******  Copyright (c) Universities Research Association, Inc.    
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
****** Revision History:
******
******   September, 2005
******     First version
******
**************************************************************************
*************************************************************************/


#include <iostream>

#include <physics_toolkit/SurveyMatcher.h>
#include <basic_toolkit/GenericException.h>
#include <beamline/beamline.h>
#include <physics_toolkit/FramePusher.h>

using namespace std;


void SurveyMatcher::print( vector<Vector>& x ) const
{
  int i = 0;
  vector<Vector>::iterator curDatum;
  for( curDatum  = x.begin();
       curDatum != x.end();
       curDatum++                      ) 
  {
    cout << ++i << ": " << *curDatum << endl;
  }  
}


SurveyMatcher::SurveyMatcher( const vector<Vector>& v, beamline* b )
:   Sage( b, true )
  , _surveyData(v)
{
  _finishConstructor();
}


SurveyMatcher::SurveyMatcher( const vector<Vector>& v, beamline& b )
: Sage( &b, true )
  , _surveyData(v)
{
  _finishConstructor();
}


void SurveyMatcher::_finishConstructor()
{
  FramePusher fp;
  DeepBeamlineIterator dbi( _myBeamlinePtr );
  bmlnElmnt* q = 0;

  while( ( 0 != (q = dbi++) ) ) {
    q->accept( fp );
    _modelCoordinates.push_back(fp.getFrame().getOrigin());
  }

  _inputModel = _modelCoordinates;

  this->removeCentroid( _surveyData, _centralDataFrame );
  this->orientPlane( _surveyData, _centralDataFrame );

  this->removeCentroid( _modelCoordinates, _centralModelFrame );
  this->orientPlane( _modelCoordinates, _centralModelFrame );
  this->rotate();
  
  this->_diff();
  this->_generateAlignments();
}


SurveyMatcher::~SurveyMatcher()
{
}


void SurveyMatcher::eraseAll()
{
  *_outputStreamPtr << "ClosedOrbitSage::eraseAll() does nothing." << endl;
}


void SurveyMatcher::removeCentroid( vector<Vector>& stuff, Frame& f )
{
  // PRECONDITIONS:
  //   stuff: collection of coordinates
  // POSTCONDITIONS: 
  //   stuff: has had centroid subtracted; 
  //          is now centered on (0,0,0)
  //   f: origin has been set to centroid of
  //      of input data (stuff)
  vector<Vector> bufferData;
  Vector centroid(3);
  double n = stuff.size();

  vector<Vector>::iterator curDatum;
  for( curDatum  = stuff.begin();
       curDatum != stuff.end();
       curDatum++                      ) 
  {
    centroid += *curDatum;
  }
  centroid /= n;
  
  for( curDatum  = stuff.begin();
       curDatum != stuff.end();
       curDatum++                      ) 
  {
    bufferData.push_back( (*curDatum) - centroid );
  }
  stuff.swap( bufferData );

  f.setOrigin( centroid );
}


void SurveyMatcher::orientPlane( vector<Vector>& stuff, Frame& f )
{
  // PRECONDITIONS: 
  //   stuff: coordinate data whose centroid is already (0,0,0)
  // POSTCONDITIONS:
  //   stuff:       centroid remains (0,0,0);
  //                vectors rotated so that (x,y) is the data plane
  //   f:           origin remains unchanged;
  //                axes are a right-handed system with +z orthogonal
  //                to the output stuff plane

  Vector o( f.getOrigin() );
  f.reset();

  Matrix M(3,3);
  vector<Vector>::iterator curDatum;
  for( curDatum  = stuff.begin();
       curDatum != stuff.end();
       curDatum++                      ) 
  {
    for( int i = 0; i < 3; i++ ) {
      for( int j = 0; j < 3; j++ ) {
        M(i,j) += (*curDatum)(i) * (*curDatum)(j);
      }
    }
  }  
  Matrix lambda(3,3);
  Matrix e(3,3);
  
  e = real( M.eigenVectors() );

  // Normalize
  lambda = e.transpose()*e;
  for( int j = 0; j < 3; j++ ) {
    double denom = sqrt( lambda(j,j) );
    for( int i = 0; i < 3; i++ ) {
      e(i,j) /= denom;
    }
  }

  lambda = e.inverse() * M * e;
  // REMOVE? lambda = e.transpose() * M * e;
  // REMOVE: lambda = real( M.eigenVectors() );

  double minLambda = lambda(0,0);
  int index = 0;
  for( int i = 1; i < 3; i++ ) {
    if( lambda(i,i) < minLambda ) {
      index = i;
      minLambda = lambda(i,i);
    }
  }
  
  if( index != Frame::zAxisIndex() ) {
    e.switch_columns( index, Frame::zAxisIndex() );
    double temp = lambda( index, index );
    lambda( index, index ) = lambda( Frame::zAxisIndex(), Frame::zAxisIndex() );
    lambda( Frame::zAxisIndex(), Frame::zAxisIndex() ) = temp;
  }

  if( !(f.setOrthonormalAxes(e)) ) {
   ostringstream uic;
   uic  << "An impossible non-orthonormal frame:\n\n"
        << e 
        << "\n\n" << e.transpose()*e;
   throw( GenericException( __FILE__, __LINE__, 
          "void SurveyMatcher::orientPlane(...)", 
          uic.str().c_str() ) );
  }
  if( !(f.isRightHanded()) ) {
    f.setAxis( Frame::xAxisIndex(), - f.getxAxis() );
  }


  // Orient for clockwise traversal in the (x,y) plane.
  Vector a(3), b(3), c(3);
  a = stuff[0];
  b = stuff[ stuff.size()/4 ];
  f.convertInPlace( a, c );
  f.convertInPlace( b, c );
  if( 0 < (b^a)(2) ) {
    f.setAxis( Frame::yAxisIndex(), - f.getyAxis() );
    f.setAxis( Frame::zAxisIndex(), - f.getzAxis() );
  }


  // Store the transformed coordinates
  Vector p(3), v(3);
  vector<Vector> bufferData;
  for( curDatum  = stuff.begin();
       curDatum != stuff.end();
       curDatum++                      ) 
  {
    p = *curDatum;
    f.convertInPlace( p, v );
    bufferData.push_back( p );
  }
  stuff.swap( bufferData );


  // Restore the initial origin
  f.setOrigin(o);
}


void SurveyMatcher::rotate()
{
  // PRECONDITION:
  //   _surveyData:       in (x,y) plane
  //   _modelCoordinates: in (x,y) plane
  // POSTCONDITION:
  //   _surveyData:       unchanged
  //   _modelCoordinates: rotated about z-axis to bring into
  //                        correspondence with surveyData
  //   _centralModelFrame: axes rotated about z-axis by same angle
  double phi = 0.0;
  Vector a(3), b(3);
  double sn, cs, denom, anorm, bnorm;
  double sum_sn = 0;
  double sum_cs = 0;
  int count = 0;

  int n = _surveyData.size();
  if( n == _modelCoordinates.size() ) {
    for( int i = 0; i < n; i++ ) {
      a = _modelCoordinates[i];
      a(2) = 0;
      b = _surveyData[i];
      b(2) = 0;
      anorm = a.Norm();
      bnorm = b.Norm();
      if( (1.0e-6 < std::abs(anorm)) && (1.0e-6 < std::abs(bnorm)) ) {
        denom = anorm*bnorm;
        sn    = (a^b)(2) / denom;
        cs    = (a*b)    / denom;
        sum_sn += sn;
        sum_cs += cs;
        count++;
      }
    }
    sn = sum_sn / ((double) count);
    cs = sum_cs / ((double) count);


    // Extreme paranoia, but necessary
    denom = sqrt(cs*cs + sn*sn);
    sn /= denom;
    cs /= denom;

    if( std::abs(sn) < 1.0e-11 ) {
      sn = 0;
      if( 0 < cs ) {
        cs = 1;
      }
      else {
        cs = - 1;
      }
    }
    else if( std::abs(cs) < 1.0e-11 ) {
      cs = 0;
      if( 0 < sn ) {
        sn = 1;
      }
      else {
        sn = -1;
      }
    }

    phi  = atan2( sn, cs );

    if( 1.0e-10 < std::abs(phi) ) {
      Matrix R(3,3);
      R(0,0) =   cos(phi);  R(0,1) = - sin(phi);
      R(1,0) =   sin(phi);  R(1,1) =   cos(phi);
      R(2,2) = 1.0;

      vector<Vector> bufferData;
      for( int i = 0; i < n; i++ ) {
        bufferData.push_back( R*_modelCoordinates[i] );
      }
      _modelCoordinates.swap( bufferData );

      _centralModelFrame.rotate( phi, _centralModelFrame.getzAxis(), false );
    }
  }
  else {
    ostringstream uic;
    uic  << "Impossible: _surveyData.size() != _modelCoordinates.size()";
    throw( GenericException( __FILE__, __LINE__, 
           "void SurveyMatcher::rotate()", 
           uic.str().c_str() ) );
  }
}


void SurveyMatcher::_diff()
{
  // PRECONDITIONS:
  //   _surveyData and _modelCoordinates must be expressed
  //     in same frame
  // POSTCONDITIONS:
  //   _surveyData: contains residuals, obtained by subtraction
  int n = _surveyData.size();
  if( n == _modelCoordinates.size() ) {
    _bufferData.clear();
    for( int i = 0; i < n; i++ ) {
      _bufferData.push_back( _surveyData[i] - _modelCoordinates[i] );
    }  
    _surveyData.swap( _bufferData );
    _bufferData.clear();
  }
  else {
    ostringstream uic;
    uic  << "Impossible: _surveyData.size() != _modelCoordinates.size()";
    throw( GenericException( __FILE__, __LINE__, 
           "void SurveyMatcher::orientPlane(...)", 
           uic.str().c_str() ) );
  }
}


void SurveyMatcher::_generateAlignments()
{
  // PRECONDITIONS:
  //   _surveyData:       contains residuals referenced to global frame
  //   _centralModelFrame: the oriented, centroid frame; referenced
  //                       to the same global frame;
  //                       model coordinates will be regenerated in
  //                       the global frame.
  // POSTCONDITIONS:
  //   _modelCoordinates: contains residuals referenced to
  //                      local frames
  FramePusher fp;
  Frame localFrame;
  beamline* bmlPtr = _myBeamlinePtr;
  DeepBeamlineIterator dbi( bmlPtr );
  bmlnElmnt* q = 0;
  Vector r(3), dr(3);
  int i = 0;

  _modelCoordinates.clear();
  while( ( 0 != (q = dbi++) ) ) {
    q->accept( fp );
    localFrame = fp.getFrame();
    r = localFrame.getOrigin();
    dr = _surveyData[i];
    localFrame.relativeTo(_centralModelFrame).convertInPlace(r,dr);
    _modelCoordinates.push_back(dr);
    i++;
  }
}


alignment SurveyMatcher::getAlignment( int i ) const
{
  alignmentData ret;
  Vector dr(3);
  dr = _modelCoordinates[i];
  ret.xOffset = dr(0);
  ret.yOffset = dr(1);
  ret.tilt = 0.0;
  return alignment( ret );
}


