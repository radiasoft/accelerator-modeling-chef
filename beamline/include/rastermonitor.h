#if !defined(__VISUAL_CPP__) && !defined(__BORLAND_CPP__) && !defined(_EXCLUDE_MONITORS_)
#ifndef RASTERMONITOR_H
#define RASTERMONITOR_H

#include "beamline.h"
class ParticleBunch;
class Particle;
class Vector;
class iopipestream;

/// rasterMonitor class.
/** This class provides a visual display of phase space for a beamline
for which it is inserted.  It is a zero-length element, so that it can
be added to a beamline without changing its physical characteristics.
Once the element is added, it will stay dormant until the
rasterMonitor::on() method is called, which actually displays the
element.  Since it takes some time (a couple of seconds or so) for the
display to come up, if you are tracking a single particle through
something very small (like a single FODO cell), the program may end
before the rasterMonitor is actually displayed.

This class differs from the tclMonitor class in that it does not allow
the user to interactively re-scale the points once they are plotted.
The rasterMonitor dos not keep track of the coordinates of the points
sent to it.  If the user wishes to change the scale of the
rasterMonitor, they must use one of the set?plotScaling() methods.
*/

class rasterMonitor : public monitor {  

private:
  const static int WISH_WAIT_TIME;
    char          registerMode;             // 
    Particle*     registerParticle;         // Particle used to set origin.
    Vector        origin;                   // Origin of plot.
    int           windowInitialized;        // Flag for determining if the 
                                            // display window has been made.
    char*         display;                  // Tcl parent window name.
    iopipestream* p;
    int           childpid;

    // x-x' phase space plot scaling            [0]    [1]     [2]    [3]
    double        x_plot_limits[4];         // x min, x max, y min, y max.

    // y-y' phase space plot scaling
    double        y_plot_limits[4];         // x min, x max, y min, y max.

    // cdt-dp/p phse space plot scaling
    double        cdt_plot_limits[4];       // x min, x max, y min, y max.

    int           size;                     // Size in pixels of the raster
                                            // widget.
    int           normalizedFlag; 	    // Use normalized phase space 
                                            // coordinates	      
    lattFunc*     plotFunc;                 // The lattice functions to be used
                                            // for plotting.

    // Physical beamline coordinates to Tk canvas coordinates conversion routines.
    int atopixx(double x, double max, double min) 
                                   { return (int)((x-min)*size/(max-min));}
    int atopixy(double y, double max, double min) 
                                   { return (int)((-y+max)*size/(max-min));}

    // Tk canvas coordinates to Physical beamline coordinates conversion routines.
    double apixtox(int x, double max, double min) 
                                   { return min+(max-min)*x/(double)size;}
    double apixtoy(int y, double max, double min) 
                                   { return max-(max-min)*y/(double)size;}

    unsigned long numParticles;  // Number of particles seen since last
                                 // call to resetX/Y/Zplot().
    double sumState[6];          // sum of the coordinates of all particles
                                 // seen since last call to resetX/Y/Zplot().
    void setNewScaling();        // Sends plot limits to raster.

    static int WISH_WAIT_TIME;

  public:
    //@Man: Constructors and Destructors
    //@{
    ///
    rasterMonitor();
    ///
    rasterMonitor(
		  /// Name identifier.
		   char*
		 );
    ///
    rasterMonitor( 
                   /// Name identifier.
		   char*,
                   /// file output name.
                   FILE*
		 );

    ///
    rasterMonitor( 
                  /// Name identifier.
		  char*,
                  /// x plot limit.
  	          double,
                  /// x prime plot limit.
	          double,
                  /// y plot limit.
	          double,
                  /// y prime plot limit.
	          double,
                  /// cdt plot limit.
	          double,
                  /// dp/p plot limit.
	          double
		 );

    ///
    rasterMonitor(
                   /// Name identifier.
		   char*,
                   /// and parent tcl window.
                   char*
		 );

    ///
    rasterMonitor(
                   /// Name identifier.
		   char*,
                   /// parent tcl window name.
                   char*,
                   /// file output name.
                   FILE*
		 );

    rasterMonitor( const rasterMonitor& );

    ///
    ~rasterMonitor();
    //@}

    //@Man: Propagators
    //@{
    ///
    void localPropagate( Particle& );
    ///
    void localPropagate( JetParticle& );
    ///
    void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
    //@}

    //@Man: Origin manipulation
    //@{
    /// Toggles origin registration.
    void RegisterOrigin( Particle& );
    /// Resets origin to zero.
    void ResetOrigin();
    //@}

    ///  Returns a string describing the type of the element ("rasterMonitor")
    char* Type() const;
  int isType(char* c) { if ( strcmp(c, "rasterMonitor") != 0 ) return monitor::isType(c); else return 1; }

    ///
    bmlnElmnt* Clone() const { return new rasterMonitor( *this ); }

    /// Turns monitor on.
    void on();
    /// Turns monitor on and defines where to display it (like dudley.fnal.gov)
    void on(char*);
    void Off();                      // THIS DOESN'T EXIST!!!!

    //@Man: Scaling routines 
    //@{
    /// Retrieve the scaling min/max for x-x' phase space plot. 
    void  getXplotScaling(double*);
    /// Set the scaling min/max for x-x' phase space plot.
    void  setXplotScaling(double*);
    /// Clears phase space plot.
    void  resetXplot();

    /// Retrieve the scaling min/max for y-y' phase space plot. 
    void  getYplotScaling(double*);
    /// Set the scaling min/max for y-y' phase space plot.
    void  setYplotScaling(double*);
    /// Clears phase space plot.
    void  resetYplot();

    /// Retrieve the scaling min/max for cdt-dp/p phase space plot.
    void  getZplotScaling(double*);
    /// Set the scaling min/max for cdt-dp/p phase space plot.
    void  setZplotScaling(double*);
    /// Clears phase space plot.
    void  resetZplot();

    /// Retrieve scaling for all plots.
    void  getAllplotScaling(double*);
    /// Set scaling for all plots.
    void  setAllplotScaling(double*);

    /// Turn on the normalized scaling.
    void  normalizedScaleOn();
    ///Memo: Turn off the normalized scaling.
    void  normalizedScaleOff();
    /// A null pointer means that the monitor should use the attached twiss Barnacle.
    void  setNormalizedScale(lattFunc*);
    //@}
    double getAveXstate();
    double getAveXPRIMEstate();
    double getAveYstate();
    double getAveYPRIMEstate();
};

#endif // RASTERMONITOR_H

#endif // Exclude under Visual C++ and Borland builds.
