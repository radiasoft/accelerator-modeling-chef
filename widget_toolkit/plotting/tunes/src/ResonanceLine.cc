/*************************************************************************
**************************************************************************
**************************************************************************
******                                    
******  File:   ResonanceLine.cc   
******                                                                
******  Copyright (c) Fermi Research Alliance LLC
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359.
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov                         
****** 
**************************************************************************
**************************************************************************
*************************************************************************/

#include <TuneDiagram.h>
#include <iostream>
#include <cstdlib>

// double find_x( double y) { return  (n_ - ny_ * y ) / nx_ ; } 
// double find_y( double x) { return  (n_ - nx_ * x ) / ny_ ; } 


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TuneDiagram::ResonanceLine::ResonanceLine( int nx, int ny, int n )
  : QwtData(), nx_( double(nx) ), ny_( double(ny) ), n_( double(n) ) 
{} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TuneDiagram::ResonanceLine::ResonanceLine( ResonanceLine const& o)
  : QwtData(o), nx_(o.nx_), ny_(o.ny_), n_(o.n_)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TuneDiagram::ResonanceLine::~ResonanceLine()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TuneDiagram::ResonanceLine* TuneDiagram::ResonanceLine::copy () const
{
  return new ResonanceLine(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

size_t  TuneDiagram::ResonanceLine::size () const
{ 
  return 2; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double TuneDiagram::ResonanceLine::x(size_t i) const
{
  //---------------------------
  // nx_ * nux +  ny_ * nuy = n_ 
  //----------------------------

  double val = (abs(ny_) < 1.0e-10) ? n_/nx_ : ( (i==0) ? 0.0 : 1.0 );
  return val;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double TuneDiagram::ResonanceLine::y(size_t i)    const
{

  double val = ( abs(ny_) < 1.0e-10 ) ? ((i==0) ? 0.0 : 1.0 ) : (( n_ - nx_ *x(i)) / ny_ );   
  return val;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if 0 
QwtDoubleRect   TuneDiagram::ResonanceLine::boundingRect ()
{
  return QwtDoubleRect(double left, double top, double width, double height);
}
#endif
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

