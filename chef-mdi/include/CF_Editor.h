/***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:                                                        ****** 
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

#ifndef CF_EDITOR_H
#define CF_EDITOR_H

#include <qtextedit.h>

class 
PrivateTextEdit: public QTextEdit {

Q_OBJECT

public:

  PrivateTextEdit(QWidget* parent, const char* name);

signals:

  void numberOfLinesChanged();

public slots:

  void paste();
  void cut();
  void removeSelectedText();
  void doKeyboardAction(KeyboardAction action);  

};



class 
CF_Editor: public QWidget {

Q_OBJECT

public:

 CF_Editor(QWidget* parent, const char* name, WFlags f=0); 

 virtual ~CF_Editor(); 

 QString text(); 
  
public slots:

 void append ( const QString& text );
 void setText( const QString& text );
 void setSelection ( int paraFrom, int indexFrom, int paraTo, int indexTo, int selNum = 0 );
 void setSelectionAttributes ( int selNum, const QColor & back, bool invertText ); 

protected:

 virtual void resizeEvent ( QResizeEvent * e);   


private:

 PrivateTextEdit*   Text_;    
 QTextEdit*         Lines_;

private slots:
 
 void renumberLines();

};

#endif
