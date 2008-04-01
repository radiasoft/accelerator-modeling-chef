////////////////////////////////////////////////////////////
// 
// File:          JetWriterTest.cc
// Author:        Leo Michelotti
// 
// Revision date: Monday. March 31, 2008.  (original version)
// 
////////////////////////////////////////////////////////////
// 
// JetWriter must create a LaTeX fragment.
// 
// 
// ------------
// COMMAND LINE
// ------------
// JetWriterTest [options]
// 
// -------
// OPTIONS
// -------
// 
////////////////////////////////////////////////////////////

#include <mxyzptlk/Jet.h>
#include <physics_toolkit/JetWriter.h>
#include <physics_toolkit/TJetWriter.tcc>

////////////////////////////////////////////////////////////

int main( int argc, char** argv )
{
  int ret = 0;

  // see mxyzptlk/tests/evaltest.cc

  Jet__environment::BeginEnvironment( 4 );
  coord x(0.5), y(0.4), z(0.0);
  Jet__environment::EndEnvironment();

  Jet v( exp( x + y + z ) );

  std::vector<std::string> symbols;
  symbols.push_back( "x_1" );
  symbols.push_back( "x_2" );
  symbols.push_back( "x_3" );

  JetWriter wrt;
  cout << wrt.write( v, symbols, std::string("v") ) << endl;

  return ret;
}
