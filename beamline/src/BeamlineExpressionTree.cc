/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      BeamlineExpressionTree.cc
******  Version:   ???
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws.
******                                                                
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
******                                                                
**************************************************************************
*************************************************************************/

#include <iostream>
#include <beamline/BeamlineExpressionTree.h>

using namespace std;

BoolEndNode::BoolEndNode()
  : _ptrCritFunc(0)
{
}


BoolEndNode::BoolEndNode( const bmlnElmnt::Discriminator* x )
{
  _ptrCritFunc = x->Clone();
}


BoolEndNode::BoolEndNode( const bmlnElmnt::Discriminator& x )
{
  _ptrCritFunc = x.Clone();
}


BoolEndNode::BoolEndNode( const BoolEndNode& x )
: _ptrCritFunc( (x._ptrCritFunc)->Clone() )
{
}


BoolEndNode::~BoolEndNode()
{
  if( 0 != _ptrCritFunc ) {
    delete _ptrCritFunc;
    _ptrCritFunc = 0;
  }
}


void BoolEndNode::eliminate() 
{ 
  delete this; 
}


void BoolEndNode::_cloneAndSet( const bmlnElmnt::Discriminator* x )
{
  if( 0 != _ptrCritFunc ) { delete _ptrCritFunc; }
  if( x ) { _ptrCritFunc = x->Clone(); }
  else { _ptrCritFunc = 0; }
}


void BoolEndNode::_cloneAndSet( const bmlnElmnt::Discriminator& x )
{
  if( 0 != _ptrCritFunc ) { delete _ptrCritFunc; }
  _ptrCritFunc = x.Clone();
}


bool BoolEndNode::evaluate( const bmlnElmnt* x ) const
{
  if( _ptrCritFunc ) { return (*_ptrCritFunc)( x ); }
  else               { return false; }
}


BoolEndNode& BoolEndNode::operator=( const BoolEndNode& x )
{
  if( this != &x ) {
    _ptrCritFunc = (x._ptrCritFunc)->Clone();
  }
  return *this;  
}



// ++++++++++++++++++++++++++++++++++++++++++++++++

BoolOpNode::BoolOpNode( )
: _left(0), _right(0)
{
}


BoolOpNode::BoolOpNode( const BoolOpNode& x )
{
  if( x._left) { _left  = (x._left) ->Clone(); }
  else { _left = 0; }
  if( x._right) { _right = (x._right)->Clone(); }
  else { _right = 0; }
}

BoolOpNode::~BoolOpNode()
{
}


void BoolOpNode::eliminate()
{ 
  if( 0 != _left  ) { ((BoolNode*) _left)->eliminate();  }
  if( 0 != _right ) { ((BoolNode*) _right)->eliminate(); }
  delete this;
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++

AndNode::AndNode( const BoolNode& l, const BoolNode& r )
{
  _left = &l;
  _right = &r;
}


AndNode::AndNode( const AndNode& x )
: BoolOpNode::BoolOpNode( (const BoolOpNode&) x )
{
}



AndNode::~AndNode()
{
}


bool AndNode::evaluate( const bmlnElmnt* x ) const
{
  return ( (_left->evaluate(x)) && (_right->evaluate(x)) );
}


void AndNode::writeTo( ostream& os ) const
{
  os << "( ";
  _left->writeTo( os );
  os << " AND ";
  _right->writeTo( os );
  os << " )";
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++


OrNode::OrNode( const BoolNode& l, const BoolNode& r )
{
  _left = &l;
  _right = &r;
}


OrNode::OrNode( const OrNode& x )
: BoolOpNode::BoolOpNode( (const BoolOpNode&) x )
{
}



OrNode::~OrNode()
{
}


bool OrNode::evaluate( const bmlnElmnt* x ) const
{
  return ( (_left->evaluate(x)) || (_right->evaluate(x)) );
}


void OrNode::writeTo( ostream& os ) const
{
  os << "( ";
  _left->writeTo( os );
  os << " OR ";
  _right->writeTo( os );
  os << " )";
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++


NotNode::NotNode( const BoolNode& l )
{
  _left = &l;
  _right = 0;
}


NotNode::NotNode( const NotNode& x )
: BoolOpNode::BoolOpNode( (const BoolOpNode&) x )
{
}



NotNode::~NotNode()
{
}


bool NotNode::evaluate( const bmlnElmnt* x ) const
{
  return ( !(_left->evaluate(x)) );
}


void NotNode::writeTo( ostream& os ) const
{
  os << "( ";
  os << " NOT ";
  _left->writeTo( os );
  os << " )";
}

// Derived Discriminators ...

// ... TypeIs
TypeIs::TypeIs( const char* x )
  : bmlnElmnt::Discriminator::Discriminator()
{
  _type = new char[ strlen(x) + 1 ];
  strcpy( _type, x );
}

TypeIs::~TypeIs()
{
  delete [] _type;  _type = 0;

}

bmlnElmnt::Discriminator* TypeIs::Clone() const
{
  return new TypeIs( _type );
}

bool TypeIs::operator()( const bmlnElmnt* x ) const
{
  return ( 0 == strcmp( x->Type(), _type ) );
}


void TypeIs::writeTo( ostream& os ) const
{
  os << "( Type = ";
  os << _type;
  os << " )";
}

// ... NameIs
NameIs::NameIs( const char* x )
  : bmlnElmnt::Discriminator::Discriminator()
{
  _name = new char[ strlen(x) + 1 ];
  strcpy( _name, x );
}

NameIs::~NameIs()
{
  delete [] _name;  _name = 0;
}

bmlnElmnt::Discriminator* NameIs::Clone() const
{
  return new NameIs( _name );
}

bool NameIs::operator()( bmlnElmnt const* x ) const
{
  return ( x->Name() == _name );
}


void NameIs::writeTo( ostream& os ) const
{
  os << "( Name = ";
  os << _name;
  os << " )";
}

// ... LengthIs
LengthIs::LengthIs( double l )
  : bmlnElmnt::Discriminator::Discriminator(),
    _length(l)
{
}

LengthIs::~LengthIs()
{
}

bmlnElmnt::Discriminator* LengthIs::Clone() const
{ 
  return new LengthIs( _length );
}

bool LengthIs::operator()( const bmlnElmnt* x ) const
{
  return ( std::abs( x->Length() - _length ) <= 0.001*_length );
}


void LengthIs::writeTo( ostream& os ) const
{
  os << "( Length = ";
  os << _length;
  os << " )";
}

// ... LengthLess
LengthLess::LengthLess( double l )
  : bmlnElmnt::Discriminator::Discriminator(),
    _length(l)
{
}

LengthLess::~LengthLess()
{
}

bmlnElmnt::Discriminator* LengthLess::Clone() const
{ 
  return new LengthLess( _length );
}

bool LengthLess::operator()( const bmlnElmnt* x ) const
{
  return ( x->Length() < _length );
}


void LengthLess::writeTo( ostream& os ) const
{
  os << "( Length < ";
  os << _length;
  os << " )";
}

// ... LengthMore
LengthMore::LengthMore( double l )
  : bmlnElmnt::Discriminator::Discriminator(),
    _length(l)
{
}

LengthMore::~LengthMore()
{
}

bmlnElmnt::Discriminator* LengthMore::Clone() const
{ 
  return new LengthMore( _length );
}

bool LengthMore::operator()( const bmlnElmnt* x ) const
{
  return ( x->Length() > _length );
}


void LengthMore::writeTo( ostream& os ) const
{
  os << "( Length > ";
  os << _length;
  os << " )";
}

// ... StrengthIs
StrengthIs::StrengthIs( double s )
  : bmlnElmnt::Discriminator::Discriminator(),
    _strength(s)
{
}

StrengthIs::~StrengthIs()
{
}

bmlnElmnt::Discriminator* StrengthIs::Clone() const
{ 
  return new StrengthIs( _strength );
}

bool StrengthIs::operator()( const bmlnElmnt* x ) const
{
  return ( std::abs( x->Strength() - _strength ) <= 0.001*_strength );
}


void StrengthIs::writeTo( ostream& os ) const
{
  os << "( Strength = ";
  os << _strength;
  os << " )";
}

// ... StrengthLess
StrengthLess::StrengthLess( double s )
  : bmlnElmnt::Discriminator::Discriminator(),
    _strength(s)
{
}

StrengthLess::~StrengthLess()
{
}

bmlnElmnt::Discriminator* StrengthLess::Clone() const
{ 
  return new StrengthLess( _strength );
}

bool StrengthLess::operator()( const bmlnElmnt* x ) const
{
  return ( x->Strength() < _strength );
}


void StrengthLess::writeTo( ostream& os ) const
{
  os << "( Strength < ";
  os << _strength;
  os << " )";
}

// ... StrengthMore
StrengthMore::StrengthMore( double s )
  : bmlnElmnt::Discriminator::Discriminator(),
    _strength(s)
{
}

StrengthMore::~StrengthMore()
{
}

bmlnElmnt::Discriminator* StrengthMore::Clone() const
{ 
  return new StrengthMore( _strength );
}

bool StrengthMore::operator()( const bmlnElmnt* x ) const
{
  return ( x->Strength() > _strength );
}


void StrengthMore::writeTo( ostream& os ) const
{
  os << "( Strength > ";
  os << _strength;
  os << " )";
}

// REMOVE: // ... StrengthIs
// REMOVE: StrengthIs::StrengthIs( double s )
// REMOVE:   : bmlnElmnt::Discriminator::Discriminator(),
// REMOVE:     _strength(s)
// REMOVE: {
// REMOVE: }
// REMOVE: 
// REMOVE: StrengthIs::~StrengthIs()
// REMOVE: {
// REMOVE: }
// REMOVE: 
// REMOVE: bmlnElmnt::Discriminator* StrengthIs::Clone() const
// REMOVE: { 
// REMOVE:   return new StrengthIs( _strength );
// REMOVE: }
// REMOVE: 
// REMOVE: bool StrengthIs::operator()( const bmlnElmnt* x ) const
// REMOVE: {
// REMOVE:   return ( std::abs( x->Strength() - _strength ) <= 0.001*std::abs(_strength) );
// REMOVE: }
// REMOVE: 
// REMOVE: void StrengthIs::writeTo( ostream& os ) const
// REMOVE: {
// REMOVE:   os << "( Strength = ";
// REMOVE:   os << _strength;
// REMOVE:   os << " )";
// REMOVE: }


// Derived EndNodes ...

TypeNode::TypeNode( const char* x )
  : BoolEndNode()
{
  _cloneAndSet( TypeIs(x) );
}

NameNode::NameNode( const char* x )
  : BoolEndNode()
{
  _cloneAndSet( NameIs(x) );
}

LengthEqNode::LengthEqNode( double x )
  : BoolEndNode()
{
  _cloneAndSet( LengthIs(std::abs(x)) );
}

LengthLtNode::LengthLtNode( double x )
  : BoolEndNode()
{
  _cloneAndSet( LengthLess(std::abs(x)) );
}

LengthGtNode::LengthGtNode( double x )
  : BoolEndNode()
{
  _cloneAndSet( LengthMore(std::abs(x)) );
}

StrengthEqNode::StrengthEqNode( double x )
  : BoolEndNode()
{
  _cloneAndSet( StrengthIs(x) );
}

StrengthLtNode::StrengthLtNode( double x )
  : BoolEndNode()
{
  _cloneAndSet( StrengthLess(x) );
}

StrengthGtNode::StrengthGtNode( double x )
  : BoolEndNode()
{
  _cloneAndSet( StrengthMore(x) );
}

