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
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws.
******                                                                
******                                                                
**************************************************************************
*************************************************************************/


#ifndef OUTPUTFORMAT_H
#define OUTPUTFORMAT_H

// Stream-IO flags (could be appended if necessary
// ??? Note: These names are a little too global and
// ??? should be changed.

class OutputFormat{
public:
  OutputFormat( const char* = 0 );
  OutputFormat( const OutputFormat& );
  ~OutputFormat();

  OutputFormat& operator= ( const OutputFormat& );
  void setPadding( const char* );
  const char* getPadding();
 
  unsigned   width;
  unsigned   precision;
  long       flags;
  unsigned   dumpbase;

  const static unsigned left   = 02;
  const static unsigned right  = 04;

  const static unsigned scientific = 04000;
  const static unsigned fixed      = 010000;

// Dump formats

  const static unsigned int BIN  =  2;
  const static unsigned int OCT  =  8;
  const static unsigned int DEC  = 10;
  const static unsigned int HEX  = 16;

 
private:
  char*    padding;
};

typedef OutputFormat* OutputFormat_t;

#endif // OUTPUTFORMAT_H
