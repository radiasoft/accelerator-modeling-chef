#if HAVE_CONFIG_H
#include <config.h>
#endif
/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      mwiremonitor.cc
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
******                All Rights Reserved                             
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#if !defined(_MSC_VER) && !defined(__BORLANDC__) && !defined(_EXCLUDE_MONITORS_)
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "ParticleBunch.h"
#include "pipestream.h"
#include "mwiremonitor.h"
#include <tk.h>
#include "tclf.h"

int mwireMonitor::WISH_WAIT_TIME = 1;

mwireMonitor::mwireMonitor() : monitor() {
  binNumber = 48;
  wireSpacing = 1;
  x_fs = 0.0;
  y_fs = 0.0;
  x_fiducial = 0.0;
  y_fiducial = 0.0;
  size = 200;                         // Full width of the canvas.
  x_offset = 0.0;
  y_offset = 0.0;
  x_bins = new int[binNumber];
  y_bins = new int[binNumber];
  int i;
  for(i=0; i< binNumber; i++){
    x_bins[i] = 0;
    y_bins[i] = 0;
  }
  windowInitialized = 0;
  display = 0;
  p = 0;
}

mwireMonitor::mwireMonitor(char* name) : monitor(name) {
  binNumber = 48;
  wireSpacing = 1;
  x_fs = 0.0;
  y_fs = 0.0;
  x_fiducial = 0.0;
  y_fiducial = 0.0;
  size = 200;                      // Full width of the canvas.
  x_offset = 0.0;
  y_offset = 0.0;
  x_bins = new int[binNumber];
  y_bins = new int[binNumber];
  int i;
  for(i=0; i< binNumber; i++){
    x_bins[i] = 0;
    y_bins[i] = 0;
  }
  windowInitialized = 0;
  display = 0;
  p = 0;
}

mwireMonitor::mwireMonitor(char* name, FILE* outFile) : monitor(name,outFile) {
  binNumber = 48;
  wireSpacing = 1;
  x_fs = 0.0;
  y_fs = 0.0;
  x_fiducial = 0.0;
  y_fiducial = 0.0;
  size = 200;                      // Full width of the canvas.
  x_offset = 0.0;
  y_offset = 0.0;
  x_bins = new int[binNumber];
  y_bins = new int[binNumber];
  int i;
  for(i=0; i< binNumber; i++){
    x_bins[i] = 0;
    y_bins[i] = 0;
  }
  windowInitialized = 0;
  display = 0;
  p = 0;
}

mwireMonitor::mwireMonitor(char* name, double spacing) : monitor(name) {
  if(spacing <= 0) {
    cerr << " mwireMonitor::mwireMonitor: Wire spacing can't be set to "
         << "value of zero or negative! " << endl;
    return;
  }
  wireSpacing = spacing; 
  binNumber = 48;
  x_fs = 0.0;
  y_fs = 0.0;
  x_fiducial = 0.0;
  y_fiducial = 0.0;
  size = 200;                      // Full width of the canvas.
  x_offset = 0.0;
  y_offset = 0.0;
  x_bins = new int[binNumber];
  y_bins = new int[binNumber];
  int i;
  for(i=0; i< binNumber; i++){
    x_bins[i] = 0;
    y_bins[i] = 0;
  }
  windowInitialized = 0;
  display = 0;
  p = 0;
}

mwireMonitor::mwireMonitor(char* name, FILE* outFile, double spacing) : 
monitor(name,outFile) {
  if(spacing <= 0) {
    cerr << " mwireMonitor::mwireMonitor: Wire spacing can't be set to "
         << "value of zero or negative! " << endl;
    return;
  }
  wireSpacing = spacing; 
  binNumber = 48;
  x_fs = 0.0;
  y_fs = 0.0;
  x_fiducial = 0.0;
  y_fiducial = 0.0;
  size = 200;                      // Full width of the canvas.
  x_offset = 0.0;
  y_offset = 0.0;
  x_bins = new int[binNumber];
  y_bins = new int[binNumber];
  int i;
  for(i=0; i< binNumber; i++){
    x_bins[i] = 0;
    y_bins[i] = 0;
  }
  windowInitialized = 0;
  display = 0;
  p = 0;
}

mwireMonitor::mwireMonitor( char* name, double spacing, double x_max, 
double y_max ) 
: monitor(name) {
  if(spacing <= 0) {
    cerr << " mwireMonitor::mwireMonitor: Wire spacing can't be set to "
         << "value of zero or negative! " << endl;
    return;
  }
  binNumber = 48;
  wireSpacing = spacing;
  x_fs = x_max;
  y_fs = y_max;
  x_fiducial = 0.0;
  y_fiducial = 0.0;
  x_offset = 0.0;
  y_offset = 0.0;
  x_bins = new int[binNumber];
  y_bins = new int[binNumber];
  int i;
  for(i=0; i< binNumber; i++){
    x_bins[i] = 0;
    y_bins[i] = 0;
  }
  size = 200;                            // Full width of the canvas.
  windowInitialized = 0;
  display = 0;
  p = 0;
}

mwireMonitor::mwireMonitor(char* name, double x_max, double y_max) 
: monitor(name) {

  wireSpacing = 1; 
  binNumber = 48;
  x_fs = x_max;
  y_fs = y_max;
  x_fiducial = 0.0;
  y_fiducial = 0.0;
  display = 0;
  x_offset = 0.0;
  y_offset = 0.0;
  x_bins = new int[binNumber];
  y_bins = new int[binNumber];
  int i;
  for(i=0; i< binNumber; i++){
    x_bins[i] = 0;
    y_bins[i] = 0;
  }
  size = 200;                           // Full width of the canvas.
  windowInitialized = 0;
  p = 0;
}

//******************************************************
//
//  Routine for copy constructor.
//
//******************************************************
mwireMonitor::mwireMonitor( const mwireMonitor& x ){
  if(ident != 0) {
    delete [] ident;
  }
  ident = new char[strlen(x.ident)+1];
  strcpy(ident,x.ident);
  
  binNumber  = x.binNumber;
  x_fs       = x.x_fs;
  y_fs       = x.y_fs;
  x_fiducial = x.x_fiducial;
  y_fiducial = x.y_fiducial;
  size       = x.size;
  x_offset   = x.x_offset;
  y_offset   = x.y_offset;
  windowInitialized = x.windowInitialized;
  display    = x.display;
  wireSpacing = x.wireSpacing;
  p = 0;
  x_bins = new int[x.binNumber];
  y_bins = new int[x.binNumber];
  int i;
  for(i=0; i< x.binNumber; i++){
    x_bins[i] = x.x_bins[i];
    y_bins[i] = x.y_bins[i];
  }
  p = 0;
}

mwireMonitor::~mwireMonitor() {
  if(onOff){  // Only kill if a window was started.
    *p << "exit " << endl;
    delete p;
    p = 0;
  }
  if(display != 0) {
    delete [] display;
    display = 0;
  }
  if(x_bins != 0) {
    delete [] x_bins;
    x_bins = 0;
  }
  if(y_bins != 0) {
    delete [] y_bins;
    y_bins = 0;
  }
}

//******************************************************
//
//  This will check the scaling and if it is non-zero, the
//  scaling in the mwireMonitor will be set.
//
//******************************************************
void mwireMonitor::scaleCheck() {
  if((x_fs != 0.0) && (y_fs != 0.0)) {
    *p << "$p.x_fr.bar axis configure y -max " << x_fs << " ; " 
       << "$p.y_fr.bar axis configure y -max " << y_fs << endl;
  } 
}


const char* mwireMonitor::Type() const  
{ 
  return "mwireMonitor"; 
}


void mwireMonitor::on() {
//******************************************************
//
//  This will turn on the mwireMonitor and if the communication
//  to the blt_wish process hasn't been established, it will
//  create it.
//
//******************************************************
  onOff = 1;
  if(!windowInitialized) {
    windowInitialized = 1;
    p = new iopipestream("blt_wish");      // Start the child process.
    *p << "source $env(FNALROOT)/tcl/scripts/mwiremonitor.tcl " 
       << endl; 
    // Give the main window the name of the monitor.
    sleep(WISH_WAIT_TIME);
    *p << "wm title . " << Name() << ";";
    sleep(WISH_WAIT_TIME);
    scaleCheck();
    sleep(WISH_WAIT_TIME);
    setWireSpacing(wireSpacing);
    x_sum = y_sum = x2_sum = y2_sum = 0.0;
    particle_count = 0;
  }
}

void mwireMonitor::on(char* whereToDisplay) {
//******************************************************
//
//  This will turn on the mwireMonitor and if the communication
//  to the blt_wish process hasn't been sestablished, it will
//  create it.  The user can specify what display the mwireMonitor
//  should appear on (like dudley.fnal.gov)
//
//******************************************************
  char processMsg[80];
  onOff = 1;
  if(!windowInitialized) {
    windowInitialized = 1;
    display = new char[strlen(whereToDisplay)+1];
    strcpy(display,whereToDisplay);
    sprintf(processMsg,"blt_wish -display %s",whereToDisplay);
    p = new iopipestream(processMsg);         // Start the child process.
    *p << "source $env(FNALROOT)/tcl/scripts/mwiremonitor.tcl " 
       << endl; 
    // Give the main window the name of the monitor.
    sleep(WISH_WAIT_TIME);
    *p << "wm title . " << Name() << ";"; 
    sleep(WISH_WAIT_TIME);
    scaleCheck();
    sleep(WISH_WAIT_TIME);
    setWireSpacing(wireSpacing);
    x_sum = y_sum = x2_sum = y2_sum = 0.0;
    particle_count = 0;
  }
}

void mwireMonitor::localPropagate( Particle& part) {
//******************************************************
//
//  This should update the display of the phase-space plot if 
//  the mwiremonitor is turned on.
//
//******************************************************
  // --- Ordinary propagation ---------------------------------
  if ( onOff ) {
    if(!windowInitialized) {
      windowInitialized = 1;
      p = new iopipestream("blt_wish");         // Start the child process.
      *p << "source $env(FNALROOT)/tcl/scripts/mwiremonitor.tcl " 
         << endl; 
      scaleCheck();
    }

    double    coords[6];  

    part.getState(coords);
    calcBin(coords);

    char msg[80];
    sprintf(msg,"set xVector(%d) %d; set yVector(%d) %d ",
	    Xindex,x_bins[Xindex],Yindex,y_bins[Yindex]);
    *p << msg << endl;

    // Calculate interim sigma
    x_sum  += coords[0];
    x2_sum += coords[0]*coords[0];
    y_sum  += coords[1];
    y2_sum += coords[1]*coords[1];

    particle_count++;
    if ( particle_count % 10 == 0 ) {
      x_sigma = 1000.0 * sqrt( x2_sum/particle_count - (x_sum*x_sum)/(particle_count*particle_count) );
      y_sigma = 1000.0 * sqrt( y2_sum/particle_count - (y_sum*y_sum)/(particle_count*particle_count) );
      
      sprintf(msg, "set xSigma %f; set ySigma %f ",
	      x_sigma, y_sigma);
      *p << msg << endl;
    }

    *p << "update idletasks " << endl;
    // If file I/O is enabled, let the base class do the work!
    if(outputFile != stdout && outputFile != 0){
      monitor::localPropagate(part);
    }
  }
}

void mwireMonitor::localPropagate( JetParticle& jpart) {
//******************************************************
//
//  This should update the display of the phase-space plot if 
//  the mwiremonitor is turned on.
//
//******************************************************
  if ( onOff ) {
    if(!windowInitialized) {
      windowInitialized = 1;
      p = new iopipestream("blt_wish");         // Start the child process.
      *p << "source $env(FNALROOT)/tcl/scripts/mwiremonitor.tcl " 
         << endl; // init script to read.
      scaleCheck();
    }

    // Pull off the value of the particle position and send to the interpreter.
    Jet    JetParticleCoords[6];
    jpart.getState(JetParticleCoords);

    if(outputFile != stdout && outputFile != 0){
      monitor::localPropagate(jpart);
    }
  }
}

void mwireMonitor::calcBin(const double *coordinates){
//******************************************************
// 
//  Method to calculate what bins to populate based on the x y coordinates. 
//
//******************************************************
  int    middleWire = (binNumber - 1)/2;               // middle wire #
  double halfwidth = (double)middleWire * wireSpacing; // middle wire #
                                                       // in terms of mm.
  double wiresPERmm =  (double)middleWire / halfwidth; 

// Calculate the X plane first.

  double corrected_xCoord = coordinates[0]*1000.0 - x_offset;
  Xindex = (int )rint( corrected_xCoord * wiresPERmm ) + middleWire;

// Use edge wires as overflow/underflow guides.  Put excess counts there.

  if(Xindex > binNumber-1) {   // Too high!!
    Xindex = binNumber-1;
  } else if (Xindex < 0) {     // Too low!!
    Xindex = 0;
  }
  x_bins[Xindex]++;
  
// Calculate the Y plane next.

  double corrected_yCoord = coordinates[1]*1000.0 - y_offset; 
  Yindex = (int )rint( corrected_yCoord * wiresPERmm )  + middleWire;

  if(Yindex > binNumber-1) {   // Too high!!
    Yindex = binNumber-1;
  } else if (Yindex < 0) {     // Too low!!
    Yindex = 0;
  }
  y_bins[Yindex]++;
}

void mwireMonitor::setWireSpacing(double spacing) {
//******************************************************
//
// 
//
//******************************************************
  if(spacing <= 0) { 
    cerr << "\n";
    cerr << "*** ERROR ***                                            \n";
    cerr << "*** ERROR *** mwireMonitor::setWireSpacing( double )     \n";
    cerr << "*** ERROR *** wire spacing can't be negative or zero.    \n";
    cerr << "*** ERROR ***                                            \n";
    cerr << "\n";
    return;
  }
  wireSpacing = spacing;
  char msg[80];
  sprintf(msg,"changeWireSpacing %lf ",wireSpacing);
  *p << msg << endl;
  return;
}

void mwireMonitor::resetXplot() {
//******************************************************
//
// This routine clears the x-plane multiwire plot.
//
//******************************************************
  for(int i=0; i<binNumber; i++){
    x_bins[i] = 0;
  }
  *p << "clearXvector " << endl;
}

void mwireMonitor::resetYplot() {
//******************************************************
//
// This routine clears the y-plane multiwire plot.
//
//******************************************************
  for(int i=0; i<binNumber; i++){
    y_bins[i] = 0;
  }
  *p << "clearYvector " << endl;
}

void mwireMonitor::setXOffset(double offset) {
  x_offset = offset;
}

void mwireMonitor::setYOffset(double offset) {
  y_offset = offset;
}

#endif // Exclude under Visual C++ and Borland builds.
