/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  widget_toolkit: a category of Qt based widget classes, 
******             providing graphical user interfaces to exercise
******             the functionality of BEAMLINE/MXYZPTLK.
******                                                                
******  File:      QueryDialog.h
******  Version:   3.0
******                                                                
******  Copyright (c) 2004  Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
**************************************************************************
******  
******  QueryDialog interactively constructs a BoolNode object that
******  can be used to select elements with a beamline.  Upon closing,
******  the BoolNode is returned as argument to the QueryDialog signal
******  useThis.
******  
******  ------------------
******  Use meta-fragment:
******  ------------------
******    #include <QueryDialog.h>
******    ...
******    QueryDialog qdl( 0, 0, false, Qt::WDestructiveClose );
******    connect( &qdl,  SIGNAL( useThis( const BoolNode& )), 
******             <receiver-address>, SLOT( <handler> );
******    ...
******    qdl->show();
******  
**************************************************************************
*************************************************************************/

#ifndef QUERYDIALOG_H
#define QUERYDIALOG_H

#ifndef QWIDGET_H
#include <qwidget.h>
#endif

class BoolNode;
class QTabWidget;
class QVBox;
class QListBox;
class QLineEdit;
class QHButtonGroup;
class QTextEdit;

class QueryDialog : public QWidget
{
Q_OBJECT

 public:
  QueryDialog( QWidget* parent = 0, const char* name = 0, WFlags f = 0 );
  ~QueryDialog();

  const BoolNode& query();
  int exec();

public slots:

  void  opEnter();
  void  opAnd();
  void  opOr();
  void  opNot();
  void  doFilter();

signals:
  void useThis( BoolNode const& );

private:
  QTextEdit*     displayPtr_;
  QTabWidget*    tabWidgetPtr_;
  QLineEdit*     nameEdt_;
  QLineEdit*     lengthEdt_;
  QHButtonGroup* lengthBtnGrp_;
  QLineEdit*     strengthEdt_;
  QHButtonGroup* strengthBtnGrp_;
  QListBox*      typeTabPtr_;
  QVBox*         opsBoxPtr_;
  BoolNode*      queryPtr_[4];  // Four registers

  void           updateDisplay();
};

#endif  // QUERYDIALOG_H
