#if !defined(__VISUAL_CPP__) && !defined(__BORLAND_CPP__)
#include <stream.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include "ParticleBunch.h"
#include "pipestream.h"
#include "rastermonitor.h"
#include <tk.h>
#include "tclf.h"

rasterMonitor::rasterMonitor() : monitor(), origin( BMLN_dynDim ) {
  registerMode    = 0;
  registerParticle = 0;
  int i;
  for( i = 0; i < BMLN_dynDim; i++ ) origin(i) = 0.0;
  for( i = 0; i < 6; i++) sumState[i] = 0.0;
  numParticles = 0;

  // Default scalings for the phase plot if the user provides nothing.
  outputFile = 0;
  x_plot_limits[0]   = -0.05;        // x_plot x-axis minimum   
  x_plot_limits[1]   =  0.05;	      // x_plot x-axis maximum   
  x_plot_limits[2]   = -0.001;	      // x_plot y-axis minimum   
  x_plot_limits[3]   =  0.001;	      // x_plot y-axis maximum   

  y_plot_limits[0]   = -0.05;	      // y_plot x-axis minimum   
  y_plot_limits[1]   =  0.05;	      // y_plot x-axis maximum   
  y_plot_limits[2]   = -0.001;	      // y_plot y-axis minimum   
  y_plot_limits[3]   =  0.001;	      // y_plot y-axis maximum   

  cdt_plot_limits[0] = -0.8;	      // cdt_plot x-axis minimum 
  cdt_plot_limits[1] =  0.8;	      // cdt_plot x-axis maximum 
  cdt_plot_limits[2] = -0.002;	      // cdt_plot y-axis minimum 
  cdt_plot_limits[3] =  0.002;	      // cdt_plot y-axis maximum 
  size = 200;                         // Full width of the canvas.
  windowInitialized = 0;
  display = 0;
  normalizedFlag = 0;
  plotFunc = 0;
}

rasterMonitor::rasterMonitor(char* name, FILE* out_file) 
: monitor(name,out_file), origin( BMLN_dynDim ) {

  registerMode = 0;
  registerParticle = 0;
  int i;
  for( i = 0; i < BMLN_dynDim; i++ ) origin(i) = 0.0;
  for( i = 0; i < 6; i++) sumState[i] = 0.0;
  numParticles = 0;

  // Default scalings for the phase plot if the user provides nothing.
  x_plot_limits[0]   = -0.05;          // x_plot x-axis minimum   
  x_plot_limits[1]   =  0.05;	        // x_plot x-axis maximum   
  x_plot_limits[2]   = -0.001;	        // x_plot y-axis minimum   
  x_plot_limits[3]   =  0.001;	        // x_plot y-axis maximum   

  y_plot_limits[0]   = -0.05;	        // y_plot x-axis minimum   
  y_plot_limits[1]   =  0.05;	        // y_plot x-axis maximum   
  y_plot_limits[2]   = -0.001;	        // y_plot y-axis minimum   
  y_plot_limits[3]   =  0.001;	        // y_plot y-axis maximum   

  cdt_plot_limits[0] = -0.8;		// cdt_plot x-axis minimum 
  cdt_plot_limits[1] =  0.8;		// cdt_plot x-axis maximum 
  cdt_plot_limits[2] = -0.002;	        // cdt_plot y-axis minimum 
  cdt_plot_limits[3] =  0.002;	        // cdt_plot y-axis maximum 
  size = 200;                           // Full width of the canvas.
  display = 0;
  windowInitialized = 0;
  normalizedFlag = 0;
  plotFunc = 0;
}

rasterMonitor::rasterMonitor(char* name) : monitor(name), 
                                           origin( BMLN_dynDim )  {
  registerMode = 0;
  registerParticle = 0;
  int i;
  for( i = 0; i < BMLN_dynDim; i++ ) origin(i) = 0.0;
  for( i = 0; i < 6; i++) sumState[i] = 0.0;
  numParticles = 0;

  // Default scalings for the phase plot if the user provides nothing.
  outputFile = 0;
  x_plot_limits[0]   = -0.05;     // x_plot x-axis minimum   
  x_plot_limits[1]   =  0.05;     // x_plot x-axis maximum   
  x_plot_limits[2]   = -0.001;    // x_plot y-axis minimum   
  x_plot_limits[3]   =  0.001;    // x_plot y-axis maximum   

  y_plot_limits[0]   = -0.05;     // y_plot x-axis minimum   
  y_plot_limits[1]   =  0.05;	   // y_plot x-axis maximum   
  y_plot_limits[2]   = -0.001;	   // y_plot y-axis minimum   
  y_plot_limits[3]   =  0.001;	   // y_plot y-axis maximum   

  cdt_plot_limits[0] = -0.8;       // cdt_plot x-axis minimum 
  cdt_plot_limits[1] =  0.8;	   // cdt_plot x-axis maximum 
  cdt_plot_limits[2] = -0.002;	   // cdt_plot y-axis minimum 
  cdt_plot_limits[3] =  0.002;	   // cdt_plot y-axis maximum 
  size = 200;                      // Full width of the canvas.
  windowInitialized = 0;
  display = 0;
  normalizedFlag = 0;
  plotFunc = 0;
}

rasterMonitor::rasterMonitor(char* name, double x, double xp, 
			     double y, double yp,
			     double cdt, double dp) 
: monitor(name), origin( BMLN_dynDim ) {

  registerMode = 0;
  registerParticle = 0;
  int i;
  for( i = 0; i < BMLN_dynDim; i++ ) origin(i) = 0.0;
  for( i = 0; i < 6; i++) sumState[i] = 0.0;
  numParticles = 0;

  // Default scalings for the phase plot if the user provides nothing.
  outputFile = 0;
  x_plot_limits[0]   =  -x;        // x_plot x-axis minimum   
  x_plot_limits[1]   =   x;	   // x_plot x-axis maximum   
  x_plot_limits[2]   =  -xp;	   // x_plot y-axis minimum   
  x_plot_limits[3]   =   xp;	   // x_plot y-axis maximum   

  y_plot_limits[0]   = -y;	   // y_plot x-axis minimum   
  y_plot_limits[1]   =  y;	   // y_plot x-axis maximum   
  y_plot_limits[2]   = -yp;	   // y_plot y-axis minimum   
  y_plot_limits[3]   =  yp;	   // y_plot y-axis maximum   

  cdt_plot_limits[0] = -cdt;	   // cdt_plot x-axis minimum 
  cdt_plot_limits[1] =  cdt;	   // cdt_plot x-axis maximum 
  cdt_plot_limits[2] = -dp;	   // cdt_plot y-axis minimum 
  cdt_plot_limits[3] =  dp;	   // cdt_plot y-axis maximum 
  size = 200;                      // Full width of the canvas.
  windowInitialized = 0;
  display = 0;
  normalizedFlag = 0;
  plotFunc = 0;
}

rasterMonitor::rasterMonitor( char* name, char* parent_wid ) 
: monitor(name), origin( BMLN_dynDim ) {

  registerMode = 0;
  registerParticle = 0;
  int i;
  for( i = 0; i < BMLN_dynDim; i++ ) origin(i) = 0.0;
  for( i = 0; i < 6; i++) sumState[i] = 0.0;
  numParticles = 0;

  // Default scalings for the phase plot if the user provides nothing.
  outputFile = 0;
  x_plot_limits[0]   = -0.05;           // x_plot x-axis minimum
  x_plot_limits[1]   =  0.05;	         // x_plot x-axis maximum   
  x_plot_limits[2]   = -0.001;	         // x_plot y-axis minimum   
  x_plot_limits[3]   =  0.001;	         // x_plot y-axis maximum   

  y_plot_limits[0]   = -0.05;	         // y_plot x-axis minimum   
  y_plot_limits[1]   =  0.05;	         // y_plot x-axis maximum   
  y_plot_limits[2]   = -0.001;	         // y_plot y-axis minimum   
  y_plot_limits[3]   =  0.001;	         // y_plot y-axis maximum   

  cdt_plot_limits[0] = -0.8;		 // cdt_plot x-axis minimum 
  cdt_plot_limits[1] =  0.8;		 // cdt_plot x-axis maximum 
  cdt_plot_limits[2] = -0.002;	         // cdt_plot y-axis minimum 
  cdt_plot_limits[3] =  0.002;	         // cdt_plot y-axis maximum 
  size = 200;                            // Full width of the canvas.
  display = new char[strlen(parent_wid)+1];
  strcpy(display,parent_wid);
  windowInitialized = 0;
  normalizedFlag = 0;
  plotFunc = 0;
}

rasterMonitor::rasterMonitor(char* name, char* parent_wid, FILE* out_file) 
: monitor(name,out_file), origin( BMLN_dynDim ) {

  registerMode = 0;
  registerParticle = 0;
  int i;
  for( i = 0; i < BMLN_dynDim; i++ ) origin(i) = 0.0;
  for( i = 0; i < 6; i++) sumState[i] = 0.0;
  numParticles = 0;

  // Default scalings for the phase plot if the user provides nothing.
  x_plot_limits[0]   = -0.05;          // x_plot x-axis minimum   
  x_plot_limits[1]   =  0.05;	        // x_plot x-axis maximum   
  x_plot_limits[2]   = -0.001;	        // x_plot y-axis minimum   
  x_plot_limits[3]   =  0.001;	        // x_plot y-axis maximum   

  y_plot_limits[0]   = -0.05;	        // y_plot x-axis minimum   
  y_plot_limits[1]   =  0.05;	        // y_plot x-axis maximum   
  y_plot_limits[2]   = -0.001;	        // y_plot y-axis minimum   
  y_plot_limits[3]   =  0.001;	        // y_plot y-axis maximum   

  cdt_plot_limits[0] = -0.8;		// cdt_plot x-axis minimum 
  cdt_plot_limits[1] =  0.8;		// cdt_plot x-axis maximum 
  cdt_plot_limits[2] = -0.002;	        // cdt_plot y-axis minimum 
  cdt_plot_limits[3] =  0.002;	        // cdt_plot y-axis maximum 
  size = 200;                           // Full width of the canvas.
  display = new char[strlen(parent_wid)+1];
  strcpy(display,parent_wid);
  windowInitialized = 0;
  normalizedFlag = 0;
  plotFunc = 0;
}

rasterMonitor::rasterMonitor( const rasterMonitor& x ) : 
monitor( (monitor &) x), origin(x.origin) {
  registerMode = x.registerMode;
  windowInitialized = 0;  // Set it up to re-initialize it's own pipe.
  numParticles      = 0;  // We've never seen particles before either.
  if(x.registerParticle != 0) {
    registerParticle  = x.registerParticle->Clone();
  } else {
    registerParticle = 0;
  }
  int i;
  for(i=0; i< 4; i++) {
    x_plot_limits[i]   = x.x_plot_limits[i];
    y_plot_limits[i]   = x.y_plot_limits[i];
    cdt_plot_limits[i] = x.cdt_plot_limits[i];
  }
  for(i=0; i< 6; i++) sumState[i] = 0.0;
  size = x.size;
  normalizedFlag = x.normalizedFlag;
  if(x.plotFunc != 0) {
    plotFunc = new lattFunc(*x.plotFunc); 
  } else {
    plotFunc = 0;
  }
  if(x.display != 0) {
    display = new char[strlen(x.display)+1];
    strcpy(display,x.display);
  } else {
    display = 0;
  }
}

rasterMonitor::~rasterMonitor() {
  if(onOff) {
    *p << "exit\n" << flush; 
    delete p;
  }
  if(display != 0) 
    delete [] display;
  if (plotFunc != 0)
    delete plotFunc;
}

void rasterMonitor::RegisterOrigin( Particle& part ) {
  if( part.State().Dim() != origin.Dim() ) origin = part.State();
  for( int i = 0; i < origin.Dim(); i++ ) origin(i) = 0.0;
  registerParticle = &part;
  registerMode = 1;
}

void rasterMonitor::ResetOrigin() {
  for( int i = 0; i < origin.Dim(); i++ ) origin(i) = 0.0;
}

void rasterMonitor::on() {
  onOff = 1;
   if(!windowInitialized) {
     windowInitialized = 1;
     p = new iopipestream("raster_wish");      // Start the child process.
     *p << "source $env(FNALROOT)/tcl/scripts/rastermonitor.tcl\n" 
        << flush; 
     // Give the main window the name of the monitor.
     *p << "wm title . " << Name() << " \n" << flush; 
     // Place coordinates on the windows: x xp y yp cdt dp
     setNewScaling();
   }
}

void rasterMonitor::on(char* whereToDisplay) {
  char processMsg[80];
  onOff = 1;
   if(!windowInitialized) {
     windowInitialized = 1;
     sprintf(processMsg,"raster_wish -display %s",whereToDisplay);
     p = new iopipestream(processMsg);         // Start the child process.
     *p << "source $env(FNALROOT)/tcl/scripts/rastermonitor.tcl\n" 
        << flush; 
     // Give the main window the name of the monitor.
     *p << "wm title . " << Name() << " \n" << flush; 
     // Place coordinates on the windows.
     setNewScaling();
   }
}

void rasterMonitor::localPropagate( Particle& part) {
//******************************************************
//
//  This should update the display of the phase-space plot if 
//  the rastermonitor is turned on.
//
//******************************************************

 // --- Registration of the origin --------------------------
 if( registerMode )
   if( &part == registerParticle ) {
     for( int i = 0; i < origin.Dim(); i++ ) 
       origin(i) = part.State(i);
     registerMode = 0;
     return;
   }

 // --- Ordinary propagation ---------------------------------
 if ( onOff ) {
   if(!windowInitialized) {
     windowInitialized = 1;
     p = new iopipestream("raster_wish");        // Start the child process.
     *p << "source ./rastermonitor.tcl\n" 
        << flush; 
     // Place coordinates on the windows.
     setNewScaling();
   }

   // Pull off the value of the particle position and send to the interpreter.
   Vector ParticleCoords( part.State() );
   ParticleCoords -= origin;

   if(outputFile != 0 && outputFile != stdout) {
     monitor::localPropagate(part);
   }

//   cout << " X = " << ParticleCoords(0) << " Xprime = " 
//        << ParticleCoords(3) << endl;
//   cout << " Y = " << ParticleCoords(1) << " Yprime = " 
//        << ParticleCoords(4) << endl;
//   cout << " cdt= " << ParticleCoords(2) << " dp/p    = " 
//        << ParticleCoords(5) << endl;

   if(numParticles == LONG_MAX) {
     cerr << "rasterMonitor::localPropagate(Particle&): Number particles seen \n"
          << "                                     exceeds particle counter.\n"
          << "                                     Rollover will occur making\n"
          << "                                     getAveState() calculations \n"
          << "                                     meaningless!!" << endl;
   }
   numParticles++;
   for(int j = 0; j < 6; j++) sumState[j] += ParticleCoords(j); 
   double scaledCoords[6];
   if((normalizedFlag == 1) && (plotFunc != 0)) {
     scaledCoords[0] = ParticleCoords(0);
     scaledCoords[3] = plotFunc->alpha.hor * ParticleCoords(0) + 
       plotFunc->beta.hor * ParticleCoords(3);
     scaledCoords[1] = ParticleCoords(1);
     scaledCoords[4] = plotFunc->alpha.ver * ParticleCoords(1) + 
       plotFunc->beta.ver * ParticleCoords(4);
     scaledCoords[2] = ParticleCoords(2);
     scaledCoords[5] = ParticleCoords(5);
     *p << ::form("update_plot %d %d %d %d %d %d\n",
		  atopixx(scaledCoords[0],x_plot_limits[1],x_plot_limits[0]),    
		  atopixy(scaledCoords[3],x_plot_limits[3],x_plot_limits[2]),    
		  atopixx(scaledCoords[1],y_plot_limits[1],y_plot_limits[0]),    
		  atopixy(scaledCoords[4],y_plot_limits[3],y_plot_limits[2]),    
		  atopixx(scaledCoords[2],cdt_plot_limits[1],cdt_plot_limits[0]),
		  atopixy(scaledCoords[5],cdt_plot_limits[3],cdt_plot_limits[2]))
       << flush;
   } else 
     *p << ::form("update_plot %d %d %d %d %d %d\n",
		  atopixx(ParticleCoords(0),x_plot_limits[1],x_plot_limits[0]),    
		  atopixy(ParticleCoords(3),x_plot_limits[3],x_plot_limits[2]),    
		  atopixx(ParticleCoords(1),y_plot_limits[1],y_plot_limits[0]),    
		  atopixy(ParticleCoords(4),y_plot_limits[3],y_plot_limits[2]),    
		  atopixx(ParticleCoords(2),cdt_plot_limits[1],cdt_plot_limits[0]),
		  atopixy(ParticleCoords(5),cdt_plot_limits[3],cdt_plot_limits[2]))
       << flush;

   *p << "update idletasks\n" << flush; 
 }
}

void rasterMonitor::localPropagate( JetParticle& jpart) {
//******************************************************
//
//  This should update the display of the phase-space plot if 
//  the rastermonitor is turned on.
//
//******************************************************

 if ( onOff ) {
   if(!windowInitialized) {
     windowInitialized = 1;
     p = new iopipestream("raster_wish");         // Start the child process.
     *p << "source ./rastermonitor.tcl\n" 
        << flush; // init script to read.
     // Place coordinates on the windows.
     setNewScaling();
   }

   // Pull off the value of the particle position and send to the interpreter.
   Jet    JetParticleCoords[6];
   jpart.getState(JetParticleCoords);

   if(outputFile != 0 && outputFile != stdout) {
     monitor::localPropagate(jpart);
   }

//   cout << " X = " << (JetParticleCoords[0]).standardPart() 
//        << " Xprime = " << (JetParticleCoords[3]).standardPart() << endl;
//   cout << " Y = " << (JetParticleCoords[1]).standardPart() 
//        << " Yprime = " << (JetParticleCoords[4]).standardPart() << endl;
//   cout << " ct= " << (JetParticleCoords[2]).standardPart() 
//        << " E      = " << (JetParticleCoords[5]).standardPart() << endl;

   double scaledCoords[6];
   if((normalizedFlag == 1) && (plotFunc != 0)) {
     scaledCoords[0] = JetParticleCoords[0].standardPart();
     scaledCoords[3] = plotFunc->alpha.hor * JetParticleCoords[0].standardPart() + 
       plotFunc->beta.hor * JetParticleCoords[3].standardPart();
     scaledCoords[1] = JetParticleCoords[1].standardPart();
     scaledCoords[4] = plotFunc->alpha.ver * JetParticleCoords[1].standardPart() + 
       plotFunc->beta.ver * JetParticleCoords[4].standardPart();
     scaledCoords[2] = JetParticleCoords[2].standardPart();
     scaledCoords[5] = JetParticleCoords[5].standardPart();
     *p << ::form("update_plot %d %d %d %d %d %d\n",
		  atopixx(scaledCoords[0],x_plot_limits[1],x_plot_limits[0]),    
		  atopixy(scaledCoords[3],x_plot_limits[3],x_plot_limits[2]),    
		  atopixx(scaledCoords[1],y_plot_limits[1],y_plot_limits[0]),    
		  atopixy(scaledCoords[4],y_plot_limits[3],y_plot_limits[2]),    
		  atopixx(scaledCoords[2],cdt_plot_limits[1],cdt_plot_limits[0]),
		  atopixy(scaledCoords[5],cdt_plot_limits[3],cdt_plot_limits[2]))
       << flush;
   } else 
     *p << ::form("update_plot %d %d %d %d %d %d\n",
		  atopixx((JetParticleCoords[0]).standardPart(),x_plot_limits[1],
			  x_plot_limits[0]),
		  atopixy((JetParticleCoords[3]).standardPart(),x_plot_limits[3],
			  x_plot_limits[2]),
		  atopixx((JetParticleCoords[1]).standardPart(),y_plot_limits[1],
			  y_plot_limits[0]),
		  atopixy((JetParticleCoords[4]).standardPart(),y_plot_limits[3],
			  y_plot_limits[2]),
		  atopixx((JetParticleCoords[2]).standardPart(),
			  cdt_plot_limits[1],cdt_plot_limits[0]),
		atopixy((JetParticleCoords[5]).standardPart(),
		        cdt_plot_limits[3],cdt_plot_limits[2]))
       << flush;
 }
}

void rasterMonitor::getXplotScaling(double* limits ) {
//******************************************************
//
//  This routine returns the min/max values for the x-x'
//  phase space plot in the order of x min, x max, y min,
//  y max. (4 parameters).
//
//******************************************************

  if(limits == 0) {          // Check for null pointer passed.
    cerr << "\n";
    cerr << "*** ERROR ***                                          \n";
    cerr << "*** ERROR *** rasterMonitor::getXplotScaling( double* )\n";
    cerr << "*** ERROR *** Null pointer used as argument.           \n";
    cerr << "*** ERROR ***                                          \n";
    cerr << "\n";
    return;
  }

  int count;
  for(count = 0; count < 4; count++){
    limits[count] = x_plot_limits[count];
  }
  return;
}

void rasterMonitor::getYplotScaling(double* limits ) {
//******************************************************
//
//  This routine returns the min/max values for the y-y'
//  phase space plot in the order of x min, x max, y min,
//  y max. (4 parameters).
//
//******************************************************

  if(limits == 0) {          // Check for null pointer passed.
    cerr << "\n";
    cerr << "*** ERROR ***                                          \n";
    cerr << "*** ERROR *** rasterMonitor::getYplotScaling( double* )\n";
    cerr << "*** ERROR *** Null pointer used as argument.           \n";
    cerr << "*** ERROR ***                                          \n";
    cerr << "\n";
    return;
  }

  int count;
  for(count = 0; count < 4; count++){
    limits[count] = y_plot_limits[count];
  }
  return;
}

void rasterMonitor::getZplotScaling(double* limits ) {
//******************************************************
//
//  This routine returns the min/max values for the cdt-dp/p
//  phase space plot in the order of x min, x max, y min,
//  y max. (4 parameters).
//
//******************************************************

  if(limits == 0) {          // Check for null pointer passed.
    cerr << "\n";
    cerr << "*** ERROR ***                                          \n";
    cerr << "*** ERROR *** rasterMonitor::getZplotScaling( double* )\n";
    cerr << "*** ERROR *** Null pointer used as argument.           \n";
    cerr << "*** ERROR ***                                          \n";
    cerr << "\n";
    return;
  }

  int count;
  for(count = 0; count < 4; count++){
    limits[count] = cdt_plot_limits[count];
  }
  return;
}

void rasterMonitor::getAllplotScaling(double* limits ) {
//******************************************************
//
//  This routine returns the min/max values for all 3 
//  phase space plots (x-x', y-y', cdt-dp/p) in the order 
//  of x min, x max, y min, y max. (12 parameters).
//
//******************************************************

  if(limits == 0) {          // Check for null pointer passed.
    cerr << "\n";
    cerr << "*** ERROR ***                                            \n";
    cerr << "*** ERROR *** rasterMonitor::getAllplotScaling( double* )\n";
    cerr << "*** ERROR *** Null pointer used as argument.             \n";
    cerr << "*** ERROR ***                                            \n";
    cerr << "\n";
    return;
  }

  int count;
  for(count = 0; count < 4; count++){
    limits[count]   = x_plot_limits[count];  
    limits[count+4] = y_plot_limits[count];  
    limits[count+8] = cdt_plot_limits[count];
  }
  return;
}

void rasterMonitor::setXplotScaling(double* limits ) {
//******************************************************
//
//  This routine sets the min/max values for the x-x' 
//  phase space plot.
//
//******************************************************

  if(limits == 0) {          // Check for null pointer passed.
    cerr << "\n";
    cerr << "*** ERROR ***                                          \n";
    cerr << "*** ERROR *** rasterMonitor::setXplotScaling( double* )\n";
    cerr << "*** ERROR *** Null pointer used as argument.           \n";
    cerr << "*** ERROR ***                                          \n";
    cerr << "\n";
    return;
  }
  x_plot_limits[0] = -1 * limits[1];
  x_plot_limits[1] = limits[1];
  x_plot_limits[2] = -1 * limits[3];
  x_plot_limits[3] = limits[3];

  // Now actually set the new scaling. 
  setNewScaling();
}

void rasterMonitor::setYplotScaling(double* limits ) {
//******************************************************
//
//  This routine sets the min/max values for the y-y'
//  phase space plot.
//
//******************************************************

  if(limits == 0) {          // Check for null pointer passed.
    cerr << "\n";
    cerr << "*** ERROR ***                                          \n";
    cerr << "*** ERROR *** rasterMonitor::setYplotScaling( double* )\n";
    cerr << "*** ERROR *** Null pointer used as argument.           \n";
    cerr << "*** ERROR ***                                          \n";
    cerr << "\n";
    return;
  }
  y_plot_limits[0] = -1 * limits[1];
  y_plot_limits[1] = limits[1];
  y_plot_limits[2] = -1 * limits[3];
  y_plot_limits[3] = limits[3];

  // Now actually set the new scaling. 
  setNewScaling();
}

void rasterMonitor::setZplotScaling(double* limits ) {
//******************************************************
//
//  This routine sets the min/max values for the cdt-dp/p
//  phase space plot.
//
//******************************************************

  if(limits == 0) {          // Check for null pointer passed.
    cerr << "\n";
    cerr << "*** ERROR ***                                          \n";
    cerr << "*** ERROR *** rasterMonitor::setZplotScaling( double* )\n";
    cerr << "*** ERROR *** Null pointer used as argument.           \n";
    cerr << "*** ERROR ***                                          \n";
    cerr << "\n";
    return;
  }
  cdt_plot_limits[0] = -1 * limits[1];
  cdt_plot_limits[1] = limits[1];
  cdt_plot_limits[2] = -1 * limits[3];
  cdt_plot_limits[3] = limits[3];

  // Now actually set the new scaling. 
  setNewScaling();
}

void rasterMonitor::resetXplot() {
//******************************************************
//
//  This routine clears the x-x' phase space plot.
//
//******************************************************
  *p << "new_scale $p.fr.x.plot x_wind \n" << flush;
  numParticles = 0;
  sumState[0] = 0.0;
  sumState[3] = 0.0;
}

void rasterMonitor::resetYplot() {
//******************************************************
//
//  This routine clears the y-y' phase space plot.
//
//******************************************************
  *p << "new_scale $p.fr.y.plot y_wind \n" << flush;
  numParticles = 0;
  sumState[1] = 0.0;
  sumState[4] = 0.0;
}

void rasterMonitor::resetZplot() {
//******************************************************
//
//  This routine clears the z-z' phase space plot.
//
//******************************************************
  *p << "new_scale $p.fr.cdt.plot cdt_wind \n" << flush;
  numParticles = 0;
  sumState[2] = 0.0;
  sumState[5] = 0.0;
}

void rasterMonitor::setAllplotScaling(double* limits ) {
//******************************************************
//
//  This routine sets the min/max values for all 3 
//  phase space plots (x-x', y-y', cdt-dp/p) in the order 
//  of x min, x max, y min, y max. (12 parameters).
//
//******************************************************

  if(limits == 0) {          // Check for null pointer passed.
    cerr << "\n";
    cerr << "*** ERROR ***                                            \n";
    cerr << "*** ERROR *** rasterMonitor::setAllplotScaling( double* )\n";
    cerr << "*** ERROR *** Null pointer used as argument.             \n";
    cerr << "*** ERROR ***                                            \n";
    cerr << "\n";
    return;
  }

  setXplotScaling(limits);
  setYplotScaling(limits+4);
  setZplotScaling(limits+8);

  // Now actually set the new scaling. 
  setNewScaling();
  return;
}

void rasterMonitor::normalizedScaleOff() {
  normalizedFlag = 0;
}

void rasterMonitor::normalizedScaleOn() {
  normalizedFlag = 1;
}

void rasterMonitor::setNormalizedScale(lattFunc* newFunc) {
  lattFunc* tmpFunc;

  if(plotFunc != 0) {
    delete plotFunc;
    plotFunc = 0;
  }
  if(newFunc != 0) {
    plotFunc = new lattFunc(*newFunc);
  } else {
    tmpFunc = (lattFunc*)dataHook.find("Twiss");
    if(tmpFunc != 0)
      plotFunc = new lattFunc(*tmpFunc);
  }
}

void rasterMonitor::setNewScaling() {
//******************************************************
//
//  This routine sends the max values for all 3 
//  phase space plots (x-x', y-y', cdt-dp/p) in the order 
//  of x max, x' max, y max, y' max, cdt max dp/p max. (6 parameters).
//
//******************************************************
  *p << ::form("place_limits %6.4f %8.6f %6.4f %8.6f %6.4f %6.4f\n",
	       x_plot_limits[1],x_plot_limits[3],y_plot_limits[1],
	       y_plot_limits[3],cdt_plot_limits[1],cdt_plot_limits[3]) 
     << flush;
}

double rasterMonitor::getAveXstate(){
//******************************************************
//
//  This routine returns the average phase space X 
//  coordinates passed through the rastermonitor since 
//  the last call 
//  to resetXplot(). 
//
//******************************************************
  if(numParticles == 0) {
    cerr << "rasterMonitor::getAveXstate(): Number of particles seen is 0!\n"
         << "                               Can't calculate average!" << endl;
    return 0;
  }
  return sumState[0]/numParticles;
}

double rasterMonitor::getAveXPRIMEstate(){
//******************************************************
//
//  This routine returns the average phase space X' 
//  coordinates passed through the rastermonitor since 
//  the last call 
//  to resetXplot(). 
//
//******************************************************
  if(numParticles == 0) {
    cerr << "rasterMonitor::getAveXPRIMEstate(): Number of particles seen is 0!\n"
         << "                                    Can't calculate average!" << endl;
    return 0;
  }
  return sumState[3]/numParticles;
}

double rasterMonitor::getAveYstate(){
//******************************************************
//
//  This routine returns the average phase space Y 
//  coordinates passed through the rastermonitor since 
//  the last call 
//  to resetYplot(). 
//
//******************************************************
  if(numParticles == 0) {
    cerr << "rasterMonitor::getAveYstate(): Number of particles seen is 0!\n"
         << "                               Can't calculate average!" << endl;
    return 0;
  }
  return sumState[1]/numParticles;
}

double rasterMonitor::getAveYPRIMEstate(){
//******************************************************
//
//  This routine returns the average phase space Y' 
//  coordinates passed through the rastermonitor since 
//  the last call 
//  to resetYplot(). 
//
//******************************************************
  if(numParticles == 0) {
    cerr << "rasterMonitor::getAveXstate(): Number of particles seen is 0!\n"
         << "                               Can't calculate average!" << endl;
    return 0;
  }
  return sumState[4]/numParticles;
}

char* rasterMonitor::Type() const  
{ 
  return "rasterMonitor"; 
}


#endif // Exclude under Visual C++ and Borland builds.
