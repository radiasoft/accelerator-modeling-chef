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
************ REVISION HISTORY
******
****** July 2007  ostiguy@fnal.gov
******
****** - refactored. Eliminated complex virtual inheritance scheme.
******   and all references to functors defined as nested classes in
******   bmlnElmnt and beamline classes.                                                                
**************************************************************************
*************************************************************************/

#include <iostream>
#include <beamline/BeamlineExpressionTree.h>

using namespace std;


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BoolEndNode::BoolEndNode( boost::function<bool( bmlnElmnt const* )>  discriminator )
 : discriminator_(discriminator) 
{}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


BoolEndNode::BoolEndNode( BoolEndNode const& x )
: discriminator_(x.discriminator_) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bool BoolEndNode::evaluate( bmlnElmnt const* x ) const
{
  return discriminator_(x);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


BoolEndNode& BoolEndNode::operator=( BoolEndNode const& x )
{
  if( this == &x )  return *this;

  discriminator_ =   x.discriminator_;
  return *this;  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// ++++++++++++++++++++++++++++++++++++++++++++++++

BoolOpNode::BoolOpNode( BoolNode const* l, BoolNode const* r )
: left_(l), right_(r)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BoolOpNode::BoolOpNode( BoolOpNode const& x )
{
  left_  = x.left_  ? x.left_->Clone() : 0; 
  right_ = x.right_ ? x.right_->Clone(): 0; 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BoolOpNode::~BoolOpNode()
{
  if( left_  ) { delete left_;  }
  if( right_ ) { delete right_; }
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++

AndNode::AndNode( BoolNode const* l, BoolNode const* r )
  : BoolOpNode( l,r )
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

AndNode::AndNode( AndNode const& x )
: BoolOpNode( x )
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

AndNode::~AndNode()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool AndNode::evaluate( bmlnElmnt const* x ) const
{
  return ( left_->evaluate(x)  && right_->evaluate(x) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void AndNode::writeTo( ostream& os ) const
{
  os << "( ";
  left_->writeTo( os );
  os << " AND ";
  right_->writeTo( os );
  os << " )";
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++


OrNode::OrNode( BoolNode const*  l, BoolNode const*  r )
 : BoolOpNode( l,r )
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OrNode::OrNode( OrNode const& x )
: BoolOpNode( x )
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OrNode::~OrNode()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool OrNode::evaluate( const bmlnElmnt* x ) const
{
  return ( left_->evaluate(x) || right_->evaluate(x) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OrNode::writeTo( ostream& os ) const
{
  os << "( ";
  left_->writeTo( os );
  os << " OR ";
  right_->writeTo( os );
  os << " )";
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++


NotNode::NotNode( BoolNode const* l )
 : BoolOpNode( l,0 )
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

NotNode::NotNode( NotNode const& x )
: BoolOpNode(x)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

NotNode::~NotNode()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool NotNode::evaluate( bmlnElmnt const* x ) const
{
  return !left_->evaluate(x);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void NotNode::writeTo( ostream& os ) const
{
  os << "( ";
  os << " NOT ";
  left_->writeTo( os );
  os << " )";
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// Discriminators ...

// ... TypeIs

class TypeIs {

public:

 TypeIs( const char* x ): type_(x) {}

 bool operator()( bmlnElmnt const * x ) const   { return (  x->Type() ==  type_ ); }

 void writeTo( ostream& os )            const  { os << "( Type = " << type_ << " )"; }
 
private:

  std::string type_;
};


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// ... NameIs

class NameIs { 

 public:

  NameIs( const char* x ) : name_(x) {}

  bool operator()( bmlnElmnt const* x ) const  { return x->Name() == name_ ;        }
  void writeTo( ostream& os )           const  {os << "( Name = " << name_ << " )"; }

 private:
 
  std::string name_; 
};


// ... LengthIs

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


class LengthIs {

 public:
   LengthIs( double l ): length_(l) {}
   bool operator()( bmlnElmnt const* x ) const { return std::abs( x->Length() - length_ ) <= 0.001*length_; }
   void writeTo( ostream& os ) const { os << "( Length = " << length_ << os << " )";                        }

 private:
  double length_;
};


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

class LengthLess {

 public: 
  LengthLess( double l ): length_(l) {}
  bool operator()( const bmlnElmnt* x ) const { return ( x->Length() < length_ );        }
  void writeTo( ostream& os )           const {  os << "( Length < " << length_ << " )"; }

 private:
  double length_;
};


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// ... LengthMore

class LengthMore {

 public:
   LengthMore( double l ) : length_(l) {}
   bool operator()( const bmlnElmnt* x )   const { return ( x->Length() > length_ );       }
   void writeTo( ostream& os ) const { os << "( Length > " << length_ << " )";             }

 private:
  double length_;
};

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// ... StrengthIs

class StrengthIs {

 public:
   StrengthIs( double s ) : strength_(s) {}
   bool operator()( const bmlnElmnt* x ) const { return ( std::abs( x->Strength() - strength_ ) <= 0.001*strength_ ); }
   void writeTo( ostream& os )           const { os << "( Strength = " << strength_ << ")";                           }
 
 private:

  double strength_;

};

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

class StrengthLess {

 public:
   StrengthLess( double s ) : strength_(s) {}
   bool operator()(bmlnElmnt const* x ) const  { return (x->Strength() < strength_);           }
 
 private:

  double strength_;

};

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

class StrengthMore {

 public:
   StrengthMore( double s ) : strength_(s) {}
   bool operator()( const bmlnElmnt* x ) const { return ( x->Strength() > strength_ );       }
 
 private:

  double strength_;

};

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Derived EndNodes ...

TypeNode::TypeNode( const char* x )
  : BoolEndNode( TypeIs( x ) ), type_(x)
{}

TypeNode::TypeNode( TypeNode const& x )
  : BoolEndNode(x), type_(x.type_)
{}

TypeNode* TypeNode::Clone() const 
{
  return new  TypeNode(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TypeNode::writeTo( ostream& os) const
{
   os << "( Type = " << type_ << ")";    
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


NameNode::NameNode( const char* x )
  : BoolEndNode( NameIs(x) ), name_(x)
{}

NameNode::NameNode( NameNode const& x )
  : BoolEndNode(x), name_(x.name_)
{}

NameNode* NameNode::Clone() const
{
  return new NameNode(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void NameNode::writeTo( ostream& os) const
{
   os << "( Name = " << name_ << ")";    
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LengthEqNode::LengthEqNode( double x )
  : BoolEndNode(LengthIs(std::abs(x) )), length_( std::abs(x) )
{}

LengthEqNode::LengthEqNode( LengthEqNode const& x )
  : BoolEndNode(x), length_( x.length_)
{}

LengthEqNode* LengthEqNode::Clone() const
{
  return new LengthEqNode(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LengthEqNode::writeTo( ostream& os) const
{
   os << "( Length = " << length_ << ")";    
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LengthLtNode::LengthLtNode( double x )
  : BoolEndNode( LengthLess(std::abs(x)) ), length_( std::abs(x) )
{}

LengthLtNode::LengthLtNode( LengthLtNode const& x )
  : BoolEndNode(x), length_( x.length_)
{}

LengthLtNode* LengthLtNode::Clone() const
{
  return new  LengthLtNode(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LengthLtNode::writeTo( ostream& os) const
{
   os << "( Length = " << length_ << ")";    
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LengthGtNode::LengthGtNode( double x )
  : BoolEndNode( LengthMore(std::abs(x)) ), length_( std::abs(x) )
{}

LengthGtNode::LengthGtNode( LengthGtNode const& x )
  : BoolEndNode(x), length_( x.length_)
{}

LengthGtNode* LengthGtNode::Clone() const
{
  return new  LengthGtNode(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LengthGtNode::writeTo( ostream& os) const
{
   os << "( Length = " << length_ << ")";    
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

StrengthEqNode::StrengthEqNode( double x )
  : BoolEndNode( StrengthIs(x) ), strength_(x)
{}

StrengthEqNode::StrengthEqNode( StrengthEqNode const& x )
  : BoolEndNode(x), strength_(x.strength_)
{}

StrengthEqNode* StrengthEqNode::Clone() const
{ 
  return new StrengthEqNode(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void StrengthEqNode::writeTo( ostream& os ) const
{
   os << "( Strength = " << strength_ << ")";    
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

StrengthLtNode::StrengthLtNode( double x )
  : BoolEndNode( StrengthLess(x) ), strength_(x)
{}

StrengthLtNode::StrengthLtNode( StrengthLtNode const& x )
  : BoolEndNode( x ), strength_(x.strength_)
{}

StrengthLtNode* StrengthLtNode::Clone() const
{ 
  return new StrengthLtNode(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void StrengthLtNode::writeTo( ostream& os ) const
{
   os << "( Strength = " << strength_ << ")";    
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

StrengthGtNode::StrengthGtNode( double x )
  : BoolEndNode(StrengthMore(x) ), strength_(x)
{}

StrengthGtNode::StrengthGtNode( StrengthGtNode const& x)
  : BoolEndNode(x), strength_(x.strength_)
{}

StrengthGtNode* StrengthGtNode::Clone() const
{
  return new StrengthGtNode(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void StrengthGtNode::writeTo( ostream& os ) const 
{ 
  os << "( Strength = " << strength_ << ")";  
}
