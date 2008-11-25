/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  XSIF Parser: Interprets XSIF input files and             
******               creates instances of class beamline.
******                                                
******                                    
******  File:      Expression.h
******                                                                
******  Copyright (c) Universities Research Association, Inc. / Fermilab  
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
******                                                                
******  Author:    Jean-Francois Ostiguy      
******             ostiguy@fnal.gov
******                                                                
******
**************************************************************************
**************************************************************************
*************************************************************************/
#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <iostream>
#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>
#include <sequential_tree.h>
#include <XsifParser_ypp.hh>


class Token {

 public:
  Token(xsif_yy::XsifParser::token_type  t ) 
   : token_(t) {}

  bool operator==(Token const& rhs)       
     { return token_ == rhs.token_;    } 

  operator int() const                    
     { return static_cast<int>(token_); }

  friend std::ostream& operator<<( std::ostream&  os, Token const& token) 
     { os << static_cast<char>(token.token_); return os; } 

 private:

  xsif_yy::XsifParser::token_type  token_;

};

//--------------------------------------------------------------------------------------

class ExprData {

  friend  std::ostream& operator<<(std::ostream &os, ExprData const& expr);

public:

  ExprData() {}
  ExprData(boost::any const&  v) : value(v) {}
  ExprData(ExprData   const& exp): value(exp.value) {}

  boost::any      value;

};

std::ostream& operator<<(std::ostream &os, ExprData const& expr);

//--------------------------------------------------------------------------------------

class Expression : public tcl::sequential_tree<ExprData> {

 friend  std::ostream&  operator<<( std::ostream& os, Expression const& exp);
 friend  std::ostream&  operator<<( std::ostream &os, ExprData   const& expr);

public:

  typedef tcl::sequential_tree<ExprData>::iterator                                 iterator; 
  typedef tcl::sequential_tree<ExprData>::const_iterator                     const_iterator; 

  typedef tcl::sequential_tree<ExprData>::pre_order_iterator              pre_order_iterator;
  typedef tcl::sequential_tree<ExprData>::const_pre_order_iterator  const_pre_order_iterator;

  Expression() {} ;
  Expression(Expression const& expr) 
   : tcl::sequential_tree<ExprData>( expr ){ }

  double evaluate() const;

  template<typename T>  
  static bool is_type( boost::any  const& operand) { return operand.type() == typeid(T); }


  iterator       begin() ; 
  const_iterator begin() const; 

  iterator       end() ; 
  const_iterator end() const; 


  pre_order_iterator        pre_order_begin(); 
  const_pre_order_iterator  pre_order_begin()  const; 

  pre_order_iterator          pre_order_end(); 
  const_pre_order_iterator    pre_order_end()  const; 


private:
  
  double evaluate ( const_iterator ) const;

};


//--------------------------------------------------------------------------------------------------





#endif  // EXPRESSION_H
