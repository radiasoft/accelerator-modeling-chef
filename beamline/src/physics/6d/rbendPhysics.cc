#include "beamline.inc"

void rbend::P_NoEdge( bmlnElmnt* p_be, Particle& p )
{
 rbend* pbe = (rbend*) p_be;
 static double csq_red = PH_MKS_c * PH_MKS_c * 1.0e-9;

 // Preliminary filter from state coordinates
 double psq = 1.0 + p.state[5];
        psq = psq*psq;
 double E_factor = 1.0 / sqrt( psq + p.pni2 );
 double beta_1 = E_factor*p.state[3];
 double beta_2 = E_factor*p.state[4];
 double beta_3 = E_factor*sqrt( psq - p.state[3]*p.state[3]
                                    - p.state[4]*p.state[4] );

 double Rho    = p.bRho / pbe->strength;        // Fiducial parameters
 double dphi   = 0.0;
 double Gamma  = p.gamma;
 double Omega  = csq_red * pbe->strength / p.E;

 Complex ui  ( 0.0, p.state[0] );
 Complex vui ( PH_MKS_c*beta_3, PH_MKS_c*beta_1 );

 // Step 1.
 double gamma = p.Energy()/p.Mass();
 double omega  = csq_red * pbe->strength / p.Energy();
 Complex bi = ( complex_i*vui / omega ) - ui;

 // Step 2.
 Complex bf = bi + pbe->length;
 
 // Step 3.
 double rho = PH_MKS_c * sqrt( beta_1*beta_1 + beta_3*beta_3 ) / omega;
 double dthmdphi = asin( real(bi)/rho ) - asin( real(bf)/rho );

 // Step 4.
 Complex expFactor( cos(dthmdphi), sin(dthmdphi) );
 Complex vuf = vui*expFactor;
 Complex uf  = ( ui + bi )*expFactor - bf;


 // Final filter back to state coordinates
 double dtheta = dthmdphi + dphi;
 double cdt    = - PH_MKS_c * dtheta / omega;
 double CDT    = - PH_MKS_c * ( - 2.0*asin( pbe->length / (2.0*Rho) ) ) / Omega;

 p.state[0] = imag( uf );
 p.state[1] = p.state[1] + beta_2*cdt;
 p.state[2] = p.state[2] + ( cdt - CDT );
 p.state[3] = imag( vuf )/( E_factor * PH_MKS_c );
}

void rbend::J_NoEdge( bmlnElmnt* p_be, JetParticle& p ) 
{
 rbend* pbe = (rbend*) p_be;
 static double csq_red = PH_MKS_c * PH_MKS_c * 1.0e-9;

 // Preliminary filter from state coordinates
 Jet psq;
 psq = 1.0 + p.state(5);
 psq = psq*psq;

 Jet E_factor;
 E_factor = 1.0 / sqrt( psq + p.pni2 );

 Jet beta_1, beta_2, beta_3;
 beta_1 = E_factor*p.state(3);
 beta_2 = E_factor*p.state(4);
 beta_3 = E_factor*sqrt( psq - p.state(3)*p.state(3)
                             - p.state(4)*p.state(4) );

 double Rho    = p.bRho / pbe->strength;        // Fiducial parameters
 double dphi   = 0.0;
 double Gamma  = p.gamma;
 double Omega  = csq_red * pbe->strength / p.E;

 JetC ui, vui;
 ui  = complex_i * p.state(0);
 vui = PH_MKS_c*( beta_3 + complex_i * beta_1 );

 // Step 1.
 Jet gamma, omega;
 JetC bi;
 gamma = p.Energy()/p.Mass();
 omega  = csq_red * pbe->strength / p.Energy();
 bi = ( complex_i*vui / omega ) - ui;

 // Step 2.
 JetC bf;
 bf = bi + pbe->length;
 // Step 3.
 Jet rho, dthmdphi;
 rho      = PH_MKS_c * sqrt( beta_1*beta_1 + beta_3*beta_3 ) / omega;
 dthmdphi = asin( real(bi)/rho ) - asin( real(bf)/rho );

 // Step 4.
 JetC expF, vuf, uf;
 expF = cos(dthmdphi) + complex_i*sin(dthmdphi);
 vuf  = vui*expF;
 uf   = ( ui + bi )*expF - bf;


 // Final filter back to state coordinates
 Jet dtheta, cdt;
 double CDT; 
 dtheta = dthmdphi + dphi;
 cdt    = - PH_MKS_c * dtheta / omega;
 CDT    = - PH_MKS_c * ( - 2.0*asin( pbe->length / (2.0*Rho) ) ) / Omega;

 ( p.state ).SetComponent( 0, imag( uf )                           );
 ( p.state ).SetComponent( 1, p.state(1) + beta_2*cdt              );
 ( p.state ).SetComponent( 2, p.state(2) + ( cdt - CDT )           );
 ( p.state ).SetComponent( 3, imag( vuf )/( E_factor * PH_MKS_c )  );
}

enum EDGE_CASE {EDGECASE_1,EDGECASE_2,EDGECASE_3,EDGECASE_4,EDGECASE_5};

void rbend::P_Exact( bmlnElmnt* p_be, Particle& p )
{
  rbend::P_InFace(p_be,p);
  rbend::P_NoEdge(p_be,p);
  rbend::P_OutFace(p_be,p);
}

void rbend::J_Exact( bmlnElmnt* p_be, JetParticle& p )
{
  rbend::J_InFace(p_be,p);
  rbend::J_NoEdge(p_be,p);
  rbend::J_OutFace(p_be,p);
}

void rbend::P_InEdge( bmlnElmnt* p_be, Particle& p )
{
  rbend::P_InFace(p_be,p);
  rbend::P_NoEdge(p_be,p);
}

void rbend::J_InEdge( bmlnElmnt* p_be, JetParticle& p )
{
  rbend::J_InFace(p_be,p);
  rbend::J_NoEdge(p_be,p);
}

void rbend::P_OutEdge( bmlnElmnt* p_be, Particle& p )
{
  rbend::P_NoEdge(p_be,p);
  rbend::P_OutFace(p_be,p);
}

void rbend::J_OutEdge( bmlnElmnt* p_be, JetParticle& p )
{
  rbend::J_NoEdge(p_be,p);
  rbend::J_OutFace(p_be,p);
}


void rbend::P_InFace( bmlnElmnt* p_be, Particle& p )
{
 rbend* pbe = (rbend*) p_be;
 EDGE_CASE whichCase;

 double Rho    = p.ReferenceBRho() / pbe->strength;        // Fiducial parameters
 
 double TrState[6];
 int i;

 if( pbe->align ) {
   pbe->align->misalign( p, pbe->geometry /* ??? */, TrState );
   p.setState(TrState);
 } else {
   for( i = 0; i < 6; i++  ) 
     TrState[i] = p.state[i];
 }
 double thetaOver2;
 if(pbe->poleFaceAngle == 2.0 * M_PI)
   thetaOver2 = asin(pbe->Length()/(2.0*Rho));
 else
   thetaOver2 = pbe->poleFaceAngle;


 double xpr, ypr, p3divpbar;
 p3divpbar = sqrt( pow( 1.0 + TrState[5], 2.0 )
                 - TrState[3]*TrState[3] 
                 - TrState[4]*TrState[4] );
 xpr = TrState[3] / p3divpbar; 
 ypr = TrState[4] / p3divpbar; 
 p3divpbar = ( 1.0 + TrState[5] ) / sqrt( 1.0 + xpr*xpr + ypr*ypr );

 double psi = xpr;
 double r = p.state[0]/cos(thetaOver2);
 double b = r * sin(thetaOver2);
 double backLength = 0.0;
 double xLength = 0.0;
 double yLength = 0.0;
 double phi = M_PI/2.0 - thetaOver2;

 if(((pbe->strength >= 0.0) && (p.Charge() >= 0.0)) ||
    (pbe->strength <= 0.0) && (p.Charge() <= 0.0)) {
   if(p.state[0] < 0.0 && psi < 0.0)
     whichCase = EDGECASE_1;
   else if(p.state[0] < 0.0 && psi > 0.0)
     whichCase = EDGECASE_2;
   else if(p.state[0] > 0.0 && psi < 0.0)
     whichCase = EDGECASE_3;
   else if(p.state[0] > 0.0 && psi > 0.0)
     whichCase = EDGECASE_4;
   else if(p.state[0] == 0.0)
     whichCase = EDGECASE_5;
 } else {
   if(p.state[0] < 0.0 && psi < 0.0)
     whichCase = EDGECASE_4;
   else if(p.state[0] < 0.0 && psi > 0.0)
     whichCase = EDGECASE_3;
   else if(p.state[0] > 0.0 && psi < 0.0)
     whichCase = EDGECASE_2;
   else if(p.state[0] > 0.0 && psi > 0.0)
     whichCase = EDGECASE_1;
   else if(p.state[0] == 0.0)
     whichCase = EDGECASE_5;

 }
				// Calculate the length needed to get to the real
				// magnet face (can be positive or negative).
 switch(whichCase) {
 case EDGECASE_1:
   xLength = b * sin(phi)/sin(M_PI-(phi+ psi));
   yLength = xLength * tan(ypr);
   backLength = sqrt(xLength*xLength + yLength*yLength);
   if(backLength > 0.0)
     backLength *= -1.0;
   break;
 case EDGECASE_2:
   xLength = b * sin(M_PI - phi)/sin(phi - psi);
   yLength = xLength * tan(ypr);
   backLength = sqrt(xLength*xLength + yLength*yLength);
   if(backLength > 0.0)
     backLength *= -1.0;
   break;
 case EDGECASE_3:
   xLength = b * sin(phi)/sin(M_PI-(phi+ psi));
   yLength = xLength * tan(ypr);
   backLength = sqrt(xLength*xLength + yLength*yLength);
   if(backLength < 0.0)
     backLength *= -1.0;
   break;
 case EDGECASE_4:
   xLength = b * sin(M_PI - phi)/sin(phi - psi);
   yLength = xLength * tan(ypr);
   backLength = sqrt(xLength*xLength + yLength*yLength);
   if(backLength < 0.0)
     backLength *= -1.0;
   break;
 case EDGECASE_5:
   backLength = 0.0;
   break;
 default:
   break;
 }

				// Propagate to the magnet face.
 if(backLength != 0.0) {
   drift tmpDrift(backLength);
   tmpDrift.propagate(p);
 }
				// Put in a kludge for the vertical focusing.

 p.state[4] -= tan(thetaOver2)/Rho * p.state[1];

				// Rotate momentum into the coord of the magnet.
 double newPx = p.state[3] * cos(thetaOver2) + p3divpbar*sin(thetaOver2);
 double newPz;
 p.state[3] = newPx;
}

void rbend::J_InFace(bmlnElmnt* p_be,JetParticle& p) {
 rbend* pbe = (rbend*) p_be;
 EDGE_CASE whichCase;

 double Rho    = p.ReferenceBRho() / pbe->strength;        // Fiducial parameters

 double thetaOver2;
 if(pbe->poleFaceAngle == 2.0 * M_PI)
   thetaOver2 = asin(pbe->Length()/(2.0*Rho));
 else
   thetaOver2 = pbe->poleFaceAngle;

 Jet TrState[6];
 int i;

 if( pbe->align ) {
   pbe->align->misalign( p, pbe->geometry /* ??? */, TrState );
   for(i=0; i < 6; i++)
     ( p.state ).SetComponent( i, TrState[i] );
 } else {
   for( i = 0; i < 6; i++  ) 
     TrState[i] = p.State(i);
 }


 Jet xpr, ypr, p3divpbar;
 p3divpbar = sqrt( pow( 1.0 + TrState[5], 2.0 )
                 - TrState[3]*TrState[3] 
                 - TrState[4]*TrState[4] );
 xpr = TrState[3] / p3divpbar; 
 ypr = TrState[4] / p3divpbar; 
 p3divpbar = ( 1.0 + TrState[5] ) / sqrt( 1.0 + xpr*xpr + ypr*ypr );

 double psi = xpr.standardPart();
 double r = p.state(0).standardPart()/cos(thetaOver2);
 double b = r * sin(thetaOver2);
 double backLength = 0.0;
 double xLength = 0.0;
 double yLength = 0.0;
 double phi = M_PI/2.0 - thetaOver2;

 if(((pbe->strength >= 0.0) && (p.Charge() >= 0.0)) ||
    (pbe->strength <= 0.0) && (p.Charge() <= 0.0)) {
   if(p.state(0).standardPart() < 0.0 && psi < 0.0)
     whichCase = EDGECASE_1;
   else if(p.state(0).standardPart() < 0.0 && psi > 0.0)
     whichCase = EDGECASE_2;
   else if(p.state(0).standardPart() > 0.0 && psi < 0.0)
     whichCase = EDGECASE_3;
   else if(p.state(0).standardPart() > 0.0 && psi > 0.0)
     whichCase = EDGECASE_4;
   else if(p.state(0).standardPart() == 0.0)
     whichCase = EDGECASE_5;
 } else {
   if(p.state(0).standardPart() < 0.0 && psi < 0.0)
     whichCase = EDGECASE_4;
   else if(p.state(0).standardPart() < 0.0 && psi > 0.0)
     whichCase = EDGECASE_3;
   else if(p.state(0).standardPart() > 0.0 && psi < 0.0)
     whichCase = EDGECASE_2;
   else if(p.state(0).standardPart() > 0.0 && psi > 0.0)
     whichCase = EDGECASE_1;
   else if(p.state(0).standardPart() == 0.0)
     whichCase = EDGECASE_5;

 }
				// Calculate the length needed to get to the real
				// magnet face (can be positive or negative).
 switch(whichCase) {
 case EDGECASE_1:
   xLength = b * sin(phi)/sin(M_PI-(phi+ psi));
   yLength = xLength * tan(ypr.standardPart());
   backLength = sqrt(xLength*xLength + yLength*yLength);

   if(backLength > 0.0)
     backLength *= -1.0;
   break;
 case EDGECASE_2:
   xLength = b * sin(M_PI - phi)/sin(phi - psi);
   yLength = xLength * tan(ypr.standardPart());
   backLength = sqrt(xLength*xLength + yLength*yLength);

   if(backLength > 0.0)
     backLength *= -1.0;
   break;
 case EDGECASE_3:
   xLength = b * sin(phi)/sin(M_PI-(phi+ psi));
   yLength = xLength * tan(ypr.standardPart());
   backLength = sqrt(xLength*xLength + yLength*yLength);

   if(backLength < 0.0)
     backLength *= -1.0;
   break;
 case EDGECASE_4:
   xLength = b * sin(M_PI - phi)/sin(phi - psi);
   yLength = xLength * tan(ypr.standardPart());
   backLength = sqrt(xLength*xLength + yLength*yLength);

   if(backLength < 0.0)
     backLength *= -1.0;
   break;
 case EDGECASE_5:
   backLength = 0.0;
   break;
 default:
   break;
 }
				// Propagate to the magnet face.
 if(backLength != 0.0) {
   drift tmpDrift(backLength);
   tmpDrift.propagate(p);
 }
 Jet newPx = p.state(3) * cos(thetaOver2) + p3divpbar*sin(thetaOver2);
 p.state(3) = newPx;

 p.state(4) = p.state(4)  - tan(thetaOver2)/Rho * p.state(1);
}
void rbend::P_OutFace( bmlnElmnt* p_be, Particle& p )
{
 rbend* pbe = (rbend*) p_be;
 EDGE_CASE whichCase;

 double Rho    = p.ReferenceBRho() / pbe->strength;        // Fiducial parameters

 double thetaOver2;
 if(pbe->poleFaceAngle == 2.0 * M_PI)
   thetaOver2 = asin(pbe->Length()/(2.0*Rho));
 else
   thetaOver2 = pbe->poleFaceAngle;

 double TrState[6];
 int i;


 for( i = 0; i < 6; i++  ) 
   TrState[i] = p.state[i];

 double xpr, ypr, p3divpbar;
 p3divpbar = sqrt( pow( 1.0 + TrState[5], 2.0 )
                 - TrState[3]*TrState[3] 
                 - TrState[4]*TrState[4] );
 xpr = TrState[3] / p3divpbar; 
 ypr = TrState[4] / p3divpbar; 
 p3divpbar = ( 1.0 + TrState[5] ) / sqrt( 1.0 + xpr*xpr + ypr*ypr );

 double psi = xpr;
 double r = p.state[0]/cos(thetaOver2);
 double b = r * sin(thetaOver2);
 double backLength = 0.0;
 double xLength = 0.0;
 double yLength = 0.0;
 double phi = M_PI/2.0 - thetaOver2;


				// Rotate momentum out of the coord of the magnet.
 for( i = 0; i < 6; i++  ) 
   TrState[i] = p.state[i];
 p3divpbar = sqrt( pow( 1.0 + TrState[5], 2.0 )
                 - TrState[3]*TrState[3] 
                 - TrState[4]*TrState[4] );
 xpr = TrState[3] / p3divpbar; 
 ypr = TrState[4] / p3divpbar; 
 p3divpbar = ( 1.0 + TrState[5] ) / sqrt( 1.0 + xpr*xpr + ypr*ypr );

 double newPx = p.state[3] * cos(thetaOver2) + p3divpbar*sin(thetaOver2);
 p.state[3] = newPx;
 psi = xpr;

				// Setup to propagate to the begining of the drift.

 if(((pbe->strength >= 0.0) && (p.Charge() >= 0.0)) ||
    (pbe->strength <= 0.0) && (p.Charge() <= 0.0)) {
   if(p.state[0] < 0.0 && psi < 0.0)
     whichCase = EDGECASE_2;
   else if(p.state[0] < 0.0 && psi > 0.0)
     whichCase = EDGECASE_1;
   else if(p.state[0] > 0.0 && psi < 0.0)
     whichCase = EDGECASE_4;
   else if(p.state[0] > 0.0 && psi > 0.0)
     whichCase = EDGECASE_3;
   else if(p.state[0] == 0.0)
     whichCase = EDGECASE_5;
 } else {
   if(p.state[0] < 0.0 && psi < 0.0)
     whichCase = EDGECASE_3;
   else if(p.state[0] < 0.0 && psi > 0.0)
     whichCase = EDGECASE_4;
   else if(p.state[0] > 0.0 && psi < 0.0)
     whichCase = EDGECASE_1;
   else if(p.state[0] > 0.0 && psi > 0.0)
     whichCase = EDGECASE_2;
   else if(p.state[0] == 0.0)
     whichCase = EDGECASE_5;

 }
				// Calculate the length needed to get to the real
				// magnet face (can be positive or negative).
 switch(whichCase) {
 case EDGECASE_1:
   xLength = b * sin(phi)/sin(M_PI-(phi+ psi));
   yLength = xLength * tan(ypr);
   backLength = sqrt(xLength*xLength + yLength*yLength);
   if(backLength > 0.0)
     backLength *= -1.0;
   break;
 case EDGECASE_2:
   xLength = b * sin(M_PI - phi)/sin(phi - psi);
   yLength = xLength * tan(ypr);
   backLength = sqrt(xLength*xLength + yLength*yLength);
   if(backLength > 0.0)
     backLength *= -1.0;
   break;
 case EDGECASE_3:
   xLength = b * sin(phi)/sin(M_PI-(phi+ psi));
   yLength = xLength * tan(ypr);
   backLength = sqrt(xLength*xLength + yLength*yLength);
   if(backLength < 0.0)
     backLength *= -1.0;
   break;
 case EDGECASE_4:
   xLength = b * sin(M_PI - phi)/sin(phi - psi);
   yLength = xLength * tan(ypr);
   backLength = sqrt(xLength*xLength + yLength*yLength);
   if(backLength < 0.0)
     backLength *= -1.0;
   break;
 case EDGECASE_5:
   backLength = 0.0;
   break;
 default:
   break;
 }
				// Propagate to the drift face.
 if(backLength != 0.0) {
   drift tmpDrift(backLength);
   tmpDrift.propagate(p);
 }
 p.state[4] -= tan(thetaOver2)/Rho * p.state[1];

 if( pbe->align ) {
   pbe->align->align( p, pbe->geometry /* ??? */, TrState );
   p.setState(TrState);
 }
}


void rbend::J_OutFace(bmlnElmnt* p_be,JetParticle& p) {
 rbend* pbe = (rbend*) p_be;
 EDGE_CASE whichCase;

 double Rho    = p.ReferenceBRho() / pbe->strength;        // Fiducial parameters

 double thetaOver2;
 if(pbe->poleFaceAngle == 2.0 * M_PI)
   thetaOver2 = asin(pbe->Length()/(2.0*Rho));
 else
   thetaOver2 = pbe->poleFaceAngle;


 Jet TrState[6];
 int i;
 for( i = 0; i < 6; i++  ) 
   TrState[i] = p.State(i);



 Jet xpr, ypr, p3divpbar;
 p3divpbar = sqrt( pow( 1.0 + TrState[5], 2.0 )
                 - TrState[3]*TrState[3] 
                 - TrState[4]*TrState[4] );
 xpr = TrState[3] / p3divpbar; 
 ypr = TrState[4] / p3divpbar; 
 p3divpbar = ( 1.0 + TrState[5] ) / sqrt( 1.0 + xpr*xpr + ypr*ypr );

 double psi = xpr.standardPart();
 double r = p.state(0).standardPart()/cos(thetaOver2);
 double b = r * sin(thetaOver2);
 double backLength = 0.0;
 double xLength = 0.0;
 double yLength = 0.0;
 double phi = M_PI/2.0 - thetaOver2;


				// Rotate momentum out of the coord of the magnet.
 for( i = 0; i < 6; i++  ) 
   TrState[i] = p.State(i);
 p3divpbar = sqrt( pow( 1.0 + TrState[5], 2.0 )
                 - TrState[3]*TrState[3] 
                 - TrState[4]*TrState[4] );
 xpr = TrState[3] / p3divpbar; 
 ypr = TrState[4] / p3divpbar; 
 p3divpbar = ( 1.0 + TrState[5] ) / sqrt( 1.0 + xpr*xpr + ypr*ypr );

 Jet newPx = p.state(3) * cos(thetaOver2) + p3divpbar*sin(thetaOver2);
 p.state(3) = newPx;
 psi = xpr.standardPart();

 if(((pbe->strength >= 0.0) && (p.Charge() >= 0.0)) ||
    (pbe->strength <= 0.0) && (p.Charge() <= 0.0)) {
   if(p.state(0).standardPart() < 0.0 && psi < 0.0)
     whichCase = EDGECASE_2;
   else if(p.state(0).standardPart() < 0.0 && psi > 0.0)
     whichCase = EDGECASE_1;
   else if(p.state(0).standardPart() > 0.0 && psi < 0.0)
     whichCase = EDGECASE_4;
   else if(p.state(0).standardPart() > 0.0 && psi > 0.0)
     whichCase = EDGECASE_3;
   else if(p.state(0).standardPart() == 0.0)
     whichCase = EDGECASE_5;
 } else {
   if(p.state(0).standardPart() < 0.0 && psi < 0.0)
     whichCase = EDGECASE_3;
   else if(p.state(0).standardPart() < 0.0 && psi > 0.0)
     whichCase = EDGECASE_4;
   else if(p.state(0).standardPart() > 0.0 && psi < 0.0)
     whichCase = EDGECASE_1;
   else if(p.state(0).standardPart() > 0.0 && psi > 0.0)
     whichCase = EDGECASE_2;
   else if(p.state(0).standardPart() == 0.0)
     whichCase = EDGECASE_5;

 }
				// Calculate the length needed to get to the real
				// magnet face (can be positive or negative).
 switch(whichCase) {
 case EDGECASE_1:
   xLength = b * sin(phi)/sin(M_PI-(phi+ psi));
   yLength = xLength * tan(ypr.standardPart());
   backLength = sqrt(xLength*xLength + yLength*yLength);
   if(backLength > 0.0)
     backLength *= -1.0;
   break;
 case EDGECASE_2:
   xLength = b * sin(M_PI - phi)/sin(phi - psi);
   yLength = xLength * tan(ypr.standardPart());
   backLength = sqrt(xLength*xLength + yLength*yLength);
   if(backLength > 0.0)
     backLength *= -1.0;
   break;
 case EDGECASE_3:
   xLength = b * sin(phi)/sin(M_PI-(phi+ psi));
   yLength = xLength * tan(ypr.standardPart());
   backLength = sqrt(xLength*xLength + yLength*yLength);
   if(backLength < 0.0)
     backLength *= -1.0;
   break;
 case EDGECASE_4:
   xLength = b * sin(M_PI - phi)/sin(phi - psi);
   yLength = xLength * tan(ypr.standardPart());
   backLength = sqrt(xLength*xLength + yLength*yLength);
   if(backLength < 0.0)
     backLength *= -1.0;
   break;
 case EDGECASE_5:
   backLength = 0.0;
   break;
 default:
   break;
 }
				// Propagate to the drift face.
 if(backLength != 0.0) {
   drift tmpDrift(backLength);
   tmpDrift.propagate(p);
 }
 p.state(4) = p.state(4)  - tan(thetaOver2)/Rho * p.state(1);

 if( pbe->align ) {
   pbe->align->align( p, pbe->geometry /* ??? */, TrState );
   for(i=0; i < 6; i++)
     ( p.state ).SetComponent( i, TrState[i] );
 }

}
