/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  Version:   3.2
******                                    
******  File:      ExpressionBrowser.cc
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

#include <iostream>

#include "GenericException.h"
#include "ExpressionBrowser.h"
#include "ExpressionBrowser.xpm"

using namespace CHEF_domain;
using namespace std;

// *****************************************************************************
// *
// * Class QBoolNode
// *
// *****************************************************************************


QBoolNode::QBoolNode( QPixmap* px, QListView* parent, const char* s1, const char* s2 )
: QListViewItem( parent, s1, s2 ), _pix(px)
{
  if(px) { this->setPixmap( px ); }
}

QBoolNode::QBoolNode( QPixmap* px, QBoolOpNode* parent, const char* s1, const char* s2 )
: QListViewItem( parent, s1, s2 ), _pix(px)
{
  if(px) { this->setPixmap( px ); }
}

QBoolNode::QBoolNode( const QBoolNode& x ) 
: QListViewItem( x )
{
  throw( GenericException( __FILE__, __LINE__, 
         "QBoolNode::QBoolNode( const QBoolNode& x ) ", 
         "QBoolNode copy constructor called." ) );
}


QBoolNode::~QBoolNode()
{
}


void QBoolNode::setPixmap( QPixmap *px )
{
  _pix = px;
  setup();
  widthChanged( 0 );
  invalidateHeight();
  repaint();
}


const QPixmap* QBoolNode::getPixmap() const
{
  return _pix;
}


const QPixmap* QBoolNode::pixmap( int i ) const
{
  if ( i ) { return 0; }
  return _pix;
}


// *****************************************************************************
// *
// * Class QBoolEndNode
// *
// *****************************************************************************


QBoolEndNode::QBoolEndNode( QListView* parent, bool v )
: QBoolNode( ExpressionBrowser::endSymbol, parent, "end", "" ),
  _value(v)
{
  if(v) {
    this->setText( 1, "true" );
  }
  else {
    this->setText( 1, "false" );
  }
}


QBoolEndNode::QBoolEndNode( QBoolOpNode* parent, bool v )
: QBoolNode( ExpressionBrowser::endSymbol, parent, "end", "" ),
  _value(v)
{
  if(v) {
    this->setText( 1, "true" );
  }
  else {
    this->setText( 1, "false" );
  }
}


QBoolEndNode::QBoolEndNode( const QBoolEndNode& x )
  : QBoolNode( x ),
  _value(x._value)
{
  throw( GenericException( __FILE__, __LINE__, 
         "QBoolEndNode::QBoolEndNode( const QBoolEndNode& x )", 
         "QBoolEndNode copy constructor called" ) );
}


QBoolEndNode::~QBoolEndNode()
{
}


bool QBoolEndNode::evaluate() const
{
  return _value;
}


QBoolNode* QBoolEndNode::clone( QBoolOpNode* parent ) const
{
  return ( new QBoolEndNode( parent, _value ) );
}


QBoolNode* QBoolEndNode::clone( QListView* parent ) const
{
  return ( new QBoolEndNode( parent, _value ) );
}


QBoolEndNode& QBoolEndNode::operator=( bool x )
{
  _value = x;
  return *this;
}


QBoolEndNode& QBoolEndNode::operator=( const QBoolNode& x )
{
  if( this != &x ) {
    _value = x.evaluate();
  }
  return *this;  
}


// *****************************************************************************
// *
// * Class QBoolOpNode
// *
// *****************************************************************************


QBoolOpNode::QBoolOpNode( QPixmap* px, QListView* parent, 
                          const QBoolNode& l, const QBoolNode& r )
: QBoolNode( px, parent, "", "" )
{
  _left = l.clone( this );
  _right = r.clone( this );
  this->setOpen( true );
}


QBoolOpNode::QBoolOpNode( QPixmap* px, QListView* parent, 
                          const QBoolNode& x )
: QBoolNode( px, parent, "", "" ) 
{
  _left = x.clone( this );
  _right = 0;
  this->setOpen( true );
}


QBoolOpNode::QBoolOpNode( QPixmap* px, QBoolOpNode* parent, 
                          const QBoolNode& l, const QBoolNode& r )
: QBoolNode( px, parent, "", "" )
{
  _left = l.clone( this );
  _right = r.clone( this );
  this->setOpen( true );
}


QBoolOpNode::QBoolOpNode( QPixmap* px, QBoolOpNode* parent, 
                          const QBoolNode& x )
: QBoolNode( px, parent, "", "" ) 
{
  _left = x.clone( this );
  _right = 0;
  this->setOpen( true );
}


QBoolOpNode::QBoolOpNode( const QBoolOpNode& x )
: QBoolNode( x )
{
  throw( GenericException( __FILE__, __LINE__, 
         "QBoolOpNode::QBoolOpNode( const QBoolOpNode& x )", 
         "QBoolOpNode copy constructor called" ) );
}

QBoolOpNode::~QBoolOpNode()
{
  delete _left;
  delete _right;
}


// *****************************************************************************
// *
// * Class QAndNode
// *
// *****************************************************************************


QAndNode::QAndNode( QListView* parent, const QBoolNode& l, const QBoolNode& r )
: QBoolOpNode( ExpressionBrowser::andSymbol, parent, l, r )
{
}


QAndNode::QAndNode( QBoolOpNode* parent, const QBoolNode& l, const QBoolNode& r )
: QBoolOpNode( ExpressionBrowser::andSymbol, parent, l, r )
{
}


QAndNode::QAndNode( const QAndNode& x )
: QBoolOpNode( x )
{
  throw( GenericException( __FILE__, __LINE__, 
         "QAndNode::QAndNode( const QAndNode& x )", 
         "QAndNode copy constructor called" ) );
}



QAndNode::~QAndNode()
{
}


bool QAndNode::evaluate() const
{
  return ( (_left->evaluate()) && (_right->evaluate()) );
}


QBoolNode* QAndNode::clone( QBoolOpNode* parent ) const
{
  return ( new QAndNode( parent, *_left, *_right ) );
}


QBoolNode* QAndNode::clone( QListView* parent ) const
{
  return ( new QAndNode( parent, *_left, *_right ) );
}


// *****************************************************************************
// *
// * Class QOrNode
// *
// *****************************************************************************


QOrNode::QOrNode( QListView* parent, const QBoolNode& l, const QBoolNode& r )
: QBoolOpNode( ExpressionBrowser::orSymbol, parent, l, r )
{
}


QOrNode::QOrNode( QBoolOpNode* parent, const QBoolNode& l, const QBoolNode& r )
: QBoolOpNode( ExpressionBrowser::orSymbol, parent, l, r )
{
}


QOrNode::QOrNode( const QOrNode& x )
: QBoolOpNode( x )
{
  throw( GenericException( __FILE__, __LINE__, 
         "QOrNode::QOrNode( const QOrNode& x )", 
         "QOrNode copy constructor called" ) );
}


QOrNode::~QOrNode()
{
}


bool QOrNode::evaluate() const
{
  return ( (_left->evaluate()) || (_right->evaluate()) );
}


QBoolNode* QOrNode::clone( QBoolOpNode* parent ) const
{
  return ( new QOrNode( parent, *_left, *_right ) );
}


QBoolNode* QOrNode::clone( QListView* parent ) const
{
  return ( new QOrNode( parent, *_left, *_right ) );
}


// *****************************************************************************
// *
// * Class QNotNode
// *
// *****************************************************************************


QNotNode::QNotNode( QListView* parent, const QBoolNode& l )
: QBoolOpNode( ExpressionBrowser::notSymbol, parent, l )
{
}


QNotNode::QNotNode( QBoolOpNode* parent, const QBoolNode& l )
: QBoolOpNode( ExpressionBrowser::notSymbol, parent, l )
{
}


QNotNode::QNotNode( const QNotNode& x )
: QBoolOpNode( x )
{
  throw( GenericException( __FILE__, __LINE__, 
         "QNotNode::QNotNode( const QNotNode& x )", 
         "QNotNode copy constructor called" ) );
}


QNotNode::~QNotNode()
{
}


bool QNotNode::evaluate() const
{
  return ( !(_left->evaluate()) );
}


QBoolNode* QNotNode::clone( QBoolOpNode* parent ) const
{
  return ( new QNotNode( parent, *_left ) );
}


QBoolNode* QNotNode::clone( QListView* parent ) const
{
  return ( new QNotNode( parent, *_left ) );
}


// *****************************************************************************
// *
// * Operators
// *
// *****************************************************************************
// 
// 
// QAndNode operator*( const BoolNode& x, const BoolNode& y )
// {
//   return QAndNode( x, y );
// }
// 
// 
// QOrNode  operator+( const BoolNode& x, const BoolNode& y )
// {
//   return QOrNode( x, y );
// }
// 
// 
// QNotNode operator-( const BoolNode& x )
// {
//   return QNotNode( x );
// }


// *****************************************************************************
// *
// * Class ExpressionBrowser
// *
// *****************************************************************************


ExpressionBrowser::ExpressionBrowser( QWidget *parent, const char *name, bool sdo )
{
  if( 0 == andSymbol ) {
    andSymbol = new QPixmap( and_xpm );
    orSymbol  = new QPixmap( or_xpm  );
    notSymbol = new QPixmap( not_xpm );
    endSymbol = new QPixmap( end_xpm );
  }

  this->setSorting(-1);  // prevents sorting
  this->addColumn( "Node type" );
  this->addColumn( "Attribute" );
  this->addColumn( "Condition" );
  this->setTreeStepSize( 20 );
  
  setAcceptDrops( TRUE );
  viewport()->setAcceptDrops( TRUE );
  
  this->setSelectionMode( QListView::Extended );
  this->resize( 400, 400 );
  this->setCaption( "CHEF: Expression Browser" );
  this->setAllColumnsShowFocus( TRUE );
  this->show();
}


ExpressionBrowser::~ExpressionBrowser()
{
}


