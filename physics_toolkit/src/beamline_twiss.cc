#include "beamline.inc"
#ifdef __VISUAL_CPP__
#include <iomanip>
#else
#include <iomanip.h>
#endif

#include "LattFuncSage.h"    // ??? Only temporary, until beamline::twiss functions vanish.
#include "ClosedOrbitSage.h" // 


int beamline::twiss( JetParticle& p, double dpp ) 
{
  static char firstTime = 1;
  if( firstTime ) {
    cout << "*** WARNING ***                                      \n"
            "*** WARNING *** beamline::twiss                      \n"
            "*** WARNING ***                                      \n"
            "*** WARNING *** This member function is obsolete.    \n"
            "*** WARNING *** Use a LattFuncSage instead.          \n"
            "*** WARNING ***                                      \n"
         << endl;
    if( cerr != cout ) {
      cerr << "*** WARNING ***                                      \n"
              "*** WARNING *** beamline::twiss                      \n"
              "*** WARNING ***                                      \n"
              "*** WARNING *** This member function is obsolete.    \n"
              "*** WARNING *** Use a LattFuncSage instead.          \n"
              "*** WARNING ***                                      \n"
           << endl;
    }
    firstTime = 0;
  }

  int ret = 0;

  if( !twissDone )
  {
    int i;
    double* zero   = new double [ BMLN_dynDim ];
    for ( i = 0; i < BMLN_dynDim; i++ ) zero[i] = 0.0;
  
    p.SetReferenceEnergy( this->Energy() );
    p.setState(zero);
  
    ClosedOrbitSage clsg( this );
    LattFuncSage lfs( this );
    lfs.set_dpp( dpp );
  
  
    // Put p on closed orbit and construct one-turn map
    clsg.setForcedCalc();
    if( ( ret = clsg.findClosedOrbit( &p ) ) == 0 )
    {
      cerr << "beamline::twiss: Closed orbit successfully calculated." << endl;
    }
    else
    {
      cerr << "beamline::twiss: Closed orbit not successfully calculated." << endl;
      delete [] zero;
      return ret;
    }
  
  
    ret = lfs.Fast_CS_Calc( &p );
  
    if( ret != 0 ) {
      cerr << "beamline::twiss: Problem calculating the Twiss \n"
  	   << "parameters." << endl;
      delete [] zero;
      return ret;
    }

    // Calculate dispersion
    // This also puts the chromaticity ring data on the beamline.
    ret = lfs.Disp_Calc( &p );
  
    if( ret != 0 ) {
      cerr << "beamline::twiss: Problem calculating dispersion."
  	   << endl;
      delete [] zero;
      return ret;
    }
  
  
  
    // Combine dispersion and lattice function information ...
    bmlnElmnt* q;
    lattFunc*  plf;
    lattFunc*  qlf;
    DeepBeamlineIterator dbi( this );
    while( q = dbi++ ) {
      if( 0 != ( plf = (lattFunc*) q->dataHook.find("Twiss") ) ) {
  	if( 0 != ( qlf = (lattFunc*) q->dataHook.find("Dispersion") ) ) {
  	  plf->dispersion.hor = qlf->dispersion.hor;
  	  plf->dispersion.ver = qlf->dispersion.ver;
  	  plf->dPrime.hor     = qlf->dPrime.hor;
  	  plf->dPrime.ver     = qlf->dPrime.ver;
  	  q->dataHook.eraseAll("Dispersion");
  	}
  	else {
  	  cerr << "beamline::twiss: Dispersion data missing from "
  	       << q->Type() << "  " << q->Name()
  	       << endl;
  	  ret = -1;
  	  delete [] zero;
  	  return ret;
  	}
      }
      else {
  	cerr << "beamline::twiss: Twiss data missing from "
  	     << q->Type() << "  " << q->Name()
  	     << endl;
  	ret = -1;
  	delete [] zero;
  	return ret;
      }
    }
  
  
  
    // .......... Cleaning up and leaving ...................
  
    lattFunc* latticeFunctions = new lattFunc;
    (*latticeFunctions) = *((lattFunc*)( lastElement()->dataHook.find( "Twiss" ) ));
    this->dataHook.eraseFirst( "Twiss" );
    this->dataHook.insert( new Barnacle( "Twiss", latticeFunctions ) );
    // dataHook.eraseFirst( "Ring" );
    // dataHook.insert( new Barnacle( "Ring", latticeRing ) );
  
  
    twissDone = 1;
    delete [] zero;
  }

  return ret;
}


int beamline::twiss( char, JetParticle& p ) {
  Jet*             z;
  dlist_iterator  getNext ( *(dlist*) this );
  double          csH, csV, snH, snV, t;
  double*         zero;
  int             i;

 // .......... Propagate a JetProton to get transfer matrix


  lattFunc*       latticeFunctions = new lattFunc;
  lattRing*       latticeRing = new lattRing;

  if( ! twissDone ) {  // .... Check to see if this was done already.
   
    zero           = new double   [ BMLN_dynDim ];
    z              = new Jet      [ BMLN_dynDim ];

   // .......... Propagate a JetProton to get transfer matrix
    for ( i = 0; i < BMLN_dynDim; i++ ) zero[i] = 0.0;
    p.setState( zero );
    propagate( p );
    
    MatrixD mtrx(BMLN_dynDim,BMLN_dynDim,0.0);
    Mapping map;
    p.getState( map );
    mtrx = map.Jacobian();

   // .......... Calculating tunes .........................

    csH = ( mtrx(0,0) + mtrx(3,3) ) / 2.0;  // cosine horizontal tune
    csV = ( mtrx(1,1) + mtrx(4,4) ) / 2.0;  // cosine vertical   tune

    if( fabs(csH) > 1.0 || fabs(csV) > 1.0 ) {
      cerr << "\n*** WARNING *** " << endl ;
      cerr << "*** WARNING *** beamline::twiss(JetParticle)  Lattice is unstable." << endl;
      cerr << "*** WARNING *** beamline::twiss() did not exit properly." << endl;
      cerr << "*** WARNING *** " << endl;
      delete [] zero;
      delete [] z;
      delete latticeFunctions;
      delete latticeRing;
      return -1;
   }
    
    snH = sqrt( -1.0* mtrx(0,3)*mtrx(3,0) - 
               (mtrx(0,0) - mtrx(3,3))*
               (mtrx(0,0) - mtrx(3,3))/4.0);
    snV = sqrt( -1.0*mtrx(1,4) * mtrx(4,1) - 
               (mtrx(1,1) - mtrx(4,4))*
               (mtrx(1,1) - mtrx(4,4))/4.0);


    if( mtrx(0,3) < 0.0 ) snH *= -1.0;       // ?? Is there a better way of
    if( mtrx(1,4) < 0.0 ) snV *= -1.0;       // ?? changing the sign?
    
   t = asin( snH );
   if( csH < 0.0 ) t  = pi - t;              // 0 < t < 2 pi
   if( t < 0.0 )   t += twoPi;
   latticeRing->tune.hor = ( t / twoPi );

   t = asin( snV );
   if( csV < 0.0 ) t  = pi - t;              // 0 < t < 2 pi
   if( t < 0.0 )   t += twoPi;
   latticeRing->tune.ver = t / twoPi;
//    t = atan2(snH,csH);
//    latticeRing->tune.hor = ( t / twoPi );

//    t = atan2(snV,csV);
//    latticeRing->tune.ver = ( t / twoPi );


   // .......... Calculating betas and alphas ..............
    latticeFunctions->beta .hor = mtrx(0,3) / snH;
    latticeFunctions->beta .ver = mtrx(1,4) / snV;
    latticeFunctions->alpha.hor = ( mtrx(0,0) - mtrx(3,3) ) / (2.0*snH);
    latticeFunctions->alpha.ver = ( mtrx(1,1) - mtrx(4,4) ) / (2.0 * snV);
    latticeFunctions->psi  .hor = 0.0;
    latticeFunctions->psi  .ver = 0.0;
 
// calculate dispersion
    
    MatrixD A(4,4,0.0);
    MatrixD Disp(4,1,0.0);
    MatrixD Long(4,1,0.0);
    
    A(0,0) = mtrx(0,0);
    A(0,1) = mtrx(0,3);
    A(0,2) = mtrx(0,1);
    A(0,3) = mtrx(0,4);
    A(1,0) = mtrx(3,0);
    A(1,1) = mtrx(3,3);
    A(1,2) = mtrx(3,1);
    A(1,3) = mtrx(3,4);
    A(2,0) = mtrx(1,0);
    A(2,1) = mtrx(1,3);
    A(2,2) = mtrx(1,1);
    A(2,3) = mtrx(1,4);
    A(3,0) = mtrx(4,0);
    A(3,1) = mtrx(4,3);
    A(3,2) = mtrx(4,1);
    A(3,3) = mtrx(4,4);
   
    Long(0,0) = -1.0 * mtrx(0,5);
    Long(1,0) = -1.0 * mtrx(3,5);
    Long(2,0) = -1.0 * mtrx(1,5);
    Long(3,0) = -1.0 * mtrx(4,5);
    MatrixD id("I",4);
    Disp = (A - id).inverse() * Long;

    latticeFunctions->dispersion.hor = Disp(0,0);
    latticeFunctions->dPrime.hor = Disp(1,0);
    latticeFunctions->dispersion.ver = Disp(2,0);
    latticeFunctions->dPrime.ver = Disp(3,0);

   // .......... Cleaning up and leaving ...................
    delete [] zero;
    delete [] z;
    twissDone = 1;
    dataHook.eraseFirst( "Twiss" );
    dataHook.insert( new Barnacle( "Twiss", latticeFunctions ) );
    dataHook.eraseFirst( "Ring" );
    dataHook.insert( new Barnacle( "Ring", latticeRing ) );
  }
  return 0;
} 

int beamline::twiss( lattFunc& W, JetParticle& p) {
  static char firstTime = 1;

  if( firstTime ) {
    firstTime = 0;
    cerr << "***WARNING***                                           \n"
            "***WARNING***  beamline::twiss( lattFunc& W, JetParticle& p)      \n"
            "***WARNING***  Using LattFuncSage is preferred.         \n"
            "***WARNING***  This member function will disappear      \n"
            "***WARNING***  one day.                                 \n"
            "***WARNING***                                           \n"
         << endl;
  }

  int ret;
  LattFuncSage et( this );

  if( ! twissDone ) {  // .... Check to see if this was done already.
    ret = et.Twiss_Calc( W, p );
    if( ret == 0 ) twissDone = 1;
    else {
      cerr << "***WARNING***                                     \n"
              "***WARNING*** beamline::twiss                     \n"
              "***WARNING*** Failed.                             \n"
              "***WARNING***                                     \n"
           << endl;
    }
  }
  return ret;
} 

