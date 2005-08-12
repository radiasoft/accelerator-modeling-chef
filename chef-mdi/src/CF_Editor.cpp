/*************************************************************************
**************************************************************************
**************************************************************************
******
******  CHEF:      An application layered on the Beamline/mxyzptlk
******             class libraries.
******
******  File:      CF_Editor.cpp
******
******  Copyright (c) Universities Research Association, Inc.
******                All Rights Reserved
******
******  Author:    Jean-Francois Ostiguy
******             Fermilab
******             ostiguy@fnal.gov
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
******* URA/FNAL reserves all rights.
*******                                                                
**************************************************************************/

#include <CF_Editor.h>

#include <iostream>
#include <sstream>
#include <iomanip>

PrivateTextEdit::PrivateTextEdit(QWidget* parent, const char* name): 
QTextEdit(parent,name)
{


}

//.......................................................................................

void
PrivateTextEdit::paste()
{
 
  QTextEdit::paste();
  emit numberOfLinesChanged();

}


//.......................................................................................

void
PrivateTextEdit::cut()
{

  QTextEdit::cut();
  emit numberOfLinesChanged();

}

//.......................................................................................

void
PrivateTextEdit::removeSelectedText()
{

  QTextEdit::removeSelectedText();
  emit numberOfLinesChanged();
}

//.......................................................................................

void 
PrivateTextEdit::doKeyboardAction(QTextEdit::KeyboardAction action)
{  

  int para;
  int index;

  getCursorPosition(&para, &index);

  QString ptxt = text(para);


  QTextEdit::doKeyboardAction(action);


  switch (action) {

    case QTextEdit::ActionBackspace: if ( index == 0 )   
                                       emit numberOfLinesChanged(); 
                                     break; 

    case QTextEdit::ActionDelete:    if ( index == paragraphLength(para)) 
                                       emit numberOfLinesChanged();
                                     break;
    
    default:                         break;

  }


}



//////////////////////////////////////////////////////////////////////////////////////////////////////////



CF_Editor::CF_Editor(QWidget* parent, const char* name, WFlags f): QWidget(parent,name)
{


  QFont editorFont ( "Lucida Typewriter", 10, QFont::Normal, false ); 

  Lines_ = new       QTextEdit(this,"Lines");
   Text_ = new PrivateTextEdit(this,"Text");   
 
  Lines_->setTextFormat(QTextEdit::PlainText);
  Lines_->setReadOnly(true);
  Lines_->setWordWrap(QTextEdit::NoWrap);
  Lines_->setAlignment(Qt::AlignRight);
  Lines_->setVScrollBarMode(QScrollView::AlwaysOff);
  Lines_->setHScrollBarMode(QScrollView::AlwaysOff);
  Lines_->setFont(editorFont);
  Lines_->setFrameShape(QFrame::NoFrame);
  Lines_->setPaper( QBrush( QColor(Qt::lightGray), Qt::SolidPattern )  );
 

   Text_->setTextFormat(QTextEdit::PlainText);
   Text_->setWordWrap(QTextEdit::NoWrap);  
   Text_->setVScrollBarMode(QScrollView::Auto);
   Text_->setFont(editorFont);
   Text_->setFrameShape(QFrame::NoFrame);

  QScrollBar* tvsb =  Text_->verticalScrollBar();
  QScrollBar* lsvb = Lines_->verticalScrollBar();

  connect(Text_,  SIGNAL( returnPressed() ) ,      this,  SLOT( renumberLines() )); 
  connect(tvsb,   SIGNAL( valueChanged(int)),      lsvb,  SLOT( setValue(int)   ));
  connect(Text_,  SIGNAL( numberOfLinesChanged()), this,  SLOT( renumberLines() ));

  

}

//.......................................................................................

CF_Editor::~CF_Editor() 
{


}

//.......................................................................................


void 
CF_Editor::resizeEvent ( QResizeEvent * e) 
{

// note: setGeometry() sets the geometry **excluding** the window frame

  
 int w = e->size().width();
 int h = e->size().height();


 Lines_->setGeometry(0,  0,     50,   h );
  Text_->setGeometry(50, 0,   w-50,   h );


}

//.......................................................................................

void
CF_Editor::renumberLines()
{

  int lns = Text_->lines();


  std::stringstream ss;

  for (int i=1; i <= lns; i++) 
  {
   
      ss.width(6); ss.fill('0');
      ss << std::right << i << std::endl;


  }

  // Lines_->setAlignment( Qt::AlignRight ); 
  Lines_->setText( QString ( ss.str().c_str() ) );
    
  // Once the lines are added, the min/max scale for the scroll "value" has also
  // changed. The line below is needed to force resync of both scrollbar values. 
 
  Lines_->verticalScrollBar()->setValue( Text_->verticalScrollBar()->value() );   

}

//.......................................................................................

void
CF_Editor::append ( const QString & text )
{

Text_->append(text);

}


//.......................................................................................

void
CF_Editor::setText( const QString& s)
{

//.......................................................................................

Text_->setText(s);
renumberLines();

}

//.......................................................................................

QString
CF_Editor::text()
{

return Text_->text();

}

//.......................................................................................

void 
CF_Editor::setSelection ( int paraFrom, int indexFrom, int paraTo, int indexTo, int selNum) 
{

 Text_->setSelection (paraFrom, indexFrom, paraTo, indexTo, selNum );

}

//.......................................................................................

void 
CF_Editor::setSelectionAttributes ( int selNum, const QColor & back, bool invertText )
{

  Text_->setSelectionAttributes ( selNum, back, invertText );

}

