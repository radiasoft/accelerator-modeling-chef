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
*************************************************************************/
#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <iostream>
#include <boost/any.hpp>
#include <sequential_tree.h>
#include <XsifParser_ypp.hh>


class Token {

 public:
  Token(xsif_yy::XsifParser::token_type  t ) : m_token(t)         {}
  bool operator==(Token const& rhs)       { return m_token == rhs.m_token;    } 
  operator int() const                    { return static_cast<int>(m_token); }

  friend std::ostream& operator<<( std::ostream&  os, Token const& token) { os << static_cast<char>(token.m_token); return os; } 

 private:

  xsif_yy::XsifParser::token_type  m_token;

};


class ExprData;
std::ostream& operator<<(std::ostream &os, const ExprData & expr);


class ExprData {

friend  std::ostream& operator<<(std::ostream &os, const ExprData & expr);

public:

  ExprData() {}
  ExprData(boost::any const&  v) : value(v) {}
  ExprData(ExprData   const& exp): value(exp.value) {}

  boost::any      value;


};



typedef  sequential_tree<ExprData>::iterator             expr_iterator_t;
typedef  sequential_tree<ExprData>::const_iterator const_expr_iterator_t;

class Expression;

std::ostream&  operator<<( std::ostream& os, Expression const& exp);


class Expression: public sequential_tree<ExprData> {

friend std::ostream&  operator<<( std::ostream& os, Expression const& exp);
friend  std::ostream& operator<<(std::ostream &os, const ExprData & expr);

public:


  Expression():                       sequential_tree<ExprData>()      { }                               
  Expression(Expression const& expr): sequential_tree<ExprData>(expr)  { }                               

  double evaluate( ) const;

  template<typename T>  
  static bool is_type(const boost::any & operand) { return operand.type() == typeid(T); }

private:
  
  double evaluate( const_expr_iterator_t ) const;

};


#endif  // EXPRESSION_H
