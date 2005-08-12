***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      messages.h                                        ****** 
******                                                               ******
******  Copyright (c) Universities Research Association, Inc.        ****** 
******                All Rights Reserved                            ****** 
******                                                               ****** 
******  Author:    Jean-Francois Ostiguy                             ******
******             Fermilab                                          ****** 
******             ostiguy@fnal.gov                                  ****** 
******                                                               ******  
******  Usage, modification, and redistribution are subject to terms ******
******  of the License supplied with this software.                  ****** 
******                                                               ******
******  Software and documentation created under                     ****** 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000.    ****** 
******  The U.S. Government retains a world-wide non-exclusive,      ****** 
******  royalty-free license to publish or reproduce documentation   ****** 
******  and software for U.S. Government purposes. This software     ****** 
******  is protected under the U.S. and Foreign Copyright Laws.      ****** 
******  URA/FNAL reserves all rights.                                ****** 
******                                                               ******
**************************************************************************/

#ifndef MESSAGES_H
#define MESSAGES_H

#include <streambuf>

// 
// In the Message class, the template parameter msgtype refers to the type of message. 
// 0 = infomational, 
// 1 = warning, 
// 2 = error    
//
// Normally, multiple instances of the Message class would be created using the *same*
// QTextEdit Widget. 
// 
// Since Message inherits from std::basic_streambuf<>, a message object can be used where
// a std::cerr or std::cout is used.   

class QTextEdit;

template <int msg_type, typename charT=char, typename traits=std::char_traits<charT> >
class Messages: public std::basic_streambuf< charT, traits > 
{

 public:

  Messages(QTextEdit* editor);
 ~Messages();

 protected:

  typename Messages<msg_type, charT, traits>::int_type overflow( typename Messages<msg_type, charT, traits>::int_type c = traits::eof() );  typename Messages<msg_type, charT, traits>::int_type uflow();
  typename Messages<msg_type, charT, traits>::int_type underflow();
  typename Messages<msg_type, charT, traits>::int_type pbackfail( typename Messages<msg_type, charT, traits>::int_type c);

 private:

  static  const int   _buffer_size      = 1024; 

  charT      _charBuf;
  bool       _takefromBuf;
  charT*     _buffer;
  charT*     _buffer_ptr;


  QTextEdit* _textedit;


 // prohibit copy and assignement

 Messages(Messages&);
 Messages& operator=(const Messages&);


};

#include "messages.tcc"

#endif
