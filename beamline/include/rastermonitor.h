#ifndef RASTERMONITOR_H
#define RASTERMONITOR_H

#include "mxyzptlk.h"
#include "beamline.inc"
#include "ParticleBunch.h"
#include "pipestream.h"

class rasterMonitor : public monitor {  

  private:
    char          registerMode;             // 
    Particle*     registerParticle;         // Particle used to set origin.
    Vector        origin;                   // Origin of plot.
    int           windowInitialized;        // Flag for determining if the 
                                            // display window has been made.
    char*         display;                  // Tcl parent window name.
    iopipestream* p;
    int           childpid;

// x-x' phase space plot scaling                [0]    [1]     [2]    [3]
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
    void setNewScaling();        // Sends plot limits to raster.

  public:
    rasterMonitor();
    rasterMonitor( char* );                 // Name identifier.
    rasterMonitor( char*,                   // Name identifier.
                   FILE* );                 // file output name.

    rasterMonitor( char*,                   // Name identifier.
  	          double,                   // x plot limit.
	          double,                   // x prime plot limit.
	          double,                   // y plot limit.
	          double,                   // y prime plot limit.
	          double,                   // cdt plot limit.
	          double);                  // dp/p plot limit.

    rasterMonitor( char*,                   // Name identifier.
                   char* );                 // and parent tcl window.

    rasterMonitor( char*,                   // Name identifier.
                   char*,                   // parent tcl window name.
                   FILE* );                 // file output name.

    ~rasterMonitor();

    void propagate( Particle& );
    void propagate( JetParticle& );
    void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }

    void RegisterOrigin( Particle& );       // Toggles origin registration.
    void ResetOrigin();                     // Resets origin to zero.

    char*      Type()  { return "rasterMonitor"; }
    bmlnElmnt* Clone() { return new rasterMonitor( *this ); }

    void on();                       // Turns monitor on.
    void on(char*);                  // Turns monitor on and defines where
                                     // to display it (like dudley.fnal.gov)
    void Off();

    void  getXplotScaling(double*);  // Retrieve the scaling min/max for x-x'
                                     // phase space plot. 
    void  setXplotScaling(double*);  // Set the scaling min/max for x-x'
                                     // phase space plot.
    void  resetXplot();              // Clears phase space plot.

    void  getYplotScaling(double*);  // Retrieve the scaling min/max for y-y'
                                     // phase space plot. 
    void  setYplotScaling(double*);  // Set the scaling min/max for y-y'
                                     // phase space plot. 
    void  resetYplot();              // Clears phase space plot.

    void  getZplotScaling(double*);  // Retrieve the scaling min/max for 
                                     // cdt-dp/p phase space plot.
    void  setZplotScaling(double*);  // Set the scaling min/max for cdt-dp/p 
                                     // phase space plot.
    void  resetZplot();              // Clears phase space plot.

    void  getAllplotScaling(double*);// Retrieve scaling for all plots.
    void  setAllplotScaling(double*);// Set scaling for all plots.
    void  normalizedScaleOn();       // Turn on the normalized scaling.
    void  normalizedScaleOff();      // Turn off the normalized scaling.
    void  setNormalizedScale(lattFunc*); // A null pointer means that the monitor should
                                         // use the attached twiss Barnacle.
};

#endif // RASTERMONITOR_H

