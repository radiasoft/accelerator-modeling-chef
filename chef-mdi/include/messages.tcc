#include <qstring.h>
#include <qdatetime.h>
#include <iostream> 


template<typename charT, typename traits>
Messages<charT,traits>::Messages<charT,traits>(QTextEdit* editor):
_textedit(editor)
{

 _textedit->setTextFormat(Qt::LogText);

 _buffer_size      = 1024;

 this->_buffer     = new charT[_buffer_size];

 this->_buffer_ptr = this->_buffer;
 for (int i=0; i<1024; i++) {
    *((this->_buffer_ptr)++) = 0;
 }
 this->_buffer_ptr = _buffer;

}


template<typename charT, typename traits>
Messages<charT,traits>::~Messages<charT,traits>() 
{

 delete[] this->_buffer;


}


#if 0
////////////////////////////// 
template<typename charT, typename traits>
Messages<charT,traits>::int_type
Messages<charT,traits>::overflow( int_type c) 
{

  if (!traits_type::eq_int_type(c, traits_type::eof())) {
     //std::cout << (int) c << std::endl;
    *(this->_buffer_ptr) = c;    
     (this->_buffer_ptr)++; 
     //std::cout << this->_buffer << std::endl; 
  }; 
  
  if ( traits_type::eq_int_type( c, 10 ) ) // detect a newline 
  {
    *( (this->_buffer_ptr)-1 ) = 0;  // replace CR by string termination character

    _textedit->append(  QDateTime::currentDateTime().toString()+QString("\n")+this->_buffer );
    this->_buffer_ptr = this->_buffer;
    return 0;
  };

  return c;

}
 

template<typename charT, typename traits>
Messages<charT,traits>::int_type
Messages<charT,traits>::uflow() 
{

  return 0;


}


template<typename charT, typename traits>
Messages<charT,traits>::int_type
Messages<charT,traits>::underflow()
{

  return 0;

}


template<typename charT, typename traits>
Messages<charT,traits>::int_type
Messages<charT,traits>::pbackfail(int_type c)
{

  return 0;

}

///////////////////////////

#endif

