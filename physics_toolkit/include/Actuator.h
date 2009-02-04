/*************************************************************************
**************************************************************************
**************************************************************************
******                                    
******  File:      Actuator.h
******                                                                
******  Copyright (c) Fermi Research Alliance LLC
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359.
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

#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <string>
#include <vector>
#include <boost/function.hpp>
#include <boost/tuple/tuple.hpp>

#include <beamline/ElmPtr.h>

class Actuator {

 public: 

  typedef void     result_type;
  typedef double argument_type; 

  typedef  boost::tuple< boost::function<double()>, boost::function<void(double)>, double > actuator_tuple_t; 


  Actuator();
  Actuator( Actuator const& o);
  virtual ~Actuator() = 0; 

  Actuator& operator=( Actuator const& o);
  Actuator& operator=( double const& o);

  operator double(); 

  virtual void operator()( double const& ) = 0;

  actuator_tuple_t &       operator[]( int );
  actuator_tuple_t const&  operator[]( int ) const;
 
  bool   empty() const;
  int     size() const;
  void  resize( int n );
  void   clear();

  void push_back(actuator_tuple_t const& );
 

 protected:

  std::string         name_; 
  double              setting_; 

  typedef  std::vector<actuator_tuple_t>::iterator iterator;

  std::vector<actuator_tuple_t>  actuators_;

};

class AbsActuator: public Actuator {

public:

  AbsActuator();
  AbsActuator( AbsActuator const& o);
 ~AbsActuator();

  operator double(); 

  AbsActuator& operator=( AbsActuator const& o);
  AbsActuator& operator=( double const& o);

  void operator()( double const& );

};
  
class RelActuator: public Actuator {

public:

  RelActuator();
  RelActuator( RelActuator const& o);
 ~RelActuator();

  RelActuator& operator=( RelActuator const& o);
  RelActuator& operator=( double const& o);

  operator double(); 
  void operator()( double const& );

};
  


#endif // ACTUATOR_H
  
