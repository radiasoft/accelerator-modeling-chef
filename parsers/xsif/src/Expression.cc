/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interpret input files and instantiate 
******                     beamline objects.                 
******                                                
******  File:      Expression.cc
******                                                                
******  Copyright (c) Universities Research Association, Inc.   
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
******  Author:  Jean-Francois Ostiguy
******           ostiguy@fnal.gov 
******
**************************************************************************
**************************************************************************/

#include <boost/any.hpp>
#include <cmath>
#include <Expression.h>
#include <XsifParser_ypp.hh>
#include <string>

using namespace std;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::ostream& operator<<(std::ostream &os, ExprData const& expr) {
  
  if (Expression::is_type<double> (expr.value) )
    os <<  "Real Value: " << boost::any_cast<double>(expr.value) << std::endl;
  else if (Expression::is_type<int>(expr.value) )
    os <<  "Integer Value: " << boost::any_cast<int>(expr.value) << std::endl;
  else if (Expression::is_type<Token> (expr.value) ) {
    int v = static_cast<int>(  boost::any_cast<Token>(expr.value) ); 
    char c[2];
    c[1] = 0; 
    if ( v < 127 ) {
      c[0] = static_cast<char>(v); 
       os <<  "Token:  " <<  c << std::endl;   
    } else
       os <<  "Token:  " <<  v << std::endl;   

  }
  else if (Expression::is_type<Expression>(expr.value) )
    os <<  "Expression " << std::endl;   
  else 
    os << "Unknown type." << std::endl;
    
  return os;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::ostream&  operator<<( std::ostream& os, Expression const& exp) {
  
  // ... prints the expression tree 

  Expression::const_pre_order_iterator it; 

  for ( it  = exp.pre_order_begin(); it != exp.pre_order_end();  ++it)
  {
      os <<  *it  << std::endl; 

  }

  return os;
}

  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double 
Expression::evaluate( ) const                         
{ 
  const_iterator    theIterator(  begin() ); 
  return evaluate(  const_iterator( theIterator ) ); 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double 
Expression::evaluate( Expression::const_iterator  arg ) const  
{

    typedef xsif_yy::XsifParser::token_type token_type; 
    typedef xsif_yy::XsifParser::token      token; 

    double result = 0;
 
    const_iterator q;

    if ( is_type<Token>(arg->value) ) {  

	 switch ( int tk = boost::any_cast<Token>(arg->value) ) {
  	    case token_type('+'):          q = arg.node()->begin(); result = evaluate(q) + evaluate (++q);          break;
	    case token_type('-'):          q = arg.node()->begin(); result = evaluate(q) - evaluate (++q);          break;
	    case token_type('*'):          q = arg.node()->begin(); result = evaluate(q) * evaluate (++q);          break;
	    case token_type('/'):          q = arg.node()->begin(); result = evaluate(q) / evaluate (++q);          break;
	    case token_type('^'):          q = arg.node()->begin(); result = std::pow(evaluate(q), evaluate (++q)); break;
  	    case token::NEG:               q = arg.node()->begin(); result = -evaluate(q)            ; break;    
  	    case token::XSIF_SQRT:         q = arg.node()->begin(); result = std::sqrt(evaluate(q))  ; break;    
	    case token::XSIF_SIN:          q = arg.node()->begin(); result = std::sin(evaluate(q))   ; break; 
	    case token::XSIF_COS:          q = arg.node()->begin(); result = std::cos(evaluate(q))   ; break; 
	    case token::XSIF_TAN:          q = arg.node()->begin(); result = std::tan(evaluate(q))   ; break; 
	    case token::XSIF_ASIN:         q = arg.node()->begin(); result = std::asin(evaluate(q))  ; break; 
	    case token::XSIF_ACOS:         q = arg.node()->begin(); result = std::acos(evaluate(q))  ; break; 
	    case token::XSIF_ATAN:         q = arg.node()->begin(); result = std::atan(evaluate(q))  ; break; 
	    case token::XSIF_EXP:          q = arg.node()->begin(); result = std::exp(evaluate(q))   ; break; 
	    case token::XSIF_LOG:          q = arg.node()->begin(); result = std::log(evaluate(q))   ; break; 
	    case token::XSIF_LOG10:        q = arg.node()->begin(); result = std::log10(evaluate(q)) ; break; 
	    case token::XSIF_GAUSS:        q = arg.node()->begin(); result = 0.0; break; // deferred evaluation -- gaussian, not implemented
	    case token::XSIF_TGAUSS:       q = arg.node()->begin(); result = 0.0; break; // deferred evaluation -- truncated gaussian - not implemented 
	    case token::XSIF_RANF:         q = arg.node()->begin(); result = 0.0; break; // deferred evaluation -- uniform random - not implemented

	    case token::XSIF_IDENTIFIER:   q = arg.node()->begin(); result = evaluate(q); break;

            default :  std::cerr << "Expression::evaluate( Expression::const_iterator arg ): Unknown operator = " 
                       << arg->value.type().name() << std::endl; 
	 }
    } else if ( is_type<double>(arg->value) ) {  
           result =  boost::any_cast<double>(arg->value);

    } else if ( is_type<int>(arg->value) ) {  
           result =  boost::any_cast<int>(arg->value);

    } else { 
 	 std::cerr << "Error: Expression::evaluate(  Expression::const_iterator arg ): Unknown type = " 
                   << arg->value.type().name() << std::endl; 
    }

     return result;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Expression::iterator  Expression::begin()
{
  return sequential_tree<ExprData>::begin();
}  

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Expression::const_iterator Expression::begin() const
{
  return sequential_tree<ExprData>::begin();
} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
Expression::iterator       Expression::end() 
{ 
  return sequential_tree<ExprData>::end();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Expression::const_iterator Expression::end() const
{ 
  return sequential_tree<ExprData>::end();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Expression::pre_order_iterator        Expression::pre_order_begin()
{ 
  return sequential_tree<ExprData>::pre_order_begin();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Expression::const_pre_order_iterator  Expression::pre_order_begin()  const
{ 
  return sequential_tree<ExprData>::pre_order_begin();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Expression::pre_order_iterator          Expression::pre_order_end()
{ 
  return sequential_tree<ExprData>::pre_order_end();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Expression::const_pre_order_iterator    Expression::pre_order_end()  const
{
  return sequential_tree<ExprData>::pre_order_end();
} 


