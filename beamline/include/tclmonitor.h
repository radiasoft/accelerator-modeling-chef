#ifndef TCLMONITOR_H
#define TCLMONITOR_H

#include "mxyzptlk.h"
#include "beamline.inc"
#include "pipestream.h"

const int MAX_COLOR = 5;      // Maximum number of colors.  Modifications here
                              // must also be reflected in tclmonitor.tcl

class tclMonitor : public monitor {  

private:
    char        registerMode;                // 
    Particle*   registerParticle;            // Particle used to set origin
    Vector      origin;                      // Origin of plot

    static int  color;                       // All tclMonitors know new color.
    int         windowInitialized;           // Flag true if window is made.
    char*       display;                     // Tcl parent window name.
    iopipestream* p;
    int         childpid;
    double      axmin,axmax,aymin,aymax;     // Tk phaseplot min and max
    double      bxmin,bxmax,bymin,bymax;     // Tk phaseplot min and max
    double      cxmin,cxmax,cymin,cymax;     // Tk phaseplot min and max
    int         size;

// Physical beamline coordinates to Tk canvas coordinates conversion rooutines.
    int atopixx(double x, double max, double min) 
                                   { return (int)((x-min)*size/(max-min));}
    int atopixy(double y, double max, double min) 
                                   { return (int)((-y+max)*size/(max-min));}

// Tk canvas coordinates to Physical beamline coordinates conversion rooutines.
    double apixtox(int x, double max, double min) 
                                   { return min+(max-min)*x/(double)size;}
    double apixtoy(int y, double max, double min) 
                                   { return max-(max-min)*y/(double)size;}

  public:
    tclMonitor();
    tclMonitor( char* );                     // Name identifier.

    tclMonitor( char*,                       // Name identifier.
  	        double,                      // x plot limit (meters).
	        double,                      // x prime plot limit (radians).
	        double,                      // y plot limit (meters).
	        double,                      // y prime plot limit (radians).
	        double,                      // cdt plot limit.
	        double);                     // dp/p plot limit.

    tclMonitor( char*,                       // Name identifier.
                char* );                     // and parent tcl window.

    tclMonitor( char*,                       // Name identifier.
                char*,                       // parent tcl window name.
                FILE* );                     // file output name.

    ~tclMonitor();

    void propagate( Particle& );
    void propagate( JetParticle& );
    void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }

    void RegisterOrigin( Particle& ); // Toggles origin registration
    void ResetOrigin();               // Resets origin to zero.

    char*      Type() { return "tclMonitor"; }
    bmlnElmnt* Clone() { return new tclMonitor( *this ); }
    int        getColor();
    void       setColor(int);
    void on();                      // Turns monitor on.
    void on(char*);                 // Turns monitor on and defines where
                                    // to display it.
    void Off();
};

#endif // TCLMONITOR_H

