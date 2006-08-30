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
**************************************************************************
*************************************************************************/

#ifndef BMLEXPTREE_H
#define BMLEXPTREE_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>


class DLLEXPORT BoolNode 
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
  virtual bool isNull() const {return false; }

};


struct DLLEXPORT BoolNullNode : public BoolNode
{
public:
  BoolNullNode() {}
  ~BoolNullNode() {}
  bool evaluate( const bmlnElmnt* ) const { return false; }
  void writeTo( std::ostream& ) const {}
  void eliminate() { delete this; }
  BoolNode* Clone() const { return new BoolNullNode; }
  virtual bool isNull() const {return true; }

};


class DLLEXPORT BoolEndNode : public virtual BoolNode
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


class DLLEXPORT BoolOpNode : public virtual BoolNode
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


class DLLEXPORT AndNode : public virtual BoolOpNode
{
 public:
   AndNode( const BoolNode&, const BoolNode& );
   AndNode( const AndNode& );
   ~AndNode();

   bool evaluate( const bmlnElmnt* ) const;
   void writeTo( std::ostream& os ) const;
   BoolNode* Clone() const { return new AndNode( *this ); }
};


class DLLEXPORT OrNode : public virtual BoolOpNode
{
 public:
   OrNode( const BoolNode&, const BoolNode& );
   OrNode( const OrNode& );
   ~OrNode();

   bool evaluate( const bmlnElmnt* ) const;
   void writeTo( std::ostream& os ) const;
   BoolNode* Clone() const { return new OrNode( *this ); }
};


class DLLEXPORT NotNode : public virtual BoolOpNode
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

class DLLEXPORT TypeIs : public virtual bmlnElmnt::Discriminator
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


class DLLEXPORT NameIs : public virtual bmlnElmnt::Discriminator
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


class DLLEXPORT  LengthIs : public virtual bmlnElmnt::Discriminator
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


class DLLEXPORT LengthLess : public virtual bmlnElmnt::Discriminator
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


class DLLEXPORT LengthMore : public virtual bmlnElmnt::Discriminator
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


class DLLEXPORT StrengthIs : public virtual bmlnElmnt::Discriminator
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


class DLLEXPORT StrengthLess : public virtual bmlnElmnt::Discriminator
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


class DLLEXPORT StrengthMore : public virtual bmlnElmnt::Discriminator
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

struct DLLEXPORT TypeNode: public virtual BoolEndNode
{
  TypeNode( const char* );
  ~TypeNode() {}
};

struct DLLEXPORT NameNode: public virtual BoolEndNode
{
  NameNode( const char* );
  ~NameNode() {}
};

struct DLLEXPORT LengthEqNode: public virtual BoolEndNode
{
  LengthEqNode( double );
  ~LengthEqNode() {}
};

struct DLLEXPORT LengthLtNode: public virtual BoolEndNode
{
  LengthLtNode( double );
  ~LengthLtNode() {}
};

struct DLLEXPORT LengthGtNode: public virtual BoolEndNode
{
  LengthGtNode( double );
  ~LengthGtNode() {}
};

struct DLLEXPORT StrengthEqNode: public virtual BoolEndNode
{
  StrengthEqNode( double );
  ~StrengthEqNode() {}
};

struct DLLEXPORT StrengthLtNode: public virtual BoolEndNode
{
  StrengthLtNode( double );
  ~StrengthLtNode() {}
};

struct DLLEXPORT StrengthGtNode: public virtual BoolEndNode
{
  StrengthGtNode( double );
  ~StrengthGtNode() {}
};


#endif // BMLEXPTREE_H

