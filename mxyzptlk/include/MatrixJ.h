/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******  Version:   4.1                    
******                                    
******  File:      MatrixJ.h
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


#ifndef MATRIXJ_H
#define MATRIXJ_H

#include "MLJ.h"


//////////////////////////////////////////////////////////////////////////
class MatrixJ {
private:
  MLJ* ml;
  Jet__environment*  myEnv;

  void copy_column( MatrixJ& x,int from_col, int to_col );
  void switch_columns( int, int );
  void switch_rows( int, int );
  MatrixJ scale( );
  MatrixJ lu_decompose( int*, int&, Jet__environment* = Jet::lastEnv );
  void lu_back_subst( int*, MatrixJ& );
  void error( char * msgl, char * msg2 = " " )const ;


public:  
  char  stacked;

  // Constructors and destructors_____________________________________
  MatrixJ( Jet__environment* = Jet::lastEnv );
  MatrixJ( int rows, Jet__environment* = Jet::lastEnv );
  MatrixJ( int rows, int columns, Jet__environment* = Jet::lastEnv );
  MatrixJ( int rows, int columns, const Jet& initval,  Jet__environment* = Jet::lastEnv );
  MatrixJ( int rows, int columns, Jet* initval, Jet__environment* = Jet::lastEnv );
  MatrixJ( char* flag, int dimension,  Jet__environment* = Jet::lastEnv ); // create an identity matrix
				     // or a symplectic matrix
  MatrixJ( const MatrixJ& X );
  ~MatrixJ();

  // Public member functions__________________________________________
  inline int rows( ) const { return ml->r;}
  inline int cols( ) const { return ml->c;}
  MatrixJ transpose( ) const;
  MatrixJ Square ( ) const;
  Jet determinant( );
  MatrixJ inverse( );
  Jet trace( );
  int CheckEnv( );
  // Operators________________________________________________________

  MatrixJ& operator=( const MatrixJ& );
  MatrixJ& DeepCopy( const MatrixJ& );
  Jet& operator()( int row, int column );
  Jet& operator()( int row );
  void operator+=( const Jet&);
  void operator-=( const Jet&);
friend char operator==( const MatrixJ&, const MatrixJ& );
friend char operator==( const MatrixJ&, const Jet& );
friend char operator==( const Jet&, const MatrixJ& );
friend ostream& operator<<(ostream&, const MatrixJ&);
//friend istream& operator>>(istream&, MatrixJ&);
friend MatrixJ operator+ ( const MatrixJ &,const MatrixJ & );
friend MatrixJ operator+ ( const MatrixJ &,const Jet& ); 
friend MatrixJ operator+ ( const Jet& ,const MatrixJ & ); 
friend MatrixJ operator-( const MatrixJ & ); 
friend MatrixJ operator-( const MatrixJ &,const MatrixJ& ); 
friend MatrixJ operator-( const MatrixJ &,const Jet& ); 
friend MatrixJ operator-( const Jet&,const MatrixJ & ); 
friend MatrixJ operator*( const MatrixJ &,const MatrixJ& ); 
friend MatrixJ operator*( const MatrixJ &,const Jet& );
friend MatrixJ operator*( const Jet&,const MatrixJ & );
friend MatrixJ operator/( const MatrixJ &,const Jet& );

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
};
/////////////////////////////////////////////////////////////////////

#endif
