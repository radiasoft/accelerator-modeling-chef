
#include <iostream>
#include <string>
#include <multitree.h>
#include <boost/any.hpp>

using boost::any_cast; 

class ExprData;
std::ostream& operator<<(std::ostream &os, const ExprData & expr);

class ExprData {

  friend std::ostream& operator<<(std::ostream &os, const ExprData & expr);

public:

  ExprData():order(++counter){}
  ExprData(boost::any v): value(v), order(++counter){}

  boost::any      value;
  int             order;
  static int      counter;

  bool operator<(const ExprData& rhs) const { return (this->counter < rhs.counter); }   
};

int ExprData::counter = 0;


bool is_double(const boost::any & operand)
{
    return operand.type() == typeid(double);
}

bool is_char(const boost::any & operand)
{
    return operand.type() == typeid(char);
}


bool is_string(const boost::any & operand)
{
    try
    {
        any_cast<std::string>(operand);
        return true;
    }
    catch(const boost::bad_any_cast &)
    {
        return false;
    }
 
}


double evaluate( multitree<ExprData>::iterator  ndata_it) {
  static int level = 0; 
  double result = 0; 

  std::cout << "Inside evaluate. recursion level = " << ++level << std::endl;

    for ( multitree<ExprData>::iterator it = ndata_it->begin();  it !=  ndata_it->end();  it++)
      if ( is_char(it->get()->value) )  result += evaluate( it );
      else {    
         std::cout <<  boost::any_cast<double>(it->get()->value) << std::endl;       
         result +=  boost::any_cast<double>(it->get()->value);
    }

    return result;

} 




int main(int argc, const char* argv[] ) {

 
  std::cout << " Begin expression tree demo ..." << std::endl;  


  // template<typename stored_type, typename node_compare_type >
  // class multitree;


  multitree<ExprData> expression; 
  
  ExprData node1('+');
  multitree<ExprData>::iterator parent_it = expression.insert(node1);  

  parent_it->insert( ExprData(1.23456) );
  parent_it = parent_it->insert( ExprData('+'));
  parent_it->insert(ExprData(12.3456));
  parent_it->insert(ExprData(12.3456));

  std::cout << "result = " << evaluate( expression.begin() ) << std::endl;

#if  0

  for ( multitree<ExprData>::iterator it = expression.begin(); it != expression.end(); it++)
  {
      if( is_char(it->get()->value) ){
        std::cout <<  boost::any_cast<char>(it->get()->value) << std::endl; 
      }
      else if ( is_double(it->get()->value ) ) {
        std::cout <<  boost::any_cast<double>(it->get()->value) << std::endl; 
      } 

  }
#endif

  std::cout << " Printing tree in pre_order " << std::endl; 
  std::cout << " ---------------------------" << std::endl; 

  for ( multitree<ExprData>::pre_order_iterator it = expression.pre_order_begin(); it != expression.pre_order_end(); it++)
  {
      if( is_char(it->get()->value) ){
        std::cout <<  boost::any_cast<char>(it->get()->value) << std::endl; 
      }
      else if ( is_double(it->get()->value ) ) {
        std::cout <<  boost::any_cast<double>(it->get()->value) << std::endl; 
      } 

  }

  std::cout << " Printing tree in pre_order " << std::endl; 
  std::cout << " ---------------------------" << std::endl; 

  for ( multitree<ExprData>::pre_order_iterator it = expression.pre_order_begin(); it != expression.pre_order_end(); it++)
  {
      if( is_char(it->get()->value) ){
        std::cout <<  boost::any_cast<char>(it->get()->value) << std::endl; 
      }
      else if ( is_double(it->get()->value ) ) {
        std::cout <<  boost::any_cast<double>(it->get()->value) << std::endl; 
      } 

  }

  
 #if 0 
 multitree<ExprData> expression_copy(expression);

  std::cout << " Printing copied tree in pre_order " << std::endl; 
  std::cout << " Printing copied tree in pre_order " << std::endl; 
  std::cout << " Printing copied tree in pre_order " << std::endl;  
  std::cout << " ----------------------------------" << std::endl; 

  for ( multitree<ExprData>::pre_order_iterator it = expression_copy.pre_order_begin(); it != expression_copy.pre_order_end(); it++)
  {
      if( is_char(it->get()->value) ){
        std::cout <<  boost::any_cast<char>(it->get()->value) << std::endl; 
      }
      else if ( is_double(it->get()->value ) ) {
        std::cout <<  boost::any_cast<double>(it->get()->value) << std::endl; 
      } 

  }
#endif

  std::cout << " --------------------------------------------------------------------------------------" << std::endl; 

  

  std::cout << "1: " << expression.begin()->get()->value <<  std::endl;   

  std::cout << "2: " << expression.begin()->begin()->get()->value << std::endl;   
 
  std::cout << "3: " << ( ++(expression.begin()->begin()) )->get()->value << std::endl;   

  std::cout << "4: " << ( ++(expression.begin()->begin()) )->begin()->get()->value << std::endl;   

  
}




std::ostream& operator<<(std::ostream &os, const ExprData & expr) {

  if (is_double (expr.value) )
    std::cout <<  boost::any_cast<double>(expr.value) << std::endl;
  else if (is_char (expr.value) )
    std::cout <<  boost::any_cast<char>(expr.value) << std::endl;

  return os;
}



