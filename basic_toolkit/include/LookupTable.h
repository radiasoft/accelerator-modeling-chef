/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      LookupTable.h
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


#ifndef LOOKUPTABLE_H
#define LOOKUPTABLE_H

#ifdef __VISUAL_CPP__
#define NULL    0
#endif

#ifndef __VISUAL_CPP__
#include <stdio.h>
#endif

class Table {
  // This is SCREAMING to be a template!!  This would be Table<double>
  double **table;
  int num_rows, num_columns;
  int valid;
  void makeTable();

 public:
  Table() { num_rows = num_columns = 0; table = NULL; }
  Table(Table& t);
  Table(int r, int c);
  Table(int r, int c, double** v);
  ~Table();

  double *operator[](int i) 	{ return table[i]; }
  void setRow(int r, double* v);
  void setTableSize(int r, int c);
  void setTable(double **v);
  int getRows() 		{ return num_rows; }
  int getColumns() 		{ return num_columns; }
  double** getTable() { return table; }
};

class LookupTable : public Table {
private:
  double* xGrid;  // A uniform grid spacing is assumed!  A decent upgrade, someday?!
  double* yGrid;
  int xsize, ysize;
  double xstep, ystep;
  int ready;
  int interpolationType;
  // Have a choice of types of interpolations to use: 2-point, 3-point or 4-point
  double interpolate2(double x, double xmin, double xmax, double fxmin, double fxmax);
  double interpolate3(double x,  double x0, double x1, double x2,
		      double y0, double y1, double y2);
  double interpolate4(double x,  double x0, double x1, double x2, double x3,
		      double y0, double y1, double y2, double y3);
  double evaluate2(double x, double y);
  double evaluate3(double x, double y);
  double evaluate4(double x, double y);
  
public:
  LookupTable();
  LookupTable(int x, int y);
  LookupTable(int x, int y, double *xg, double *yg, double** v);
  LookupTable(LookupTable &lt);
  ~LookupTable();
  
  void setXGrid(double *xg);
  void setYGrid(double *yg);
  void setFXY(double **fxy) { Table::setTable(fxy); }

  void setInterpolationType(int i) { interpolationType = i; }
  int getInterpolationType() { return interpolationType; }
  int getXSize() { return xsize; }
  int getYSize() { return ysize; }
  double *getXGrid() { return xGrid; }
  double *getYGrid() { return yGrid; }
  
  double evaluate(double x, double y)
    {
      switch (interpolationType) {
      case 2: return evaluate2(x,y);
      case 3: return evaluate3(x,y);
      default: return evaluate4(x,y);
      }
    }
};

#endif /* LOOKUPTABLE_H */
