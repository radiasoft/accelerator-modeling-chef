#include <ioredirector.h>
#include <iostream>
#include <strstream>
#include <qstring.h>


IORedirector::IORedirector()  {
  
  
}

void IORedirector::read( const char* s) {

  char line[256];
  std::strstream inout;
  inout.getline (line, sizeof(line) );
  s = line;
  
}


void IORedirector::write( const char* s) {
  QString qs= QString( s );
  emit textReady( qs );
  
}





