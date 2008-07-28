/*******************************************************************************
********************************************************************************
********************************************************************************
********************************************************************************
******                                                                    ******
******    BEAMLINE:  C++ objects for design and analysis                  ******
******              of beamlines, storage rings, and                      ******
******              synchrotrons.                                         ******
******                                                                    ******
******    File:      beamline.tcc                                         ******
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
******  REVISION HISTORY                                                  ******
******  ----------------                                                  ******
******                                                                    ******
******  Mar 2008 ostiguy@fnal.gov                                         ******
****** -Refactored to take advantage of hierarchical information          ******
****** -misc fixes to improve interoperability of const and non-const     ******
******  iterator types                                                    ******
******
********************************************************************************
********************************************************************************
********************************************************************************
********************************************************************************/


#include <boost/utility.hpp>

//------------------------------------------------------------------------------  
// STL compatible iterators for the beamline class ( author: ostiguy@fnal.gov)
//------------------------------------------------------------------------------
// Note: much of the complexity in the code below results from the fact that both 
//       const and non-const varieties of iterators are dealt with simultaneously.
//
// Note:  iter_traits is defined as a nested class with two parameters and only 
//        partially specialized. This is done to work around the fact that the 
//        C++ (98) standard does not allow full specialization of nested class 
//        templates ... but _does_ allow _partial_ specialization ;-) 
//        The integer parameter Dummy is introduced and arbitrarily set to 0 
//        in the nested iterator classes where the traits are instantiated.   
//--------------------------------------------------------------------------------        


 template<int Dummy>
 struct beamline::iter_traits<ElmPtr, Dummy> {
    typedef  std::list<ElmPtr>::iterator            list_iter_t;
    typedef  iter<ElmPtr>                                iter_t;
    typedef  ElmPtr                                      val_t;
    typedef  beamline                                    bml_t;
  };

  template<int Dummy>
  struct beamline::iter_traits<ElmPtr const, Dummy> {
  typedef  std::list<ElmPtr>::const_iterator        list_iter_t;
    typedef  iter<ElmPtr const>                          iter_t;
    typedef  ConstElmPtr                                  val_t;
    typedef  beamline const                               bml_t;

  };

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//
// beamline::iterator
//
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 template <typename held_t>
 class beamline::iter: public boost::iterator_adaptor <    iter<held_t>, 
                                                  typename iter_traits<held_t,0>::list_iter_t >  // Base
{
    friend class beamline;

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
        
      typedef typename iter_traits<held_t,0>::list_iter_t list_iter_t;
      typedef typename iter_traits<held_t,0>::bml_t             bml_t;

      iter()
         : iter::iterator_adaptor_(), bml_(0) {}

      iter( bml_t* bml, list_iter_t const& it )
         : iter::iterator_adaptor_( it ), bml_(bml) {}

      template <typename OtherType>
      iter( iter<OtherType> const& other, 
        	      typename boost::enable_if<boost::is_convertible<OtherType*, held_t*>, enabler >::type = enabler() ) 
            	      : iter::iterator_adaptor_( other.base() ), bml_(other.bml_){} 
      

      iter begin() { return bml_->begin(); }  
      iter end()   { return bml_->end();   }  
 
   private:
         
     friend class boost::iterator_core_access;

     bml_t*   bml_;
  
    };


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//
// beamline::reverse_iterator
//
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    template <typename held_t>
    class beamline::reverse_iter: public boost::reverse_iterator<iter<held_t> >  {

    private:

       struct enabler {};  // empty private type (to avoid accidental misuse). Used in conjunction with 
                           // boost::enable_if() below.  
    public: 

      friend class boost::iterator_core_access;

      reverse_iter()                                                       
                   : boost::reverse_iterator<iter<held_t> >()    {}

      template <typename OtherType>
      reverse_iter( reverse_iter<OtherType> const& rit, 
                    typename boost::enable_if<boost::is_convertible<OtherType*, held_t*>, enabler >::type = enabler() ) 
                   : reverse_iter(rit) {} 

      reverse_iter( iter<held_t>         const&  it )
                  : boost::reverse_iterator<iter<held_t> >(it) {} 



    };


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//
// beamline::pre_order_iterator
//
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

   
  template <typename held_t>
  class beamline::pre_order_iter: public boost::iterator_adaptor<  pre_order_iter<held_t>, // Derived
                                                         iter<held_t>         >  // Base

  {

     template <typename T>
     friend class pre_order_iter;

     template <typename T>
     friend class deep_iter;

     private:

     struct enabler {};  // empty private type (to avoid accidental misuse). Used in conjunction with 
                           // boost::enable_if() below.  
     public:

    
      typedef typename iter_traits<held_t,0>::list_iter_t list_iter_t;
      typedef typename iter_traits<held_t,0>::bml_t             bml_t;

      pre_order_iter() 
        : pre_order_iter::iterator_adaptor_()   {}

      pre_order_iter( iter<held_t> it )
	: pre_order_iter::iterator_adaptor_(it) {}

      pre_order_iter( pre_order_iter const& it) 
	 : pre_order_iter::iterator_adaptor_(it) {}

      template <typename OtherType>
      pre_order_iter(pre_order_iter<OtherType> const& pit,
          typename boost::enable_if<boost::is_convertible<OtherType*, held_t*>, enabler >::type = enabler() ) 
 	: pre_order_iter(pit) {}

      void parent() 
      {
         // move the iterator up in the hierarchy
 
         bml_t* bml   = this->base_reference().bml_->parent();
         bml_t* child = this->base_reference().bml_;

         if (!bml) return; // already at the top. Do nothing; 

         for (  iter<held_t> it = bml->begin(); it != bml->end(); ++it ) {

	          if ( (*it).get() == child ) { 
	              this->base_reference() =  it;
                      break;
		  }
	 }
       
       
       } // parent

     private:
 
      friend class boost::iterator_core_access;
      
     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

     void increment()
     {

       if ( !(**this)->isBeamline() ) {
            
           ++this->base_reference(); 

           //--------------------------------------------------------
           // we may have reached the end of a child beamline. 
           // In that case, go up the hierarchy until we reach     
           // an iterator pointing to a valid element _or_ 
           // an iterator at the top level (i.e. the end iterator ) 
           //----------------------------------------------------------

           while ( this->base_reference() == this->base_reference().bml_->end() ) {

	     bml_t* bml = this->base_reference().bml_->parent();
             if (!bml) return ; // we reached the end of the top level 

	     parent(); 
             ++this->base_reference(); 

           }
           
           if ( this->base_reference() == this->base_reference().bml_->end() ) return;


       } 
       else {  

         if ( this->base_reference() == this->base_reference().bml_->end() ) { ++this->base_reference(); return;} // empty beamline
         (*this) = pre_order_iter( boost::dynamic_pointer_cast<bml_t>(**this)->begin() );

       }    

       return;

     } // void increment() 


      //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

      void decrement()
      {


        if ( this->base_reference() == this->base_reference().bml_->end() ) { --this->base_reference(); return; }

 	if( this->base() == this->base_reference().bml_->begin() ) {  
           parent();
           --this->base_reference(); 
           return;  
        };


        if( (**this)->isBeamline() ) { 
          (*this) = pre_order_iter( --boost::dynamic_pointer_cast<bml_t>(**this)->end() );
        }

        --this->base_reference(); 

        return;

        }// void decrement() 

 };


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// 
// beamline::reverse_pre_order_iterator
// 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    template <typename held_t>
    class beamline::reverse_pre_order_iter: public boost::reverse_iterator<pre_order_iter<held_t> > {

    private:

     struct enabler {};  // empty private type (to avoid accidental misuse). Used in conjunction with 
                           // boost::enable_if() below.  
    public: 

      friend class boost::iterator_core_access;

      reverse_pre_order_iter()
                 : boost::reverse_iterator<pre_order_iter<held_t> >()    {}

      explicit reverse_pre_order_iter( pre_order_iter<held_t> it ) 
                 : boost::reverse_iterator<pre_order_iter<held_t> >(it) {} 
 
      template <typename OtherType>
      reverse_pre_order_iter( reverse_pre_order_iter<OtherType> const& rit,
                    typename boost::enable_if<boost::is_convertible<OtherType*, held_t*>, enabler >::type = enabler() ) 
                   : reverse_pre_order_iter(rit) {} 

    };
   
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// 
// beamline::post_order_iterator
// 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

  
  template <typename held_t>
  class beamline::post_order_iter: public boost::iterator_adaptor<  post_order_iter<held_t>, // Derived
                                                          iter<held_t>           > // Base

   {

     private:

     struct enabler {};     // empty private type (to avoid accidental misuse). Used in conjunction with 
                           // boost::enable_if() below.  
     public:

      typedef typename iter_traits<held_t,0>::list_iter_t list_iter_t;
      typedef typename iter_traits<held_t,0>::bml_t             bml_t;

       post_order_iter()
         : post_order_iter::iterator_adaptor_(), elms_() {}
   
       post_order_iter( iter<held_t> it)
	: post_order_iter::iterator_adaptor_(it), elms_() {}

       post_order_iter( post_order_iter const& it) 
	 : post_order_iter::iterator_adaptor_(it), elms_(it.elms_) {}

       template <typename OtherType>
       post_order_iter( post_order_iter<OtherType> const& pit,         
                    typename boost::enable_if<boost::is_convertible<OtherType*, held_t*>, enabler >::type = enabler() ) 
                : post_order_iter::iterator_adaptor_(pit.base()) {}
      

      void parent() 
      {
         // move the iterator up in the hierarchy
 
         bml_t* bml   = this->base_reference().bml_->parent();
         bml_t* child = this->base_reference().bml_;

         if (!bml) return; // already at the top. Do nothing; 

         for (  iter<held_t> it = bml->begin(); it != bml->end(); ++it ) {

	          if ( (*it).get() == child ) { 
	              this->base_reference() =  it;
                      break;
		  }
	 }
       
       
       } // parent

    private:

      friend class boost::iterator_core_access;
       
      std::stack<held_t*> elms_;

     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
       
     void increment()
     {       

        //--------------------------------------------------------
        // we may have reached the end of a child beamline. 
        // In that case, go up the hierarchy.
        //----------------------------------------------------------
 
         if( this->base_reference() == this->base_reference().bml_->end() ) {
	     bml_t* bml = this->base_reference().bml_->parent();
             if (!bml) return; // reached top level
	     elms_.pop();
	     this->parent();  
             ++this->base_reference(); 
             return;
        } 

           
        ++this->base_reference(); 
        
        if( this->base_reference() == this->base_reference().bml_->end() ) return;
        
        if ( (*this->base_reference())->isBeamline() ) {
           elms_.push( &(*this->base_reference()) );
           this->base_reference() = boost::dynamic_pointer_cast<bml_t>(*this->base_reference() )->begin();
        }


           
       return;

     } // increment


     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

      void decrement()
      {

       if( this->base() == this->base_reference().bml_->begin() ) {  
           parent();
           elms_.pop();
       }

       --this->base_reference();  
       
       if( (**this)->isBeamline() ) { 
          elms_.push( &(*this->base_reference()) );
          this->base_reference() = boost::dynamic_pointer_cast<bml_t>(**this)->end();
          return; 
       }

       return;

      }// void decrement() 


     //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

     held_t&  dereference() const
     {

      return ( this->base_reference() == 
               this->base_reference().bml_->end() ) ? *(elms_.top()) : *this->base_reference(); 
     }


};

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// 
// beamline::reverse_post_order_iterator
// 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    template <typename held_t>
    class beamline::reverse_post_order_iter : public boost::reverse_iterator<beamline::post_order_iter<held_t> >  {

    public: 

      friend class boost::iterator_core_access;

      reverse_post_order_iter()
                 : boost::reverse_iterator<beamline::post_order_iterator>()    {}

      reverse_post_order_iter( reverse_post_order_iter const& rit )
                 : boost::reverse_iterator<post_order_iter<held_t> >(rit) {} 

      reverse_post_order_iter( post_order_iter<held_t> const& it )      
                 : boost::reverse_iterator<post_order_iter<held_t> >(it) {} 


    };
   
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// 
// beamline::deep_iterator
// 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

  
  template <typename held_t>
  class beamline::deep_iter: public boost::iterator_adaptor<       deep_iter<held_t>,                // Derived
                                                         iter<held_t>   >                  // Base

  {

   template <typename T>
   friend class deep_iter;

    private:

      struct enabler {};  // empty private type (to avoid accidental misuse). Used in conjunction with 
                           // boost::enable_if() below.  
    public:
 
       typedef typename iter_traits<held_t,0>::bml_t             bml_t;

       deep_iter() 
              : deep_iter::iterator_adaptor_() {}

       deep_iter( iter<held_t> it) 
	 : deep_iter::iterator_adaptor_(it) {}


       template <typename OtherType>
       deep_iter( deep_iter<OtherType> const& dit,
                  typename boost::enable_if<boost::is_convertible<OtherType*, held_t*>, enabler >::type = enabler() ) 
                : deep_iter::iterator_adaptor_(dit.base()) {}


       void parent() 
       {
         // move the iterator up in the hierarchy
 
         bml_t* bml   = this->base_reference().bml_->parent();
         bml_t* child = this->base_reference().bml_;
     
         if (!bml) return; // already at the top. Do nothing; 

         for (  iter<held_t> it = bml->begin(); it != bml->end(); ++it ) {

	          if ( (*it).get() == child ) { 
	              this->base_reference() =  it;
                      break;
		  }
	 }
       
       
       } // parent

    private:

       friend class boost::iterator_core_access;

 
      void increment() {
       
       if ( !(**this)->isBeamline() ) {
            
           ++this->base_reference(); 

           //--------------------------------------------------------
           // we may have reached the end of a child beamline. 
           // In that case, go up the hierarchy until we reach     
           // an iterator pointing to a valid element _or_ 
           // an iterator at the top level (i.e. the end iterator ) 
           //----------------------------------------------------------

           while ( this->base_reference() == this->base_reference().bml_->end() ) {

	     bml_t* bml = this->base_reference().bml_->parent();

             if (!bml) return ; // we reached the end of the top level 

	     parent(); 
             ++this->base_reference(); 

           }
           
           if ( this->base_reference() == this->base_reference().bml_->end() ) return;


       }   

  
       while( (**this)->isBeamline() ) { 

	 if ( this->base_reference().bml_->begin() == this->base_reference().bml_->end() ) break; //  empty beamline 

         (*this) = deep_iter( boost::dynamic_pointer_cast<bml_t>(**this)->begin() );

       }    

       return;
       
       }  // increment

       //-----------------------------------------------------------------------------------------------

       void decrement() {

	while ( this->base() == this->base_reference().bml_->begin() ) {  
           parent();
        }

        --this->base_reference(); 

        while( (**this)->isBeamline() ) { 
          (*this) = deep_iter( --boost::dynamic_pointer_cast<bml_t>(**this)->end() );
        }
        return;

       } // decrement

};


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// 
// beamline::reverse_deep_iterator
// 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    template <typename held_t>
    class beamline::reverse_deep_iter: public boost::reverse_iterator<deep_iter<held_t> >  {

    public: 

      friend class boost::iterator_core_access;

      reverse_deep_iter()
                 : boost::reverse_iterator<deep_iter<held_t> >()    {}


      template <typename OtherType>
      reverse_deep_iter( reverse_deep_iter<OtherType> const& rit )
                 : boost::reverse_iterator<deep_iter<held_t> >(rit) {} 

      reverse_deep_iter( deep_iter<held_t> const& it )      
                 : boost::reverse_iterator<deep_iter<held_t> >(it)  {} 


    };



#endif // BEAMLINE_ITERATORS_H
#endif // BEAMLINE_H
