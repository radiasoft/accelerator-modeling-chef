/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      BeamlineExpressionTree.h
******  Version:   ???
******                                                                
******  Copyright (c) 2003 Universities Research Association, Inc.    
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


#ifndef BMLEXPTREE_H
#define BMLEXPTREE_H

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif

class BoolNode 
{
public:
  virtual bool evaluate( const bmlnElmnt* ) const = 0;
  virtual void writeTo( std::ostream& ) const = 0;
  virtual ~BoolNode() {}
  virtual void eliminate() = 0;  // Dangerous function: acts like delete.
                                 // Assumes everything exists
                                 // on the heap. Should not be
                                 // used otherwise.
  virtual BoolNode* Clone() const = 0;
};


struct BoolNullNode : public BoolNode
{
public:
  BoolNullNode() {}
  ~BoolNullNode() {}
  bool evaluate( const bmlnElmnt* ) const { return false; }
  void writeTo( std::ostream& ) const {}
  void eliminate() { delete this; }
  BoolNode* Clone() const { return new BoolNullNode; }
};


class BoolEndNode : public virtual BoolNode
{
public: 
  BoolEndNode();   // Useful for derived EndNodes
  BoolEndNode( const bmlnElmnt::Discriminator* );
  BoolEndNode( const bmlnElmnt::Discriminator& );
  BoolEndNode( const BoolEndNode& );
  virtual ~BoolEndNode();

  bool evaluate( const bmlnElmnt* ) const;

  BoolEndNode& operator=( const BoolEndNode& );

  virtual BoolNode* Clone() const { return new BoolEndNode( *this ); }

protected:
  void _cloneAndSet( const bmlnElmnt::Discriminator* );
  void _cloneAndSet( const bmlnElmnt::Discriminator& );

private:
  // Owned data: the Discriminator argument in the
  // constructor is copied using its .Clone method.
  // Done this way to facilitate definition of 
  // derived BoolEndNodes (e.g., NameNode).
  // Object is deleted by base class destructor.
  const bmlnElmnt::Discriminator* _ptrCritFunc;

public:
  void writeTo( std::ostream& os ) const { _ptrCritFunc->writeTo(os); }
  void eliminate();
};


class BoolOpNode : public virtual BoolNode
{
protected:
  BoolOpNode();
  BoolOpNode( const BoolOpNode& );
  virtual ~BoolOpNode();
  
  virtual bool evaluate( const bmlnElmnt* ) const = 0;
  virtual void writeTo( std::ostream& ) const = 0;
  virtual BoolNode* Clone() const = 0;

protected:
  // These pointers are not owned.
  // Responsibility for garbage collection
  // lies elsewhere.
  const BoolNode* _left;
  const BoolNode* _right;

public:
  void eliminate();
};


class AndNode : public virtual BoolOpNode
{
 public:
   AndNode( const BoolNode&, const BoolNode& );
   AndNode( const AndNode& );
   ~AndNode();

   bool evaluate( const bmlnElmnt* ) const;
   void writeTo( std::ostream& os ) const;
   BoolNode* Clone() const { return new AndNode( *this ); }
};


class OrNode : public virtual BoolOpNode
{
 public:
   OrNode( const BoolNode&, const BoolNode& );
   OrNode( const OrNode& );
   ~OrNode();

   bool evaluate( const bmlnElmnt* ) const;
   void writeTo( std::ostream& os ) const;
   BoolNode* Clone() const { return new OrNode( *this ); }
};


class NotNode : public virtual BoolOpNode
{
 public:
   NotNode( const BoolNode& );
   NotNode( const NotNode& );
   ~NotNode();

   bool evaluate( const bmlnElmnt* ) const;
   void writeTo( std::ostream& os ) const;
   BoolNode* Clone() const { return new NotNode( *this ); }
};


AndNode operator*( const BoolNode&, const BoolNode& );


// Derived Discriminators

class TypeIs : public virtual bmlnElmnt::Discriminator
{
public:
  TypeIs( const char* );
  ~TypeIs();

  bmlnElmnt::Discriminator* Clone() const;
  bool operator()( const bmlnElmnt* ) const;
  void writeTo( std::ostream& os ) const;
private: 
  char* _type;
};


class NameIs : public virtual bmlnElmnt::Discriminator
{
public:
  NameIs( const char* );
  ~NameIs();

  bmlnElmnt::Discriminator* Clone() const;
  bool operator()( const bmlnElmnt* ) const;
  void writeTo( std::ostream& os ) const;
private: 
  char* _name;
};


class LengthIs : public virtual bmlnElmnt::Discriminator
{
public:
  LengthIs( double );
  ~LengthIs();

  bmlnElmnt::Discriminator* Clone() const;
  bool operator()( const bmlnElmnt* ) const;
  void writeTo( std::ostream& os ) const;
private: 
  double _length;
};


class LengthLess : public virtual bmlnElmnt::Discriminator
{
public:
  LengthLess( double );
  ~LengthLess();

  bmlnElmnt::Discriminator* Clone() const;
  bool operator()( const bmlnElmnt* ) const;
  void writeTo( std::ostream& os ) const;
private: 
  double _length;
};


class LengthMore : public virtual bmlnElmnt::Discriminator
{
public:
  LengthMore( double );
  ~LengthMore();

  bmlnElmnt::Discriminator* Clone() const;
  bool operator()( const bmlnElmnt* ) const;
  void writeTo( std::ostream& os ) const;
private: 
  double _length;
};


class StrengthIs : public virtual bmlnElmnt::Discriminator
{
public:
  StrengthIs( double );
  ~StrengthIs();

  bmlnElmnt::Discriminator* Clone() const;
  bool operator()( const bmlnElmnt* ) const;
  void writeTo( std::ostream& os ) const;
private: 
  double _strength;
};


class StrengthLess : public virtual bmlnElmnt::Discriminator
{
public:
  StrengthLess( double );
  ~StrengthLess();

  bmlnElmnt::Discriminator* Clone() const;
  bool operator()( const bmlnElmnt* ) const;
  void writeTo( std::ostream& os ) const;
private: 
  double _strength;
};


class StrengthMore : public virtual bmlnElmnt::Discriminator
{
public:
  StrengthMore( double );
  ~StrengthMore();

  bmlnElmnt::Discriminator* Clone() const;
  bool operator()( const bmlnElmnt* ) const;
  void writeTo( std::ostream& os ) const;
private: 
  double _strength;
};


// Derived EndNodes

struct TypeNode: public virtual BoolEndNode
{
  TypeNode( const char* );
  ~TypeNode() {}
};

struct NameNode: public virtual BoolEndNode
{
  NameNode( const char* );
  ~NameNode() {}
};

struct LengthEqNode: public virtual BoolEndNode
{
  LengthEqNode( double );
  ~LengthEqNode() {}
};

struct LengthLtNode: public virtual BoolEndNode
{
  LengthLtNode( double );
  ~LengthLtNode() {}
};

struct LengthGtNode: public virtual BoolEndNode
{
  LengthGtNode( double );
  ~LengthGtNode() {}
};

struct StrengthEqNode: public virtual BoolEndNode
{
  StrengthEqNode( double );
  ~StrengthEqNode() {}
};

struct StrengthLtNode: public virtual BoolEndNode
{
  StrengthLtNode( double );
  ~StrengthLtNode() {}
};

struct StrengthGtNode: public virtual BoolEndNode
{
  StrengthGtNode( double );
  ~StrengthGtNode() {}
};


#endif // BMLEXPTREE_H

