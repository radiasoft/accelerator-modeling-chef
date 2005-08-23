/***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      messages.tcc                                      ****** 
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

#include <qstring.h>
#include <qapplication.h>
#include <qeventloop.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qtextedit.h>

template<int msg_type, typename charT, typename traits>
Messages<msg_type, charT, traits>::Messages<msg_type, charT, traits>(QTextEdit* editor):
_textedit(editor)
{

 _textedit->setTextFormat(Qt::LogText);

 this->_buffer     = new charT[_buffer_size];

 this->_buffer_ptr = this->_buffer;
 for (int i=0; i<_buffer_size; i++) {
    *((this->_buffer_ptr)++) = 0;
 }
 this->_buffer_ptr = _buffer; 
 
}


template<int msg_type, typename charT, typename traits>
Messages<msg_type, charT, traits>::~Messages<msg_type, charT, traits>() 
{

 delete[] this->_buffer;


}


template<int msg_type, typename charT, typename traits>
typename Messages<msg_type, charT, traits>::int_type
Messages<msg_type, charT, traits>::overflow( typename Messages<msg_type, charT, traits>::int_type c) 
{

  //
  // Note: in a QTextEdit Widget in Log Mode
  // The characters <, > and & are escaped by using &lt;, &gt; and &amp; respectively. 
  // 
  
  if (!traits::eq_int_type(c, traits::eof())) {
 
    if (  traits::eq_int_type( c, '<' ) ) 
    { 
         *(this->_buffer_ptr++) = '&'; 
         *(this->_buffer_ptr++) = 'l'; 
         *(this->_buffer_ptr++) = 't'; 
         *(this->_buffer_ptr++) = ';'; 
    }
    else if (  traits::eq_int_type( c, '>' ) ) 
    {
         *(this->_buffer_ptr++) = '&'; 
         *(this->_buffer_ptr++) = 'l'; 
         *(this->_buffer_ptr++) = 't'; 
         *(this->_buffer_ptr++) = ';'; 
    }
    else if (  traits::eq_int_type( c, '&' ) )
    {  
         *(this->_buffer_ptr++) = '&'; 
         *(this->_buffer_ptr++) = 'a'; 
         *(this->_buffer_ptr++) = 'm'; 
         *(this->_buffer_ptr++) = 'p'; 
         *(this->_buffer_ptr++) = ';'; 

    }
    else
    {
         *(this->_buffer_ptr++) = c;    
    }
  } 
  

  if ( traits::eq_int_type( c, 10 ) ) // detect a newline 
  {
    *( (this->_buffer_ptr)-1 ) = 0;  // replace the last CR by a string termination character

     

    switch( msg_type ) {
 
      case 0 : 
               _textedit->append( QString("<font color=black>")  + this->_buffer + QString("</font>") );
               break;
      case 1 : 
               _textedit->append( QString("<font color=red>")    + this->_buffer + QString("</font>") );
               break;

      case 2 : _textedit->append( QString("<font color=red><i>") + this->_buffer + QString("</i></font>") );
               break;

      default: 
               break;
    };
    
    this->_buffer_ptr = this->_buffer;
  
    qApp->eventLoop()->processEvents(QEventLoop::ExcludeUserInput );
    return 0;
  }

  return c;

}
 

template<int msg_type, typename charT, typename traits>
typename Messages<msg_type, charT, traits>::int_type
Messages<msg_type, charT, traits>::uflow() 
{

  return 0;


}


template<int msg_type, typename charT, typename traits>
typename Messages<msg_type, charT, traits>::int_type
Messages<msg_type, charT, traits>::underflow()
{

  return 0;

}


template<int msg_type, typename charT, typename traits>
typename Messages<msg_type, charT, traits>::int_type
Messages<msg_type, charT, traits>::pbackfail(typename Messages<msg_type, charT, traits>::int_type /*c */)
{

  return 0;

}
