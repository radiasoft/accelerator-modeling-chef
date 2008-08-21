/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      bmlnElmnt.h
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
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
****** REVISION HISTORY:
****** -----------------
****** 
****** Oct 2006:   Jean-Francois Ostiguy  ostiguy@fnal.gov
****** - beamline: improved implementation. beamline is no longer 
******             derived from a list container, but rather contains 
******             an instance of a list.  std:list<> is used rather 
******             than the old style (void*) dlist, which is not type 
******             safe and could not hold smart pointers. 
****** Dec 2006:   ostiguy@fnal.gov  
****** - fixed (possibly long standing) memory corruption problems 
******   in bmlnElmnt destructor for many element types.   
******   Introduced elm_core_access empty class to grant Propagators 
******   access privileges to element private data.
****** - cleanup of constructor code
****** - eliminated raw c-style strings 
****** - Eliminated obsolete tagging functions 
****** - various public interface cleanups
****** Mar 2007:   ostiguy@fnal.gov
****** - support for reference counted elements    
****** July 2007   ostiguy@fnal.gov
****** - new, less memory hungry implementation for PinnedFrameSet
****** - eliminated nested functor base classes (e.g. CRITFUNC) 
****** Dec 2007   ostiguy@fnal.gov
****** - new typesafe propagators
****** May 2008 ostiguy@fnal.gov
****** - proper, explicit assignment operator
****** - propagator moved (back) to base class
****** - no assumption about internal structure
****** - Propagate/localPropagate functions now const correct
**************************************************************************
*************************************************************************/
#ifndef BMLNELMNT_H
#define BMLNELMNT_H

#include <string>
#include <iostream>
#include <exception>

#include <list>
#include <map>
#include <boost/any.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <basic_toolkit/globaldefs.h>
#include <basic_toolkit/Frame.h>
#include <basic_toolkit/Barnacle.h>

#include <beamline/ElmPtr.h>
#include <beamline/BasePropagator.h>

#define NOTKNOWN   -123456.789
#define OSTREAM_DOUBLE_PREC setprecision(20)

//------------------------------
// Forward declarations
//------------------------------

class bmlnElmnt;
class alignmentData;
class beamline;
class BmlVisitor;
class ConstBmlVisitor;
class Aperture;
class Particle;
class JetParticle;
class sector;
class alignment;

template <typename Particle_t>
class TBunch;

typedef TBunch<Particle>       ParticleBunch;
typedef TBunch<JetParticle> JetParticleBunch;


typedef boost::shared_ptr<beamline>        BmlPtr;
typedef boost::shared_ptr<beamline const>  ConstBmlPtr;

template <typename T>
class TVector;

template <typename T>
class TMapping;

template <typename T>
class TJet;

template <typename T>
class TJetVector;

typedef TVector<double>                VectorD;
typedef TJet<double>                   Jet;
typedef TJet<std::complex<double> >    JetC;
typedef TJetVector<double>             JetVector;
typedef TMapping<double>               Mapping;

bmlnElmnt* read_istream(std::istream&);


class DLLEXPORT bmlnElmnt {

  friend class beamline; 
  friend class core_access; 

 public:
 
   typedef boost::shared_ptr<BasePropagator> PropagatorPtr;   

   class core_access;

   //--------------------------------------------------------------
   // PinnedFrameSet 
   //--------------------------------------------------------------
   // If the element never moves, both upStream_ and downStream_  
   // should be the identity frames. 
   // If it moves, then upStream_ will be the element's original 
   // in-frame AS SEEN BY its current in-frame; 
   // downStream_ will be its original out-frame AS SEEN BY 
   // its current out-frame.
   // Note that in fact, this is redundant information.  
   // Either upStream_ or downStream_ could be eliminated. !!!

  class PinnedFrameSet
  {

    public:

      PinnedFrameSet();
      PinnedFrameSet( PinnedFrameSet const& );

     ~PinnedFrameSet();

      PinnedFrameSet& operator=( PinnedFrameSet const& rhs);

      Frame const&   upStream()   const;
      Frame const&   downStream() const;
    
      void   upStream( Frame const& );
      void downStream( Frame const& );
    
      bool  altered() const;
      void  reset();

   private:

      Frame*  upStream_;
      Frame*  downStream_;

  };

  // comparison operator for hash table.  

  struct eqstr
  {
     bool operator()(const char* s1, const char* s2) const
     {
       return strcmp(s1, s2) == 0;
     }
  };

public:
  
  bmlnElmnt( std::string const& name="",  double const&  length=0.0, double const& strength=0.0 );

  bmlnElmnt( bmlnElmnt const&  );

  virtual  bmlnElmnt* Clone() const = 0;  
  virtual ~bmlnElmnt();

  bmlnElmnt& operator=( bmlnElmnt const& a );  
 
  boost::any& operator[](std::string const& s);    // a type-safe facility to attach attributes of any type

  bool        attributeExists(std::string const& s) const;     
  void        attributeClear (std::string const& s);           
  void        attributeClear ();                                

  virtual void accept( BmlVisitor& ) = 0;
  virtual void accept( ConstBmlVisitor& ) const = 0;

  void acceptInner( BmlVisitor& );
  void acceptInner( ConstBmlVisitor& ) const;


  // Methods to set alignment without 
  // (a little overkill, but so what?)

  virtual bool        alignRelX( double const&  meters );
  virtual bool        alignRelY( double const&  meters );
  virtual bool        alignAbsX( double const&  meters );
  virtual bool        alignAbsY( double const&  meters );
  virtual bool      alignRelXmm( double const&  mm     );
  virtual bool      alignRelYmm( double const&  mm     );
  virtual bool      alignAbsXmm( double const&  mm     );
  virtual bool      alignAbsYmm( double const&  mm     );
  virtual bool     alignRelRoll( double const& radians );
  virtual bool     alignAbsRoll( double const& radians );
  virtual bool alignRelRollmrad( double const& mrad    );
  virtual bool alignAbsRollmrad( double const& mrad    );
  virtual bool     setAlignment( alignmentData const& );

  bool    hasMoved()  const  { return ( pinnedFrames_.altered() || align_  ); }
  void    realign();  // Resets element to its original position.
  void    markPins(); // WRITE // Marks the current positions of the element as "original"
  void    loadPinnedCoordinates( Particle const &, Vector&, double=1.0 ) const; 

    
  // Pinned coordinates of the particle are returned in the vector
  //   argument.  
  // 
  // The third argument must be within [0,1]; it is internally
  //   set to 1.0 otherwise.  It indicates the percentage of distance
  //   from the upstream to downstream faces of the element at which 
  //   the coordinates are to be calculated.
  // 
  // Default value: 1.0 -> return pinned coordinates at the downstream end.

  Frame const& getUpstreamPinnedFrame() const 
    { return pinnedFrames_.upStream(); }

  Frame const& getDownstreamPinnedFrame() const 
    { return pinnedFrames_.downStream(); }


  void propagate(         Particle& p ) const;
  void propagate(      JetParticle& p ) const;
  void propagate(    ParticleBunch& b ) const;
  void propagate( JetParticleBunch& b ) const;

  virtual void localPropagate(         Particle& p ) const;
  virtual void localPropagate(      JetParticle& p ) const;
  virtual void localPropagate(    ParticleBunch& b ) const;
  virtual void localPropagate( JetParticleBunch& b ) const;

  void enterLocalFrame( Particle&                )   const;
  void enterLocalFrame( JetParticle&             )   const;

  void leaveLocalFrame( Particle&                )   const;
  void leaveLocalFrame( JetParticle&             )   const;

  // Editing functions

  virtual void Split( double const& pct, ElmPtr&, ElmPtr& ) const;
                                   // Splits the element at percent orbitlength
                                   // pct from the in-face and returns
                                   // addresses to the two pieces
                                   // in the second and third arguments.

  virtual void  peekAt( double& s, Particle const& ) const;


  // ... Tagging methods

  void              setTag(  std::string const& tag){ tag_   = tag; }           
  std::string       getTag()           const        { return tag_;  }  


  //  ... Modifiers

  void    setLength   ( double const& );
  void    setStrength ( double const& );
  void    setStrengthScale( double const&);

          void setAperture   ( Aperture* );
          void rename        ( std::string const& name );  


  virtual double getReferenceTime()                    const;     // returns ctRef_
  virtual void setReferenceTime( double const& );               
  virtual void setReferenceTime( Particle&     );               



  // ... Query functions 

  alignmentData        Alignment( void )        const;
  Aperture*            getAperture();           // returns a clone of the aperture class
  bool                 hasAperture()            const  { return  pAperture_; }

  double               Strength()               const;
  double               strengthScale()          const;
  virtual double       Length()                 const;


  virtual bool hasParallelFaces() const;
  virtual bool hasStandardFaces() const;

  virtual bool isBeamline()  const;                   
  virtual bool   isSimple()  const;
  virtual bool   isMagnet()  const = 0;
  virtual bool  isPassive()  const = 0;                   
  virtual bool     isThin()  const = 0;                   


  std::string const&          Name()   const  { return ident_; }
  virtual char const*         Type()   const = 0;


  virtual double OrbitLength( Particle const& ) { return length_; }
                                   // Returns length of design orbit
                                   // segment through the element.
                                   // Will be different from "length"
                                   // for rbends.

private:

  double                            strength_;           // Interpretation depends on object.
  double                            pscale_;             // momentum scaling factor ( for a linac, pscale_ != 1.0 )     

protected:

  std::string                       ident_;              // Name identifier of the element.

  double                            length_;             // Length of object [ meters ]
  
  alignment*                        align_;
  PinnedFrameSet                    pinnedFrames_;

  mutable double                    ctRef_;              // (normalized) time required for
                                                         // a reference particle to cross
                                                         // the element. Normally, established by a RefRegVisitor.

  std::map<std::string, boost::any>  attributes_;

  std::string                        tag_;

  Aperture*                          pAperture_;       // O.K.

  BmlPtr                             bml_;             // The element may be composite.
  ElmPtr                             elm_;             // with one active part.

  PropagatorPtr  propagator_;

private:

  void init_internals(BmlPtr const& bml, ElmPtr const& elm); 

  /* All the work is done in friend ostream& operator<<(),
     placeholder for if descendants want to do somthing. */

  virtual std::ostream&    writeTo(std::ostream      & os) {return os;}     
  virtual std::istream&   readFrom(std::istream      & is) {return is;}

  friend  std::ostream& operator<<(std::ostream&, bmlnElmnt&);

  friend bmlnElmnt* read_istream(std::istream&);

public:

  BarnacleList dataHook;   // Carries data as service to application program.

};

class bmlnElmnt::core_access {

 public:

  static ElmPtr const&  get_ElmPtr( bmlnElmnt const& o) { return o.elm_;   }
  static BmlPtr const&  get_BmlPtr( bmlnElmnt const& o) { return o.bml_;   }
  static double const&  get_ctRef ( bmlnElmnt const& o) { return o.ctRef_; }
  static ElmPtr&        get_ElmPtr( bmlnElmnt& o)       { return o.elm_;   }
  static BmlPtr&        get_BmlPtr( bmlnElmnt& o)       { return o.bml_;   }
  static double&        get_ctRef ( bmlnElmnt& o)       { return o.ctRef_; }

};
   

#endif // BMLNELMNT_H
