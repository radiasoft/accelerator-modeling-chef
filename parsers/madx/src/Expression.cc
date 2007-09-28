#include <boost/any.hpp>
#include <cmath>
#include <Expression.h>
#include <MadxParser_ypp.hh>

std::ostream&  operator<<( std::ostream& os, Expression const& exp) {
  
#if 0
  Expression::const_pre_order_iterator it; 

  for ( it  = exp.pre_order_begin(); it != exp.pre_order_end();  ++it)
  {
      os <<  *it->get()  << std::endl; 

  }

#endif

  os << exp.evaluate(); 

  return os;
}

  

double 
Expression::evaluate( ) const                         
{ 
  return evaluate(  this->begin() ); 
}


double 
Expression::evaluate( const_expr_iterator_t  arg ) const  
{

    double result = 0;
 
    const_expr_iterator_t q;

    if ( is_Token(arg->get()->value) ) {  

	 switch ( int tk = boost::any_cast<Token>(arg->get()->value) ) {
  	    case '+':               q = arg->begin(); result = evaluate(q) + evaluate (++q); break;
	    case '-':               q = arg->begin(); result = evaluate(q) - evaluate (++q); break;
	    case '*':               q = arg->begin(); result = evaluate(q) * evaluate (++q); break;
	    case '/':               q = arg->begin(); result = evaluate(q) / evaluate (++q); break;
	    case '^':               q = arg->begin(); result = std::pow(evaluate(q), evaluate (++q)); break;
  	    case NEG:               q = arg->begin(); result = -evaluate(q); break;    
  	    case MADX_SQRT:         q = arg->begin(); result = std::sqrt(evaluate(q)); break;    
	    case MADX_SIN:          q = arg->begin(); result = std::sin(evaluate(q)); break; 
	    case MADX_COS:          q = arg->begin(); result = std::cos(evaluate(q)); break; 
	    case MADX_TAN:          q = arg->begin(); result = std::tan(evaluate(q)); break; 
	    case MADX_ASIN:         q = arg->begin(); result = std::asin(evaluate(q)); break; 
	    case MADX_ACOS:         q = arg->begin(); result = std::acos(evaluate(q)); break; 
	    case MADX_ATAN:         q = arg->begin(); result = std::atan(evaluate(q)); break; 
	    case MADX_EXP:          q = arg->begin(); result = std::exp(evaluate(q)); break; 
	    case MADX_LOG:          q = arg->begin(); result = std::log(evaluate(q)); break; 
	    case MADX_LOG10:        q = arg->begin(); result = std::log10(evaluate(q)); break; 
	    case MADX_GAUSS:        q = arg->begin(); result = 0.0; break; // gaussian, not implemented
	    case MADX_TGAUSS:       q = arg->begin(); result = 0.0; break; //truncated gaussain - no implemented 
	    case MADX_RANF:         q = arg->begin(); result = 0.0; break; // uniform random - not implemented

	    case MADX_IDENTIFIER:   q = arg->begin(); result = evaluate(q); break;

            default :  std::cout << "Expression::evaluate( expr_iterator_t  arg ): Unknown operator = " 
                       << arg->get()->value.type().name() << std::endl; 
	 }
    } else if ( is_double(arg->get()->value) ) {  
           result =  boost::any_cast<double>(arg->get()->value);
    } else if ( is_int(arg->get()->value) ) {  
           result =  boost::any_cast<int>(arg->get()->value);
    } else { 
 	 std::cout << "Error: Expression::evaluate( expr_iterator_t  arg ): Unknown type = " 
                   << arg->get()->value.type().name() << std::endl; 
    }

     return result;
}


std::ostream& operator<<(std::ostream &os, const ExprData & expr) {

  if (is_double (expr.value) )
    std::cout <<  boost::any_cast<double>(expr.value) << std::endl;
  else if (is_char (expr.value) )
    std::cout <<  (int) boost::any_cast<Token>(expr.value) << std::endl;   
  else 
    std::cout << "Unknown type." << std::endl;
    
  return os;
}


bool is_double(const boost::any & operand)
{
  return operand.type() == typeid(double);
}

bool is_char(const boost::any & operand)
{
  return operand.type() == typeid(char);
}

bool is_int(const boost::any & operand)
{
  return operand.type() == typeid(int);
}


bool is_string(const boost::any & operand)
{
    try
      {
	boost::any_cast<std::string>(operand);
        return true;
      }
    catch(const boost::bad_any_cast &)
      {
        return false;
      }
}

bool is_Token(const boost::any & operand)
{
    try
      {
	boost::any_cast<Token>(operand);
        return true;
      }
    catch(const boost::bad_any_cast &)
      {
        return false;
      }
}
