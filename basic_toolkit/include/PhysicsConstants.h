/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      PhysicsConstants.h
******                                                                
******  Copyright (c) Fermi Research Alliance
******                Universities Research Association, Inc.
******                Fermilab
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms
******  of the License supplied with this software.
******
******  Software and documentation created under
******  U.S. Department of Energy Contracts No. DE-AC02-76CH03000
******  and No. DE-AC02-07CH11359.
******
******  The U.S. Government retains a world-wide non-exclusive,
******  royalty-free license to publish or reproduce documentation
******  and software for U.S. Government purposes. This software
******  is protected under the U.S. and Foreign Copyright Laws.
******
******
******  Original author: Leo Michelotti  (michelotti@fnal.gov)
******
******  ----------------
******  REVISION HISTORY
******  ----------------
******
******  Apr 1995       michelotti@fnal.gov
******  - Original version. Numerical values of all physical
******    constants used by the software libraries.
******  
******  Oct 2007       michelotti@fnal.gov
******  - Updated numbers using LBL Particle Data Group (PDG)
******    2006 Summary Tables and most recent NIST values.
******  
******  Aug 2013       Eric G. Stern  (egstern@fnal.gov)
******  - Preprocessor directive selects which set of 
******    constants to use. Default to PDG 2010 values.
******  - Interdependent quantities calculated rather than
******    entered independently, with priority given to
******    speed of light and mu_0.
******  
******  Apr 2014       James Amundson (amundson@fnal.gov)
******  - Use 2012 PDG values.
******  
**************************************************************************
*************************************************************************/

/*
 *  Useful constants with their units
 *
 *  Leo Michelotti
 *  April 10, 1995
 */

#include <basic_toolkit/MathConstants.h>   // needed for M_PI and M_TWOPI

#ifndef PHYSC_H
#define PHYSC_H

#define PDG_VERSION 2012

// Planck's constant
// -----------------
#define PH_MKS_h           6.62606957e-34     /* Planck's constant [ Joule-sec ]     */
#define PH_CGS_h           (PH_MKS_h*1.0e7)   /* Planck's constant [ erg-sec ]       */
#define PH_NORM_h (PH_MKS_h/(PH_MKS_e*1.0e9)) /* Planck's constant [ GeV-sec ]       */
/*                 -- approx. 4.13567e-24 GeV-sec --                                 */
#define PH_MKS_hbar       (PH_MKS_h/M_TWOPI)  /* Planck's constant [ twopi J-sec]    */
#define PH_CGS_hbar       (PH_CGS_h/M_TWOPI)  /* Planck's constant [ twopi erg-sec]  */
#define PH_NORM_hbar      (PH_NORM_h/M_TWOPI) /* Planck's constant [ twopi GeV-sec ] */



#if PDG_VERSION == 2012
#define PH_NORM_me         0.510998928e-3     /* Mass of electron  [GeV/c^2]         */
#define PH_NORM_mmu        0.1056583715       /* mass of muon      [GeV/c^2]         */
#define PH_MKS_e           1.602176565e-19    /* Proton charge     [Coulombs]        */
#define PH_NORM_mp         0.938272046        /* Mass of proton    [GeV/c^2]         */

#else
#if PDG_VERSION == 2010 // use PDG_VERSION 2010 values

#define PH_NORM_mp         0.938272013        /* Mass of proton    [GeV/c^2]         */
#define PH_NORM_me         0.510998910e-3     /* Mass of electron  [GeV/c^2]         */
#define PH_NORM_mmu        0.105658367        /* mass of muon      [GeV/c^2]         */
#define PH_MKS_e           1.602176487e-19    /* Proton charge     [Coulombs]        */
#else
#if PDG_VERSION == 2008 // use PDG_VERSION 2008 value

#define PH_NORM_mp         0.938272013        /* Mass of proton    [GeV/c^2]         */
#define PH_NORM_me         0.510998910e-3     /* Mass of electron  [GeV/c^2]         */
#define PH_NORM_mmu        0.105658367        /* mass of muon      [GeV/c^2]         */
#define PH_MKS_e           1.602176487e-19    /* Proton charge     [Coulombs]        */

#else
#if PDG_VERSION == -1 // legacy

#define PH_NORM_mp         0.93827203         /* Mass of proton    [GeV/c^2]         */
#define PH_NORM_me         0.51099892e-3      /* Mass of electron  [GeV/c^2]         */
#define PH_NORM_mmu        0.105658369        /* mass of muon      [GeV/c^2]         */
#define PH_MKS_e           1.602176487e-19    /* Proton charge     [Coulombs]        */

#else // legacy
#error "No selection for physics_constants PDG_VERSION set"

#endif // legacy
#endif // 2008
#endif // 2010
#endif // 2012

#define PH_MKS_c           2.99792458e8       /* Speed of light    [m/sec] (exact)    */
#define PH_MKS_h           6.62606957e-34     /* Planck's constant [Joule-sec]        */

#define PH_MKS_omega_p     ((PH_MKS_c*PH_MKS_c)/(PH_MKS_mp*1.0e9))  /* Proton cyclotron frequence    */
                                                                    /* = e / m_p [rad/sec/Tesla]     */
#define PH_MKS_e_div_h     (PH_MKS_e/PH_MKS_h)                      /* = e/h     [Amps/Joule]        */
#define PH_MKS_mu0         (4.0*M_PI*1.0e-7)                        /* permittivity of free space    */ 
#define PH_MKS_eps0        (1.0/(PH_MKS_c*PH_MKS_C*PH_MKS_mu0))     /* Epsilon nought                */
                                                                    /* permeability of free space    */
                                                                    /* Units: [F/m] or [Amp-sec/V-m] */
/*
 * classical radius of particle with mass m
 *  e^2/(4 pi epsilon_0 m c**2)
 * if m is in GeV/c**2 becomes
 * e / (4 pi epsilon_0 m 1.0e9)
 * but
 * 1/(4 pi epsilon0) is 1.0e-7 * c**2
 * so this is
 * e * 1.0e-16 * c**2 / m
 */

#define PH_MKS_re    (1.0e-16 * PH_MKS_e*PH_MKS_c*PH_MKS_c/PH_NORM_me)  /* Classical radius   */
                                                                        /* of electron  [m]   */
#define PH_MKS_rp    (1.0e-16 * PH_MKS_e*PH_MKS_c*PH_MKS_c/PH_NORM_mp)  /* Classical radius   */
                                                                        /* of proton    [m]   */
#define PH_MKS_cwe         2.4263102175e-12     /* Compton wavelength     */
                                                /* of electron       [m]  */
#define PH_MKS_r_bohr      0.52917720859e-10    /* Bohr radius       [m]  */

#define PH_MKS_r_earth     6.37815e6            /* Radius of earth   [m]  */
#define PH_MKS_d_earth     (2.0*PH_MKS_r_earth) /* Diameter of earth [m]  */



/* In each of the conversion factors below, the factor   */
/* converts a quantity from one system to another        */
/* via multiplication.  That is, PH_CNV_x_to_y will      */
/* convert from x units to y units, so that              */
/*                                                       */
/*    Q [ y units ] = PH_CNV_x_to_y * Q [ x units ]      */

#define PH_CNV_eV_to_cgs   (PH_MKS_E * 1.0e7)   /* Energy [erg] = -- * Energy [eV]          */
#define PH_CNV_eV_to_mks   PH_MKS_e             /* Energy [N-m] = -- * Energy [eV]          */
#define PH_CNV_brho_to_p   (1.0e-9 * PH_MKS_c)  /* p [Gev/c] = -- * B*rho [ Tesla meters ]  */

/* Connections:                                                               */
/*                                                                            */
/* PH_CNV_eV_to_mks = PH_MKS_e = 10^-7 PH_CNV_eV_to_cgs                       */
/*                                                                            */
/* PH_CNV_brho_to_p = 10-9 PH_MKS_c                                           */
/*                                                                            */
/* PH_MKS_rp        = PH_MKS_re*PH_NORM_me/PH_NORM_mp                         */
/*                                                                            */
/* PH_MKS_re        = e^2 / (4 pi epsilon_0 mc^2)                             */
/*                  = PH_MKS_e^2 / (4 pi epsilon_0 PH_NORM_me^2)              */

#endif // PHYSC_H
