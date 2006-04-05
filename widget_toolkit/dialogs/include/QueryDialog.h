/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  Version:   3.0                    
******                                    
******  File:      QueryDialog.h
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
  QueryDialog( QWidget* parent = 0, 
               const char* name = 0, 
               WFlags f = 0 );
  ~QueryDialog();

  const BoolNode& query();
  int exec();

public slots:
  void _opEnter();
  void _opAnd();
  void _opOr();
  void _opNot();
  void _doFilter();

signals:
  void _useThis( const BoolNode& );

private:
  QTextEdit*     _displayPtr;
  QTabWidget*    _tabWidgetPtr;
  QLineEdit*     _nameEdt;
  QLineEdit*     _lengthEdt;
  QHButtonGroup* _lengthBtnGrp;
  QLineEdit*     _strengthEdt;
  QHButtonGroup* _strengthBtnGrp;
  QListBox*      _typeTabPtr;
  QVBox*         _opsBoxPtr;
  BoolNode*      _queryPtr[4];  // Four registers

  void           _updateDisplay();
};

#endif  // QUERYDIALOG_H
