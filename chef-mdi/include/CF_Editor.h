/**********************************************************************/
/*                                                                    */
/* File:           CF_Editor.h                                        */
/* Author:         Jean-Francois Ostiguy                              */
/*                 ostiguy@fnal.gov                                   */
/*                                                                    */
/* Creation Date:  January 2005                                       */
/*                                                                    */ 
/* Copyright:      (C) URA/Fermilab                                   */
/*                                                                    */
/**********************************************************************/

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
