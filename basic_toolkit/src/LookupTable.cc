#if HAVE_CONFIG_H
#include <config.h>
#endif
/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      LookupTable.cc
******                                                                
******  Copyright (c) 1990 Universities Research Association, Inc.    
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


#include <LookupTable.h>
#include <MathConstants.h>

#include <iostream>

using namespace std;

#ifndef ABS
inline double ABS(double x) {
  if ( x < 0 )
    return -x;
  else
    return x;
}
#endif

// ----------------------------------------------------------------------
// Table methods
Table::Table(int r, int c) {
  valid = false;
  num_rows = r;
  num_columns = c;
  table = NULL;
  makeTable();
  valid = true;
}

Table::Table(Table &t) {
  num_rows = t.getRows();
  num_columns = t.getColumns();
  setTable(t.getTable());
}

Table::~Table() {
  // Clean up the malloc'ed table
  if ( table != NULL ) {
    for ( int i=0; i<num_rows; i++ )
      delete table[i];
    delete table;
  }
}

void Table::makeTable() {
  if ( table ) delete table;
  table = new double*[num_rows];
  for ( int i=0; i<num_rows; i++ )
    table[i] = new double[num_columns];
}

Table::Table(int r, int c, double** f) {
  num_rows = r;
  num_columns = c;
  table = NULL;
  makeTable();
  valid = true;
  setTable(f);
}

void Table::setRow(int r, double* v) {
  if ( valid && v != NULL ) {
    for ( int i=0; i<getColumns(); i++ ) 
      table[r][i] = v[i];
  }
}

void Table::setTableSize(int r, int c) {
  num_rows = r;
  num_columns = c;
  makeTable();
  valid = true;
}

void Table::setTable(double** v) {
  if ( valid && v != NULL ) {
    if ( table == NULL ) {
      // First time through, make the table to hold this stuff.
      table = new double*[getRows()];
      for ( int m=0; m<getRows(); m++ )
	table[m] = new double[getColumns()];
    }

    for ( int i=0; i<getRows(); i++ )
      // Note: No checking to see if user has passed sane information
      for ( int j=0; j<getColumns(); j++ )
	table[i][j] = v[i][j];
  }
}    

// ----------------------------------------------------------------------
// LookupTable methods

LookupTable::LookupTable() : Table() {
  xGrid = NULL;
  yGrid = NULL;
  xstep = 0;
  ystep = 0;
  ready = 0;
  interpolationType = 4;
}

LookupTable::LookupTable(int x, int y) : Table(x,y) {
  xsize = x;
  ysize = y;
  xGrid = new double[xsize];
  yGrid = new double[ysize];
  xstep = 0;
  ystep = 0;
  ready = 1;
  interpolationType = 4;
}

LookupTable::LookupTable(int x, int y, double *xg, double *yg, double **f) : Table(x, y, f) {
  xsize = x;
  ysize = y;
  xGrid = new double[xsize];
  yGrid = new double[ysize];
  setXGrid(xg);
  setYGrid(yg);
  ready = 1;
  interpolationType = 4;
}

LookupTable::LookupTable(LookupTable &lt) {
  // Copy constructor
  xsize = lt.getXSize();
  ysize = lt.getYSize();
  xGrid = lt.getXGrid();
  yGrid = lt.getYGrid();
  interpolationType = lt.getInterpolationType();
  setTable(lt.getTable());
}

LookupTable::~LookupTable() {
  delete xGrid;
  delete yGrid;
}

void LookupTable::setXGrid(double *xg) {
  if ( ready ) {
    if ( xGrid != NULL ) delete xGrid;
    xGrid = new double[xsize];

    for ( int i=0; i<xsize; i++ )
      xGrid[i] = xg[i];
  }
  // Assume evenly-spaced grid!
  xstep = xGrid[1] - xGrid[0];

  if ( ABS(xstep - (xGrid[xsize-1] - xGrid[xsize-2])) > 1E-5 ) {
    cerr << "LookupTable:setXGrid(): Non uniform grid being specified, "
	 << xGrid[xsize-1] << " " << xGrid[xsize-1] << " " << xstep << endl;
  }
}

void LookupTable::setYGrid(double *yg) {
  if ( ready ) {
    if ( yGrid != NULL ) delete yGrid;
    yGrid = new double[ysize];

    for ( int i=0; i<ysize; i++ )
      yGrid[i] = yg[i];
  }
  ystep = yGrid[1] - yGrid[0];

  if ( ABS(ystep - (yGrid[ysize-1] - yGrid[ysize-2])) > 1E-5 ) {
    cerr << "LookupTable:setYGrid(): Non uniform grid being specified, "
	 << yGrid[ysize-1] << " " << yGrid[ysize-1] << " " << ystep << endl;
  }
}

double LookupTable::interpolate2(double x, double xmin, double xmax,
				double fxmin, double fxmax) 
{
  // Linear interpolation.  

  // This should linearly extrapolate, based on the two end values.
  return ((x-xmin)/(xmax-xmin)) * (fxmax-fxmin) + fxmin;
}

double LookupTable::interpolate3(double x,  double x0, double x1, double x2,
				 double y0, double y1, double y2) 
{
  // Three-point interpolation; better than 2-point "weighted average"
  double t1, t2, t3;

  t1 = y0 * (x-x1)*(x-x2)/((x0-x1)*(x0-x2));
  t2 = y1 * (x-x0)*(x-x2)/((x1-x0)*(x1-x2));
  t3 = y2 * (x-x0)*(x-x1)/((x2-x0)*(x2-x1));
  double p = t1 + t2 + t3;
  
  return p;
}

double LookupTable::interpolate4(double x,  double x0, double x1, double x2, double x3,
				 double y0, double y1, double y2, double y3) 
{
  // four-point interpolation, assuming evenly-spaced grid
  // Really excellent agreement with "eyeball" interpolation, including wicked extrapolation!
  double t1, t2, t3, t4, h, p;
  h = x1-x0;

  if ( h != 0 ) {
    t1 = - y0 * (x-x1)*(x-x2)*(x-x3)/6.0;
    t2 =   y1 * (x-x0)*(x-x2)*(x-x3)/2.0;
    t3 = - y2 * (x-x0)*(x-x1)*(x-x3)/2.0;
    t4 =   y3 * (x-x0)*(x-x1)*(x-x2)/6.0;
    p = (1.0/(h*h*h)) * (t1 + t2 + t3 + t4);
  } else {
    p = 0;
  }
  // printf("X: %.1f %.1f %.1f %.1f %.1f -- Y: %.1f %.1f %.1f %.1f = %.1f\n",
  // x, x0, x1, x2, x3, y0, y1, y2, y3, p);
  return p;
}

double LookupTable::evaluate2(double x, double y) {
  // Find indexes of x and y in xGrid and yGrid
  int ix, iy;
  for ( ix=1; ix<xsize; ix++ )
    if ( x < xGrid[ix] ) break;
  if ( ix >= xsize ) ix = xsize-1;

  for ( iy=1; iy<ysize; iy++ )
    if ( y < yGrid[iy] ) break;
  if ( iy >= ysize ) iy = ysize-1;

  // Determine the "low" and "hi" values of x along y
  double xlow = interpolate2(x, xGrid[ix-1], xGrid[ix],
			    (*this)[ix-1][iy-1], (*this)[ix][iy-1]);
  double xhi  = interpolate2(x, xGrid[ix-1], xGrid[ix],
			    (*this)[ix-1][iy], (*this)[ix][iy]);
  // Now find the interpolated value between the x's
  double retval = interpolate2(y, yGrid[iy-1], yGrid[iy], xlow, xhi);
  return retval;
}

double LookupTable::evaluate3(double x, double y) {
  // Find indexes of x and y in xGrid and yGrid using 3-point interpolation
  int ix, iy, noshiftx=0, noshifty=0;
  int ix1, ix2, iy1, iy2;

  for ( ix=1; ix<xsize; ix++ )
    if ( x < xGrid[ix] ) break;
  ix1 = ix + 1;
  ix2 = ix + 2;
  if ( ix2 >= xsize ) {
    int diff = ix2-xsize+1;
    ix2 -= diff;
    ix1 -= diff;
    ix  -= diff;
    noshiftx = 1;
  }

  for ( iy=1; iy<ysize; iy++ )
    if ( y < yGrid[iy] ) break;
  iy1 = iy + 1;
  iy2 = iy + 2;
  if ( iy2 >= ysize ) {
    int diff = iy2-ysize+1;
    iy2 -= diff;
    iy1 -= diff;
    iy  -= diff;
    noshifty = 1;
  }

  // Determine the "low" and "hi" values of x along y
  double fx0 = interpolate3(x, xGrid[ix], xGrid[ix1], xGrid[ix2],
			    (*this)[ix][iy],  (*this)[ix1][iy],  (*this)[ix2][iy]);
  double fx1 = interpolate3(x, xGrid[ix], xGrid[ix1], xGrid[ix2],
			    (*this)[ix][iy1], (*this)[ix1][iy1], (*this)[ix2][iy1]);
  double fx2 = interpolate3(x, xGrid[ix], xGrid[ix1], xGrid[ix2],
			    (*this)[ix][iy2], (*this)[ix2][iy2], (*this)[ix2][iy2]);
  // Now find the interpolated value between the x's
  double retval = interpolate3(y, yGrid[iy], yGrid[iy1], yGrid[iy2], fx0, fx1, fx2);
  return retval;
}

double LookupTable::evaluate4(double x, double y) {
  // Find indexes of x and y in xGrid and yGrid using 4-point interpolation
  // Assumes evenly-spaced grid
  // The grid points should be: 0 & 1 fall "below" coordinate and 2 & 3 fall "above" it.
  // This is modified significantly by the edge effects.
  int ix0, iy0;
  int ix1, ix2, ix3, iy1, iy2, iy3;

  if ( xstep == 0 || ystep == 0 )
    cerr << "LookupTable::evaluate4: Expecting non-zero step size and failed test\n";
  ix1 = (int) ((x - xGrid[0])/xstep);
  ix0 = ix1 - 1;
  ix2 = ix1 + 1;
  ix3 = ix1 + 2;
  if ( ix0 < 0 ) {		// X Grid too low, shift up
    int diff = ix0;
    ix0 -= diff;
    ix1 -= diff;
    ix2 -= diff;
    ix3 -= diff;
  }
  if ( ix3 >= xsize ) {		// X Grid too high, shift down
    int diff = ix3-xsize+1;
    ix3 -= diff;
    ix2 -= diff;
    ix1 -= diff;
    ix0 -= diff;
  }

  iy1 = (int) ((y - yGrid[0])/ystep);
  iy0 = iy1 - 1;
  iy2 = iy1 + 1;
  iy3 = iy1 + 2;
  if ( iy0 < 0 ) {		// Y Grid too low, shift up
    int diff = iy0;
    iy0 -= diff;
    iy1 -= diff;
    iy2 -= diff;
    iy3 -= diff;
  }
  if ( iy3 >= ysize ) {		// Y grid too high, shift down
    int diff = iy3-ysize+1;
    iy3 -= diff;
    iy2 -= diff;
    iy1 -= diff;
    iy0 -= diff;
  }

  // Get the four Y points necessary to do interpolation from the x points.
  double fx0 = 
    interpolate4(x, xGrid[ix0], xGrid[ix1], xGrid[ix2], xGrid[ix3],
		 (*this)[ix0][iy0], (*this)[ix1][iy0], (*this)[ix2][iy0], (*this)[ix3][iy0]);
  double fx1 = 
    interpolate4(x, xGrid[ix0], xGrid[ix1], xGrid[ix2], xGrid[ix3],
		 (*this)[ix0][iy1], (*this)[ix1][iy1], (*this)[ix2][iy1], (*this)[ix3][iy1]);
  double fx2 = 
    interpolate4(x, xGrid[ix0], xGrid[ix1], xGrid[ix2], xGrid[ix3],
		 (*this)[ix0][iy2], (*this)[ix1][iy2], (*this)[ix2][iy2], (*this)[ix3][iy2]);
  double fx3 = 
    interpolate4(x, xGrid[ix0], xGrid[ix1], xGrid[ix2],xGrid[ix3],
		 (*this)[ix0][iy3], (*this)[ix1][iy3], (*this)[ix2][iy3], (*this)[ix3][iy3]);
  // Now find the interpolated value between the x's
  double retval = interpolate4(y, yGrid[iy0], yGrid[iy1], yGrid[iy2], yGrid[iy3],
			       fx0, fx1, fx2, fx3);
  return retval;
}

