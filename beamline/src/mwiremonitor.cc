#include "mwiremonitor.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include <tk.h>
#include "tclf.h"

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
}

mwireMonitor::~mwireMonitor() {
  if(onOff){  // Only kill if a window was started.
    *p << "exit\n" << flush;
    delete p;
  }
  if(display != 0) 
    delete [] display;

  delete [] x_bins;
  delete [] y_bins;
}

//******************************************************
//
//  This will check the scaling and if it is non-zero, the
//  scaling in the mwireMonitor will be set.
//
//******************************************************
void mwireMonitor::scaleCheck() {
  if((x_fs != 0.0) && (y_fs != 0.0)) {
    *p << "$p.x_fr.bar yaxis configure -max " << x_fs << "\n" << flush;
    *p << "$p.y_fr.bar yaxis configure -max " << y_fs << "\n" << flush;
  } 
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
    *p << "source $env(FNALROOT)/tcl/scripts/mwiremonitor.tcl\n" 
       << flush; 
    // Give the main window the name of the monitor.
    *p << "wm title . " << Name() << " \n" << flush; 
    scaleCheck();
    char msg[80];
    setWireSpacing(wireSpacing);
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
    *p << "source $env(FNALROOT)/tcl/scripts/mwiremonitor.tcl\n" 
       << flush; 
    // Give the main window the name of the monitor.
    *p << "wm title . " << Name() << " \n" << flush; 
    scaleCheck();
    setWireSpacing(wireSpacing);
  }
}

void mwireMonitor::propagate( Particle& part) {
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
      *p << "source $env(FNALROOT)/tcl/scripts/mwiremonitor.tcl\n" 
         << flush; 
      scaleCheck();
    }

    double    coords[6];  

    part.getState(coords);
    calcBin(coords);

    char msg[80];
    sprintf(msg,"set xVector(%d) %d\n",Xindex,x_bins[Xindex]);
    *p << msg << flush;
    sprintf(msg,"set yVector(%d) %d\n",Yindex,y_bins[Yindex]);
    *p << msg << flush;
    *p << "update idletasks\n" << flush; 

    // If file I/O is enabled, let the base class do the work!
    if(outputFile != stdout && outputFile != 0){
      monitor::propagate(part);
    }
  }
}

void mwireMonitor::propagate( JetParticle& jpart) {
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
      *p << "source $env(FNALROOT)/tcl/scripts/mwiremonitor.tcl\n" 
         << flush; // init script to read.
      scaleCheck();
    }

    // Pull off the value of the particle position and send to the interpreter.
    Jet    JetParticleCoords[6];
    double Coords[6];
    jpart.getState(JetParticleCoords);

    if(outputFile != stdout && outputFile != 0){
      monitor::propagate(jpart);
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

void mwireMonitor::setSpacing(double spacing ) {
//******************************************************
// 
//  Method to set the wire spacing of the mwireMonitor.
//
//******************************************************

  if(spacing <= 0) { 
    cerr << "\n";
    cerr << "*** ERROR ***                                            \n";
    cerr << "*** ERROR *** mwireMonitor::setSpacing( double )\n";
    cerr << "*** ERROR *** Setting wire spacing to negative or zero.  \n";
    cerr << "*** ERROR ***                                            \n";
    cerr << "\n";
    return;
  }
  wireSpacing = spacing;  
  return;
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
  sprintf(msg,"changeWireSpacing %lf\n",wireSpacing);
  *p << msg << flush;
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
  *p << "clearXvector \n" << flush;
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
  *p << "clearYvector \n" << flush;
}

void mwireMonitor::setXOffset(double offset) {
  x_offset = offset;
}

void mwireMonitor::setYOffset(double offset) {
  y_offset = offset;
}
