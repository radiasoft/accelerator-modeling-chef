#include "marker.h"

// **************************************************
//   class marker 
// **************************************************

marker::marker() : bmlnElmnt() 
{
}

marker::marker( char* n ) : bmlnElmnt(n) 
{
}

marker::marker( const marker& x ) : bmlnElmnt( (bmlnElmnt&) x ) 
{
}

marker::marker( bmlnElmntData& x ) : bmlnElmnt( x ) 
{
}

marker::~marker() {
}



const char* marker::Type() const 
{ 
  return "marker"; 
}

