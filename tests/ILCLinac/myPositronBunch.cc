#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

#include <beamline/beamline.h>
#include <myPositronBunch.h>
#include <basic_toolkit/Distribution.h>

using std::cerr; 
using std::endl;
 
myTruncatorBunchPredicate::myTruncatorBunchPredicate(double z0, double z1):
_zMin(z0), _zMax(z1) {;}

bool myTruncatorBunchPredicate::operator()( Particle const& p) const {

//      std::cerr << " cdt of particle " << p.get_cdt() << "nn " 
//                << numPScan << std::endl;
      bool accept = ( (p.get_cdt() > _zMin) && (p.get_cdt() < _zMax));
//      if (!accept)  std::cerr << " cdt of reject particle " << p.get_cdt() << std::endl;
//      else std::cerr << " keep  particle at y " << p.get_y() << " cdt " 
//                     <<  p.get_cdt() << std::endl;
      return (!accept); // Used in a remove context!   
}


myTwissParam::myTwissParam():defined(false), betaX(0.), alphaX(0.), 
          betaY(0.0), alphaY(0.), DY(0.), DYp(0.), DX(0.), DXp(0.) {;}
	  
myPositronBunch::myPositronBunch(double e): ParticleBunch(Positron(e)),
                                    ENominal(e), 
                                    originalTotalCharge(2.0e10),
				    numMacroParticleAtCreation(0) {}

myPositronBunch::~myPositronBunch() {
}

void myPositronBunch::generate( MatrixC& EMatched, int nPart, 
                                    double epsX, double epsY, 
                                    double sigCTau, double sigdPoP) {
   				    
// Gaussian Model 			    
  Positron samplePositron(ENominal);
  bool usingBoostGen = true;
  if (usingBoostGen) { 
// using boost 
    boost::minstd_rand generator(42u);
    basUnifGen angleRan(-M_PI, M_PI);
  // Multiplying by sqrt (2pi), to get one sigma emittance, 
  // Linear Collider crowd... 
    basGaussGen xActionRan(0.0,epsX*2.50599/(M_TWOPI*samplePositron.Gamma()));
    basGaussGen yActionRan(0.0,epsY*2.50599/(M_TWOPI*samplePositron.Gamma()));
  // Phase space angle flat.
    varUnifGen psiDist(generator,angleRan);
  // Gaussian model, again
    varGaussGen xActionDist(generator,xActionRan);
    varGaussGen yActionDist(generator,yActionRan);
  
// Longitudinal action; for now, treat sigma ctau and sigma P uncorrelated. 
// not true coming out of the Bunch rotator, probably...
    basGaussGen cTauRan(0.0, sigCTau);
    basGaussGen dPoPRan(0.0, sigdPoP);
    varGaussGen cTauDist(generator,cTauRan);
    varGaussGen dPoPDist(generator,dPoPRan);

    MatrixC a(6,1);
    double psi;
    for( int i = 0; i < nPart; i++ ) {
       psi = psiDist();
      a(0) = sqrt(std::abs(xActionDist()))*std::complex<double>( -sin(psi), cos(psi) );
      a(3) = std::conj(a(0));

      psi = psiDist();
      a(1) = sqrt(std::abs(yActionDist()))*std::complex<double>( -sin(psi), cos(psi) );
      a(4) = std::conj(a(1));
      a(2) = cTauDist();
      a(5) = dPoPDist ();
//    std::cerr << " ctau = " << a(2) << " dPoP " << a(5) << std::endl;
      Vector state(6);
      state = real( EMatched*a );
      Positron samplePosTmp(ENominal, state );
      this->append( samplePosTmp );
    }
  
  } else {  // Using basic_toolkit from CHEF itself
  
    Vector average(6);
    for (int k=0; k != 6; ++k) average(k) = 0.;
    Vector sigma(6);
    sigma(0) = sqrt(epsX*2.50599/(M_TWOPI*samplePositron.Gamma()));
    sigma(3) = sigma(0);
    sigma(2) = sigCTau;
    sigma(1) = sqrt(epsY*2.50599/(M_TWOPI*samplePositron.Gamma()));
    sigma(4) = sigma(1);
    sigma(5) = sigdPoP;
    MultiGaussian myMG(average, sigma);
    MatrixD aRot(6,6);
    for (int j=0; j!=6; ++j) 
      for (int k=0; k!=6; ++k) aRot(j,k) = real(EMatched(j,k));
    
    // Dos not work, need to convert the matching matrix to an orthogonal matrix 
    
    // Cancel the longitudinal-transver correlation, which is wrong... 
    for (int k=0; k != 3; ++k)  aRot(2,k) = 0.;
    aRot(2,2) = 1.;
    for (int k=0; k != 3; ++k)  aRot(5,k) = 0.;
    aRot(5,5) = 1.;
    myMG.setRotation(aRot);
    for( int i = 0; i < nPart; i++ ) {
      Vector aState = myMG.getValue();
      Positron samplePosTmp(ENominal, aState );
      this->append( samplePosTmp );
    }
  } // end of Multi-Gaussian
  numMacroParticleAtCreation = this->size();				    
}
void myPositronBunch::generate(const myTwissParam& aTwiss, int nPart, 
                                    double epsX, double epsY, 
                                    double sigCTau, double sigdPoP) {
   				    
// Gaussian Model 			    
  Positron samplePositron(ENominal);
//  std::cerr << " Using generate wit hTwiss " << endl;

// using boost 
    boost::minstd_rand generator(42u);
    basUnifGen angleRan(-M_PI, M_PI);
  // Multiplying by sqrt (2pi), to get one sigma emittance, 
  // Linear Collider crowd... 
    basGaussGen ggActionRan(0.0,1.);
  // Phase space angle flat.
    varUnifGen psiDist(generator,angleRan);
  // Gaussian model, again
    varGaussGen gActionDist(generator, ggActionRan);
  
// Longitudinal action; for now, treat sigma ctau and sigma P uncorrelated. 
// not true coming out of the Bunch rotator, probably...
    basGaussGen cTauRan(0.0, sigCTau);
    basGaussGen dPoPRan(0.0, sigdPoP);
    varGaussGen cTauDist(generator,cTauRan);
    varGaussGen dPoPDist(generator,dPoPRan);
    double factEmitt = 2.50599/(M_TWOPI*samplePositron.Gamma());
    factEmitt *= 2; // not sure where this factor two comes from.. 
    double psi;
    for( int i = 0; i < nPart; i++ ) {
       psi = psiDist();
      double aX = std::sqrt(factEmitt * epsX * aTwiss.betaX *std::abs(gActionDist()));
      double x = aX*std::sin(psi);
      double xp = (aX*std::cos(psi) - aTwiss.alphaX*x)/aTwiss.betaX;
       psi = psiDist();
      double aY = std::sqrt(factEmitt * epsY * aTwiss.betaY *std::abs(gActionDist()));
      double y = aY*std::sin(psi);
      double yp = (aY*std::cos(psi) - aTwiss.alphaY*y)/aTwiss.betaY;
      
      double dz = cTauDist();
      double dPop = dPoPDist ();
      
      x += dPop * aTwiss.DX;
      xp += dPop * aTwiss.DXp;
      
      y += dPop * aTwiss.DY;
      yp += dPop * aTwiss.DYp;
      
      Vector state(6);
      state(0) = x; state(1) = y; state(2) = dz; state(3) = xp;  state(4) = yp;  state(5) = dPop; 
      Positron samplePosTmp(ENominal, state );
      this->append( samplePosTmp );
    }
  
  numMacroParticleAtCreation = this->size();				    
}

// Only deal with the centroid of the bunch. 

void myPositronBunch::shiftCenter(Vector& stateCenter) {

     Vector currentAverage(6);    
    for (int k=0; k != 6; ++k) currentAverage(k) = 0.;
    for(ParticleBunch::const_iterator pPtr=this->begin(); pPtr != this->end(); ++pPtr) {
       Vector tmpState = pPtr->State(); 
       currentAverage += tmpState;
    }   
   
    for (int k=0; k != 6; ++k) currentAverage(k) /= this->size();
    for(ParticleBunch::iterator pPtr=this->begin(); 
                       pPtr != this->end(); ++pPtr) {
       Vector tmpState = pPtr->State(); 
       tmpState -= currentAverage; 
       tmpState += stateCenter; 
       pPtr->set_x (tmpState[0]);
       pPtr->set_y (tmpState[1]);
       pPtr->set_cdt (tmpState[2]);
       pPtr->set_npx (tmpState[3]);
       pPtr->set_npy (tmpState[4]);
       pPtr->set_ndp (tmpState[5]);
    }   
}

void myPositronBunch::SortByCT() { 
 // See the overloading of operator < applied on ParticlePtr.
  sort( myPositronZPredicate() ); 

}

size_t myPositronBunch::TruncateZ(double zmin, double zmax) {

// Using now the discriminator.. 
   myTruncatorBunchPredicate pred(zmin, zmax);
   this->remove( pred ); 

   return this->size();
}

double myPositronBunch::getXPosition() const {
    if (this->size() == 0) return 0.;
    double aa = 0.;
    for(ParticleBunch::const_iterator pPtr=this->begin(); 
        pPtr != this->end(); ++pPtr) aa += pPtr->get_x(); 
    return aa/this->size();
}

double myPositronBunch::getZPosition() const {
    if (this->size() == 0) return 0.;
    double aa = 0.;
    for(ParticleBunch::const_iterator pPtr=this->begin(); 
        pPtr != this->end(); ++pPtr) aa += pPtr->get_cdt(); 
    return aa/this->size();
}

double myPositronBunch::getXPrime() const {
    if (this->size() == 0) return 0.;
    double aa = 0.;
    for(ParticleBunch::const_iterator pPtr=this->begin();
         pPtr != this->end(); ++pPtr) aa += pPtr->get_npx(); 
    return aa/this->size();
}

double myPositronBunch::getAverageRealMomentum() const {
    if (this->size() == 0) return 0.;
    double aa = 0.;
    for(ParticleBunch::const_iterator pPtr=this->begin(); 
      pPtr != this->end(); ++pPtr) aa += pPtr->Momentum(); 
    return aa/this->size();
}


double myPositronBunch::getReferenceMomentum() const {
    if (this->size() == 0) return 0.;
    double aa = 0.;
    ParticleBunch::const_iterator pPtr=this->begin();
    return pPtr->ReferenceMomentum();
}
//void myPositronBunch::getStateCenter(Vector& stateCenter) const {
void myPositronBunch::getStateCenter(Vector& stateCenter) const {

    for (int k=0; k != 6; ++k) stateCenter(k) = 0.;
    for(ParticleBunch::const_iterator pPtr=this->begin(); 
      pPtr != this->end(); ++pPtr)  {
       Vector tmpState = pPtr->State(); 
       stateCenter += tmpState;
    }
    for (int k=0; k != 6; ++k) stateCenter(k) /= this->size();
}

double myPositronBunch::getYPosition() const  {
    if (this->size() == 0) return 0.;
    double aa = 0.;
    for(ParticleBunch::const_iterator pPtr=this->begin(); 
      pPtr != this->end(); ++pPtr) aa += pPtr->get_y(); 
    return aa/this->size();
}

double myPositronBunch::getZAverage(double zL, double zM)  const {
    if (this->size() == 0) return 0.;
    double aa = 0.;
    int n = 0;
    for(ParticleBunch::const_iterator pPtr=this->begin(); 
      pPtr != this->end(); ++pPtr) {
      double zz = pPtr->get_cdt();
      if (zz < zL) continue;
      if (zz > zM) continue;
      aa += zz; 
      n++;
    }
    return aa/n;
}
double myPositronBunch::getYAverage(double zL, double zM) const {
    if (this->size() == 0) return 0.;
    double aa = 0.;
    int n = 0;
    for(ParticleBunch::const_iterator pPtr=this->begin(); 
      pPtr != this->end(); ++pPtr) {
     double zz = pPtr->get_cdt();
     if (zz < zL) continue;
     if (zz > zM) continue;
     aa += pPtr->get_y(); 
     n++;
    }
    return aa/n;
}
double myPositronBunch::getYpAverage(double zL, double zM)  const {
    if (this->size() == 0) return 0.;
    double aa = 0.;
    int n = 0;
    for(ParticleBunch::const_iterator pPtr=this->begin(); 
      pPtr != this->end(); ++pPtr) {
     double zz = pPtr->get_cdt();
     if (zz < zL) continue;
     if (zz > zM) continue;
     aa += pPtr->get_npy(); 
     n++;
    }
    return aa/n;
}

double myPositronBunch::getYPrime()  const {
    if (this->size() == 0) return 0.;
    double aa = 0.;
    for(ParticleBunch::const_iterator pPtr=this->begin(); 
      pPtr != this->end(); ++pPtr) aa += pPtr->get_npy(); 
    return aa/this->size();
}

double myPositronBunch::getVerticalProjectedEmittance(bool yYPrimeOpt) const { 
    if (this->size() < 2) return 0.;
    double aCenter = 0.;
    double apCenter = 0.;
    double aSig2 = 0.;
    double apSig2 = 0.;
    double aapSig2 = 0.;
    for(ParticleBunch::const_iterator pPtr=this->begin(); 
      pPtr != this->end(); ++pPtr) { 
       double a = pPtr->get_y(); 
       double ap = pPtr->get_npy();
       if (yYPrimeOpt) 
              ap *= 1.0/sqrt( ( 1.0 + pPtr->get_ndp() ) * ( 1.0 + pPtr->get_ndp() )
                 -  pPtr->get_npx()* pPtr->get_npx()
                 -  pPtr->get_npy()* pPtr->get_npy()); 
     	       
        aCenter += a;
        apCenter += ap;
	aSig2 += a*a;
	apSig2 += ap*ap;
	aapSig2 += a*ap;
    }
    int nm1 = this->size() -1;
    double sigaSq = (aSig2 - aCenter*aCenter/this->size())/nm1;
    double  sigapSq = (apSig2 - apCenter*apCenter/this->size())/nm1;
    aapSig2 = (aapSig2 - aCenter*apCenter/this->size())/nm1;
    ParticleBunch::const_iterator pPtr2=this->begin();
    double gamFactor =  pPtr2->ReferenceGamma();
    double emittND = gamFactor*sqrt(sigaSq*sigapSq-pow(aapSig2,2));    
    return emittND;
}

double myPositronBunch::getHorizontalProjectedEmittance() const {
    if (this->size() < 2) return 0.;
    double aCenter = 0.;
    double apCenter = 0.;
    double aSig2 = 0.;
    double apSig2 = 0.;
    double aapSig2 = 0.;
    for(ParticleBunch::const_iterator pPtr=this->begin(); 
      pPtr != this->end(); ++pPtr) { 
        double a = pPtr->get_x(); 
        double ap = pPtr->get_npx(); 
        aCenter += a;
        apCenter += ap;
	aSig2 += a*a;
	apSig2 += ap*ap;
	aapSig2 += a*ap;
    }
    int nm1 = this->size() -1;
    double sigaSq = (aSig2 - aCenter*aCenter/this->size())/nm1;
    double  sigapSq = (apSig2 - apCenter*apCenter/this->size())/nm1;
    aapSig2 = (aapSig2 - aCenter*apCenter/this->size())/nm1;
    ParticleBunch::const_iterator pPtr2=this->begin();
    double gamFactor =  pPtr2->ReferenceGamma();
    double emittND = gamFactor*sqrt(sigaSq*sigapSq-pow(aapSig2,2));
   return emittND;
}
  
double myPositronBunch::getVerticalNoDispEmittance(bool yYPrimeOpt) const {
   if (this->size() < 2) return 0.;
   double S_yvsp = 0., S_ypvsp = 0., S_y=0. , S_yp = 0.;
   double S_pp = 0., averP = 0.;
   for(ParticleBunch::const_iterator pPtr=this->begin(); 
      pPtr != this->end(); ++pPtr) { 
     double yp = pPtr->get_npy();
     if (yYPrimeOpt) 
              yp *= 1.0/sqrt( ( 1.0 + pPtr->get_ndp() ) * ( 1.0 + pPtr->get_ndp() )
                 -  pPtr->get_npx()* pPtr->get_npx()
                 -  pPtr->get_npy()* pPtr->get_npy()); 
     	       
     averP += pPtr->get_npz();
     S_yvsp += pPtr->get_y()* pPtr->get_npz();
     S_ypvsp += yp * pPtr->get_npz() ;
     S_pp += pPtr->get_npz()* pPtr->get_npz();
     S_y += pPtr->get_y();
     S_yp += yp;
   } 
   S_yvsp /= this->size();
   S_ypvsp /= this->size();
   S_pp /= this->size();
   S_y /= this->size();
   S_yp /= this->size();
   averP /= this->size();
   
   double Delta_pp = ( S_pp - averP*averP);
   double corryvsP = ( S_yvsp - averP*S_y)/Delta_pp;
   double corrypvsP = ( S_ypvsp - averP*S_yp)/Delta_pp;
   ParticleBunch::const_iterator pPtrOne = this->begin();
   double e1= pPtrOne->ReferenceMomentum();
   
   double sig2y=0., sig2yp = 0., sigyyp = 0., ay = 0., ayp = 0.;
   for(ParticleBunch::const_iterator pPtr=this->begin(); 
      pPtr != this->end(); ++pPtr) { 
     double yC = pPtr->get_y() - corryvsP*(pPtr->get_npz() - averP);
     double ypC =  pPtr->get_npy() - corrypvsP*(pPtr->get_npz() - averP);
     ay += yC;
     ayp +=  ypC;
     sig2y += yC*yC;
     sig2yp += ypC*ypC;
     sigyyp += yC*ypC;
   }
   
   int nm1 = this->size() -1;
   double sigySq = (sig2y - ay*ay/this->size())/nm1;
   double sigypSq = (sig2yp - ayp*ayp/this->size())/nm1;
   sigyyp = (sigyyp - ay*ayp/this->size())/nm1;
   double gamFactor =  pPtrOne->ReferenceGamma();
   double emittYND = gamFactor*sqrt(sigySq*sigypSq-pow(sigyyp,2));
   // Use matrices, 
   return emittYND;
}

double myPositronBunch::getVerticalNoDispEmittance(double zL, double zM,
        bool yYPrimeOpt) const {
   if (this->size() < 2) return 0.;
   double S_yvsp = 0., S_ypvsp = 0., S_y=0. , S_yp = 0.;
   double S_pp = 0., averP = 0.;
   int n = 0;
   for(ParticleBunch::const_iterator pPtr=this->begin(); 
      pPtr != this->end(); ++pPtr) { 
     double zz = pPtr->get_cdt();
     if (zz < zL) continue;
     if (zz > zM) continue;
     n++;
     double yp = pPtr->get_npy();
     if (yYPrimeOpt) 
              yp *= 1.0/sqrt( ( 1.0 + pPtr->get_ndp() ) * ( 1.0 + pPtr->get_ndp() )
                 -  pPtr->get_npx()* pPtr->get_npx()
                 -  pPtr->get_npy()* pPtr->get_npy()); 
     	       
     averP += pPtr->get_npz();
     S_yvsp += pPtr->get_y()* pPtr->get_npz();
     S_ypvsp += yp * pPtr->get_npz() ;
     S_pp += pPtr->get_npz()* pPtr->get_npz();
     S_y += pPtr->get_y();
     S_yp += yp;
   } 
   S_yvsp /= n;
   S_ypvsp /= n;
   S_pp /= n;
   S_y /= n;
   S_yp /= n;
   averP /= n;
   
   double Delta_pp = ( S_pp - averP*averP);
   double corryvsP = ( S_yvsp - averP*S_y)/Delta_pp;
   double corrypvsP = ( S_ypvsp - averP*S_yp)/Delta_pp;
   ParticleBunch::const_iterator pPtrOne = this->begin();
   double e1= pPtrOne->ReferenceMomentum();
   n = 0;
   double sig2y=0., sig2yp = 0., sigyyp = 0., ay = 0., ayp = 0.;
   for(ParticleBunch::const_iterator pPtr=this->begin(); 
      pPtr != this->end(); ++pPtr) { 
     double zz = pPtr->get_cdt();
     if (zz < zL) continue;
     if (zz > zM) continue;
     n++;
     double yC = pPtr->get_y() - corryvsP*(pPtr->get_npz() - averP);
     double ypC =  pPtr->get_npy() - corrypvsP*(pPtr->get_npz() - averP);
     ay += yC;
     ayp +=  ypC;
     sig2y += yC*yC;
     sig2yp += ypC*ypC;
     sigyyp += yC*ypC;
   }
   
   int nm1 = n -1;
   double sigySq = (sig2y - ay*ay/n)/nm1;
   double sigypSq = (sig2yp - ayp*ayp/n)/nm1;
   sigyyp = (sigyyp - ay*ayp/n)/nm1;
   double gamFactor =  pPtrOne->ReferenceGamma();
   double emittYND = gamFactor*sqrt(sigySq*sigypSq-pow(sigyyp,2));
   return emittYND;
}

double myPositronBunch::getVerticalNoDispEmittance(double disp, double dispP, 
                                                   double zL, double zM) const {
   if (this->size() < 2) return 0.;
   
    
   ParticleBunch::const_iterator pPtrOne = this->begin();
   double e1= pPtrOne->ReferenceMomentum();
   int n = 0;
   double averP = 0.;
   for(ParticleBunch::const_iterator pPtr=this->begin(); 
      pPtr != this->end(); ++pPtr) averP +=  pPtr->get_npz();
   averP /= this->size();  
   
   double sig2y=0., sig2yp = 0., sigyyp = 0., ay = 0., ayp = 0.;
   for(ParticleBunch::const_iterator pPtr=this->begin(); 
      pPtr != this->end(); ++pPtr) { 
     double zz = pPtr->get_cdt();
     if (zz < zL) continue;
     if (zz > zM) continue;
     n++;
     double yC = pPtr->get_y() - disp*(pPtr->get_npz() - averP);
     double ypC =  pPtr->get_npy() - dispP*(pPtr->get_npz() - averP); // Dispersion Prime 
     ay += yC;
     ayp +=  ypC;
     sig2y += yC*yC;
     sig2yp += ypC*ypC;
     sigyyp += yC*ypC;
   }
   
   int nm1 = n -1;
   double sigySq = (sig2y - ay*ay/n)/nm1;
   double sigypSq = (sig2yp - ayp*ayp/n)/nm1;
   sigyyp = (sigyyp - ay*ayp/n)/nm1;
   double gamFactor =  pPtrOne->ReferenceGamma();
   double emittYND = gamFactor*sqrt(sigySq*sigypSq-pow(sigyyp,2));
   return emittYND;
}

double myPositronBunch::get4DNoDispEmittance(double zL, 
                                    double zM) const { 
   if (this->size() < 2) return 0.;
   double S_yvsp = 0., S_ypvsp = 0., S_y=0. , S_yp = 0.;
   double S_xvsp = 0., S_xpvsp = 0., S_x=0. , S_xp = 0.;
   double S_pp = 0., averP = 0.;
   int n = 0;
   for(ParticleBunch::const_iterator pPtr=this->begin(); 
      pPtr != this->end(); ++pPtr) { 
     double zz = pPtr->get_cdt();
     if (zz < zL) continue;
     if (zz > zM) continue;
     n++;
     double yp = pPtr->get_npy();
     averP += pPtr->get_npz();
     S_yvsp += pPtr->get_y()* pPtr->get_npz();
     S_ypvsp += yp * pPtr->get_npz() ;
     S_pp += pPtr->get_npz()* pPtr->get_npz();
     S_y += pPtr->get_y();
     S_yp += yp;
     double xp = pPtr->get_npx();
     S_xvsp += pPtr->get_x()* pPtr->get_npz();
     S_xpvsp += xp * pPtr->get_npz() ;
     S_x += pPtr->get_x();
     S_xp += xp;
   } 
   S_yvsp /= n;
   S_ypvsp /= n;
   S_xvsp /= n;
   S_xpvsp /= n;
   S_pp /= n;
   S_y /= n;
   S_yp /= n;
   S_x /= n;
   S_xp /= n;
   averP /= n;
   
   double Delta_pp = ( S_pp - averP*averP);
   double corryvsP = ( S_yvsp - averP*S_y)/Delta_pp;
   double corrypvsP = ( S_ypvsp - averP*S_yp)/Delta_pp;
   double corrxvsP = ( S_xvsp - averP*S_x)/Delta_pp;
   double corrxpvsP = ( S_xpvsp - averP*S_xp)/Delta_pp;
   ParticleBunch::const_iterator pPtrOne = this->begin();
   double e1= pPtrOne->ReferenceMomentum();
   n = 0;
   double sig2y=0., sig2yp = 0., sigyyp = 0., ay = 0., ayp = 0.;
   double sig2x=0., sig2xp = 0., sigxxp = 0., ax = 0., axp = 0.;
   double sigxy = 0., sigxpy = 0., sigxyp = 0., sigxpyp = 0.;
   for(ParticleBunch::const_iterator pPtr=this->begin(); 
      pPtr != this->end(); ++pPtr) { 
     double zz = pPtr->get_cdt();
     if (zz < zL) continue;
     if (zz > zM) continue;
     n++;
     double yC = pPtr->get_y() - corryvsP*(pPtr->get_npz() - averP);
     double ypC =  pPtr->get_npy() - corrypvsP*(pPtr->get_npz() - averP);
     double xC = pPtr->get_x() - corrxvsP*(pPtr->get_npz() - averP);
     double xpC =  pPtr->get_npx() - corrxpvsP*(pPtr->get_npz() - averP);
     ay += yC;
     ayp +=  ypC;
     ax += xC;
     axp +=  xpC;
     sig2y += yC*yC;
     sig2x += xC*xC;
     sig2yp += ypC*ypC;
     sig2xp += xpC*xpC;
     sigyyp += yC*ypC;
     sigxxp += xC*xpC;
     sigxy += yC*xC;
     sigxpy += xpC * yC;
     sigxyp += ypC * xC;
     sigxpyp += ypC * xpC;
   }
   
   int nm1 = n -1;
   double sigySq = (sig2y - ay*ay/n)/nm1;
   double sigypSq = (sig2yp - ayp*ayp/n)/nm1;
   double sigxSq = (sig2x - ax*ax/n)/nm1;
   double sigxpSq = (sig2xp - axp*axp/n)/nm1;
   sigyyp = (sigyyp - ay*ayp/n)/nm1;
   sigxxp = (sigxxp - ax*axp/n)/nm1;
   sigxy = (sigxy - ax*ay/n)/nm1;
   sigxyp = (sigxyp - ax*ayp/n)/nm1;
   sigxpy = (sigxpy - axp*ay/n)/nm1;
   sigxpyp = (sigxpyp - axp*ayp/n)/nm1;
   // Use matrices, 
   Matrix md(4,4);
   for (int i=0; i != 4; ++i) 
      for (int j=0; j !=4; j++) md(i,j) = 0.; 
   md(0,0) = sigxSq;
   md(1,1) = sigySq;
   md(2,2) = sigxpSq;
   md(3,3) = sigypSq;
//   double detVal = 1.;
////   for (int i=0; i != 4; i++) detVal *= md(i,i);
//   cerr << " My val Det, diagonal  " << detVal << " detVal " << md.determinant() << endl;
   md(0,1) = sigxy;
   md(1,0) = sigxy;
   md(0,2) = sigxxp;
   md(2,0) = sigxxp;
   md(0,3) = sigxyp;
   md(3,0) = sigxyp;
   
   md(1,2) = sigxpy;
   md(2,1) = sigxpy;
   md(1,3) = sigyyp;
   md(3,1) = sigyyp;
 
   md(2,3) = sigxpyp;
   md(3,2) = sigxpyp;
//    cerr << " .... with off-diganoal  " << md.determinant() << endl;
   
   double gamFactor =  pPtrOne->ReferenceGamma();

   double emittYNDD = gamFactor*gamFactor*sqrt(md.determinant());
//   std::cerr << " getVerticalNoDispEmittance " << emittYNDD << endl; 
   
   return emittYNDD;				    
				    
}

double myPositronBunch::get6DEmittance(double zL, 
                                    double zM, bool noDisp) const { 
   if (this->size() < 2) return 0.;
   double S_yvsp = 0., S_ypvsp = 0., S_y=0. , S_yp = 0.;
   double S_xvsp = 0., S_xpvsp = 0., S_x=0. , S_xp = 0.;
   double S_pp = 0., averP = 0.;
   int n = 0;
   double corryvsP = 0.;
   double corrypvsP = 0.;
   double corrxvsP = 0.;
   double corrxpvsP = 0.;
   if (noDisp) { 
     for(ParticleBunch::const_iterator pPtr=this->begin(); 
        pPtr != this->end(); ++pPtr) { 
       double zz = pPtr->get_cdt();
       if (zz < zL) continue;
       if (zz > zM) continue;
       n++;
       double yp = pPtr->get_npy();
       averP += pPtr->get_npz();
       S_yvsp += pPtr->get_y()* pPtr->get_npz();
       S_ypvsp += yp * pPtr->get_npz() ;
       S_pp += pPtr->get_npz()* pPtr->get_npz();
       S_y += pPtr->get_y();
       S_yp += yp;
       double xp = pPtr->get_npx();
       S_xvsp += pPtr->get_x()* pPtr->get_npz();
       S_xpvsp += xp * pPtr->get_npz() ;
       S_x += pPtr->get_x();
       S_xp += xp;
     } 
     S_yvsp /= n;
     S_ypvsp /= n;
     S_xvsp /= n;
     S_xpvsp /= n;
     S_pp /= n;
     S_y /= n;
     S_yp /= n;
     S_x /= n;
     S_xp /= n;
     averP /= n;
     double Delta_pp = ( S_pp - averP*averP);
     corryvsP = ( S_yvsp - averP*S_y)/Delta_pp;
     corrypvsP = ( S_ypvsp - averP*S_yp)/Delta_pp;
     corrxvsP = ( S_xvsp - averP*S_x)/Delta_pp;
     corrxpvsP = ( S_xpvsp - averP*S_xp)/Delta_pp;
   } // correct for dispersion.  Since we compute a 6D emittance, 
   // should not be needed...Self consistency check.
   ParticleBunch::const_iterator pPtrOne = this->begin();
   double e1= pPtrOne->ReferenceMomentum();
   n = 0;
   Matrix sigVar(6,6);
   Vector aVar(6);
   for (size_t i=0; i !=6; ++i) {
     aVar(i) =0.;
     for (size_t j=0; j !=6; ++j) sigVar(i,j) = 0.;
   }
   for(ParticleBunch::const_iterator pPtr=this->begin(); 
      pPtr != this->end(); ++pPtr) { 
     double zz = pPtr->get_cdt();
     if (zz < zL) continue;
     if (zz > zM) continue;
     n++;
     double yC = pPtr->get_y() - corryvsP*(pPtr->get_npz() - averP);
     double ypC =  pPtr->get_npy() - corrypvsP*(pPtr->get_npz() - averP);
     double xC = pPtr->get_x() - corrxvsP*(pPtr->get_npz() - averP);
     double xpC =  pPtr->get_npx() - corrxpvsP*(pPtr->get_npz() - averP);
     double zC = pPtr->get_cdt();
     double zpC = pPtr->get_ndp();
     aVar(0) += xC; aVar(1) += yC; aVar(2) += zC;
     aVar(3) += xpC; aVar(4) += ypC; aVar(5) += zpC;
     sigVar(0,0) += xC*xC;  sigVar(0,1) += xC*yC;  sigVar(0,2) += xC*zC; 
     sigVar(0,3) += xC*xpC; sigVar(0,4) += xC*ypC; sigVar(0,5) += xC*zpC; 
     sigVar(1,0) += yC*xC;  sigVar(1,1) += yC*yC;  sigVar(1,2) += yC*zC; 
     sigVar(1,3) += yC*xpC; sigVar(1,4) += yC*ypC; sigVar(1,5) += yC*zpC; 
     sigVar(2,0) += zC*xC;  sigVar(2,1) += zC*yC;  sigVar(2,2) += zC*zC; 
     sigVar(2,3) += zC*xpC; sigVar(2,4) += zC*ypC; sigVar(2,5) += zC*zpC; 
     sigVar(3,0) += xpC*xC;  sigVar(3,1) += xpC*yC;  sigVar(3,2) += xpC*zC; 
     sigVar(3,3) += xpC*xpC; sigVar(3,4) += xpC*ypC; sigVar(3,5) += xpC*zpC; 
     sigVar(4,0) += ypC*xC;  sigVar(4,1) += ypC*yC;  sigVar(4,2) += ypC*zC; 
     sigVar(4,3) += ypC*xpC; sigVar(4,4) += ypC*ypC; sigVar(4,5) += ypC*zpC; 
     sigVar(5,0) += zpC*xC;  sigVar(5,1) += zpC*yC;  sigVar(5,2) += zpC*zC; 
     sigVar(5,3) += zpC*xpC; sigVar(5,4) += zpC*ypC; sigVar(5,5) += zpC*zpC; 
   }
   Matrix sigV(6,6); 
   int nm1 = n -1;
   for (size_t i=0; i !=6; ++i) {
     for (size_t j=0; j !=6; ++j)
        sigV(i,j) = (sigVar(i,j) - aVar(i)*aVar(j)/n)/nm1;
   }
//    cerr << " .... with off-diganoal  " << md.determinant() << endl;
   
   double gamFactor =  pPtrOne->ReferenceGamma();

   double emittYNDD = gamFactor*gamFactor*gamFactor*sqrt(sigV.determinant());
//   std::cerr << " getVerticalNoDispEmittance " << emittYNDD << endl; 
   
   return emittYNDD;				    
				    
}

double  myPositronBunch::getDispersionWithin(double zL, double zM, 
                 bool yYPrimeOpt) const {

   if (this->size() < 2) return 0.;
   double S_yvsp = 0., S_ypvsp = 0., S_y=0. , S_yp = 0.;
   double S_pp = 0., averP = 0.;
   int n = 0;
   for(ParticleBunch::const_iterator pPtr=this->begin(); 
      pPtr != this->end(); ++pPtr) { 
     double zz = pPtr->get_cdt();
     if (zz < zL) continue;
     if (zz > zM) continue;
     n++;
     double yp = pPtr->get_npy();
     if (yYPrimeOpt) 
              yp *= 1.0/sqrt( ( 1.0 + pPtr->get_ndp() ) * ( 1.0 + pPtr->get_ndp() )
                 -  pPtr->get_npx()* pPtr->get_npx()
                 -  pPtr->get_npy()* pPtr->get_npy()); 
     	       
     averP += pPtr->get_npz();
     S_yvsp += pPtr->get_y()* pPtr->get_npz();
     S_ypvsp += yp * pPtr->get_npz() ;
     S_pp += pPtr->get_npz()* pPtr->get_npz();
     S_y += pPtr->get_y();
     S_yp += yp;
   } 
   averP /= n;
   
   double Delta_pp = ( S_pp - n*averP*averP);
   double corryvsP = ( S_yvsp - averP*S_y)/Delta_pp;
   return  corryvsP;  
}
  
double myPositronBunch::getVerticalNoDispEmittance(double ddExt, bool yYPrimeOpt) const {
   if (this->size() < 2) return 0.;
   double S_yvsp = 0., S_ypvsp = 0., S_y=0. , S_yp = 0.;
   double S_pp = 0., averP = 0.;
   for(ParticleBunch::const_iterator pPtr=this->begin(); 
      pPtr != this->end(); ++pPtr) { 
     double yp = pPtr->get_npy();
     if (yYPrimeOpt) 
              yp *= 1.0/sqrt( ( 1.0 + pPtr->get_ndp() ) * ( 1.0 + pPtr->get_ndp() )
                 -  pPtr->get_npx()* pPtr->get_npx()
                 -  pPtr->get_npy()* pPtr->get_npy()); 
     	       
     averP += pPtr->get_npz();
     S_yvsp += pPtr->get_y()* pPtr->get_npz();
     S_ypvsp += yp * pPtr->get_npz() ;
     S_pp += pPtr->get_npz()* pPtr->get_npz();
     S_y += pPtr->get_y();
     S_yp += yp;
   } 
   S_yvsp /= this->size();
   S_ypvsp /= this->size();
   S_pp /= this->size();
   S_y /= this->size();
   S_yp /= this->size();
   averP /= this->size();
   
   double Delta_pp = ( S_pp - averP*averP);
   double corryvsP = ( S_yvsp - averP*S_y)/Delta_pp;
   std::cout << " Over-writing Dispersion from " <<  corryvsP  
             << " to " << ddExt/1.0e6 << std::endl;
   corryvsP = ddExt/1.0e6;	     
   double corrypvsP = ( S_ypvsp - averP*S_yp)/Delta_pp;
   ParticleBunch::const_iterator pPtrOne = this->begin();
   double e1= pPtrOne->ReferenceMomentum();
   
   double sig2y=0., sig2yp = 0., sigyyp = 0., ay = 0., ayp = 0.;
   for(ParticleBunch::const_iterator pPtr=this->begin(); 
      pPtr != this->end(); ++pPtr) { 
     double yC = pPtr->get_y() - corryvsP*(pPtr->get_npz() - averP);
     double ypC =  pPtr->get_npy() - corrypvsP*(pPtr->get_npz() - averP);
     ay += yC;
     ayp +=  ypC;
     sig2y += yC*yC;
     sig2yp += ypC*ypC;
     sigyyp += yC*ypC;
   }
   
   int nm1 = this->size() -1;
   double sigySq = (sig2y - ay*ay/this->size())/nm1;
   double sigypSq = (sig2yp - ayp*ayp/this->size())/nm1;
   sigyyp = (sigyyp - ay*ayp/this->size())/nm1;
   double gamFactor =  pPtrOne->ReferenceGamma();
   double emittYND = gamFactor*sqrt(sigySq*sigypSq-pow(sigyyp,2));
   return emittYND;
}
  
double myPositronBunch::getHorizontalNoDispEmittance() const {

   if (this->size() < 2) return 0.;
   double S_xvsp = 0., S_xpvsp = 0., S_x=0. , S_xp = 0.;
   double S_pp = 0., averP = 0.;
   
   for(ParticleBunch::const_iterator pPtr=this->begin(); 
      pPtr != this->end(); ++pPtr) { 
     averP += pPtr->get_npz();
     S_xvsp += pPtr->get_x()*pPtr->get_npz();
     S_xpvsp += pPtr->get_npx()*pPtr->get_npz() ;
     S_pp += pPtr->get_npz()*pPtr->get_npz();
     S_x += pPtr->get_x();
     S_xp += pPtr->get_npx();
   } 
   S_xvsp /= this->size();
   S_xpvsp /= this->size();
   S_pp /= this->size();
   S_x /= this->size();
   S_xp /= this->size();
   averP /= this->size();
   
   double Delta_pp = ( S_pp - averP*averP);
   double corrxvsP = ( S_xvsp - averP*S_x)/Delta_pp;
   double corrxpvsP = ( S_xpvsp - averP*S_xp)/Delta_pp;
   ParticleBunch::const_iterator pPtrOne=this->begin();
   double e1= pPtrOne->ReferenceMomentum();
   
   double sig2x=0., sig2xp = 0., sigxxp = 0., ax = 0., axp = 0.;
   for(ParticleBunch::const_iterator pPtr=this->begin(); 
      pPtr != this->end(); ++pPtr) { 
     double xC = pPtr->get_x() - corrxvsP*(pPtr->get_npz() - averP);
     double xpC =  pPtr->get_npx() - corrxpvsP*(pPtr->get_npz() - averP);
     ax += xC;
     axp +=  xpC;
     sig2x += xC*xC;
     sig2xp += xpC*xpC;
     sigxxp += xC*xpC;
   }
   
   int nm1 = this->size() -1;
   double sigxSq = (sig2x - ax*ax/this->size())/nm1;
   double sigxpSq = (sig2xp - axp*axp/this->size())/nm1;
   sigxxp = (sigxxp - ax*axp/this->size())/nm1;
   double gamFactor =  pPtrOne->ReferenceGamma();
   double emittXND = gamFactor*sqrt(sigxSq*sigxpSq-pow(sigxxp,2));
   return emittXND;
}

double myPositronBunch::GetTotalCharge() const {
   double fractRemain = ((double) this->size())/numMacroParticleAtCreation;
   return fractRemain*originalTotalCharge;
   
}

double myPositronBunch::getSigma(size_t iCoord) const {
    double a = 0.;
    double s = 0.;
    if (iCoord > 6) return 0.; // silent goofy error! 
    for(ParticleBunch::const_iterator pPtr=this->begin(); 
      pPtr != this->end(); ++pPtr) { 
      Vector state = pPtr->State();
      a += state(iCoord);
      s += state(iCoord)*state(iCoord);
    }
   int nm1 = this->size() -1;
   double sigSq = (s - a*a/this->size())/nm1;
   return std::sqrt(sigSq);
}


void myPositronBunch::dumpItForR(const char *fileName) const { // Suppose to be const.. No const_iterator.

    std::ofstream ofst1(fileName);
    ofst1.precision(9);
    ofst1 << " x y z xp yp dPoP " << std::endl;
    for(ParticleBunch::const_iterator pPtr=this->begin(); 
      pPtr != this->end(); ++pPtr) { 
      Vector state = pPtr->State();
      for (int kk=0; kk !=5; kk++) ofst1 << " " << 1.0e6*state(kk);
      ofst1 << " " << state(5) << std::endl;
    }
    ofst1.close();  
}
// Make a pixel map of the beam.
double myPositronBunch::dumpItForMovie(const std::string& tokenName,
                                   double yMaxArg, double ypMaxArg,
				   short numPixelsY, 
				   short numPixelsYp) const { // Suppose to be const.. No const_iterator.

    double yMax = yMaxArg;
    double ypMax = ypMaxArg;
    double yCenter = 0.;
    double ypCenter = 0.;
    bool debugOut = false;
    if (yMaxArg < 0.) {  // O.K., all relative scaling.  Resize appropriatly 
       yMax = -1.;
       double yAv = 0.; double ypAv = 0.; double ySQ = 0.;
       for(ParticleBunch::const_iterator pPtr=this->begin(); 
           pPtr != this->end(); ++pPtr) { 
         double y = 1.e6*pPtr->get_y(); yAv += y; 
	 double yp = 1.e6*pPtr->get_npy(); ypAv += yp;
	 ySQ += y*y;
       }
       yAv /= this->size();
       yCenter = yAv;
       ypCenter = ypAv/this->size();
       double sigY = 
        std::sqrt((ySQ - yAv*yAv*this->size())/(this->size() -1));
       yMax = 5.0 * sigY;
       ypMax = yMax * std::abs(ypMaxArg); // then, the ratio y /yp
       if (debugOut) cerr << " yCenter " << yCenter << " ypCenter " 
             << ypCenter << " yMax  "<< yMax << " ypMax " << ypMax << endl;
    }
    
    
    std::vector<double> yy(0);
    std::vector<double> yyp(0);
    for(ParticleBunch::const_iterator pPtr=this->begin(); 
           pPtr != this->end(); ++pPtr) { 
      double y = 1.e6*pPtr->get_y() - yCenter;
      double yp = 1.e6*pPtr->get_npy() - ypCenter;
      if (std::abs(y) >= yMax) continue;
      if (std::abs(yp) >= ypMax) continue;
      yy.push_back(y);
      yyp.push_back(yp);
    }
//    bool debugOut =  ((tokenName.find("p702") != std::string::npos) || 
//                      (tokenName.find("p703") != std::string::npos) ||
//		      (tokenName.find("p704") != std::string::npos));
    if (debugOut)  cerr << " myPositronBunch::dumpItForMovie, Name " << tokenName ;
    if (yy.size() < 3) {
       if (debugOut) cerr << " Not enough particle in frame " << endl;
       return yMax;
    } else {
       if (debugOut) cerr << " O.K. " << endl;
    }
    double pixels[numPixelsY][numPixelsYp];
    for (size_t i=0; i != numPixelsY; i++) {
      for (size_t j=0; j != numPixelsYp; j++) pixels[i][j] = 0.;
    }
    // now fill the pixels..double version. 
    // beam spot always centered..
    double gaFact = std::sqrt(1.0/(2.0*M_PI)); 
    double ym2 = yMax*2.0;
    double ymp2 = ypMax*2.0;
    double pixYSize = ym2/numPixelsY;
    double pixYpSize = ymp2/numPixelsYp;
    double sig2Y = 0.; double avY = 0.;
    double sig2Yp = 0.; double avYp = 0.;
    for (int k=0; k != yy.size(); k++) {
      avY += yy[k]; avYp += yyp[k];
      sig2Y += yy[k]*yy[k]; sig2Yp += yyp[k]*yyp[k];
    }
    double sigY = std::sqrt((sig2Y - avY*avY/yy.size())/(yy.size()-1));
    double sigYp = std::sqrt((sig2Yp - avYp*avYp/yyp.size())/(yyp.size()-1));
    avY /= yy.size(); avYp /= yyp.size();
    sigY /= pixYSize; sigYp /= pixYpSize; // in pixel units 
    bool drawArrow = false;
    if ((sigY < 5.) && (sigYp < 5.)) drawArrow = true;
//      cerr << " Sigma Y in pixels " << sigY << " Sigma Yp in pixels " << 
//        sigYp << endl;
    int sigYInt = (int) (sigY); 
    int sigYpInt = (int) (sigYp);
    for (int k=0; k != yy.size(); k++) {
      int iC = (int) ((yy[k] + yMax)/pixYSize);
      int jC = (int) ((yyp[k] + ypMax)/pixYpSize);
      int iMin = iC - 3*sigYInt; int iMax = iC + 3*sigYInt +1;
      if (iMin < 0) iMin = 0; 
      if (iMax > numPixelsY) iMax = numPixelsY;
      int jMin = jC - 3*sigYpInt; int jMax = jC + 3*sigYpInt +1;
      if (jMin < 0) jMin = 0; 
      if (jMax > numPixelsYp) jMax = numPixelsYp;
      size_t nTmpPixY = (size_t) (iMax - iMin); double wy[nTmpPixY];
      size_t nTmpPixYp = (size_t) (jMax - jMin);  double wyp[nTmpPixYp];
      for (size_t i = 0; i != nTmpPixY; i++) {
        double dy = ((i+iMin)*pixYSize - yMax - yy[k])/(2.0*sigY);
        wy[i] = (gaFact/sigY) * std::exp(-1.0*dy*dy);
      }		
      for (size_t j = 0; j != nTmpPixYp; j++) {
          double dyp = ((j+jMin)*pixYpSize - ypMax - yyp[k])/(2.0*sigYp);
          wyp[j] = (gaFact/sigYp) * std::exp(-1.0*dyp*dyp);
      }
      double sumCheck = 0.;
      for (size_t i = 0; i != nTmpPixY; i++) {
       for (size_t j = 0; j != nTmpPixYp; j++) {
          pixels[i+iMin][j+jMin] += wy[i]*wyp[j];
	  sumCheck += pixYSize*pixYpSize*wy[i]*wyp[j];
//	    if (k < 5) cerr << " Pixel " << i+iMin << " - " << j+jMin 
//	         << " = " <<  pixYSize*pixYpSize*wy[i]*wyp[j] << endl;
	  }
      }
//      cerr << " at " << yy[k] << " - " << yyp[k] << " sum = " 
//           << sumCheck << endl;
      
    }
    double ampMax = -1.0;
    for (size_t j = 0; j != numPixelsYp; j++) {
      for (size_t i = 0; i != numPixelsY; i++) {
         if ( pixels[i][j] >  ampMax) ampMax = pixels[i][j];
       }
    }
    // Now rasterized. assume 8 bits..
    // Now dump the file out 
    
    std::string ffNamePrefix("/tmp/CHEFSteering_XXXXXX");
    size_t ll = ffNamePrefix.size()+1;
    char ffTmp[ll];
    memcpy((void *) ffTmp, (const void *) ffNamePrefix.c_str(), ll);
    mkstemp(ffTmp);
    std::ofstream ofst1(ffTmp);
    ofst1 << "P3 " << endl;
    int numColors = 256; 
    ofst1 << numPixelsY <<  " " << numPixelsYp 
          << " " << numColors << " " << endl;
    int dimArrowY = numPixelsY/5;
    if (dimArrowY < 5) drawArrow = false;
    int dimArrowYp = dimArrowY/3;
    if (dimArrowYp < 3) dimArrowYp = 3;
//    if (drawArrow) 
//      std::cerr << " dimArrows " << dimArrowY << " - " << dimArrowYp << endl;
    bool arrowPix[dimArrowY][dimArrowYp];
    for (int j = 0; j != dimArrowYp; j++) {
      for (int i = 0; i != dimArrowY; i++) arrowPix[i][j] = false;
    }
    int iCA = (int) ((avY + yMax)/pixYSize - 4.0*sigY);
    int jCA = (int) ((avYp + ypMax)/pixYpSize);
    if (drawArrow) {
      int jYp = dimArrowYp/2;
      for (int i = 0; i != dimArrowY; i++) {
          arrowPix[i][jYp-1] = true; 
          arrowPix[i][jYp] = true; 
      }
      for (int i = 3*dimArrowY/4; i != dimArrowY; i++) {
	float xxx = (float) (dimArrowY - i)/(dimArrowY*0.25);
	int jMax = (int) (xxx * dimArrowYp * 0.5 * 0.75);
	for (int j = jYp - jMax-1; j != jYp + jMax+1; j++)
          if ((j >= 0) && (j < dimArrowYp))  arrowPix[i][j] = true;
       }
      
    }
/*
    if (drawArrow) { 
      for (int j = dimArrowYp-1; j != -1; j--) {
	for (int i = 0; i != dimArrowY; i++) {
	  if (arrowPix[i][j]) std::cerr <<  "*";
	  else std::cerr <<  " ";
	}
	std::cerr << endl;
      }
    }
*/
    int iArrowLowLim = iCA - dimArrowY;
    int iArrowUpLim = iCA;
    int jArrowLowLim = jCA - dimArrowYp/2;
    int jArrowUpLim = jCA + dimArrowYp/2 + 1;
//    cerr << " Arrow location " << iArrowLowLim << " " << iArrowUpLim 
//         << " " << jArrowLowLim << " " << jArrowUpLim << endl; 
    for (int j = numPixelsYp-1; j != -1; j--) { //  Start on top
      for (int i = 0; i != numPixelsY; i++) {  // From left to right.
        double aa = 256.*pixels[i][j]/ampMax;
        short iRed = 0; if (aa > 26.) iRed = (int) aa;
	short iGreen = 0;
	short iBlue = 26;
	if (aa < 128.) iBlue = (int) (0.8*aa/10 + 26.);
	else iBlue = (int) (261.0236 - 1.0236*aa);	
	if (drawArrow && (i >= iArrowLowLim) && (i < iArrowUpLim) &&
	      (j >= jArrowLowLim) && (j < jArrowUpLim) && 
	      arrowPix[i-iArrowLowLim][j-jArrowLowLim])  {
	         iBlue = 255;
		 iRed = 255;
		 iGreen = 255;
	}
	ofst1 << iRed << " " << iGreen << " " << iBlue << " ";
     }
     ofst1 << endl;
    }
    ofst1.close();
    std::string ffName(tokenName); ffName += ".gif";
    std::string cmd("/usr/bin/convert ");
    cmd += std::string(ffTmp) + std::string(" ") + ffName;
    if (debugOut) cerr << " Converting: " << cmd << endl;
    system(cmd.c_str()); 
    cmd = "/bin/rm ";
    cmd += std::string(ffTmp);
    if (debugOut) cerr << " Removing : " << cmd << endl;
    system(cmd.c_str());
    return yMax; 
}

void myPositronBunch::dumpOneParticle(std::ostream& out, double s, ElmPtr q,
                                      const std::string& rTag )  { 
				       // will use const when but fixed 
// Suppose to be const.. No const_iterator.
    for(ParticleBunch::iterator pPtr=this->begin(); 
           pPtr != this->end(); ++pPtr) { 
      if (rTag != pPtr->getTag()) continue;
      Vector state(Particle::PSD );
      q->loadPinnedCoordinates( *pPtr, state );
      
      out << " " << s;
      for (int kk=0; kk !=5; kk++) out << " " << 1.0e6*state(kk);
      out << " " << state(5) << " " << 
          pPtr->ReferenceMomentum() << std::endl;
      break;
    }
}

void myPositronBunch::dumpOneParticleMerlinParam(std::ostream& out, double s, ElmPtr  q,
                                      const std::string& rTag ) {
// Suppose to be const.. No const_iterator.
    for(ParticleBunch::iterator pPtr=this->begin(); 
           pPtr != this->end(); ++pPtr) { 
      if (rTag != pPtr->getTag()) continue;
      Vector state(Particle::PSD );
      q->loadPinnedCoordinates( *pPtr, state );
      double factz =  1.0/sqrt( ( 1.0 + state(5) ) * ( 1.0 + state(5) )
                 - state(3)*state(3) 
                 - state(4)*state(4) );
      state(3) *= factz;	       
      state(4) *= factz;	       
      out << " " << s;
      for (int kk=0; kk !=5; kk++) out << " " << 1.0e6*state(kk);
      out << " " << state(5) << " " << 
          pPtr->ReferenceMomentum() << std::endl;
      break;
    }
}

void myPositronBunch::getFromMerlin(const char *fileName, 
                                     const std::string& selectedLabel) {

    // Reset 
    this->clear();
    std::ifstream ifst1(fileName);
    int i = 0;
    char line[256];
    
    bool doPvsPActual = true;
    bool doPvsPz = true;
    while(!ifst1.eof()) {
      i++;
      Vector state(6);
      double refT, refP; // unused here...
      ifst1.getline(line, 256);
      if (i == 1) continue; // skip the first particle. 
      std::istringstream istr1(line);
      istr1.precision(10);
      istr1 >> refT;
      istr1 >> refP;
      istr1 >> state(0);
      istr1 >> state(3);
      istr1 >> state(1);
      istr1 >> state(4);
      istr1 >> state(2);
      istr1 >> state(5);
//  Change or adapt the definition of y': Chef want the normalized momentum.
     
      double factPz = 1.0;
      if (doPvsPz && doPvsPActual) factPz = sqrt( ( 1.0 + state(5) ) * ( 1.0 + state(5) )
                 - state(3)*state(3) 
                 - state(4)*state(4) );
      else if (doPvsPz) factPz = sqrt(1.0 - state(3)*state(3) - state(4)*state(4) );	 
      state(3) *= factPz;
      state(4) *= factPz;
      // temporary tag...we'll do something if need be
      std::string strLine(line);
      size_t iPosL = strLine.find("L_");
      std::ostringstream myTag;
      if (iPosL != std::string::npos) {
	 std::string t3(strLine, iPosL+2);
         std::istringstream istr2(t3);
	 int kk;
	 istr2 >> kk;
         myTag << "L_" << kk;
	 std::cout.precision(8);
	 if (kk == 21) {
           std::cout << "-----Start L_21 ------------------------------" << std::endl;
           std::cout << "------ "  << 1.0e6*state(0) << " "  << 1.0e6*state(1);
           std::cout << " "  << 1.0e6*state(2) << " "  << 1.0e6*state(3);
           std::cout << " "  << 1.0e6*state(4) << " "  << state(5) << std::endl;
           std::cout << " "  << 1.0e6*state(3)*factPz << " "  
	             << 1.0e6*state(4)*factPz << std::endl;
	   
	 }
      } else {
         myTag << "M_" << i-1;
      } 
      const std::string myTagFinal(myTag.str());
      if ((i > 3) && (selectedLabel.size() > 1) && 
                     (selectedLabel != myTagFinal)) continue; 
      
      Positron posTmp(ENominal, state );
      posTmp.setTag(myTagFinal);
      this->append(posTmp);
   }
   ifst1.close();  
   numMacroParticleAtCreation = this->size();				    
}
void myPositronBunch::getFromFile(const char *fileName, double ENom) {

    // Reset 
    this->clear();
    std::ifstream ifst1(fileName);
    int i = 0;
    char line[256];
    
    while(!ifst1.eof()) {
      i++;
      Vector state(6);
      double refT, refP; // unused here...
      ifst1.getline(line, 256);
      std::string lineCheck(line);
      if (lineCheck.size() < 5) continue;
      if (i == 1) {
	 if (lineCheck.find("x y z xp yp dPoP") == std::string::npos) {
	   std::cerr << " myPositronBunch::getFromFile, unexpected header " 
	              << " Empty bunch " << endl;
	   ifst1.close ();
	   return;
         }
         continue; // skip the first line, header. 
      } 
      std::istringstream istr1(lineCheck);
      for (int kk=0; kk !=5; kk++) {
         istr1 >> state(kk);
         state(kk) *= 1.e-6;
      }
      istr1 >> state(5);      
      Positron posTmp(ENominal, state );
      this->append( posTmp );
   }
   ifst1.close();  
   numMacroParticleAtCreation = this->size();				    
   ParticleBunch::const_iterator pPtrOne=this->begin();
   double gamFactor =  pPtrOne->ReferenceGamma();
   ENominal = pPtrOne->ReferenceEnergy();
   std::cerr << " Reference Momentum " 
             <<  pPtrOne->ReferenceMomentum() <<  
                " Reference Gamma " 
             <<  pPtrOne->ReferenceGamma() << std::endl;
   
}

