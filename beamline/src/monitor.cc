/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      monitor.cc
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
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


#include "monitor.h"

using namespace std;

// **************************************************
//   class monitor 
// **************************************************

monitor::monitor() : bmlnElmnt() {
 outputFile = stdout;
 onOff      = 0;
 driftFraction = 0.5;
 rgr   = new double [ BMLN_dynDim ];
 for( int i = 0; i < BMLN_dynDim; i++ ) rgr[i] = 0.0;
}

monitor::monitor( char* n ) : bmlnElmnt(n) {
 outputFile = stdout;
 onOff      = 0;
 driftFraction = 0.5;
 rgr   = new double [ BMLN_dynDim ];
 for( int i = 0; i < BMLN_dynDim; i++ ) rgr[i] = 0.0;
}

monitor::monitor( char* n, double l ) : bmlnElmnt(n, l) {
 outputFile = stdout;
 onOff      = 0;
 driftFraction = 0.5;
 rgr   = new double [ BMLN_dynDim ];
 for( int i = 0; i < BMLN_dynDim; i++ ) rgr[i] = 0.0;
}

monitor::monitor( FILE* of ) : bmlnElmnt() {
 outputFile = of;
 onOff      = 0;
 driftFraction = 0.5;
 rgr   = new double [ BMLN_dynDim ];
 for( int i = 0; i < BMLN_dynDim; i++ ) rgr[i] = 0.0;
}

monitor::monitor( char* n, FILE* of ) : bmlnElmnt(n) {
 outputFile = of;
 onOff      = 0;
 driftFraction = 0.5;
 rgr   = new double [ BMLN_dynDim ];
 for( int i = 0; i < BMLN_dynDim; i++ ) rgr[i] = 0.0;
}

monitor::monitor( bmlnElmntData& x ) : bmlnElmnt( x ) {
 onOff = 0;
 driftFraction = 0.5;
 rgr   = new double [ BMLN_dynDim ];
 for( int i = 0; i < BMLN_dynDim; i++ ) rgr[i] = 0.0;
}

monitor::monitor( const monitor& x )
: bmlnElmnt( (bmlnElmnt&) x )
{
 onOff = x.onOff;
 driftFraction = x.driftFraction;
 rgr   = new double [ BMLN_dynDim ];
 for( int i = 0; i < BMLN_dynDim; i++ ) rgr[i] = x.rgr[i];
}

monitor::~monitor() {
 delete [] rgr;
}

void monitor::on() {
 onOff = 1;
}

void monitor::off() {
 onOff = 0;
}

void monitor::setOutputFile( FILE* of ) {
 outputFile = of;
}


const char* monitor::Type() const 
{ 
  return "monitor"; 
}


double monitor::operator[]( int n ) {
 if( n < 0 || n > 5 ) return 0.0;
 return rgr[n];
}

ostream& monitor::writeTo(ostream &os) {
  if ( Length() != 0 ) 
    os << driftFraction << endl;
  return os;
}

istream& monitor::readFrom(istream &is) {
  if ( Length() != 0 ) {
    is >> driftFraction ;
    if ( driftFraction < 0 || driftFraction > 1 ) {
      cerr << "monitor::readFrom(istream&): Read a drift fraction of " << driftFraction << "; substituting 0.5" << endl;
      driftFraction = 0.5;
    }
  } else {
    driftFraction = 0.5; // Basically irrelevant if the monitor has no length!
  }
  return is;
}


// **************************************************
//   class vmonitor 
// **************************************************

vmonitor::vmonitor() : monitor() {
}

vmonitor::vmonitor( char* n ) : monitor( n ){
}

vmonitor::vmonitor( char* n, double l ) : monitor( n,l ){
}

vmonitor::vmonitor( FILE* of ) : monitor( of ) {
}

vmonitor::vmonitor( char* n, FILE* of ) : monitor( n, of ) {
}

vmonitor::vmonitor( bmlnElmntData& x ) : monitor( x ) {
}

vmonitor::vmonitor( const vmonitor& x ) 
: monitor( (monitor&) x ) 
{
}

vmonitor::~vmonitor() {
}

double vmonitor::operator[]( int n ) {
 if( n == 0 ) return rgr[1];
 if( n == 1 ) return rgr[4];
 return 0.0;
}


const char* vmonitor::Type() const 
{ 
  return "vmonitor"; 
}


// **************************************************
//   class hmonitor 
// **************************************************

hmonitor::hmonitor() : monitor() {
}

hmonitor::hmonitor( char* n ) : monitor( n ){
}

hmonitor::hmonitor( char* n, double l ) : monitor( n, l ){
}

hmonitor::hmonitor( FILE* of ) : monitor( of ) {
}

hmonitor::hmonitor( char* n, FILE* of ) : monitor( n, of ) {
}

hmonitor::hmonitor( bmlnElmntData& x ) : monitor( x ) {
}

hmonitor::hmonitor( const hmonitor& x ) 
: monitor( (monitor&) x ) 
{
}

hmonitor::~hmonitor() {
}

double hmonitor::operator[]( int n ) {
 if( n == 0 ) return rgr[0];
 if( n == 1 ) return rgr[3];
 return 0.0;
}


const char* hmonitor::Type() const 
{ 
  return "hmonitor"; 
}
