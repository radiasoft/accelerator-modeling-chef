/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      OutputFormat.h
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


#ifndef OUTPUTFORMAT_H
#define OUTPUTFORMAT_H

// Stream-IO flags (could be appended if necessary
// ??? Note: These names are a little too global and
// ??? should be changed.
#define LEFT   02
#define RIGHT  04
#define SCIENTIFIC 04000
#define FIXED 010000

// Dump formats
#define BIN  2
#define OCT  8
#define DEC 10
#define HEX 16

class OutputFormat{
public:
  OutputFormat( const char* = 0 );
  OutputFormat( const OutputFormat& );
  ~OutputFormat();

  OutputFormat& operator= ( const OutputFormat& );
  void setPadding( const char* );
  const char* getPadding();
 
  unsigned width;
  unsigned precision;
  long     flags;
  unsigned dumpbase;
 
private:
  char*    padding;
};

typedef OutputFormat* OutputFormat_t;

#endif // OUTPUTFORMAT_H
