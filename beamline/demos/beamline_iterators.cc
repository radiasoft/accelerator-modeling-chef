//------------------------------------------------------------------------------------------------------
//
// FILE:   beamline_iterators.cc
// AUTHOR: Jean-Francois Ostiguy ostiguy@fnal.gov
// 
// demo of STL compliant iterators for the beamline class
// 
//------------------------------------------------------------------------------------------------------
#include <iostream>
#include <beamline/bmlnElmnt.h>
#include <beamline/beamline.h>
#include <beamline/BeamlineIterator.h>
#include <bmlfactory/bmlfactory.h>
#include <beamline/Particle.h>

int main(int argc, const char* argv[] ) {

  double energy =150.0;

  //bmlfactory bfact("Tevatron-E.lat");
bmlfactory bfact("tevgen2_v3h01v2_new_new.lat"); 

Proton pr(energy);

std::cout << "Reference BRho = ", pr.ReferenceBRho();

//beamline* tevatron = bfact.create_beamline("normal_tevatron", pr.ReferenceBRho() );

beamline* tevatron = bfact.create_beamline("TEVE0",pr.ReferenceBRho() ); 

int nelm = tevatron->countHowManyDeeply();

 std::cout << "nelm = " << nelm << std::endl;


 bmlnElmnt* elm = 0;



#if 1
 std::cout << "=======================================================================" << std::endl;
 std::cout << "new style pre-order iteration " << std::endl;
 std::cout << "=======================================================================" << std::endl;

 for ( beamline::pre_order_iterator it = tevatron->pre_begin(); it != tevatron->pre_end(); ++it ) {

   std::cout << (*it)->Name() << std::endl;
 }
#endif

#if 1
 std::cout << "=======================================================================" << std::endl;
 std::cout << "new-style reverse pre_order iteration " << std::endl;
 std::cout << "=======================================================================" << std::endl;

 for ( beamline::reverse_pre_order_iterator it = tevatron->rpre_begin(); it != tevatron->rpre_end(); ++it ) {

   std::cout << (*it)->Name() << std::endl;
 }

#endif

#if 1
 std::cout << "=======================================================================" << std::endl;
 std::cout << "new style post-order iteration " << std::endl;
 std::cout << "=======================================================================" << std::endl;

 for ( beamline::post_order_iterator it = tevatron->post_begin(); it != tevatron->post_end(); ++it ) {

   std::cout << (*it)->Name() << std::endl;
 }
#endif

#if 1
 std::cout << "=======================================================================" << std::endl;
 std::cout << "new-style reverse post_order iteration " << std::endl;
 std::cout << "=======================================================================" << std::endl;

 for ( beamline::reverse_post_order_iterator it = tevatron->rpost_begin(); it != tevatron->rpost_end(); ++it ) {

   std::cout << (*it)->Name() << std::endl;
 }

#endif

#if 1
 std::cout << "=======================================================================" << std::endl;
 std::cout << "new-style deep iteration " << std::endl;
 std::cout << "=======================================================================" << std::endl;

 for ( beamline::deep_iterator it = tevatron->deep_begin(); it != tevatron->deep_end(); ++it ) {

   std::cout << (*it)->Name() << std::endl;
 }

#endif

#if 1
 std::cout << "=======================================================================" << std::endl;
 std::cout << "new-style reverse deep iteration " << std::endl;
 std::cout << "=======================================================================" << std::endl;

 for ( beamline::reverse_deep_iterator it = tevatron->rdeep_begin(); it != tevatron->rdeep_end(); ++it ) {

   std::cout << (*it)->Name() << std::endl;
 }
#endif


return 0;

}
