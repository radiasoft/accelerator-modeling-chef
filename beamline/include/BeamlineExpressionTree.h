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
****** REVISION HISTORY
******
****** July 2007  ostiguy@fnal.gov
******
****** - refactored. Eliminated complex virtual inheritance scheme.
******   and all references to functors defined as nested classes in
******   bmlnElmnt and beamline classes.
**************************************************************************
*************************************************************************/

#ifndef BMLEXPTREE_H
#define BMLEXPTREE_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>
#include <boost/function.hpp>


class DLLEXPORT BoolNode {

public:

  virtual  BoolNode* Clone()                 const = 0;
  virtual ~BoolNode() {}

  virtual  bool evaluate( bmlnElmnt const* ) const = 0;
  virtual  void writeTo( std::ostream& )     const = 0;
  virtual  bool isNull()                     const {return false; }

};

struct DLLEXPORT BoolNullNode : public BoolNode {

public:

  BoolNullNode() {}
 ~BoolNullNode() {}

  bool evaluate( bmlnElmnt const* ) const { return false;              }
  void writeTo( std::ostream& )     const {}
  BoolNode* Clone()                 const { return new BoolNullNode(); } 
  bool isNull()                     const { return true;               }

};


class DLLEXPORT BoolEndNode : public BoolNode {

public: 

  BoolEndNode( boost::function<bool( bmlnElmnt const* )>  discriminator );
  BoolEndNode( BoolEndNode const& );

  bool evaluate( bmlnElmnt const* ) const;

protected:

  BoolEndNode& operator=( BoolEndNode const& x );
  boost::function<bool( bmlnElmnt const* )> discriminator_;

};


class DLLEXPORT BoolOpNode : public BoolNode {

public:

  BoolOpNode( BoolNode const* l, BoolNode const* r );
  BoolOpNode( BoolOpNode const& );
 ~BoolOpNode();

  BoolOpNode* Clone()               const = 0;
  bool evaluate( bmlnElmnt const* ) const = 0;
  void writeTo( std::ostream& )     const = 0;
 
 
protected:

  BoolNode const* left_;
  BoolNode const* right_;

};


class DLLEXPORT AndNode : public BoolOpNode {

 public:
   AndNode( BoolNode const* , BoolNode const* );
   AndNode( AndNode const& );
  ~AndNode();
   AndNode* Clone()                    const { return new AndNode( *this ); }

   bool evaluate( bmlnElmnt const* )   const;
   void writeTo( std::ostream& os )    const;
};


class DLLEXPORT OrNode : public BoolOpNode {
 public:
   OrNode( BoolNode const*, BoolNode const* );
   OrNode( OrNode const& );
   ~OrNode();
   OrNode* Clone()                    const { return new OrNode( *this ); }

   bool evaluate( const bmlnElmnt* )  const;
   void writeTo( std::ostream& os )   const;
};


class DLLEXPORT NotNode : public BoolOpNode {

 public:

   NotNode( BoolNode const* );
   NotNode( NotNode  const& );
  ~NotNode();
   NotNode* Clone()                    const { return new NotNode( *this ); }

   bool evaluate( bmlnElmnt const* )   const;
   void writeTo( std::ostream& os )    const;
};


// Derived EndNodes

class DLLEXPORT TypeNode: public BoolEndNode
{
 public:
   TypeNode( const char* );
   TypeNode(TypeNode const& );
   TypeNode* Clone() const;

   void writeTo( std::ostream& os )    const;

 private:
   std::string type_;
};

class DLLEXPORT NameNode: public BoolEndNode
{
 public:
  NameNode( const char* );
  NameNode( NameNode const& );
  NameNode* Clone() const; 

  void writeTo( std::ostream& os )    const;
 private:
   std::string name_;
};

class DLLEXPORT LengthEqNode: public BoolEndNode
{
 public:
  LengthEqNode( double );
  LengthEqNode( LengthEqNode const& );
  LengthEqNode* Clone() const;

  void writeTo( std::ostream& os )    const;

 private:
   double length_;
};

class DLLEXPORT LengthLtNode: public BoolEndNode
{
 public:
  LengthLtNode( double );
  LengthLtNode( LengthLtNode const& );
  LengthLtNode* Clone() const;

  void writeTo( std::ostream& os )    const;
 private:
   double length_;
};

class DLLEXPORT LengthGtNode: public BoolEndNode
{
 public:
  LengthGtNode( double );
  LengthGtNode( LengthGtNode const& );
  LengthGtNode* Clone() const;

  void writeTo( std::ostream& os )    const;
 private:
  double length_;
};

class DLLEXPORT StrengthEqNode: public BoolEndNode
{
 public:
  StrengthEqNode( double );
  StrengthEqNode( StrengthEqNode const& );
  StrengthEqNode* Clone() const;

  void writeTo( std::ostream& os )    const;
 private:
   double strength_;
};

class DLLEXPORT StrengthLtNode: public BoolEndNode
{
 public:
  StrengthLtNode( double );
  StrengthLtNode(StrengthLtNode const&);
  StrengthLtNode* Clone() const;

  void writeTo( std::ostream& os )    const;

 private:
   double strength_;
};

class DLLEXPORT StrengthGtNode: public BoolEndNode
{
 public:
  StrengthGtNode( double );
  StrengthGtNode(StrengthGtNode const&);
  
  StrengthGtNode* Clone() const;

  void writeTo( std::ostream& os )    const;
 private:
   double strength_;

};


#endif // BMLEXPTREE_H

