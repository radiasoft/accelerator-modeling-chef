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
	: m_parent(pit.m_parent), m_iter( pit.m_iter), m_visited( pit.m_visited ) {}


      pre_order_iterator( beamline::iterator const& it)
	: m_parent(), m_iter( it ), m_visited( it.end()) { } 
        
  
     ~pre_order_iterator() { }
 
     private:

      pre_order_iterator( boost::shared_ptr<beamline::pre_order_iterator> const& parent, iterator const& it )                 
	 : m_parent(parent), m_iter( it.m_bml, it.base() ), m_visited( it.end()) {} 


      friend class boost::iterator_core_access;

      boost::shared_ptr<beamline::pre_order_iterator>         m_parent; 
      beamline::iterator                                      m_iter;
      beamline::iterator                                      m_visited;
                                                                            
     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

     void increment()
      {

          if( typeid(**m_iter) == typeid(beamline) )  {  // NOTE: need to dereference twice in order for typeid test to work !
 
            beamline* bml      = static_cast<beamline*>(*m_iter);

            m_visited = m_iter;  
            pre_order_iterator down( boost::shared_ptr<beamline::pre_order_iterator>( new pre_order_iterator(*this) ),  bml->begin() ); 

            std::swap(*this, down); 
  
            return;
             
          }

          ++m_iter; 

          while (  (m_iter == m_iter.end()) &&  (m_parent) )  {  

            pre_order_iterator up( *m_parent ); 
            
            ++up.m_iter; 

            std::swap(*this, up); 
 
          }

	  return; 

      } // void increment() 

      //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

      void decrement()
      {

	if ( m_iter == m_iter.end() ) {
 	  --m_iter;
	  while (typeid(**m_iter) == typeid(beamline)) { 
            beamline* bml      = static_cast<beamline*>(*m_iter);
            pre_order_iterator down( boost::shared_ptr<beamline::pre_order_iterator>( new pre_order_iterator(*this) ), 
                                     --(bml->end()) ); 
            std::swap(*this, down); 
          }
          return;
        }

	if ( (m_iter == m_iter.begin()) && (m_parent) ) {
 
	  if ( (typeid(**m_iter) == typeid(beamline)) && (m_iter != m_visited) ) {
 
             beamline* bml      = static_cast<beamline*>(*m_iter);
             pre_order_iterator down( boost::shared_ptr<beamline::pre_order_iterator>( new pre_order_iterator(*this) ), 
                                     --(bml->end()) ); 
             std::swap(*this, down); 
             return;
          }
          
          pre_order_iterator up( *m_parent ); 
          std::swap(*this, up); 
          m_visited = m_iter;            

          return;

	 }


	--m_iter;

	 while ( (typeid(**m_iter) == typeid(beamline)) && (m_iter != m_visited) ) {
           beamline* bml      = static_cast<beamline*>(*m_iter);
           pre_order_iterator down( boost::shared_ptr<beamline::pre_order_iterator>( new pre_order_iterator(*this) ), 
                                     --(bml->end()) ); 
           std::swap(*this, down); 
         }

         return; 
             
     }// void decrement() 


     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

      bool equal( beamline::pre_order_iterator const& other) const {
         
	if (  (m_iter == m_iter.end() ) || ( other.m_iter == other.m_iter.end() ) ) {
            if ( m_iter   ==  other.m_iter ) return true; 
            else                             return false;
        }


	if ( *m_iter      !=  *(other.m_iter) )     return false;

        if ( !m_parent    &&   other.m_parent   )   return false;
        if (  m_parent    &&  (!other.m_parent) )   return false;
        if ( !m_parent    &&  (!other.m_parent) )   return true;
        if ( **m_parent   !=  **other.m_parent  )   return false;

        return true;
      }

     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


     bmlnElmnt*& dereference() const { 
       return  *m_iter;
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

       post_order_iterator() {}
   
       post_order_iterator( post_order_iterator const& pit )                 
	 :  m_parent(pit.m_parent), m_iter(pit.m_iter), m_visited(pit.m_visited) {}
      

       post_order_iterator( beamline::iterator const& it)
        :  m_parent(), m_iter(it), m_visited(it.end()) 
       { 

         if ( it == it.end() ) return;

	 while ( (typeid(**m_iter) == typeid(beamline)) && ( m_iter != m_visited) )  {  
 
              beamline* bml      = static_cast<beamline*>(*m_iter);
              post_order_iterator 
                  down( boost::shared_ptr<beamline::post_order_iterator>( new post_order_iterator(*this) ),  bml->begin() ); 
              std::swap(*this, down); 
  
         }
         return; 
       }

     private:

      post_order_iterator( boost::shared_ptr<beamline::post_order_iterator> const& parent, iterator const& it )                 
	 : m_parent(parent), m_iter( it.m_bml, it.base() ), m_visited( it.end()) {} 


      boost::shared_ptr<beamline::post_order_iterator>     m_parent;       
      beamline::iterator                                   m_iter;
      beamline::iterator                                   m_visited;


      friend class boost::iterator_core_access;

     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

       
     void increment()
      {

          ++m_iter; 

          if (  (m_iter == m_iter.end()) &&  (!m_parent) )   return;   

          if (  (m_iter == m_iter.end()) &&  ( m_parent) )  {  

            post_order_iterator up( *m_parent ); 
            std::swap(*this, up); 
            m_visited = m_iter; 
            return; 
          }

	 if ( (typeid(**m_iter) == typeid(beamline)) && ( m_iter != m_visited) ) { 
	   while ( (typeid(**m_iter) == typeid(beamline)) && ( m_iter != m_visited) )  {  
 
              beamline* bml      = static_cast<beamline*>(*m_iter);
              m_visited = m_iter;  
              post_order_iterator 
                  down( boost::shared_ptr<beamline::post_order_iterator>( new post_order_iterator(*this) ),  bml->begin() ); 
              std::swap(*this, down); 
  
           }
           return; 
         }


	  return; 

      } // void increment() 


     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

      void decrement()
      {

	if ( m_iter == m_iter.end() ) { 
          --m_iter;
          return;
        }
	

        if( (typeid(**m_iter)==typeid(beamline))  && (m_iter != m_visited) )  {
           beamline* bml      = static_cast<beamline*>(*m_iter);
           post_order_iterator down( boost::shared_ptr<beamline::post_order_iterator>( new post_order_iterator(*this) ), 
                                     --(bml->end()) ); 
           std::swap(*this, down); 
           return;
        } 

        while ( (m_iter == m_iter.begin()) && (m_parent) ) {
          
          if ( (typeid(**m_iter)==typeid(beamline))  && (m_iter != m_visited) )  {
              beamline* bml      = static_cast<beamline*>(*m_iter);
              post_order_iterator down( boost::shared_ptr<beamline::post_order_iterator>( new post_order_iterator(*this) ), 
                                     --(bml->end()) ); 
              std::swap(*this, down); 
              return; 
          }

          post_order_iterator up( *m_parent ); 
          std::swap(*this, up); 
          m_visited = m_iter; 

        }

	--m_iter;

	if ( (typeid(**m_iter) == typeid(beamline)) && (m_iter != m_visited) ) {
           beamline* bml      = static_cast<beamline*>(*m_iter);
           post_order_iterator down( boost::shared_ptr<beamline::post_order_iterator>( new post_order_iterator(*this) ), 
                                     --(bml->end()) ); 
           std::swap(*this, down); 
           return;
        }

        while ( (typeid(**m_iter)==typeid(beamline))  && (m_iter == m_visited) ) --m_iter;

        return; 
             
      }// void decrement() 

     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

      bool equal( beamline::post_order_iterator const& other) const {
         
	if (  (m_iter == m_iter.end() ) || ( other.m_iter == other.m_iter.end() ) ) {
            if ( m_iter   ==  other.m_iter ) return true; 
            else                             return false;
        }


	if ( *m_iter      !=  *(other.m_iter) )     return false;

        if ( !m_parent    &&   other.m_parent   )   return false;
        if (  m_parent    &&  (!other.m_parent) )   return false;
        if ( !m_parent    &&  (!other.m_parent) )   return true;
        if ( **m_parent   !=  **other.m_parent  )   return false;

        return true;
      }

     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


     bmlnElmnt*& dereference() const { 
        return  *m_iter;
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

       deep_iterator( pre_order_iterator const& pit) 
              : deep_iterator::iterator_adaptor_(pit) 
        {
	  if (  pit.m_iter  == pit.m_iter.end() )      return; 
	  if ( typeid(***this) ==  typeid(beamline)  ) increment(); 
        }



    private:

       friend class boost::iterator_core_access;


       void increment() {

	 base_reference().increment();  

         if ( base().m_iter  == base().m_iter.end() ) {
	    return;
         }

         while ( typeid(***this) == typeid(beamline) ) { 
            increment(); // skip beamlines 
         }
       }   

       void decrement() {

	 base_reference().decrement();  

         if ( base().m_iter == base().m_iter.end() ) {
	    return;
         }

         while ( typeid(***this) == typeid(beamline) ) {
	   if (base().m_iter == base().m_iter.begin()) break;  
            decrement();  
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

