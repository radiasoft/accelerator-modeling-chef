#if !defined(_MSC_VER) && !defined(__BORLANDC__) && !defined(_EXCLUDE_MONITORS_)
#ifndef TCLMONITOR_H
#define TCLMONITOR_H

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif

class iopipestream;
class Particle;
class ParticleBunch;
class Vector;

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

    /// Physical beamline coordinates to Tk canvas X coordinates conversion routines.
    int atopixx(double x, double max, double min) 
                                   { return (int)((x-min)*size/(max-min));}
    /// Physical beamline coordinates to Tk canvas Y coordinates conversion routines.
    int atopixy(double y, double max, double min) 
                                   { return (int)((-y+max)*size/(max-min));}

    /// Tk canvas X coordinates to Physical beamline coordinates conversion routines.
    double apixtox(int x, double max, double min) 
                                   { return min+(max-min)*x/(double)size;}
    /// Tk canvas Y coordinates to Physical beamline coordinates conversion routines.
    double apixtoy(int y, double max, double min) 
                                   { return max-(max-min)*y/(double)size;}

  public:
    tclMonitor();
    /// Name identifier.
    tclMonitor( char* );
    /// Name identifier, x plot limit (meters), x prime plot limit (radians), y plot limit (meters), y prime plot limit (radians), cdt plot limit, dp/p plot limit.
    tclMonitor( char*, double, double, double, double, double, double);
    /// Name identifier, parent tcl window. 
    tclMonitor( char*, char* );
    /// Name identifier, parent tcl window name, file output name.
    tclMonitor( char*, char*, FILE* );
    /// Copy constructor.
    tclMonitor( const tclMonitor& );

    ~tclMonitor();

    void localPropagate( Particle& );
    void localPropagate( JetParticle& );
    void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }

    /// Toggles origin registration
    void RegisterOrigin( Particle& );
    /// Resets origin to zero.
    void ResetOrigin();

    const char* Type() const;
    bmlnElmnt* Clone() const { return new tclMonitor( *this ); }
    int        getColor();
    void       setColor(int);
    /// Turns monitor on.
    void on();
    /// Turns monitor on and defines where to display it.
    void on(char*);
    void Off();
};

#endif // TCLMONITOR_H

#endif // Exclude under Visual C++ and Borland builds.
