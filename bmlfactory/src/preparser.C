//
// This program parse MAD file, collect all
// variables (not beam elements, beamlines etc)
// into map[ Variable name, Variable Value ],
// so the last variable definition will be
// "the right" one.
//
// All variables will be printed first,
// and only after this lattice structure
// (beamelements and beamlines) will be
// printed
//
// written by OK (C) 1997
//

#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <algorithm>

extern "C" void bmlfactory_exit();

const size_t buffsize = 2048;
typedef map< string, pair<bool, string> > container_type;

static char*
strcompress( char* szStr ) {
  char* out = szStr;
  char* end = szStr + strlen( szStr ) ;
  for( char* p = szStr; *p; p++ ) {
    if ( !isspace( *p ) ) {
      *out++ = *p;
    } else {
      end--;
    }
  }
  *end = '\000';
  return szStr;
}

int
main( void ) {
  
  ifstream* is = new ifstream( "lattice.mad" );

  char buffer[buffsize];
  char tmpbuf[buffsize];
  container_type varmap;
  list<string>   varlist;

  string beamenergy;
  
  if ( *is ) {
    while( !is->eof() ) {
      is->getline( buffer, buffsize );
      size_t len = strlen( buffer );
      if ( len == buffsize-1 ) {
        cerr << "buffsize too small. increase and recompile\n";
        return 1;
      }

      while( buffer[len-1] == '&' ) {
        is->getline( buffer+len-1, buffsize-(len-1) );
        size_t len = strlen( buffer );
        if ( len == buffsize-1 ) {
          cerr << "buffsize too small. increase and recompile\n";
          return 1;
        }
      }

      char* ptr = strstr( buffer, ":=" );
      bool var = false;
      
      if ( ptr != 0 ) {
        strcpy( tmpbuf, buffer );
        char* ptr = strstr( tmpbuf, ":=" );
        *ptr = 0;
        strcompress( tmpbuf );
        varmap[tmpbuf] = pair<bool, string>( false, ptr+2 );
        varlist.push_back( tmpbuf );
        var = true;
      } else {
        ptr = strstr( buffer, "=" );
        if ( ptr != 0 ) {
          strcpy( tmpbuf, buffer );
          char* ptr = strstr( tmpbuf, "=" );
          *ptr = 0;
          strcompress( tmpbuf );
          if ( strchr( tmpbuf, ',' ) || strchr( tmpbuf, ':' ) ) {
          } else {
            varmap[tmpbuf] = pair<bool, string>( false, ptr+2 );
            varlist.push_back( tmpbuf );
            var = true;
          }
        }
      }
      if ( var == false ) {
        if ( buffer[0] != '!' ) {
          if ( strstr( buffer, "beam" ) && strstr( buffer, "energy" ) ) {
            beamenergy = buffer;
          }
        }
      }
    }
  }

  delete is;
/*
  is = new ifstream( "lattice.mad" );
  if ( *is ) {
    while( !is->eof() ) {
      is->getline( buffer, buffsize );
      size_t len = strlen( buffer );
      if ( len == buffsize-1 ) {
        cerr << "buffsize too small. increase and recompile\n";
        return 1;
      }

      while( buffer[len-1] == '&' ) {
        is->getline( buffer+len-1, buffsize-(len-1) );
        size_t len = strlen( buffer );
        if ( len == buffsize-1 ) {
          cerr << "buffsize too small. increase and recompile\n";
          return 1;
        }
      }

      char* ptr = strstr( buffer, ":=" );
      bool  var = false;
      container_type::iterator j = varmap.end();
      
      if ( ptr != 0 ) {
        strcpy( tmpbuf, buffer );
        char* ptr = strstr( tmpbuf, ":=" );
        *ptr = 0;
        strcompress( tmpbuf );

        list<string>::iterator i = find( varlist.begin(), varlist.end(), string( tmpbuf ) );
        if ( i == varlist.end() ) {
          cerr << "No variable stored in second pass\n";
          bmlfactory_exit();
        } else {
          j = varmap.find( tmpbuf );
          if ( j == varmap.end() ) {
            cerr << "Variable in List but not in Map\n";
            bmlfactory_exit();
          } else {
            var = true;
          }
        }
      } else {
        ptr = strstr( buffer, "=" );
        if ( ptr != 0 ) {
          strcpy( tmpbuf, buffer );
          char* ptr = strstr( tmpbuf, "=" );
          *ptr = 0;
          strcompress( tmpbuf );
          if ( strchr( tmpbuf, ',' ) || strchr( tmpbuf, ':' ) ) {
          } else {
            list<string>::iterator i = find( varlist.begin(), varlist.end(), string( tmpbuf ) );
            if ( i == varlist.end() ) {
              cerr << "No variable stored in second pass\n";
              bmlfactory_exit();
            } else {
              j = varmap.find( tmpbuf );
              if ( j == varmap.end() ) {
                cerr << "Variable in List but not in Map\n";
                bmlfactory_exit();
              } else {
                var = true;
              }
            }
          }
        }
      }

      if ( var ) {
        if ( (j->second).first == false ) {
          cout << j->first;
          cout << " := ";
          cout << (j->second).second << endl;

          (j->second).first = true;
        }
      } else {
        cout << buffer << endl;
      }
    }
  }
  */

  cout << beamenergy << endl;

  for( list<string>::iterator i = varlist.begin(); i != varlist.end(); ++i ) {
    container_type::iterator j = varmap.find( *i );
    if ( j == varmap.end() ) {
      cerr << "something wrong with map/list\n";
      bmlfactory_exit();
    } else {
      if ( (j->second).first == false ) { // not yet printed
        cout << j->first;
        cout << " := ";
        cout << (j->second).second << endl;
        
        (j->second).first = true;
      }
    }
  }
  
  is = new ifstream( "lattice.mad" );
  
  if ( *is ) {
    while( !is->eof() ) {
      
      is->getline( buffer, buffsize );
      size_t len = strlen( buffer );
      if ( len == buffsize-1 ) {
        cerr << "buffsize too small. increase and recompile\n";
        return 1;
      }

      while( buffer[len-1] == '&' ) {
        is->getline( buffer+len-1, buffsize-(len-1) );
        size_t len = strlen( buffer );
        if ( len == buffsize-1 ) {
          cerr << "buffsize too small. increase and recompile\n";
          return 1;
        }
      }

      char* ptr = strstr( buffer, ":=" );
      bool  var = false;
      container_type::iterator j = varmap.end();
      
      if ( ptr != 0 ) {
        strcpy( tmpbuf, buffer );
        char* ptr = strstr( tmpbuf, ":=" );
        *ptr = 0;
        strcompress( tmpbuf );

        list<string>::iterator i = find( varlist.begin(), varlist.end(), string( tmpbuf ) );
        if ( i == varlist.end() ) {
          cerr << "No variable stored in second pass\n";
          bmlfactory_exit();
        } else {
          j = varmap.find( tmpbuf );
          if ( j == varmap.end() ) {
            cerr << "Variable in List but not in Map\n";
            bmlfactory_exit();
          } else {
            var = true;
          }
        }
      } else {
        ptr = strstr( buffer, "=" );
        if ( ptr != 0 ) {
          strcpy( tmpbuf, buffer );
          char* ptr = strstr( tmpbuf, "=" );
          *ptr = 0;
          strcompress( tmpbuf );
          if ( strchr( tmpbuf, ',' ) || strchr( tmpbuf, ':' ) ) {
          } else {
            list<string>::iterator i = find( varlist.begin(), varlist.end(), string( tmpbuf ) );
            if ( i == varlist.end() ) {
              cerr << "No variable stored in second pass\n";
              bmlfactory_exit();
            } else {
              j = varmap.find( tmpbuf );
              if ( j == varmap.end() ) {
                cerr << "Variable in List but not in Map\n";
                bmlfactory_exit();
              } else {
                var = true;
              }
            }
          }
        }
      }

      if ( !var ) {
        cout << buffer << endl;
      }
    }
  }

  delete is;
  return 0;
}
