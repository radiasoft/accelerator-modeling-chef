#include <iostream>
#include <string>
#include <stdlib.h>

#include <boost/test/minimal.hpp>

#include <basic_toolkit/PhysicsConstants.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/beamline.h>
#include <beamline/LinacCavity.h>
#include <beamline/quadrupole.h>
#include <beamline/kick.h>
#include <beamline/monitor.h>
#include <parsers/xsif/XSIFFactory.h>

/* ILC Linac Test 1: 

   Read a lattice file, count the number of dipoles, quadrupoles and 
   rf cavities.  
   
   Paul L. G. Lebrun, January/February 2008 
   
*/

int test_main( int, char *[] )             // note the name!
{
  
  // Get the lattice file...
   
  char* ilcTestDataChr = getenv("ILCLINACTESTDATA");
  std::string defDataDir("./data");
  if (ilcTestDataChr == 0) {   
//     BOOST_FAIL( " Environment varialble where test data resides not defined " );
      ilcTestDataChr = (char *) defDataDir.c_str();
  }
  
  std::string ilcTestDataStr(ilcTestDataChr);
  ilcTestDataStr += std::string("/ilc_linacKirti3.xsif");
  
 // This is to ensure the default directory won't change under us.. 
 // This is probably not necessary for current version of CHEF, 
 // albeit might be platform dependent 
 
  char myCDir[1024];
  getcwd(myCDir, 1023);
  XSIFFactory bfFull(ilcTestDataStr.c_str() );
  chdir(myCDir);
  
   
  BmlPtr bmlPtrFull = bfFull.create_beamline("LINAC");
   
  if (!bmlPtrFull)   
     BOOST_FAIL( " Beamline Pointer returned null, XSIF parser failure " );
  
   
   // flatten it, speed improvements. (perhaps); 
   
   bmlPtrFull = BmlPtr( new beamline(bmlPtrFull->flatten() ));

   // I think I know this!.. 

  int expectedNumDipoles = 241;
  int expectedNumQuads = 240;
  int expectedNumCavs = 7680;
  int expectedNumBPMs = 241;
  // 
  int numDipoles = 0;
  int numQuads = 0;
  int numCavs = 0;
  int numBPMs = 0;
  
   for(beamline::iterator dbi = bmlPtrFull->begin(); 
        dbi != bmlPtrFull->end(); ++dbi) {
     ElmPtr q = *dbi;	
     if( QuadrupolePtr quadPtr= boost::dynamic_pointer_cast<quadrupole> (q) ) numQuads++;
     else if(LinacCavityPtr rfcPtr= boost::dynamic_pointer_cast<LinacCavity> (q))  numCavs++;
     else if(VKickPtr dipPtr= boost::dynamic_pointer_cast<vkick> (q)) numDipoles++;
     else if(MonitorPtr aBPM = boost::dynamic_pointer_cast<monitor> (q) ) numBPMs++;

  }
  
  // Now check...
  
  bool allOK = true;
  if ( (expectedNumDipoles != numDipoles) ||  (expectedNumQuads != numQuads) 
      || (expectedNumCavs != numCavs) || (expectedNumBPMs != numBPMs)) { 
      
      BOOST_ERROR( "Unexpected number of Beamline Elements, suspecting BeamLine Factory problem " );            // #3 continues on error
      if (expectedNumDipoles != numDipoles) { 
        std::ostringstream eStrStr;
        eStrStr << " Expected number of  dipoles (VKick) = " << 
	           expectedNumDipoles << " Counted " << numDipoles;
        std::string eStr(eStrStr.str());
        BOOST_ERROR( eStr.c_str() );
      }
      if (expectedNumQuads != numQuads) { 
        std::ostringstream eStrStr;
        eStrStr << " Expected # quadrupoles = " << expectedNumQuads << " Counted " << numQuads;
        std::string eStr(eStrStr.str());
        BOOST_ERROR( eStr.c_str() );
      }
      if (expectedNumCavs != numCavs) { 
        std::ostringstream eStrStr;
        eStrStr << " Expected # Linac Cavities = " << expectedNumCavs << " Counted " << numCavs;
        std::string eStr(eStrStr.str());
        BOOST_ERROR( eStr.c_str() );
      }
       if (expectedNumBPMs != numBPMs) { 
        std::ostringstream eStrStr;
        eStrStr << " Expected # BPMs = " << expectedNumBPMs << " Counted " << numBPMs;
        std::string eStr(eStrStr.str());
        BOOST_ERROR( eStr.c_str() );
      }
      
      allOK = false;
      
  }
  
  if (allOK) return 0;
  else return 1;
}
