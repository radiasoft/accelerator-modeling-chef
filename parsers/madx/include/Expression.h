#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <iostream>
#include <boost/any.hpp>
#include <sequential_tree.h>

bool is_double(const boost::any & operand);
bool is_int(const boost::any & operand);
bool is_char(const boost::any & operand);
bool is_string(const boost::any & operand);
bool is_Token(const boost::any & operand);



class Token {

 public:
  Token(int t) : m_token(t)         {}
  bool operator==(const Token& rhs) { return m_token == rhs.m_token; } 
  operator int ()                   { return m_token; }

 private:

  int m_token;

};


class ExprData;
std::ostream& operator<<(std::ostream &os, const ExprData & expr);


class ExprData {

friend  std::ostream& operator<<(std::ostream &os, const ExprData & expr);

public:

  ExprData() {}
  ExprData(boost::any v): value(v) {}
  ExprData(ExprData const& exp): value(exp.value) {}

  boost::any      value;

};



typedef  sequential_tree<ExprData>::iterator expr_iterator_t;
typedef  sequential_tree<ExprData>::const_iterator const_expr_iterator_t;

class Expression;

std::ostream&  operator<<( std::ostream& os, Expression const& exp);


class Expression: public sequential_tree<ExprData> {

friend std::ostream&  operator<<( std::ostream& os, Expression const& exp);

public:


  Expression(): sequential_tree<ExprData>()            {             }                               
  double evaluate( ) const;

private:
  
  double evaluate( const_expr_iterator_t arg) const;

};


#endif  // EXPRESSION_H
