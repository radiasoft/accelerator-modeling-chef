/*******************************************************************************
********************************************************************************
********************************************************************************
********************************************************************************
******                                                                    ******
******    BEAMLINE:  C++ objects for design and analysis                  ******
******              of beamlines, storage rings, and                      ******
******              synchrotrons.                                         ******
******                                                                    ******
******    File:      beamline_iterators.h                                 ******
******                                                                    ******
******    Copyright (c) Universities Research Association, Inc./Fermilab  ******
******                  All Rights Reserved                               ******
******                                                                    ******
******    Usage, modification, and redistribution are subject to terms    ******
******    of the License supplied with this software.                     ******
******                                                                    ******
******    Software and documentation created under                        ******
******    U.S. Department of Energy Contract No. DE-AC02-76CH03000.       ******
******    The U.S. Government retains a world-wide non-exclusive,         ******
******    royalty-free license to publish or reproduce documentation      ******
******    and software for U.S. Government purposes. This software        ******
******    is protected under the U.S. and Foreign Copyright Laws.         ******
******                                                                    ******
******    Author: Jean-Francois Ostiguy                                   ******
******            ostiguy@fnal.gov                                        ******
******                                                                    ******
******                                                                    ******
******                                                                    ******
********************************************************************************
********************************************************************************
********************************************************************************
********************************************************************************/

// **** NOTE: this is a private include file meant to be included *only* from beamline.h ...  

#ifndef BEAMLINE_ITERATORS_H
#define BEAMLINE_ITERATORS_H

#include <boost/utility.hpp>

//------------------------------------------------------------------------------  
// STL compatible iterators for the beamline class ( author: ostiguy@fnal.gov)
//------------------------------------------------------------------------------
// Note: much of the complexity in the code below results from the fact that both 
//       const and non-const varieties of iterators are dealt with simultaneously.
// 
//        pre_order_iter<> and and post_order_iter<> both have a member called    
//        m_parent that is a (smart pointer) pointer to the parent of the level
//        iterator. This pointer has been declared mutable in order to enable the 
//        use of the swap() algorithm with pre and post_iter objects. This may 
//        not be the optimal type-safe solution ...   
//    
// -----------------------------------------------------------------------------

    template<typename held_type>
    class iter;

    typedef iter<ElmPtr>                                                   iterator;     
    typedef iter <ElmPtr const>                                      const_iterator;     

    template<typename held_type>
    class reverse_iter;

    typedef reverse_iter<ElmPtr>                                   reverse_iterator;     
    typedef reverse_iter<ElmPtr const>                       const_reverse_iterator;     

    template<typename held_type>
    class pre_order_iter;

    typedef pre_order_iter<ElmPtr>                               pre_order_iterator;     
    typedef pre_order_iter<ElmPtr const>                   const_pre_order_iterator;     

    template<typename held_type>
    class reverse_pre_order_iter;

    typedef reverse_pre_order_iter<ElmPtr>               reverse_pre_order_iterator;
    typedef reverse_pre_order_iter<ElmPtr const>   const_reverse_pre_order_iterator;

    template<typename held_type>
    class post_order_iter;

    typedef post_order_iter<ElmPtr>                             post_order_iterator;     
    typedef post_order_iter<ElmPtr const>                 const_post_order_iterator;     


    template<typename held_type>
    class reverse_post_order_iter;

    typedef reverse_post_order_iter<ElmPtr>              reverse_post_order_iterator;
    typedef reverse_post_order_iter<ElmPtr const>  const_reverse_post_order_iterator;

    template<typename held_type>
    class deep_iter;

    typedef deep_iter<ElmPtr>                                          deep_iterator;
    typedef deep_iter<ElmPtr const>                              const_deep_iterator;


    template<typename held_type>
    class reverse_deep_iter;    

    typedef  reverse_deep_iter<ElmPtr>                         reverse_deep_iterator;
    typedef  reverse_deep_iter<ElmPtr const>             const_reverse_deep_iterator;


//-------------------------------------------------------------------------------------------------------------
// Note:  beamline_iter_traits is defined as a nested class with two parameters and only 
//        partially specialized. This is done to work around the fact that the C++ standard
//        does not allow full specialization of nested class templates (but does allow partial specialization). 
//        The integer parameter Dummy is defined (arbitrarily set to 0) in the nested iterator
//        classes where the traits are instantiated.   
//--------------------------------------------------------------------------------------------------------------        

    template<typename U, int Dummy>
    struct beamline_iter_traits{ };

    template<int Dummy>
    struct beamline_iter_traits<ElmPtr, Dummy> {
      typedef  beamline*                                     bmlptr_type;
      typedef  std::list<ElmPtr>::iterator                   list_iter_type;
      typedef  boost::shared_ptr<pre_order_iter<ElmPtr>  >   pre_order_parent_type;
      typedef  boost::shared_ptr<post_order_iter<ElmPtr> >   post_order_parent_type;
    };

    template<int Dummy>
    struct beamline_iter_traits<ElmPtr const, Dummy> {
      typedef  beamline const*                                           bmlptr_type;
      typedef  std::list<ElmPtr>::const_iterator                         list_iter_type;
      typedef  boost::shared_ptr<pre_order_iter<ElmPtr const>  >         pre_order_parent_type;
      typedef  boost::shared_ptr<post_order_iter<ElmPtr const> >         post_order_parent_type;
    };


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//
// beamline::iterator
//
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    template <typename held_type>
    class iter: public boost::iterator_adaptor <  iter<held_type>,
                                                  typename beamline_iter_traits<held_type,0>::list_iter_type,  // Base
                                                  held_type,                                                  // Value
                                                  boost::bidirectional_traversal_tag > {  


    template <typename T> 
    friend class iter;

    template <typename T> 
    friend class reverse_iter;

    template <typename T> 
    friend class pre_order_iter;

    template <typename T> 
    friend class post_order_iter;

    template <typename T> 
    friend class deep_iter;

    private:

       struct enabler {};  // empty private type (to avoid accidental misuse). Used in conjunction with 
                           // boost::enable_if() below.  

    public:
    
      typedef typename beamline_iter_traits<held_type,0>::bmlptr_type bmlptr_type;
 
      iter() 
              : iter::iterator_adaptor_(), m_bml(0) {}

       template <typename OtherType>
	 iter( iter<OtherType> const& other, 
	         typename boost::enable_if<boost::is_convertible<OtherType*, held_type*>, enabler >::type = enabler() ) 
	      : iter::iterator_adaptor_(other.base()), m_bml(other.m_bml) {} 


       explicit iter( bmlptr_type const& bml,  typename beamline_iter_traits<held_type,0>::list_iter_type const& it )
              : iter::iterator_adaptor_(it), m_bml(bml) { } 
       


       iter end()   const { return m_bml->end();       } 
       iter begin() const { return m_bml->begin();     } 


    private:
         
     
       friend class boost::iterator_core_access;

       bmlptr_type   m_bml;

 
       template <typename OtherType>
       bool equal( OtherType const& x) const {
         return ( (m_bml == x.m_bml) && (x.base() == this->base()) ); 
       }

    };


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//
// beamline::reverse_iterator
//
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    template <typename held_type>
    class reverse_iter: public boost::reverse_iterator<iter<held_type> >  {

    private:

       struct enabler {};  // empty private type (to avoid accidental misuse). Used in conjunction with 
                           // boost::enable_if() below.  
    public: 

      friend class boost::iterator_core_access;

      reverse_iter()                                                       
                   : boost::reverse_iterator<iter<held_type> >()    {}

      template <typename OtherType>
      reverse_iter( reverse_iter<OtherType> const& rit, 
                    typename boost::enable_if<boost::is_convertible<OtherType*, held_type*>, enabler >::type = enabler() ) 
                   : boost::reverse_iterator<iter<held_type> >(rit) {} 

      reverse_iter( iter<held_type>         const&  it )
                   : boost::reverse_iterator<iter<held_type> >(it) {} 


      reverse_iter rbegin() const { return this->base_reference().m_bml->rbegin();     } 
      reverse_iter rend()   const { return this->base_reference().m_bml->rend();       } 


    };


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//
// beamline::pre_order_iterator
//
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

   
   template <typename held_type>
   class pre_order_iter: public boost::iterator_facade <pre_order_iter<held_type>,     // Derived
                                                   held_type,                          // Value
                                                   boost::bidirectional_traversal_tag, // class CategoryOrTraversal
                                                   held_type&,                         // Reference  
                                                   ptrdiff_t                           // Difference // ???? 
                                                  >   
   {

     template <typename T>
     friend class pre_order_iter;

     template <typename T>
     friend class deep_iter;

     private:

     struct enabler {};  // empty private type (to avoid accidental misuse). Used in conjunction with 
                           // boost::enable_if() below.  
     public:

    
      pre_order_iter() {}


      template <typename OtherType>
	pre_order_iter(pre_order_iter<OtherType> const& pit,
          typename boost::enable_if<boost::is_convertible<OtherType*, held_type*>, enabler >::type = enabler() ) 
 	: m_parent(pit.m_parent), m_iter( pit.m_iter), m_visited( pit.m_visited ) {}


      pre_order_iter( iter<held_type> const& it )
	: m_parent(), m_iter( it ), m_visited( iter<held_type>(it).end()) { } 
        
 
     private:

     typedef typename beamline_iter_traits<held_type,0>::pre_order_parent_type pre_order_parent_type;

     pre_order_iter( pre_order_parent_type const& parent, iter<held_type> const& it )                 
	 : m_parent(parent), m_iter( it.m_bml, it.base() ), m_visited( it.end()) {} 


      friend class boost::iterator_core_access;

      mutable pre_order_parent_type                         m_parent; 
      typename beamline::iter<held_type>                    m_iter;
      typename beamline::iter<held_type>                    m_visited;
                                                                            
     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

     void increment()
      {

          if( typeid(**m_iter) == typeid(beamline) )  {  // NOTE: need to dereference twice in order for typeid test to work !
 
            beamline* bml      = static_cast<beamline*>( (*m_iter).get());

            m_visited = m_iter;  

            pre_order_iter down( pre_order_parent_type(new pre_order_iter(*this)),  bml->begin() ); 

            std::swap(*this, down); 
  
            return;
             
          }

          ++m_iter; 

          while (  (m_iter == m_iter.end() ) &&  ( m_parent ) )  {  

            pre_order_iter up( *m_parent ); 
            
            ++(up.m_iter); 

            std::swap(*this, up); 
 
          }

	  return; 

      } // void increment() 

      //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

      void decrement()
      {

	if ( m_iter == m_iter.end() ) {
 	  --m_iter;
	  while ( typeid(**m_iter) == typeid(beamline)) { 
            beamline* bml      = static_cast<beamline*>( (*m_iter).get() );
            pre_order_iter down( pre_order_parent_type( new pre_order_iter(*this)), --(bml->end()) ); 
            std::swap(*this, down); 
          }
          return;
        }

	if ( (m_iter == m_iter.begin()) && (m_parent) ) {
 
	  if ( (typeid(**m_iter) == typeid(beamline) ) && (m_iter != m_visited) ) {
 
             beamline* bml      = static_cast<beamline*>( (*m_iter).get() );
             pre_order_iter down( pre_order_parent_type(new pre_order_iter(*this)), --(bml->end()) ); 
             std::swap(*this, down); 
             return;
          }
          
          pre_order_iter up( *m_parent ); 
          std::swap(*this, up); 
          m_visited = m_iter;            

          return;

	 }


	--m_iter;

	 while ( (typeid(**m_iter) == typeid(beamline)) && (m_iter != m_visited) ) {
           beamline* bml      = static_cast<beamline*>( (*m_iter).get());
           pre_order_iter down( pre_order_parent_type (new pre_order_iter(*this)), --(bml->end()) ); 
           std::swap(*this, down); 
         }

         return; 
             
     }// void decrement() 


     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

      template <typename OtherType>
      bool equal( OtherType const& other) const {
         

	if (   m_iter    !=   other.m_iter )   return false;   // level iterators are different
    
        if (  !m_parent  &&   other.m_parent)  return false;   

	if (   m_parent  &&  !other.m_parent)  return false;

	if (  !m_parent  &&  !other.m_parent)  return true;

        if (  *m_parent  !=  *other.m_parent)  return false;   // parent iterators are different
        
					       return true;                                         // 

      }

     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


     held_type& dereference() const { 
       return  *m_iter;
     }
    
 };



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// 
// beamline::reverse_pre_order_iterator
// 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    template <typename held_type>
    class reverse_pre_order_iter: public boost::reverse_iterator<pre_order_iter<held_type> > {

    private:

     struct enabler {};  // empty private type (to avoid accidental misuse). Used in conjunction with 
                           // boost::enable_if() below.  
    public: 

      friend class boost::iterator_core_access;

      reverse_pre_order_iter()
                 : boost::reverse_iterator<pre_order_iter<held_type> >()    {}

      template <typename OtherType>
      reverse_pre_order_iter( OtherType const& rit,
                    typename boost::enable_if<boost::is_convertible<OtherType*, held_type*>, enabler >::type = enabler() ) 
                 : boost::reverse_iterator<OtherType>(rit) {} 

      reverse_pre_order_iter( pre_order_iter<held_type> const& it )      
                 : boost::reverse_iterator<pre_order_iter<held_type> >(it) {} 


    };
   
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// 
// beamline::post_order_iterator
// 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

   template <typename held_type>
   class post_order_iter : public boost::iterator_facade <post_order_iter<held_type>,  // Derived
                                                         held_type,                    // Value
                                                         boost::forward_traversal_tag, // class CategoryOrTraversal
                                                         held_type&,                   // Reference  
                                                         ptrdiff_t                     // Difference // ???? 
                                                       >   
   {
     private:

     struct enabler {};     // empty private type (to avoid accidental misuse). Used in conjunction with 
                           // boost::enable_if() below.  
     public:

       post_order_iter() {}
   
       template <typename OtherType>
       post_order_iter( post_order_iter<OtherType> const& pit,         
                    typename boost::enable_if<boost::is_convertible<OtherType*, held_type*>, enabler >::type = enabler() ) 
	 :  m_parent(pit.m_parent), m_iter(pit.m_iter), m_visited(pit.m_visited) {}
      

       post_order_iter( iter<held_type> const& it)
        :  m_parent(), m_iter(it), m_visited( iter<held_type>(it).end()) 
       { 

         if ( m_iter == m_iter.end() ) return;

	 while ( (typeid(**m_iter) == typeid(beamline)) && ( m_iter != m_visited) )  {  
              beamline* bml      = static_cast<beamline*>( (*m_iter).get());
              post_order_iter 
                   down( post_order_parent_type(new post_order_iter(*this)),  bml->begin() ); 
              std::swap(*this, down); 
  
         }
         return; 
       }

     private:

      typedef typename beamline_iter_traits<held_type,0>::post_order_parent_type  post_order_parent_type;

      post_order_iter( post_order_parent_type const& parent, iter<held_type> const& it )                 
	 : m_parent(parent), m_iter( it.m_bml, it.base() ), m_visited( it.end()) {} 


      mutable post_order_parent_type                      m_parent;       
      typename beamline::iter<held_type>                  m_iter;
      typename beamline::iter<held_type>                  m_visited;


      friend class boost::iterator_core_access;

     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

       
     void increment()
      {

          ++m_iter; 

          if (  (m_iter == m_iter.end()) &&  (!m_parent) )   return;   

          if (  (m_iter == m_iter.end()) &&  ( m_parent) )  {  

            post_order_iter up( *m_parent ); 
            std::swap(*this, up); 
            m_visited = m_iter; 
            return; 
          }

	 if ( (typeid(**m_iter) == typeid(beamline)) && ( m_iter != m_visited) ) { 
	   while ( (typeid(**m_iter) == typeid(beamline)) && ( m_iter != m_visited) )  {  
              beamline* bml      = static_cast<beamline*>( (*m_iter).get());
              m_visited  = m_iter;  
              post_order_iter down( post_order_parent_type(new post_order_iter(*this)),  bml->begin() ); 
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
	

        if((typeid(**m_iter) ==typeid(beamline))  && (m_iter != m_visited) )  {
           beamline* bml      = static_cast<beamline*>( (*m_iter).get() );
           post_order_iter down( post_order_parent_type(new post_order_iter(*this) ) , --(bml->end()) ); 
           std::swap(*this, down); 
           return;
        } 

        while ( (m_iter == m_iter.begin()) && (m_parent) ) {
          
          if ( (typeid(*(*m_iter).get())==typeid(beamline))  && (m_iter != m_visited) )  {
              beamline* bml      = static_cast<beamline*>( (*m_iter).get() );
              post_order_iter down( post_order_parent_type(new post_order_iter(*this) ), --(bml->end()) ); 
              std::swap(*this, down); 
              return; 
          }

          post_order_iter up( *m_parent ); 
          std::swap(*this, up); 
          m_visited = m_iter; 

        }

	--m_iter;

	if ((typeid(**m_iter)  == typeid(beamline)) && (m_iter != m_visited) ) {
           beamline* bml      = static_cast<beamline*>( (*m_iter).get() );
           post_order_iter down(  post_order_parent_type(new post_order_iter(*this) ), --(bml->end()) ); 
           std::swap(*this, down); 
           return;
        }

        while ( (typeid(**m_iter)==typeid(beamline))  && (m_iter == m_visited) ) --m_iter;

        return; 
             
      }// void decrement() 

     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 
      template<typename OtherType>
      bool equal( OtherType const& other) const {
         

	if (   m_iter    !=   other.m_iter )   return false;   // level iterators are different
    
        if (  !m_parent  &&   other.m_parent)  return false;   

	if (   m_parent  &&  !other.m_parent)  return false;

	if (  !m_parent  &&  !other.m_parent)  return true;

        if (  *m_parent  !=  *other.m_parent)  return false;   // parent iterators are different
        
					       return true;                                         // 

      }


     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


     ElmPtr& dereference() const { 
        return  *m_iter;
     }
    
     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 };

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// 
// beamline::reverse_post_order_iterator
// 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    template <typename held_type>
    class reverse_post_order_iter : public boost::reverse_iterator<beamline::post_order_iter<held_type> >  {

    public: 

      friend class boost::iterator_core_access;

      reverse_post_order_iter()
                 : boost::reverse_iterator<beamline::post_order_iterator>()    {}


      reverse_post_order_iter( reverse_post_order_iter const& rit )
                 : boost::reverse_iterator<post_order_iter<held_type> >(rit) {} 

      reverse_post_order_iter( post_order_iter<held_type> const& it )      
                                                                                
                 : boost::reverse_iterator<post_order_iter<held_type> >(it) {} 


    };
   
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// 
// beamline::deep_iterator
// 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    template <typename held_type>
    class deep_iter : public boost::iterator_adaptor <  deep_iter<held_type>,
                                                        beamline::pre_order_iter<held_type>, // Base
                                                        boost::use_default,                  // Value
                                                        boost::bidirectional_traversal_tag > {  
   template <typename T>
   friend class deep_iter;

    private:

      struct enabler {};  // empty private type (to avoid accidental misuse). Used in conjunction with 
                           // boost::enable_if() below.  
    public:
 
       deep_iter() 
              : deep_iter::iterator_adaptor_() {}

       template <typename OtherType>
       deep_iter( deep_iter<OtherType> const& dit,
                  typename boost::enable_if<boost::is_convertible<OtherType*, held_type*>, enabler >::type = enabler() ) 
                : deep_iter::iterator_adaptor_(dit) {}

       

       deep_iter( pre_order_iter<held_type> const& pit) 
              : deep_iter::iterator_adaptor_(pit) 
        {
	  if (  pit.m_iter  == pit.m_iter.end() )      return; 
	  if ( typeid(***this) ==  typeid(beamline)  ) increment(); 
        }


    private:

       friend class boost::iterator_core_access;



       template <class OtherType>
       bool equal(deep_iter<OtherType> const& other) const
       { 
           return this->base() == other.base();
       }

       void increment() {

	 this->base_reference().increment();  

         if ( this->base().m_iter  == this->base().m_iter.end() ) {
	    return;
         }

         while ( typeid(***this) == typeid(beamline) ) { 
            increment(); // skip beamlines 
         }
       }   

       void decrement() {

	 this->base_reference().decrement();  

         if ( this->base().m_iter == this->base().m_iter.end() ) {
	    return;
         }

         while ( typeid(***this) == typeid(beamline) ) {
	   if ( this->base().m_iter == this->base().m_iter.begin()) break;  
            decrement();  
         }
       }

};


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// 
// beamline::reverse_deep_iterator
// 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    template <typename held_type>
    class reverse_deep_iter: public boost::reverse_iterator<deep_iter<held_type> >  {

    public: 

      friend class boost::iterator_core_access;

      reverse_deep_iter()
                 : boost::reverse_iterator<deep_iter<held_type> >()    {}


      template <typename OtherType>
      reverse_deep_iter( reverse_deep_iter<OtherType> const& rit )
                 : boost::reverse_iterator<deep_iter<held_type> >(rit) {} 

      reverse_deep_iter( deep_iter<held_type> const& it )      
                 : boost::reverse_iterator<deep_iter<held_type> >(it)  {} 


    };


    //-------------------------------------------------------------------------------------------
    // member functions for the beamline class used to set iterators.
    //-------------------------------------------------------------------------------------------

    beamline::const_iterator begin() const    { return const_iterator(this, theList_.begin());    }       
    beamline::iterator       begin()          { return       iterator(this, theList_.begin());    }       
                     

    beamline::const_iterator end()   const    { return const_iterator(this, theList_.end());      }   
    beamline::iterator end()                  { return       iterator(this, theList_.end());      }   
  

    beamline::const_reverse_iterator rbegin() const { return           const_iterator(this, theList_.end() );  } 
    beamline::reverse_iterator rbegin()             { return                 iterator(this, theList_.end() );  } 


    beamline::const_reverse_iterator rend()   const { return           const_iterator(this, theList_.begin()); }   
    beamline::reverse_iterator       rend()         { return                 iterator(this, theList_.begin()); }   
   

    beamline::const_pre_order_iterator pre_begin() const { return      const_iterator(this, theList_.begin() ); } 
    beamline::pre_order_iterator       pre_begin()       { return            iterator(this, theList_.begin() ); } 


    beamline::const_pre_order_iterator pre_end() const   { return       const_iterator(this, theList_.end()   ); }   
    beamline::pre_order_iterator       pre_end()         { return             iterator(this, theList_.end()   ); }   
 

    beamline::const_reverse_pre_order_iterator rpre_begin()  const 
                      { return const_pre_order_iterator( const_iterator(this, theList_.end()   )); } 
    beamline::reverse_pre_order_iterator rpre_begin()  
                      { return       pre_order_iterator(       iterator(this, theList_.end()   )); } 


    beamline::const_reverse_pre_order_iterator rpre_end() const  
                      { return  const_pre_order_iterator(  const_iterator(this, theList_.begin() )); }   
    beamline::reverse_pre_order_iterator       rpre_end()   
                      { return        pre_order_iterator(        iterator(this, theList_.begin() )); }   


    beamline::const_post_order_iterator post_begin() const 
                      { return                               const_iterator(this, theList_.begin()   ); } 
    beamline::post_order_iterator       post_begin() 
                      { return                                     iterator(this, theList_.begin()   ); } 


    beamline::const_post_order_iterator post_end()  const    
                      { return                               const_iterator(this, theList_.end()     ) ;}   
    beamline::post_order_iterator post_end()     
                      { return                                     iterator(this, theList_.end()     ); }   


    beamline::const_reverse_post_order_iterator rpost_begin() const  
                      { return     const_post_order_iterator( const_iterator(this, theList_.end()   )); } 
    beamline::reverse_post_order_iterator rpost_begin()  
                      { return           post_order_iterator(       iterator(this, theList_.end()   )); } 


    beamline::const_reverse_post_order_iterator rpost_end() const  
                      { return const_post_order_iterator(  const_iterator(this, theList_.begin() )); }   
    beamline::reverse_post_order_iterator rpost_end()   
                      { return       post_order_iterator(        iterator(this, theList_.begin() )); }   


    beamline::const_deep_iterator deep_begin() const 
                      { return                       const_pre_order_iterator(  const_iterator(this, theList_.begin() )); } 
    beamline::deep_iterator deep_begin() 
                      { return                             pre_order_iterator(        iterator(this, theList_.begin() )); } 
 

    beamline::const_deep_iterator deep_end() const  
                      { return                       const_pre_order_iterator(  const_iterator(this, theList_.end()   )); }   
    beamline::deep_iterator deep_end()   
                      { return                             pre_order_iterator(        iterator(this, theList_.end()   )); }   


    beamline::const_reverse_deep_iterator rdeep_begin() const  
                      { return  const_deep_iterator( const_pre_order_iterator(  const_iterator(this, theList_.end()  ))); } 
    beamline::reverse_deep_iterator rdeep_begin()  
                      { return        deep_iterator(       pre_order_iterator(  iterator(this, theList_.end()  ))); } 


    beamline::const_reverse_deep_iterator rdeep_end() const    
                      { return  const_deep_iterator(  const_pre_order_iterator(  const_iterator(this, theList_.begin() ))); }   
    beamline::reverse_deep_iterator rdeep_end()    
                      { return        deep_iterator(        pre_order_iterator(        iterator(this, theList_.begin() ))); }   


#endif // BEAMLINE_ITERATORS_H

