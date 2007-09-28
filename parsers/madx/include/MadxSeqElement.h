#ifndef MADXSEQELEMENT_H
#define MADXSEQELEMENT_H

#include <list>

typedef std::list<std::string> sequence_list_t; 

struct MadxElement {

  std::string        label; 
  std::string        type; 
  bool               shared;
  sequence_list_t*   elements; 

}




#endif  //  MADXSEQELEMENT_H
