#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>
#include <BeamlineIterator.h>
#include <bmlnElmnt.h>

using namespace boost::python;

static object pass_through(object const& o) { return o; }


static bmlnElmnt* next(DeepBeamlineIterator& o) 
    {

        bmlnElmnt* result = o++;
        if (!result) { 
             //PyErr_SetString(PyExc_StopIteration, "No more data."); 
             //boost::python::throw_error_already_set();
          return 0;
          
        }
     
        return result;
    }



bmlnElmnt* (DeepBeamlineIterator::* local_iterator)(int) = &DeepBeamlineIterator::operator++;

void wrap_beamlineiterator() {


 class_<DeepBeamlineIterator>("DeepBeamlineIterator", init<beamline&>()) 
        .def("next",      &next, return_value_policy<reference_existing_object>())
        .def("__iter__",  &pass_through )
        .def("reset",     &DeepBeamlineIterator::reset);

}



#if 0

class BeamlineIterator
{
public:
  BeamlineIterator( const beamline& );
  BeamlineIterator( const beamline* );
  BeamlineIterator( const BeamlineIterator& );
  ~BeamlineIterator();

  bmlnElmnt* operator++( int );    // postfix increment
  void reset();
  void goBack( int = 1 );

private:
  dlist_iterator* _getNext;
};


class DeepBeamlineIterator 
{
public:
  DeepBeamlineIterator( const beamline& );
  DeepBeamlineIterator( const beamline* );
  DeepBeamlineIterator( const DeepBeamlineIterator& );
  ~DeepBeamlineIterator();

  bmlnElmnt* operator++( int );    // postfix increment
  void reset();

private:
  dlist_iterator*       _getNext;
  DeepBeamlineIterator* _subIterator;
};


class ReverseBeamlineIterator
{
public:
  ReverseBeamlineIterator( const beamline& );
  ReverseBeamlineIterator( const beamline* );
  ReverseBeamlineIterator( const ReverseBeamlineIterator& );
  ~ReverseBeamlineIterator();

  bmlnElmnt* operator++( int );    // postfix increment
  void reset();
  void goBack( int = 1 );

private:
  dlist_reverseIterator* _getNext;
};


class DeepReverseBeamlineIterator
{
public:
  DeepReverseBeamlineIterator( const beamline& );
  DeepReverseBeamlineIterator( const beamline* );
  DeepReverseBeamlineIterator( const DeepReverseBeamlineIterator& );
  ~DeepReverseBeamlineIterator();

  bmlnElmnt* operator++( int );    // postfix increment
  void reset();

private:
  dlist_reverseIterator*       _getNext;
  DeepReverseBeamlineIterator* _subIterator;
};


#endif 

