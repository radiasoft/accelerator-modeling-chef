/*
 *  Useful constants with their units
 *
 *  Leo Michelotti
 *  April 10, 1995
 */

#ifndef PHYSC_H
#define PHYSC_H

#define PH_NORM_h          4.13566727e-24     // Planck's constant [ GeV-sec ]
#define PH_NORM_hbar       6.58211889e-25     // Planck's constant [ twopi GeV-sec ]

#define PH_NORM_mp         0.93827231         // Mass of proton  [GeV/c^2]
#define PH_NORM_me         0.5110034e-3       // Mass of electron [GeV/c^2]
#define PH_NORM_mmu        0.105658387        // mass of muon [GeV/c^2]

#define PH_MKS_e           1.6021892e-19      // Proton charge   [Coulombs]
#define PH_ESU_e           4.803242e-10       // Proton charge   [esu]
#define PH_MKS_c           2.99792458e8       // Speed of light  [m/sec]
#define PH_MKS_omega_p     9.578756e7         // Proton cyclotron frequence
                                              // = e / m_p [ rad/sec/Tesla ]
#define PH_MKS_eps0        8.854187817e-12    // Epsilon nought:
                                              // permittivity of free space
                                              // Units:[F/m] or  [Amp-sec/V-m]
#define PH_MKS_re          2.8179381e-15      // Classical radius
                                              // of electron     [m]
#define PH_MKS_rp          1.534698e-18       // Classical radius
                                              // of proton       [m]
#define PH_MKS_cwe         2.426310215e-12    // Compton wavelength 
                                              // of electron     [m] 
#define PH_MKS_d_earth     1.27563e7          // Diameter of earth [m]
#define PH_MKS_r_earth     6.37815e6          // Radius of earth [m]


#define PH_MKS_r_bohr      0.5291772083e-10   // Bohr radius     [m] 

// In each of the conversion factors below, the factor
// converts a quantity from one system to another
// via multiplication.  That is, PH_CNV_x_to_y will
// convert from x units to y units, so that
//
//    Q [ y units ] = PH_CNV_x_to_y * Q [ x units ]

#define PH_CNV_eV_to_cgs   1.6021892e-12  // Energy [erg] = -- * Energy [eV]
#define PH_CNV_eV_to_mks   1.6021892e-19  // Energy [N-m] = -- * Energy [eV]
#define PH_CNV_brho_to_p   0.299792458    // p [Gev/c] = -- * B*rho [ Tesla meters ]

// Connections:
// 
// PH_CNV_eV_to_mks = PH_MKS_e = 10^-7 PH_CNV_eV_to_cgs
// 
// PH_CNV_brho_to_p = 10-9 PH_MKS_c
//
// PH_MKS_rp        = PH_MKS_re*PH_NORM_me/PH_NORM_mp

#endif // PHYSC_H
