#include <fstream>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, const char* argv[]) {

  char line[256];

  ifstream  ifs ( argv[1] );
  ofstream  ofs ( (string(argv[1])+string(".cstr")).c_str() );

  string cstr;
  string sline; 

  cout << "\"\\" << endl;   
  ofs  << "\"\\" << endl;   

  int pos  = 0;

  while ( ifs.getline(line,256) )
  {
    pos = 0;
     sline = string(line);

     while ( (pos = sline.find ('"', pos)) && (pos != string::npos) )
     { 
         // cout << "found \" at position :" << pos << endl;
         sline.insert( pos, string("\"") );
         pos +=2;
     };

     pos = 0;
     while ( (pos = sline.find ('\\', pos)) && (pos != string::npos) )
     { 
         cout << "found \\ at position :" << pos << endl;
         sline.insert(pos,string("\\"));
         pos += 2;
     };

     cstr = sline+string("\\n\\");   
     
     cout << cstr << endl;
     ofs  << cstr << endl;
     

  }
 
  cout << "\"" << endl;   
  ofs  << "\"" << endl;   

  
}
