/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      sector.cc
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
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
****** REVISION HISTORY
******
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - reduced src file coupling due to visitor interface. 
******   visit() takes advantage of (reference) dynamic type.
****** - use std::string for string operations.
******
****** July 2007         ostiguy@fnal.gov
****** - restructured constructors   
******
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/iosetup.h>
#include <beamline/sector.h>
#include <beamline/BmlVisitor.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;


// **************************************************
//   class sector
// **************************************************

sector::sector(   std::vector<double> const& bH,  std::vector<double> const& aH,  std::vector<double> const& pH
                , std::vector<double> const& bV,  std::vector<double> const& aV,  std::vector<double> const& pV
                , double const& l  ) 
  :  bmlnElmnt(l), 
      mapType_(0),        
        myMap_(),
        betaH_(bH),     // 0 = entry;  1 = exit
       alphaH_(aH),    
    deltaPsiH_(0.0),
        betaV_(bV),     
       alphaV_(aV),    
    deltaPsiV_(0.0),
     mapMatrix_(BMLN_dynDim,BMLN_dynDim)
{

 if( pH[1] <= pH[0] ) {
   throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
          "sector::sector( ...)"
          "Horizontal phases inverted." ) );
 }
 else deltaPsiH_ = pH[1] - pH[0];

 if( pV[1] <= pV[0] ) {
   throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
          "sector::sector( ...)", 
          "Vertical phases inverted." ) );
 }
 else deltaPsiV_ = pV[1] - pV[0];

 for   ( int i=0; i< BMLN_dynDim; ++i) {
     mapMatrix_(i,i) = 1.0;
 }

 double    dummy = sqrt( betaH_[1] / betaH_[0] );   // --- Horizontal sector
 double       cs = cos( deltaPsiH_ );
 double       sn = sin( deltaPsiH_ );
 mapMatrix_(0,0) = ( cs + alphaH_[0] * sn ) * dummy;
 mapMatrix_(3,3) = ( cs - alphaH_[1] * sn ) / dummy;
 dummy           = sqrt( betaH_[0] * betaH_[1] );
 mapMatrix_(0,3) = dummy * sn;
 mapMatrix_(3,0) = (   ( alphaH_[0] - alphaH_[1]     ) * cs
                     - ( 1.0 + alphaH_[0]*alphaH_[1] ) * sn
                   ) / dummy;

           dummy = sqrt( betaV_[1] / betaV_[0] );   // --- Vertical sector
              cs = cos( deltaPsiV_ );
              sn = sin( deltaPsiV_ );
 mapMatrix_(1,1) = ( cs + alphaV_[0] * sn ) * dummy;
 mapMatrix_(4,4) = ( cs - alphaV_[1] * sn ) / dummy;
           dummy = sqrt( betaV_[0] * betaV_[1] );
 mapMatrix_(1,4) = dummy * sn;
 mapMatrix_(4,1) = (   ( alphaV_[0] - alphaV_[1]     ) * cs
                     - ( 1.0 + alphaV_[0]*alphaV_[1] ) * sn
                   ) / dummy;


}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sector::sector( const char* n, std::vector<double> const& bH,  std::vector<double> const& aH,  std::vector<double> const& pH, 
                               std::vector<double> const& bV,  std::vector<double> const& aV,  std::vector<double> const& pV, double const& l  ) 
  : bmlnElmnt( n, l ), 
      mapType_(0),        
        myMap_(),
        betaH_(bH),     // 0 = entry;  1 = exit
       alphaH_(aH),    
    deltaPsiH_(0.0),
        betaV_(bV),     
       alphaV_(aV),    
    deltaPsiV_(0.0),
     mapMatrix_(BMLN_dynDim,BMLN_dynDim)
{

 if( pH[1] <= pH[0] ) {
   throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
          "sector::sector( ...)"
          "Horizontal phases inverted." ) );
 }
 else deltaPsiH_ = pH[1] - pH[0];

 if( pV[1] <= pV[0] ) {
   throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
          "sector::sector( ...)", 
          "Vertical phases inverted." ) );
 }
 else deltaPsiV_ = pV[1] - pV[0];

 for   ( int i=0; i< BMLN_dynDim; ++i) {
     mapMatrix_(i,i) = 1.0;
 }

 double    dummy = sqrt( betaH_[1] / betaH_[0] );   // --- Horizontal sector
 double       cs = cos( deltaPsiH_ );
 double       sn = sin( deltaPsiH_ );
 mapMatrix_(0,0) = ( cs + alphaH_[0] * sn ) * dummy;
 mapMatrix_(3,3) = ( cs - alphaH_[1] * sn ) / dummy;
 dummy           = sqrt( betaH_[0] * betaH_[1] );
 mapMatrix_(0,3) = dummy * sn;
 mapMatrix_(3,0) = (   ( alphaH_[0] - alphaH_[1]     ) * cs
                     - ( 1.0 + alphaH_[0]*alphaH_[1] ) * sn
                   ) / dummy;

           dummy = sqrt( betaV_[1] / betaV_[0] );   // --- Vertical sector
              cs = cos( deltaPsiV_ );
              sn = sin( deltaPsiV_ );
 mapMatrix_(1,1) = ( cs + alphaV_[0] * sn ) * dummy;
 mapMatrix_(4,4) = ( cs - alphaV_[1] * sn ) / dummy;
           dummy = sqrt( betaV_[0] * betaV_[1] );
 mapMatrix_(1,4) = dummy * sn;
 mapMatrix_(4,1) = (   ( alphaV_[0] - alphaV_[1]     ) * cs
                     - ( 1.0 + alphaV_[0]*alphaV_[1] ) * sn
                   ) / dummy;


} // end function sector::sector( std::vector<double>& bH, ... )


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sector::sector( Mapping const& m, double const& l, char mpt, PropFunc* prop   ) 
  : bmlnElmnt(l, prop), 
      mapType_(mpt),        
        myMap_(m),
        betaH_(),     
       alphaH_(),    
    deltaPsiH_(0.0),
        betaV_(),     
       alphaV_(),    
    deltaPsiV_(0.0),
    mapMatrix_()
{
 if( mpt == 0 ) { mapMatrix_ = myMap_.Jacobian(); }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sector::sector( const char* n, Mapping const& m, double const& l, char mpt,PropFunc* prop ) 
  : bmlnElmnt( n, l, prop ), 
      mapType_(mpt),        
        myMap_(m),
        betaH_(),     
       alphaH_(),    
    deltaPsiH_(0.0),
        betaV_(),     
       alphaV_(),    
    deltaPsiV_(0.0),
    mapMatrix_()
{
 if( mpt == 0 ) { mapMatrix_ = myMap_.Jacobian(); } 
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sector::sector( const char* n, double const& l ) 
  :  bmlnElmnt( n, l ), 
      mapType_(0),        
        myMap_(0),
        betaH_(),     
       alphaH_(),    
    deltaPsiH_(0.0),
        betaV_(),     
       alphaV_(),    
    deltaPsiV_(0.0),
    mapMatrix_(BMLN_dynDim,BMLN_dynDim)

{
  for ( int i=0; i < BMLN_dynDim; ++i) {
     mapMatrix_(i,i) = 1.0;
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sector::sector( const sector& x )
  :  bmlnElmnt(x),
      mapType_(x.mapType_),        
        myMap_(x.myMap_),
        betaH_(x.betaH_),     
       alphaH_(x.alphaH_),    
    deltaPsiH_(x.deltaPsiH_),
        betaV_(x.betaV_),     
       alphaV_(x.alphaV_),    
    deltaPsiV_(x.deltaPsiV_),
    mapMatrix_(x.mapMatrix_)
{}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sector::~sector() 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Mapping const& sector::getMap() const
{
  return myMap_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sector::setFrequency( double (*fcn)( double const& ) ) 
{
  DeltaT = fcn;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sector::setFrequency( Jet (*fcn)( const Jet& ) ) 
{
  JetDeltaT = fcn;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* sector::Type() const 
{ 
  return "sector"; 
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sector::setLength( double const& )
{
  (*pcerr) << "*** WARNING ***                                \n"
          "*** WARNING *** sector::setLength does nothing.\n"
          "*** WARNING ***                                \n"
       << endl;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& sector::writeTo ( ostream& os )
{
  if( mapType_ == 0 )
  {
    os << myMap_;
  }
  else 
  {
    (*pcerr) << "*** ERROR ***                                    \n"
         << "*** ERROR *** sector::writeTo                    \n"
         << "*** ERROR *** This is written only to handle     \n"
         << "*** ERROR *** mapType = 0.                       \n"
         << "*** ERROR ***                                    \n"
         << "*** ERROR *** The sector is not streamed.        \n"
         << "*** ERROR ***                                    \n"
         << endl;
  }
  
  return os;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& sector::readFrom( istream& is )
{
  mapType_ = 0;
  is >> myMap_;
  return is;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  sector::isMagnet() const 
{
  return false;  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  sector::accept( BmlVisitor& v )
{
  v.visit(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  sector::accept( ConstBmlVisitor& v ) const
{
  v.visit(*this);
}

