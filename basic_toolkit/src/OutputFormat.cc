#include <stdlib.h>
#include <string.h>
#include "LinearAlgebra/OutputFormat.h"

OutputFormat::OutputFormat( const char* pad )
{
  width     =   10;
  precision =    3;
  flags     =    SCIENTIFIC;
  dumpbase  =    0;
 
  if( pad ) {
    padding = new char [ strlen( pad ) + 1 ];
    strcpy( padding, pad );
  }
  else {
    padding = new char [2];
    padding[0] = '\t';
    padding[1] = '\0';
  }
}
 
OutputFormat::OutputFormat( const OutputFormat& x)
{
  width = x.width;
  precision = x.precision;
  flags = x.flags;
  padding = new char [ strlen( x.padding ) + 1 ];
  strcpy( padding, x.padding );
  dumpbase = x.dumpbase;
}

OutputFormat::~OutputFormat()
{
  delete [] padding;
}

OutputFormat& OutputFormat::operator=( const OutputFormat& x )
{
  if( this != &x ) {
    width = x.width;
    precision = x.precision;
    flags = x.flags;
    if ( padding != NULL ) delete [] padding;
    padding = new char [ strlen( x.padding ) + 1 ];
    strcpy( padding, x.padding );
    dumpbase = x.dumpbase;
  }
  return *this;
}


void OutputFormat::setPadding( const char* pad )
{
  if ( padding != NULL ) delete [] padding;
  padding = new char [ strlen( pad ) + 1 ];
  strcpy( padding, pad );
}


const char* OutputFormat::getPadding()
{
  return padding;
}
