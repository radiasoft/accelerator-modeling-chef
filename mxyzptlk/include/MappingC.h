/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                ****
******  MXYZPTLK:  A C++ implementation of differential algebra.      ****
******                                                                ****
******  Copyright (c) 1990  Universities Research Association, Inc.   ****
******                All Rights Reserved                             ****
******                                                                ****
******  Author:    Leo Michelotti                                     ****
******                                                                ****
******             Fermilab                                           ****
******             P.O.Box 500                                        ****
******             Mail Stop 345                                      ****
******             Batavia, IL   60510                                ****
******                                                                ****
******             Phone: (708) 840 4956                              ****
******             Email: michelot@hazel.fnal.gov                     ****
******                    michelotti@adcalc.fnal.gov                  ****
******                    almond::michelotti                          ****
******                                                                ****
******  Release    Version 1.0 : January   31, 1990                   ****
******  Dates:              .1 : October   11, 1990                   ****
******                      .2 : November   6, 1990                   ****
******                      .3 : October    2, 1992                   ****
******                      .4 : July      20, 1993                   ****
******                     2.0   August    12, 1993                   ****
******                     3.0   May       19, 1994                   ****
******                      .1   September 14, 1994                   ****
******                                                                ****
**************************************************************************
**************************************************************************
******                                                                ****
******  This material resulted from work developed under a            ****
******  Government Contract and is subject to the following           ****
******  license: The Government retains a paid-up, nonexclusive,      ****
******  irrevocable worldwide license to reproduce, prepare           ****
******  derivative works, perform publicly and display publicly       ****
******  by or for the Government, including the right to distribute   ****
******  to other Government contractors.  Neither the United          ****
******  States nor the United States Department of Energy, nor        ****
******  any of their employees, makes any warranty, express or        ****
******  implied, or assumes any legal liability or responsibility     ****
******  for the accuracy, completeness or usefulness of any           ****
******  information, apparatus, product, or process disclosed, or     ****
******  represents that its use would not infringe privately owned    ****
******  rights.                                                       ****
******                                                                ****
******  These files are made avaliable for use by specific            ****
******  individuals or a specific group.  They are not to be          ****
******  reproduced or redistributed.                                  ****
******                                                                ****
******  These files are provided "as is" with no warranties of any    ****
******  kind including the warranties of design,                      ****
******  merchantibility and fitness for a particular purpose,         ****
******  or arising from a course of dealing, usage or trade           ****
******  practice.                                                     ****
******                                                                ****
******  These files are provided with no support or obligation on     ****
******  the part of U.R.A. or Fermilab to assist in their use,        ****
******  correction, modification or enhancement.                      ****
******                                                                ****
******  Neither U.R.A. nor Fermilab shall be liable with              ****
******  respect to the infringement of copyrights, trade              ****
******  secrets or any patents by these files or any part             ****
******  thereof.                                                      ****
******                                                                ****
******  In no event will U.R.A., Fermilab, or the author(s) be        ****
******  liable for any lost revenue or profits or other               ****
******  special, indirect and consequential damages, even if          ****
******  they have been advised of the possibility of such             ****
******  damages.                                                      ****
******                                                                ****
**************************************************************************
*************************************************************************/

#ifndef MAPC_HXX
#define MAPC_HXX

#include "JetCVector.h"

class MappingC : public JetCVector
{
 private:

  MappingC EpsInverse( JetC__environment* ) const;

 protected:
  // Friends
  // friend class  JetC;
  // friend struct JL;
  // friend class  CJetCVector;

 public: 

  MappingC( const int&  /* dimension */    = JetC::lastEnv->SpaceDim,
        const JetC*  /* components */  = 0, 
              JetC__environment*       = JetC::lastEnv 
     );
  MappingC( const MappingC& );
  MappingC( const JetCVector& );
  MappingC( char*, 
        JetC__environment* = JetC::lastEnv );    // Produces the identity.
  ~MappingC();

  MappingC& operator=( const MappingC& );
  MappingC& operator=( const Mapping&  );
  MappingC operator()( const MappingC& ) const;  // MappingC composition.
  MappingC operator* ( const MappingC& ) const;  // MappingC composition also; an alias.

  MatrixC Jacobian() const;
  JetC& operator()( const int& ) const; 

  MappingC Inverse() const;

  // friend MappingC operator*( /* const */ MatrixC&, const MappingC& );

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
};

void 
operator*=( MappingC& x, const MappingC& y );


// inline MappingC operator*( /* const */ MatrixC& x, const MappingC& y ) 
// {
//   return (MappingC) ( x*( (JetCVector&) y ) );
// }


inline 
JetC& 
MappingC::operator()( const int& i ) 
const
{
  return JetCVector::operator()( i );
}


inline
MappingC& 
MappingC::operator=( const Mapping& x )
{
  JetCVector::operator=( (JetVector&) x );
  return *this;
}


#endif // MAP_HXX
