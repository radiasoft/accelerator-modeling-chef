#ifndef MESSAGES_H
#define MESSAGES_H

#include <qtextedit.h>
#include <streambuf>


template <typename charT=char, typename traits=std::char_traits<charT> >
class Messages: public std::basic_streambuf< charT, traits > 
{

 public:

  Messages(QTextEdit* editor);
 ~Messages();

 protected:

#if 0
  int_type overflow(int_type c = traits_type::eof() );
  int_type uflow();
  int_type underflow();
  int_type pbackfail(int_type c);
#endif

 private:

  charT      _charBuf;
  bool       _takefromBuf;
  charT*     _buffer;
  charT*     _buffer_ptr;
  int        _buffer_size;

  QTextEdit* _textedit;


 // prohibit copy and assignement

 Messages(Messages&);
 Messages& operator=(const Messages&);


};

#include "messages.tcc"

#endif
