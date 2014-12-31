//------------------------------------------------------------------------------------------------------
//
// FILE:   beamline_iterators.cc
// AUTHOR: Jean-Francois Ostiguy ostiguy@fnal.gov
// 
// demo of STL compliant iterators for the beamline class
// 
//------------------------------------------------------------------------------------------------------
#include <iostream>

#include <beamline/quadrupole.h>
#include <beamline/drift.h>
#include <beamline/beamline.h>
#include <beamline/Particle.h>

int main(int argc, const char* argv[] ) 
{
  double energy = 150.0; // GeV
  Proton pr(energy);

  double length = 10.0;
  drift OO ( "OO", length );
  drift O  ( "O",  length/2.0 );

  double focalLength = 7.0;
  double strength = pr.ReferenceBRho() / focalLength;
  thinQuad F ( "F",   strength );
  thinQuad D ( "D", - strength );

  beamline cell;
           cell.append( F );
           cell.append( OO );
           cell.append( D );
           cell.append( O );
           cell.append( O );

  BmlPtr model( new beamline( "model" ) );
  for( int i = 0; i < 4; ++i ) {
    model->append(cell);
  }

  std::cout << "Number of elements = " 
            << model->countHowManyDeeply()
            << std::endl;

  bmlnElmnt* elm = 0;

  #if 1
  std::cout << "=======================================================================" << std::endl;
  std::cout << "new style pre-order iteration " << std::endl;
  std::cout << "=======================================================================" << std::endl;

  for ( beamline::pre_order_iterator it = model->pre_begin(); 
                                     it != model->pre_end(); 
                                   ++it ) 
  {
    std::cout << (*it)->Name() << std::endl;
  }
  #endif

  #if 1
  std::cout << "=======================================================================" << std::endl;
  std::cout << "new-style reverse pre_order iteration " << std::endl;
  std::cout << "=======================================================================" << std::endl;

  for ( beamline::reverse_pre_order_iterator it = model->rpre_begin(); 
                                             it != model->rpre_end(); 
                                           ++it ) 
  {
    std::cout << (*it)->Name() << std::endl;
  }

  #endif

  #if 1
  std::cout << "=======================================================================" << std::endl;
  std::cout << "new style post-order iteration " << std::endl;
  std::cout << "=======================================================================" << std::endl;

  for ( beamline::post_order_iterator it = model->post_begin(); 
                                      it != model->post_end(); 
                                    ++it ) 
  {
    std::cout << (*it)->Name() << std::endl;
  }
  #endif

  #if 1
  std::cout << "=======================================================================" << std::endl;
  std::cout << "new-style reverse post_order iteration " << std::endl;
  std::cout << "=======================================================================" << std::endl;

  for ( beamline::reverse_post_order_iterator it = model->rpost_begin(); 
                                              it != model->rpost_end(); 
                                            ++it ) 
  {
    std::cout << (*it)->Name() << std::endl;
  }
  #endif

  #if 1
  std::cout << "=======================================================================" << std::endl;
  std::cout << "new-style deep iteration " << std::endl;
  std::cout << "=======================================================================" << std::endl;

  for ( beamline::deep_iterator it = model->deep_begin(); 
                                it != model->deep_end(); 
                              ++it ) 
  {
    std::cout << (*it)->Name() << std::endl;
  }
  #endif

  #if 1
  std::cout << "=======================================================================" << std::endl;
  std::cout << "new-style reverse deep iteration " << std::endl;
  std::cout << "=======================================================================" << std::endl;

  for ( beamline::reverse_deep_iterator it = model->rdeep_begin(); 
                                        it != model->rdeep_end(); 
                                      ++it ) 
  {
    std::cout << (*it)->Name() << std::endl;
  }
  #endif


  return 0;
}
