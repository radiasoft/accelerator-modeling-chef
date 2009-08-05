/************************************************************************                                                                
*************************************************************************                                                                
*************************************************************************                                                                
******
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      PhysicsConstants.cc
******                                                                
****** 
******
**************************************************************************
*************************************************************************/

#include <basic_toolkit/PhysicsConstants.h>

double const PhysicsConstants::PH_NORM_h          = 4.13566733e-24;     // Planck's constant [ GeV-sec ]       
double const PhysicsConstants::PH_NORM_hbar       = 6.58211899e-25;     // Planck's constant [ twopi GeV-sec ] 

double const PhysicsConstants::PH_NORM_mp         = 0.93827203;         // Mass of proton    [GeV/c^2]         
double const PhysicsConstants::PH_NORM_me         = 0.51099892e-3;      // Mass of electron  [GeV/c^2]         
double const PhysicsConstants::PH_NORM_mmu        = 0.105658369;        // mass of muon      [GeV/c^2]         

double const PhysicsConstants::PH_MKS_e           = 1.602176487e-19;    // Proton charge     [Coulombs]        
double const PhysicsConstants::PH_MKS_c           = 2.99792458e8;       // Speed of light    [m/sec]           
double const PhysicsConstants::PH_MKS_h           = 6.62606896e-34;     // Planck's constant [Joule-sec]       

double const PhysicsConstants::PH_MKS_omega_p     = 9.578833753e7;      // Proton cyclotron frequence  = e / m_p [ rad/sec/Tesla ]     
double const PhysicsConstants::PH_MKS_e_div_h     = 2.417989454e14;     // e/h [Amps/Joule]                    
double const PhysicsConstants::PH_MKS_eps0        = 8.854187817e-12;    // Epsilon nought: permittivity of free space  [F/m] or  [Amp-sec/V-m]
double const PhysicsConstants::PH_MKS_re          = 2.8179402894e-15;   // Classical radius  of electron       [m] 
double const PhysicsConstants::PH_MKS_rp          = 1.5346982522e-18;   // Classical radius  of proton         [m]
double const PhysicsConstants::PH_MKS_cwe         = 2.4263102175e-12;   // Compton wavelength of electron      [m]
double const PhysicsConstants::PH_MKS_r_bohr      = 0.52917720859e-10;  // Bohr radius       [m]               

double const PhysicsConstants::PH_MKS_d_earth     = 1.27563e7;          // Diameter of earth [m]               
double const PhysicsConstants::PH_MKS_r_earth     = 6.37815e6;          // Radius of earth   [m]               

//--------------------------------------------------------//
//  In each of the conversion factors below, the factor   //
//  converts a quantity from one system to another        //
//  via multiplication.  That is, PH_CNV_x_to_y will      //
//  convert from x units to y units, so that              //
//                                                        //
//  Q [ y units ] = PH_CNV_x_to_y * Q [ x units ]         //
//--------------------------------------------------------//

double const PhysicsConstants::PH_CNV_eV_to_cgs   = 1.602176487e-12;   // Energy [erg] = -- * Energy [eV]          
double const PhysicsConstants::PH_CNV_eV_to_mks   = 1.602176487e-19;   // Energy [N-m] = -- * Energy [eV]
double const PhysicsConstants::PH_CNV_brho_to_p   = 0.299792458;       //    p [Gev/c] = -- * B*rho  [ Tesla meters ]

//----------------------------------------------------------------------------//
// Connections:                                                               //
//                                                                            //
// PH_CNV_eV_to_mks = PH_MKS_e = 10^-7 PH_CNV_eV_to_cgs                       //
//                                                                            //
// PH_CNV_brho_to_p = 10-9 PH_MKS_c                                           //
//                                                                            //
// PH_MKS_rp        = PH_MKS_re*PH_NORM_me/PH_NORM_mp                         //
//                                                                            //
// PH_MKS_re        = e^2 / (4 pi epsilon_0 mc^2)                             //
//                  = PH_MKS_e^2 / (4 pi epsilon_0 PH_NORM_me^2)              //
//----------------------------------------------------------------------------//


