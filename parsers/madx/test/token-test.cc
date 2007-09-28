#include <boost/any.hpp>
#include <iostream>

class token {

public:

  token(int t): m_token(t) {}

private:
 
  int m_token;

};


bool is_token(const boost::any & operand)
{
    try
      {
	boost::any_cast<token>(operand);
        return true;
      }
    catch(const boost::bad_any_cast &)
      {
        return false;
      }
}



int main(int argc, const char* argv) {


  token t1(155);
  token t2('a');

  boost::any v1 = t1;
  boost::any v2 = t2;
  boost::any v3 = 100;
  boost::any v4 = 'b';

  std::cout << "v1 is token = " <<   is_token(v1) << std::endl;
  std::cout << "v2 is token = " <<   is_token(v2) << std::endl;
  std::cout << "v3 is token = " <<   is_token(v3) << std::endl;
  std::cout << "v4 is token = " <<   is_token(v3) << std::endl;


}
