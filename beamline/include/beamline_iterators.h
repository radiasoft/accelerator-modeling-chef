/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      beamline_iterators.h
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
******  Author: Jean-Francois Ostiguy  
******          ostiguyfnal.gov
******
******                                                             
**************************************************************************
*************************************************************************/

// **** NOTE: this is a private include file meant to be included *only* from beamline.h ...  

#ifndef BEAMLINE_ITERATORS_H
#define BEAMLINE_ITERATORS_H

//---------------------------------------------------------------------------------------------------  
// STL compatible iterators for the beamline class ( author: ostiguy@fnal.gov)
// ----------------------------------------------------------------------------------------------------

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//
// beamline::iterator
//
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    class iterator : public boost::iterator_adaptor <  iterator,
                                                       std::list<bmlnElmnt*>::iterator,   // Base
                                                       boost::use_default,                // Value
                                                       boost::bidirectional_traversal_tag > {  
    friend class beamline::pre_order_iterator;
    friend class beamline::post_order_iterator;
    friend class beamline::deep_iterator;

    public:
 
       iterator() 
              : iterator::iterator_adaptor_(), m_bml(0) {}

       iterator( beamline::iterator const& it )
              : iterator::iterator_adaptor_(it), m_bml(it.m_bml) { } 

       explicit iterator( beamline* bml, std::list<bmlnElmnt*>::iterator const& it )
              :  iterator::iterator_adaptor_(it), m_bml(bml) { } 
       
       iterator end()   const { return m_bml->end(); } 
       iterator begin() const { return m_bml->begin(); } 

    private:

       friend class boost::iterator_core_access;

       beamline*       m_bml;

 
       bool equal( beamline::iterator const& x) const {
         return ( (m_bml == x.m_bml) && (x.base() == base()) ); 
       }

    };

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//
// beamline::reverse_iterator
//
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    class reverse_iterator : public boost::reverse_iterator<beamline::iterator>  {

    public: 

      friend class boost::iterator_core_access;

      reverse_iterator()                                                       
                   : boost::reverse_iterator<beamline::iterator>()    {}

      reverse_iterator( beamline::iterator         const&  it )
                   : boost::reverse_iterator<beamline::iterator>(it) {} 

      reverse_iterator( beamline::reverse_iterator const& rit )               
                   : boost::reverse_iterator<beamline::iterator>(rit) {} 

    };
   

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//
// beamline::pre_order_iterator
//
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

  
 
   class pre_order_iterator : public boost::iterator_facade <pre_order_iterator,           // Derived
                                                         bmlnElmnt*,                       // Value
                                                         boost::forward_traversal_tag,     // class CategoryOrTraversal
                                                         bmlnElmnt*&,                      // Reference  
                                                         ptrdiff_t                         // Difference // ???? 
                                                       >   
   {

     friend class deep_iterator;

     public:

       pre_order_iterator() {}
   
       pre_order_iterator( pre_order_iterator const& pit )                 
	 : m_recursed_into(pit.m_recursed_into) { 

         if ( pit.m_iter_ptr) m_iter_ptr = boost::shared_ptr<beamline::iterator>( new iterator(*(pit.m_iter_ptr)) ); 

	 if ( pit.m_subiter_ptr ) {
           m_subiter_ptr = boost::shared_ptr<beamline::pre_order_iterator>( new pre_order_iterator(*(pit.m_subiter_ptr)) ); 
         }
       } 

       pre_order_iterator( beamline::iterator const& it)
        :  m_recursed_into(false) { 
           m_iter_ptr = boost::shared_ptr<beamline::iterator>( new iterator(it) );
       }

     private:

      friend class boost::iterator_core_access;

      boost::shared_ptr<beamline::iterator>                m_iter_ptr;
      boost::shared_ptr<beamline::pre_order_iterator>      m_subiter_ptr;           // NOTE: incomplete type mandatory 
                                                                                    // due to recursive definition ! 
      bool                                                 m_recursed_into;     // true if the child has already been
                                                                                // visited (recursed into) 
                                                                            
     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

      void increment()
      {

	if ( m_subiter_ptr ) {
      
          ++(*m_subiter_ptr); 

          if(  *m_subiter_ptr->m_iter_ptr != m_subiter_ptr->m_iter_ptr->end() ) return;
         
          // end of sub (child) beamline reached 

          m_subiter_ptr = boost::shared_ptr<beamline::pre_order_iterator>(); // null ptr
  	  ++(*m_iter_ptr); // increment the parent 
	  return; 
	}	 
              
        
        if( typeid(***m_iter_ptr) == typeid(beamline) )  { // NOTE: need to dereference twice in order for typeid test to work !
	    beamline* bml = static_cast<beamline*>(**m_iter_ptr);

            m_subiter_ptr    
               = boost::shared_ptr<beamline::pre_order_iterator>( new pre_order_iterator( bml->begin() ));
	    m_recursed_into = true;

        } else {

              ++(*m_iter_ptr); 
        }
     	 

      }// void increment() 

      //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

      void decrement()
      {


	if ( m_subiter_ptr ) {
         
         if ( (*m_subiter_ptr->m_iter_ptr == (m_subiter_ptr->m_iter_ptr->begin())  &&   
	      ( !m_subiter_ptr->m_subiter_ptr ) ) ) { 

	   std::cout << "begin reached ... " << std::endl;

	      // is the first element of the child beamline itself a beamline ? 
              // If so, decrement the current beamline iterator only if the child bml has 
              // already been recursed into ... 
              
              if (   ( typeid(***(m_subiter_ptr->m_iter_ptr)) == typeid(beamline) ) &&
		     ( !m_subiter_ptr->m_recursed_into ) ){

			 --(*m_subiter_ptr); 
                         return;  
	      }  

              m_subiter_ptr = boost::shared_ptr<beamline::pre_order_iterator>(); // null ptr

              if (*m_iter_ptr != m_iter_ptr->begin()) { 
                --(*m_iter_ptr);  
              } else { 
		m_iter_ptr = boost::shared_ptr<beamline::iterator>(); // null ptr
             --(*m_iter_ptr); // increment the parent 

              }
              return;
               

	 }  else  {
             --(*m_subiter_ptr);
         }         

 	return; 

       }
       //--------------------------------------------------------------------------------------------------
       // NOTE: unfortunately, the iterators need to know about their own end() because of the 
       //       need to detect end() here ! typeid(beamline) in the test below throws an exception 
       //       when *m_iter is not a valid bmlElmnt* ...      
       //---------------------------------------------------------------------------------------------------



      if(  *m_iter_ptr == m_iter_ptr->end() ) { 
         --(*m_iter_ptr); 
         return;
       }
 
       if(  typeid( ***m_iter_ptr) == typeid(beamline) )  { 
            beamline* bml  = static_cast<beamline*>(**m_iter_ptr); 
            m_subiter_ptr    
	      = boost::shared_ptr<beamline::pre_order_iterator>(new pre_order_iterator( --(bml->end()) ) );
	    m_recursed_into = true; 
    
       } else {
	    --(*m_iter_ptr); 
       }
 
     }// void decrement() 

     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

      bool equal( beamline::pre_order_iterator const& other) const {
         
        if ( m_subiter_ptr &&  !other.m_subiter_ptr) return false; 

        if ( m_subiter_ptr && other.m_subiter_ptr) 
           return ( ( *m_iter_ptr == *other.m_iter_ptr ) && (  (*m_subiter_ptr) == (*other.m_subiter_ptr)  ) );  

        return ( (*m_iter_ptr == *other.m_iter_ptr) ); 
      }

     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


     bmlnElmnt*& dereference() const { 
       return  ( m_subiter_ptr ) ?  *(*m_subiter_ptr) : **m_iter_ptr;
     }
    
 };

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// 
// beamline::reverse_pre_order_iterator
// 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    class reverse_pre_order_iterator : public boost::reverse_iterator<beamline::pre_order_iterator>  {

    public: 

      friend class boost::iterator_core_access;

      reverse_pre_order_iterator()
                 : boost::reverse_iterator<beamline::pre_order_iterator>()    {}


      reverse_pre_order_iterator( beamline::reverse_pre_order_iterator const& rit )
                 : boost::reverse_iterator<beamline::pre_order_iterator>(rit) {} 

      reverse_pre_order_iterator( beamline::pre_order_iterator const& it )      
                                                                                
                 : boost::reverse_iterator<beamline::pre_order_iterator>(it) {} 


    };
   
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// 
// beamline::post_order_iterator
// 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

   class post_order_iterator : public boost::iterator_facade <post_order_iterator,          // Derived
                                                         bmlnElmnt*,                       // Value
                                                         boost::forward_traversal_tag,     // class CategoryOrTraversal
                                                         bmlnElmnt*&,                      // Reference  
                                                         ptrdiff_t                         // Difference // ???? 
                                                       >   
   {
     public:

       post_order_iterator(): m_sub_end_reached(false) {}
   
       post_order_iterator( post_order_iterator const& pit )                 
	 :  m_sub_end_reached(pit.m_sub_end_reached), m_iter(pit.m_iter) { 
	 if ( pit.m_subiter_ptr ) {
           m_subiter_ptr = boost::shared_ptr<beamline::post_order_iterator>( new post_order_iterator(*(pit.m_subiter_ptr)) ); 
         }
       } 

       post_order_iterator( beamline::iterator const& it)
        :  m_sub_end_reached(false), m_iter(it) { }

     private:

      bool                                                 m_sub_end_reached;  
      beamline::iterator                                   m_iter;
      boost::shared_ptr<beamline::post_order_iterator>     m_subiter_ptr;       // NOTE: incomplete type mandatory due to recursive definition ! 

      friend class boost::iterator_core_access;

     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

       
      void increment()
      {


	if ( m_subiter_ptr ) {
      
          ++(*m_subiter_ptr); 

          if( m_subiter_ptr->m_iter != m_subiter_ptr->m_iter.end() ) return;
 
          m_subiter_ptr      = boost::shared_ptr<beamline::post_order_iterator>(); // null ptr
          m_sub_end_reached = true; 
          ++m_iter;
          return;

       } 
              

        if (m_sub_end_reached) { m_sub_end_reached = false; ++m_iter; return; }
 
        beamline::iterator next = m_iter; ++next;
        if( next == next.end() ) {
          ++m_iter;  
	  return;
        }
 

        if( typeid(**next) == typeid(beamline) )  { // NOTE: need to dereference twice in order for typeid test to work !
	    beamline* bml = static_cast<beamline*>(*next);
            m_subiter_ptr    
               = boost::shared_ptr<beamline::post_order_iterator>( new post_order_iterator( bml->begin() ));
            return;
        } 

        ++m_iter; 
       
     	 

      }// void increment() 

     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

      void decrement()
      {


	if ( m_subiter_ptr ) {
        
          if( (m_subiter_ptr->m_iter ==  (m_subiter_ptr->m_iter).begin())  &&  !(m_subiter_ptr->m_subiter_ptr) ) { 
              m_subiter_ptr = boost::shared_ptr<beamline::post_order_iterator>(); // null ptr
              m_sub_end_reached = true;  
	  }  else  {
             --(*m_subiter_ptr);
          }         

	return; 

	}

       //--------------------------------------------------------------------------------------------------
       // NOTE: unfortunately, the iterators need to know about their own end() because of the 
       //       need to detect end() here ! typeid(beamline) in the test below throws an exception 
       //       when *m_iter is not a valid bmlElmnt* ...      
       //---------------------------------------------------------------------------------------------------

      if(  m_sub_end_reached) { m_sub_end_reached = false; --m_iter; return;}
 
      if(  m_iter == m_iter.end() ) { 
         --m_iter; 
         return;
       }
 
       beamline::iterator next = m_iter; --next;

       if(  typeid( **next) == typeid(beamline) )  { 
            beamline* bml  = static_cast<beamline*>(*next); 
            m_subiter_ptr    
	      = boost::shared_ptr<beamline::post_order_iterator>(new post_order_iterator( --(bml->end()) ));
            
       }
   
      --m_iter; 
   
 
      }// void decrement() 

     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

      bool equal( beamline::post_order_iterator const& other) const {

        if ( m_subiter_ptr && !other.m_subiter_ptr ) return false;   

        if ( m_subiter_ptr ) 
           return ( ( m_iter == other.m_iter ) && (  (*m_subiter_ptr) == (*other.m_subiter_ptr)  ) );  
        return ( (m_iter == other.m_iter) ); 
      }

     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


     bmlnElmnt*& dereference() const { 
        return  ( m_subiter_ptr ) ?  *(*m_subiter_ptr) : *m_iter;
       }
    
     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



   };

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// 
// beamline::reverse_post_order_iterator
// 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    class reverse_post_order_iterator : public boost::reverse_iterator<beamline::post_order_iterator>  {

    public: 

      friend class boost::iterator_core_access;

      reverse_post_order_iterator()
                 : boost::reverse_iterator<beamline::post_order_iterator>()    {}


      reverse_post_order_iterator( beamline::reverse_post_order_iterator const& rit )
                 : boost::reverse_iterator<beamline::post_order_iterator>(rit) {} 

      reverse_post_order_iterator( beamline::post_order_iterator const& it )      
                                                                                
                 : boost::reverse_iterator<beamline::post_order_iterator>(it) {} 


    };
   
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// 
// beamline::deep_iterator
// 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    class deep_iterator : public boost::iterator_adaptor <  deep_iterator,
                                                            beamline::pre_order_iterator,      // Base
                                                            boost::use_default,                // Value
                                                            boost::bidirectional_traversal_tag > {  
    public:
 
       deep_iterator() 
              : deep_iterator::iterator_adaptor_() {}

       deep_iterator( deep_iterator const& dit) 
              : deep_iterator::iterator_adaptor_(dit) {}

       deep_iterator( pre_order_iterator const& dit) 
              : deep_iterator::iterator_adaptor_(dit) {}


    private:

       friend class boost::iterator_core_access;


       void increment() {
        
	 base_reference().increment();  

         if ( *(base().m_iter_ptr) == base().m_iter_ptr->end() ) {
	    return;
         }

         if ( typeid(***this) == typeid(beamline) ) 
            increment(); // skip beamlines 
         }

       void decrement() {
        
	 base_reference().decrement();  

         if ( *base().m_iter_ptr == base().m_iter_ptr->end() ) {
	    std::cerr << " decrement reached end of beamline ! " << std::endl;
	    return;
         }

         if ( typeid(***this) == typeid(beamline) ) {
	    std::cerr << " found beamline: " << (**this)->Name() << std::endl; 
            decrement(); // skip beamlines 
         }
       }

    };


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// 
// beamline::reverse_deep_iterator
// 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    class reverse_deep_iterator : public boost::reverse_iterator<beamline::deep_iterator>  {

    public: 

      friend class boost::iterator_core_access;

      reverse_deep_iterator()
                 : boost::reverse_iterator<beamline::deep_iterator>()    {}


      reverse_deep_iterator( beamline::reverse_deep_iterator const& rit )
                 : boost::reverse_iterator<beamline::deep_iterator>(rit) {} 

      reverse_deep_iterator( beamline::deep_iterator const& it )      
                                                                                
                 : boost::reverse_iterator<beamline::deep_iterator>(it) {} 


    };

#endif // BEAMLINE_ITERATORS_H

