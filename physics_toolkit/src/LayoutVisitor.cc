/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      LayoutVisitor.cc
******  Version:   1.1
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
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


/*
 * File: LayoutVisitor.cc
 * 
 * class LayoutVisitor
 * 
 * Provides coordinates for simple 2D diagram
 * of the beamline. FramePusher should be used
 * for more detailed site maps.
 *
 * Leo Michelotti
 * May 7, 2002
 * 
 * Version 1.1  June 27, 2003
 *
 */


#include "LayoutVisitor.h"

#include "bmlnElmnt.h"
#include "quadrupole.h"
#include "rbend.h"
#include "sbend.h"
#include "CF_rbend.h"
#include "CF_sbend.h"
#include "sextupole.h"
#include "sector.h"

// Static error codes

const int LayoutVisitor::OKAY          = 0;
const int LayoutVisitor::SECTORVISITED = 1;
const int LayoutVisitor::NOFILEOPENED  = 2;

using namespace std;

// Constructors

LayoutVisitor::LayoutVisitor( double bsl, double bh, double qh, double sh )
: _s(0.0),
  _baseline(bsl),
  _bendHeight(bh),
  _quadHeight(qh),
  _sextHeight(sh),
  _specialHeight(0.0),
  _streamPtr(0),
  _errorCode(OKAY),
  _ptrCritFunc(0)
{
}


LayoutVisitor::LayoutVisitor( const LayoutVisitor& x )
: _s(0.0),
  _baseline(x._baseline),
  _bendHeight(x._bendHeight),
  _quadHeight(x._quadHeight),
  _sextHeight(x._sextHeight),
  _specialHeight(x._specialHeight),
  _streamPtr(0),
  _errorCode(x._errorCode),
  _ptrCritFunc(x._ptrCritFunc)
{
  static bool firstTime = true;
  if( firstTime ) {
    cerr << "\n*** WARNING *** "
            "\n*** WARNING *** You should not copy a LayoutVisitor."
            "\n*** WARNING *** "
         << endl;
    firstTime = false;
  }
}


LayoutVisitor::~LayoutVisitor()
{
  if( _streamPtr ) {
    this->closeFile();
  }
}


void LayoutVisitor::setDiscriminator( bmlnElmnt::Discriminator* dsc, 
                                      double hght )
{
  _specialHeight = hght;
  _ptrCritFunc = dsc;
}


// Visiting functions

void LayoutVisitor::visitSector   ( sector*     )
{
  cerr << "*** WARNING ***                                \n"
       << "*** WARNING *** LayoutVisitor::visitSector       \n"
       << "*** WARNING ***                                \n"
       << "*** WARNING *** Sectors are not handled        \n"
       << "*** WARNING *** properly. Errorcode is         \n"
       << "*** WARNING *** being set. Results will be     \n"
       << "*** WARNING *** unreliable.                    \n"
       << "*** WARNING ***                                \n"
       << endl;
  _errorCode = SECTORVISITED;
}


void LayoutVisitor::visitBmlnElmnt( bmlnElmnt* x )
{
  if( _streamPtr ) {

    if( _ptrCritFunc ) {
      if( (*_ptrCritFunc)(x) ) {
        _processSpecialElement( x );
        return;
      }
    }

    if( x->Length() > 0.0 ) {
      _s += x->Length();
      (*_streamPtr) << _s << "  " << _baseline << endl;
    }
  }

  else {
    _errorCode = NOFILEOPENED;
  }
}


void LayoutVisitor::visitQuadrupole( quadrupole* x )
{
  if( _streamPtr ) {

    if( _ptrCritFunc ) {
      if( (*_ptrCritFunc)(x) ) {
        _processSpecialElement( x );
        return;
      }
    }

    if( 0 == x->Strength() ) {
      this->visitBmlnElmnt(x);
    }
    else if ( 0 < x->Strength() ) {
      (*_streamPtr) << _s << "  " << _baseline + _quadHeight << endl;
      _s += x->Length();
      (*_streamPtr) << _s << "  " << _baseline + _quadHeight << endl;
      (*_streamPtr) << _s << "  " << _baseline << endl;
    }
    else {
      (*_streamPtr) << _s << "  " << _baseline - _quadHeight << endl;
      _s += x->Length();
      (*_streamPtr) << _s << "  " << _baseline - _quadHeight << endl;
      (*_streamPtr) << _s << "  " << _baseline << endl;
    }
  }
  else {
    _errorCode = NOFILEOPENED;
  }
}


void LayoutVisitor::_visit_bend( bmlnElmnt* x )
{
  if( _streamPtr ) {

    if( _ptrCritFunc ) {
      if( (*_ptrCritFunc)(x) ) {
        _processSpecialElement( x );
        return;
      }
    }

    if( 0 == x->Strength() ) {
      this->visitBmlnElmnt(x);
    }
    else if ( 0 < x->Strength() ) {
      (*_streamPtr) << _s << "  " << _baseline + _bendHeight << endl;
      _s += x->Length();
      (*_streamPtr) << _s << "  " << _baseline + _bendHeight << endl;
      (*_streamPtr) << _s << "  " << _baseline << endl;
    }
    else {
      (*_streamPtr) << _s << "  " << _baseline - _bendHeight << endl;
      _s += x->Length();
      (*_streamPtr) << _s << "  " << _baseline - _bendHeight << endl;
      (*_streamPtr) << _s << "  " << _baseline << endl;
    }
  }
  else {
    _errorCode = NOFILEOPENED;
  }
}


void LayoutVisitor::visitRbend( rbend* x )
{
  this->_visit_bend(x);
}


void LayoutVisitor::visitSbend( sbend* x )
{
  this->_visit_bend(x);
}


void LayoutVisitor::visitCF_rbend( CF_rbend* x )
{
  this->_visit_bend(x);
}


void LayoutVisitor::visitCF_sbend( CF_sbend* x )
{
  this->_visit_bend(x);
}


void LayoutVisitor::visitSextupole( sextupole* x )
{
  if( _streamPtr ) {

    if( _ptrCritFunc ) {
      if( (*_ptrCritFunc)(x) ) {
        _processSpecialElement( x );
        return;
      }
    }

    if( 0 == x->Strength() ) {
      this->visitBmlnElmnt(x);
    }
    else if ( 0 < x->Strength() ) {
      (*_streamPtr) << _s << "  " << _baseline + _sextHeight << endl;
      _s += x->Length();
      (*_streamPtr) << _s << "  " << _baseline + _sextHeight << endl;
      (*_streamPtr) << _s << "  " << _baseline << endl;
    }
    else {
      (*_streamPtr) << _s << "  " << _baseline - _sextHeight << endl;
      _s += x->Length();
      (*_streamPtr) << _s << "  " << _baseline - _sextHeight << endl;
      (*_streamPtr) << _s << "  " << _baseline << endl;
    }
  }
  else {
    _errorCode = NOFILEOPENED;
  }
}


// File manipulation

int LayoutVisitor::openFile( const char* fileName )
{
  if( _streamPtr ) {
    this->closeFile();
  }
  _streamPtr = new ofstream( fileName );

  // Record first point
  (*_streamPtr) << _s << "  " << _baseline << endl;
}


int LayoutVisitor::closeFile()
{
  int ret;

  if( _streamPtr ) {
    (*_streamPtr) << endl;
    (*_streamPtr) << 0.0 << "  " << _baseline << endl;
    (*_streamPtr) << _s  << "  " << _baseline << endl;
    _streamPtr->close();
    delete _streamPtr;
    _streamPtr = 0;
    ret = OKAY;
  }

  else {
    ret = NOFILEOPENED;
  }

  return ret;
}


int LayoutVisitor::getErrorCode() const
{
  return _errorCode;
}


void LayoutVisitor::_processSpecialElement( bmlnElmnt* x )
{
  (*_streamPtr) << _s << "  " << _baseline + _specialHeight << endl;
  _s += x->Length();
  (*_streamPtr) << _s << "  " << _baseline + _specialHeight << endl;
  (*_streamPtr) << _s << "  " << _baseline << endl;
}
