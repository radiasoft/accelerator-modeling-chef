#if !defined(__VISUAL_CPP__) && !defined(__BORLAND_CPP__)
#ifndef TCLMONITOR_H
#define TCLMONITOR_H

#include "mxyzptlk.h"
#include "beamline.inc"
#include "pipestream.h"

const int MAX_COLOR = 5;      // Maximum number of colors.  Modifications here
                              // must also be reflected in tclmonitor.tcl

/// tclMonitor class.
/** This class provides a visual display of phase space for a beamline
for which it is inserted.  It is a zero-length element, so that it can
be added to a beamline without changing its physical characteristics.
Once the element is added, it will stay dormant until the
tclMonitor::on() method is called, which actually displays the
element.  Since it takes some time (a couple of seconds or so) for the
display to come up, if you are tracking a single particle through
something very small (like a single FODO cell), the program may end
before the tclMonitor is actually displayed.

Once the data is displayed, the user can change the scale for any of
the three phase space plots, and the results will re-plotted
immediately.  Since the tclMonnitor keeps track of the coordinates of
each particle it sees, if there are alot of particles to re-scale,
this could take some time.
*/

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

    // Physical beamline coordinates to Tk canvas coordinates 
    // conversion rooutines.
    int atopixx(double x, double max, double min) 
                                   { return (int)((x-min)*size/(max-min));}
    int atopixy(double y, double max, double min) 
                                   { return (int)((-y+max)*size/(max-min));}

    // Tk canvas coordinates to Physical beamline coordinates 
    // conversion rooutines.
    double apixtox(int x, double max, double min) 
                                   { return min+(max-min)*x/(double)size;}
    double apixtoy(int y, double max, double min) 
                                   { return max-(max-min)*y/(double)size;}

  public:
    tclMonitor();
    tclMonitor( char* );                     /// Name identifier.

    tclMonitor( char*,                       /// Name identifier.
  	        double,                      /// x plot limit (meters).
	        double,                      /// x prime plot limit (radians).
	        double,                      /// y plot limit (meters).
	        double,                      /// y prime plot limit (radians).
	        double,                      /// cdt plot limit.
	        double);                     /// dp/p plot limit.

    tclMonitor( char*,                       /// Name identifier.
                char* );                     /// and parent tcl window.

    tclMonitor( char*,                       /// Name identifier.
                char*,                       /// parent tcl window name.
                FILE* );                     /// file output name.

    tclMonitor( const tclMonitor& );         /// Copy constructor.

    ~tclMonitor();

    void propagate( Particle& );
    void propagate( JetParticle& );
    void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }

    /// Toggles origin registration
    void RegisterOrigin( Particle& );
    /// Resets origin to zero.
    void ResetOrigin();

    char*      Type() { return "tclMonitor"; }
    bmlnElmnt* Clone() { return new tclMonitor( *this ); }
    int        getColor();
    void       setColor(int);
    /// Turns monitor on.
    void on();
    // Turns monitor on and defines where to display it.
    void on(char*);
    void Off();
};

#endif // TCLMONITOR_H

#endif // Exclude under Visual C++ and Borland builds.
