#include "Interval.h"
#include <stdlib.h>

const Interval Interval::inv() const {
  if( top >= 0 && bot <= 0 ) {
    cerr<<"Error: division of Interval containing zero"<<endl;
    exit(1);
  }
  return Interval(1/top, 1/bot);
}
const Interval Interval::operator+(const Interval& v) const {
  return Interval(bot+v.bot, top+v.top);
}
const Interval Interval::operator-(const Interval& v) const {
  return Interval(bot-v.top, top-v.bot);
}
const Interval Interval::operator/(const Interval& v) const {
  return operator*(v.inv());
}
const Interval Interval::operator*(const Interval& v) const {
  return Interval(min(min(bot*v.bot, bot*v.top),min(top*v.bot, top*v.top)),
		  max(max(bot*v.bot, bot*v.top),max(top*v.bot, top*v.top)));
}
const Interval operator+(const double d, const Interval& v) {
  return (Interval)d+v;
}
const Interval operator-(const double d, const Interval& v) {
  return (Interval)d-v;
}
const Interval operator/(const double d, const Interval& v) {
  return (Interval)d/v;
}
const Interval operator*(const double d, const Interval& v) {
  return (Interval)d*v;
}
const Interval Interval::operator-() const {
  return Interval(-top, -bot);
}
ostream& operator<<(ostream& os, const Interval& v) {
  return os<<"["<<v.bot<<", "<<v.top<<"]";
}
const double d(const Interval& a, const Interval& b) {
  return Interval::max(fabs(a.bot-b.bot), fabs(a.top-b.top));
}
const Interval Interval::intpow(int n) const 
{
  if( n<=0 ) {
    cerr<<"Error: Interval intpow called with nonpositive value"<<endl;
    exit(1);
  }
  if( bot>0 || (n%2==1) )
    return Interval(pow(bot, n), pow(top, n));
  else if(top<0 && (n%2==0))
    return Interval(pow(top, n), pow(bot, n));
  return Interval(0, pow(abs(), n));
}
