#if !defined(__VISUAL_CPP__) && !defined(__BORLAND_CPP__)
#ifndef MWIREMONITOR_H
#define MWIREMONITOR_H

#include "beamline.h"
class ParticleBunch;
class iopipestream;

class mwireMonitor : public monitor {  

  private:

    int           windowInitialized;   // Flag for determining if the 
                                       // display window has been made.
    char*         display;             // Tcl parent window name.
    iopipestream* p;                   // Pipestream where a blt_wish process
                                       // will listen.  Send commands here.  
    int           childpid;
    int           binNumber;           // Number of bins.
    double        wireSpacing;         // Wire spacing in mm.
    double        x_fs;                // Full scale value on x-plane plot.
    double        x_fiducial;          // User-defined marker location on SWIC.
    int*          x_bins;              // Array to hold counts.
    double        y_fs;                // Full scale value on y-plane plot.
    double        y_fiducial;          // User-defined marker location on SWIC.
    int*          y_bins;              // Array to hold counts.
    int           size;                // Size in pixels of the widget.
    int           Xindex;              // The latest bin populated by calcBin()
    int           Yindex;              // The latest bin populated by calcBin()
    void          scaleCheck();        // check scaling on constructor/on() 
                                       // and turn autoscaling off if not 0.
    double        x_offset;            // x offset of x plot. 
    double        y_offset;            // x offset of y plot.

    void          calcBin(const double*);  // calculates what X & Y bins to 
                                           // increment.

  public:
    mwireMonitor();
    mwireMonitor( char* );              // Name identifier.
    mwireMonitor( char*,                // Name identifier.
		  FILE*                 // Data written to file.
                );
    mwireMonitor( char*,                // Name identifier.
		  double                // Wire spacing (mm).
		);
    mwireMonitor( char*,                // Name identifier.
		  FILE*,                 // Data written to file.
		  double                // Wire spacing (mm).
		);
    mwireMonitor( char*,                // Name identifier.
		  double,               // Wire spacing (mm).
                  double,               // x full scale value.
                  double                // y full scale value.
                );
    mwireMonitor( char*,                // Name identifier.
                  double,               // x full scale value.
                  double                // y full scale value.
                );
    mwireMonitor( const mwireMonitor& );

    ~mwireMonitor();

    void localPropagate( Particle& );
    void localPropagate( JetParticle& );
    void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }

    char*      Type() const;
    inline bmlnElmnt* Clone() const { return new mwireMonitor( *this ); }

    void on();                       // Turns monitor on.
    void on(char*);                  // Turns monitor on and defines where
                                     // to display it (like dudley.fnal.gov)

    void   setSpacing(double);                  // Set wire spacing.
    inline double getSpacing() { return wireSpacing; } // Return spacing.
    void   setWireSpacing(double);           // Set wire spacing.

    void   resetXplot();             // Clears X-plane multiwire plot. 
    void   resetYplot();             // Clears Y-plane multiwire plot.

    void   setXOffset(double);	     // Sets the user defined offset in X.
    inline double getXOffset() { return x_offset; }  // Gets X axis
                                                     // offset of the X plot.
    void   setYOffset(double);       // Sets the user defined offset in Y.
    inline double getYOffset() { return y_offset; }  // Gets X axis
                                                     // offset of the Y plot.
};

#endif // MWIREMONITOR_H

#endif // Exclude under Visual C++ and Borland builds.
