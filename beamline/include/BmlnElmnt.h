/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      BmlnElmnt.h
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
****** Nov 2008 ostiguy@fnal.gov
****** - added virtual bool queries
****** May 2008 ostiguy@fnal.gov
****** - proper, explicit assignment operator
****** - propagator moved (back) to base class
****** - no assumption about internal structure
****** - Propagate/localPropagate functions now const correct
****** - added physical apertures
****** Dec 2007   ostiguy@fnal.gov
****** - new typesafe propagators
****** July 2007   ostiguy@fnal.gov
****** - new, less memory hungry implementation for PinnedFrameSet
****** - eliminated nested functor base classes (e.g. CRITFUNC) 
****** Mar 2007:   ostiguy@fnal.gov
****** - support for reference counted elements    
****** Dec 2006:   ostiguy@fnal.gov  
****** - fixed (possibly long standing) memory corruption problems 
******   in BmlnElmnt destructor for many element types.   
******   Introduced elm_core_access empty class to grant Propagators 
******   access privileges to element private data.
****** - cleanup of constructor code
****** - eliminated raw c-style strings 
****** - Eliminated obsolete tagging functions 
****** - various public interface cleanups
****** Oct 2006:   ostiguy@fnal.gov
****** - beamline: improved implementation. beamline is no longer 
******             derived from a list container, but rather contains 
******             an instance of a list.  std:list<> is used rather 
******             than the old style (void*) dlist, which is not type 
******             safe and could not hold smart pointers. 
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
#include <boost/tuple/tuple.hpp>
#include <basic_toolkit/globaldefs.h>
#include <basic_toolkit/Frame.h>
#include <beamline/ElmPtr.h>

//------------------------------
// Forward declarations
//------------------------------
#include <basic_toolkit/VectorFwd.h>
#include <mxyzptlk/JetFwd.h>
#include <mxyzptlk/MappingFwd.h>
#include <beamline/ParticleFwd.h>
#include <beamline/ParticleBunchFwd.h>

class BmlnElmnt;
class beamline;
class BmlVisitor;
class ConstBmlVisitor;
class Alignment;
class Aperture;
class sector;
class BasePropagator;

typedef boost::shared_ptr<beamline>        BmlPtr;
typedef boost::shared_ptr<beamline const>  ConstBmlPtr;

BmlnElmnt* read_istream(std::istream&);


class DLLEXPORT BmlnElmnt {

  friend class beamline; 
  friend class core_access; 

 public:
 
   enum aperture_t { infinite, elliptical, rectangular};

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
  
  BmlnElmnt( std::string const& name="",  double const&  length=0.0, double const& strength=0.0 );

  BmlnElmnt( BmlnElmnt const&  );

  virtual  BmlnElmnt* clone() const = 0;  
  virtual ~BmlnElmnt();

  BmlnElmnt& operator=( BmlnElmnt const& a );  
 
  boost::any& operator[](std::string const& s);    // a type-safe facility to attach attributes of any type

  bool         queryAttribute(std::string const& s) const;     
  void        removeAttribute(std::string const& s);           
  void        removeAttribute(); 

  virtual void accept( BmlVisitor&      )       = 0;
  virtual void accept( ConstBmlVisitor& ) const = 0;

  bool         hasAperture() const;
  void         setAperture( aperture_t type, double hor, double ver);
  boost::tuple<aperture_t,double,double> aperture() const;

  bool         hasAlignment() const;
  void         setAlignment( Vector const& translation, Vector const& rotation);  
  boost::tuple<Vector,Vector> getAlignment() const;

  void          enableEdges(bool usedge, bool dsedge);
  bool      hasUpstreamEdge() const;
  bool    hasDownstreamEdge() const;

  // Methods to set alignment without 
  // (a little overkill, but so what?)

  bool        alignRelX( double const&  meters );
  bool        alignRelY( double const&  meters );
  bool        alignAbsX( double const&  meters );
  bool        alignAbsY( double const&  meters );
  bool      alignRelXmm( double const&  mm     );
  bool      alignRelYmm( double const&  mm     );
  bool      alignAbsXmm( double const&  mm     );
  bool      alignAbsYmm( double const&  mm     );
  bool     alignRelRoll( double const& radians );
  bool     alignAbsRoll( double const& radians );
  bool alignRelRollmrad( double const& mrad    );
  bool alignAbsRollmrad( double const& mrad    );
  bool     setAlignment( Alignment const& );

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

  void localPropagate(         Particle& p ) const;
  void localPropagate(      JetParticle& p ) const;
  void localPropagate(    ParticleBunch& b ) const;
  void localPropagate( JetParticleBunch& b ) const;

  template  <typename Particle_t>
  void enterLocalFrame( Particle_t&  )   const;

  template  <typename Particle_t>
  void leaveLocalFrame( Particle_t&  )   const;

  // Editing functions

  virtual std::pair<ElmPtr,ElmPtr>  split( double const& pc ) const;
                                   // Splits the element at percent orbitlength from the in-face.

  // ... Tagging methods

  void              setTag(  std::string const& tag){ tag_   = tag; }           
  std::string       getTag()           const        { return tag_;  }  


  //  ... Modifiers

  void  setLength   ( double const& );
  void  setStrength ( double const& );

  void  rename        ( std::string const& name );  


  double getReferenceTime() const;     
  void   setReferenceTime( double );               

  // ... Query functions 

  Alignment          alignment()         const;

  double             Strength()          const;
  virtual double     Length()            const;

  virtual double OrbitLength( Particle const& ) const;  // Returns length of design orbit
                                                        // segment through the element.
                                                        // Will be different from "length"
                                                        // for rbends.

  virtual bool hasParallelFaces() const;
  virtual bool hasStandardFaces() const;
  virtual bool     isBeamline()   const;                   
  virtual bool   isDriftSpace()   const = 0;
  virtual bool       isMagnet()   const = 0;
  virtual bool      isPassive()   const = 0;                   
  virtual bool         isThin()   const = 0;                   


  char const*                 Name()   const;
  virtual char const*         Type()   const = 0;


private:

  double   strengthScale()          const;
  void     setStrengthScale( double const&);

  double                             strength_;           // Interpretation depends on object.
  double                             pscale_;             // momentum scaling factor ( for a linac, pscale_ != 1.0 )     

protected:

  virtual void  propagateReference( Particle& p, double initialBhro, bool scaling );

  std::string                        ident_;              // Name identifier of the element.

  double                             length_;             // Length of object [ meters ]

  Alignment*                         align_;              // Alignment object

  PinnedFrameSet                     pinnedFrames_;

  aperture_t                         aperture_;          // aperture type: elliptical, rectangular
  double                             aperturex_;         // hor aperture 1/2 width;  < 0 ==> ignored  
  double                             aperturey_;         // ver aperture 1/2 width;  < 0 ==> ignored  


  std::map<std::string, boost::any>  attributes_;

  std::string                        tag_;             // FIXME !!! OBSOLETE

  bool                               usedge_;
  bool                               dsedge_;

  PropagatorPtr                      propagator_;



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


  /* All the work is done in friend ostream& operator<<(),
     placeholder for if descendants want to do somthing. */

  virtual std::ostream&    writeTo(std::ostream      & os) {return os;}     
  virtual std::istream&   readFrom(std::istream      & is) {return is;}

  friend  std::ostream& operator<<(std::ostream&, BmlnElmnt&);

  friend BmlnElmnt* read_istream(std::istream&);

};

   
#ifndef  BEAMLINE_EXPLICIT_TEMPLATES
#include <beamline/BmlnElmnt.tcc> 
#endif

#endif // BMLNELMNT_H
