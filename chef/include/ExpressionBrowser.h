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
******  File:      ExpressionBrowser.h
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


#ifndef BOOLBROWSER_H
#define BOOLBROWSER_H

#include <qlistview.h>
#include <qstring.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtimer.h>
#include <qpixmap.h>

#include "slist.h"

namespace CHEF_domain
{
// Forward class declarations
class QBoolOpNode;

class QBoolNode : public QListViewItem
{
public:
  QBoolNode( QPixmap*, QListView*,   const char*, const char* );
  QBoolNode( QPixmap*, QBoolOpNode*, const char*, const char* );
  QBoolNode( const QBoolNode& );
  virtual ~QBoolNode();

  void setPixmap( QPixmap *px );
  const QPixmap* getPixmap() const;
  const QPixmap* pixmap( int ) const;

  virtual bool evaluate() const = 0;
  virtual QBoolNode* clone( QBoolOpNode* ) const = 0;
  virtual QBoolNode* clone( QListView*   ) const = 0;

protected:
  QPixmap*     _pix;
};


class QBoolEndNode : public QBoolNode
{
public: 
  QBoolEndNode( QListView*,   bool=false );
  QBoolEndNode( QBoolOpNode*, bool=false );
  QBoolEndNode( const QBoolEndNode& );
  ~QBoolEndNode();

  bool evaluate() const;
  QBoolNode* clone( QBoolOpNode* ) const;
  QBoolNode* clone( QListView*   ) const;

  QBoolEndNode& operator=( bool );
  QBoolEndNode& operator=( const QBoolNode& );

private:
  bool _value;
};


class QBoolOpNode : public QBoolNode
{
protected:
  QBoolOpNode( QPixmap*, QListView*, const QBoolNode&, const QBoolNode& );
  QBoolOpNode( QPixmap*, QListView*, const QBoolNode& );
  QBoolOpNode( QPixmap*, QBoolOpNode*, const QBoolNode&, const QBoolNode& );
  QBoolOpNode( QPixmap*, QBoolOpNode*, const QBoolNode& );
  // The arguments will be copied and/or cloned: a QBoolOpNode owns its data.
  QBoolOpNode( const QBoolOpNode& );
  virtual ~QBoolOpNode();
  
  virtual bool evaluate() const = 0;
  virtual QBoolNode* clone( QBoolOpNode* ) const = 0;
  virtual QBoolNode* clone( QListView* )   const = 0;

protected:
  const QBoolNode* _left;
  const QBoolNode* _right;
};


class QAndNode : public QBoolOpNode
{
 public:
   QAndNode( QListView*,   const QBoolNode&, const QBoolNode& );
   QAndNode( QBoolOpNode*, const QBoolNode&, const QBoolNode& );
   QAndNode( const QAndNode& );
   ~QAndNode();

   bool evaluate() const;

   QBoolNode* clone( QBoolOpNode* ) const;
   QBoolNode* clone( QListView*   ) const;
};


class QOrNode : public QBoolOpNode
{
 public:
   QOrNode( QListView*,   const QBoolNode&, const QBoolNode& );
   QOrNode( QBoolOpNode*, const QBoolNode&, const QBoolNode& );
   QOrNode( const QOrNode& );
   ~QOrNode();

   bool evaluate() const;

   QBoolNode* clone( QBoolOpNode* ) const;
   QBoolNode* clone( QListView*   ) const;
};


class QNotNode : public QBoolOpNode
{
 public:
   QNotNode( QListView*,   const QBoolNode& );
   QNotNode( QBoolOpNode*, const QBoolNode& );
   QNotNode( const QNotNode& );
   ~QNotNode();

   bool evaluate() const;

   QBoolNode* clone( QBoolOpNode* ) const;
   QBoolNode* clone( QListView*   ) const;
};



// QAndNode operator*( const QBoolNode&, const QBoolNode& );
// QOrNode  operator+( const QBoolNode&, const QBoolNode& );
// QNotNode operator-( const QBoolNode& );


class ExpressionBrowser : public QListView
{
    Q_OBJECT

public:

  ExpressionBrowser( QWidget *parent = 0, const char *name = 0, bool sdo = FALSE );
  ~ExpressionBrowser();
  
  static const char* and_xpm[19];
  static const char* or_xpm[19];
  static const char* not_xpm[19];
  static const char* end_xpm[19];
  
  static QPixmap* andSymbol;
  static QPixmap* orSymbol;
  static QPixmap* notSymbol;
  static QPixmap* endSymbol;
  
  // int removeExpression();

  // protected slots:
  //   void slotShowData( QBoolNode* );

  // protected:
  //     void contentsMousePressEvent( QMouseEvent *e );
  //     void contentsMouseReleaseEvent( QMouseEvent *e );

private:
    // static void _displayLine( QBmlRoot*, ReverseBeamlineIterator&, double& );
    slist _topLevelItems;
};


}; // end namespace CHEF_domain

#endif // EXPRESSIONBROWSER_H
