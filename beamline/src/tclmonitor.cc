#if !defined(__VISUAL_CPP__) && !defined(__BORLAND_CPP__)
#include <stream.h>
#include "pipestream.h"
#include "ParticleBunch.h"
#include "tclmonitor.h"
#include <tk.h>
#include "tclf.h"

int tclMonitor::color = 0;

tclMonitor::tclMonitor() : monitor(), origin( BMLN_dynDim ) {
  registerMode    = 0;
  registerParticle = 0;
  for( int i = 0; i < BMLN_dynDim; i++ ) origin(i) = 0.0;

  // Default scalings for the phase plot if the user provides nothing.
  outputFile = 0;
  axmin = -0.005;
  axmax =  0.005;
  aymin = -0.0001;
  aymax =  0.0001;

  bxmin = -0.005;
  bxmax =  0.005;
  bymin = -0.0001;
  bymax =  0.0001;

  cxmin = -0.8;
  cxmax =  0.8;
  cymin = -0.002;
  cymax =  0.002;
  size = 200;        // Full width of the canvas.
  windowInitialized = 0;
  display = 0;
}

tclMonitor::tclMonitor(char* name) : monitor(name), origin( BMLN_dynDim )  {
  registerMode = 0;
  registerParticle = 0;
  for( int i = 0; i < BMLN_dynDim; i++ ) origin(i) = 0.0;

  // Default scalings for the phase plot if the user provides nothing.
  outputFile = 0;
  axmin = -0.005;
  axmax =  0.005;
  aymin = -0.0001;
  aymax =  0.0001;

  bxmin = -0.005;
  bxmax =  0.005;
  bymin = -0.0001;
  bymax =  0.0001;

  cxmin = -0.8;
  cxmax =  0.8;
  cymin = -0.002;
  cymax =  0.002;
  size = 200;        // Full width of the canvas.
  windowInitialized = 0;
  display = 0;
}

tclMonitor::tclMonitor(char* name, double x, double xp, 
                                   double y, double yp,
                                   double cdt, double dp) 
: monitor(name), origin( BMLN_dynDim ) {

  registerMode = 0;
  registerParticle = 0;
  for( int i = 0; i < BMLN_dynDim; i++ ) origin(i) = 0.0;

  // Default scalings for the phase plot if the user provides nothing.
  outputFile = 0;
  axmin =  -x;
  axmax =   x;
  aymin =  -xp;
  aymax =   xp;

  bxmin = -y;
  bxmax =  y;
  bymin = -yp;
  bymax =  yp;

  cxmin = -cdt;
  cxmax =  cdt;
  cymin = -dp;
  cymax =  dp;
  size = 200;        // Full width of the canvas.
  windowInitialized = 0;
  display = 0;
}

tclMonitor::tclMonitor( char* name, char* parent_wid ) 
: monitor(name), origin( BMLN_dynDim ) {

  registerMode = 0;
  registerParticle = 0;
  for( int i = 0; i < BMLN_dynDim; i++ ) origin(i) = 0.0;

  // Default scalings for the phase plot if the user provides nothing.
  outputFile = 0;
  axmin = -0.005;
  axmax =  0.005;
  aymin = -0.0001;
  aymax =  0.0001;

  bxmin = -0.005;
  bxmax =  0.005;
  bymin = -0.0001;
  bymax =  0.0001;

  cxmin = -0.8;
  cxmax =  0.8;
  cymin = -0.002;
  cymax =  0.002;
  size = 200;        // Full width of the canvas.
  display = new char[strlen(parent_wid)+1];
  strcpy(display,parent_wid);
  windowInitialized = 0;
}

tclMonitor::tclMonitor(char* name, char* parent_wid, FILE* out_file) 
: monitor(name,out_file), origin( BMLN_dynDim ) {

  registerMode = 0;
  registerParticle = 0;
  for( int i = 0; i < BMLN_dynDim; i++ ) origin(i) = 0.0;

  // Default scalings for the phase plot if the user provides nothing.
  axmin = -0.005;
  axmax =  0.005;
  aymin = -0.0001;
  aymax =  0.0001;

  bxmin = -0.005;
  bxmax =  0.005;
  bymin = -0.0001;
  bymax =  0.0001;

  cxmin = -0.8;
  cxmax =  0.8;
  cymin = -0.002;
  cymax =  0.002;
  size = 200;        // Full width of the canvas.
  display = new char[strlen(parent_wid)+1];
  strcpy(display,parent_wid);
  windowInitialized = 0;
}

tclMonitor::tclMonitor( const tclMonitor& x ) 
: monitor( (monitor&) x), origin(x.origin) {
  registerMode = x.registerMode;
  windowInitialized = x.windowInitialized;
  axmin = x.axmin;
  axmax = x.axmax;
  aymin = x.aymin;
  aymax = x.aymax;
  bxmin = x.bxmin;
  bxmax = x.bxmax;
  bymin = x.bymin;
  bymax = x.bymax;
  cxmin = x.cxmin;
  cxmax = x.cxmax;
  cymin = x.cymin;
  cymax = x.cymax;
  size = x.size;
  if(x.display != 0) {
    display = new char[strlen(x.display)+1];
  strcpy(display,x.display);
  } else {
    display = 0;
  }
}

tclMonitor::~tclMonitor() {
  if(onOff) {
    *p << "exit\n" << flush;
    delete p;
  }
  if(display != 0)
    delete [] display;
}

void tclMonitor::RegisterOrigin( Particle& part ) {
  if( part.State().Dim() != origin.Dim() ) origin = part.State();
  for( int i = 0; i < origin.Dim(); i++ ) origin(i) = 0.0;
  registerParticle = &part;
  registerMode = 1;
}

void tclMonitor::ResetOrigin() {
  for( int i = 0; i < origin.Dim(); i++ ) origin(i) = 0.0;
}

void tclMonitor::on() {
  onOff = 1;
   if(!windowInitialized) {
     windowInitialized = 1;
     p = new iopipestream("wish");             // Start the child process.
     *p << "source $env(FNALROOT)/tcl/scripts/tclmonitor.tcl\n" 
        << flush; 
     // Give the main window the name of the monitor.
     *p << "wm title . " << Name() << " \n" << flush; 
     // Place coordinates on the windows.
     *p << ::form("place_limits %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f\n",
	     axmax,aymax,bxmax,bymax,cxmax,cymax) << flush;
   }
}

void tclMonitor::on(char* whereToDisplay) {
  char processMsg[80];
  onOff = 1;
   if(!windowInitialized) {
     windowInitialized = 1;
     sprintf(processMsg,"wish -display %s:0",whereToDisplay);
     p = new iopipestream(processMsg);             // Start the child process.
     *p << "source $env(FNALROOT)/tcl/scripts/tclmonitor.tcl\n" 
        << flush; 
     // Give the main window the name of the monitor.
     *p << "wm title . " << Name() << " \n" << flush; 
     // Place coordinates on the windows.
     *p << ::form("place_limits %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f\n",
	     axmax,aymax,bxmax,bymax,cxmax,cymax) << flush;
   }
}

void tclMonitor::localPropagate( Particle& part) {
//******************************************************
//
//  This should update the display of the phase-space plot if 
//  the tclmonitor is turned on.
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

   // Pull off the value of the particle position and send to the interpreter.
   Vector ParticleCoords( part.State() );
   ParticleCoords -= origin;

   if(outputFile != 0) {
      fprintf( outputFile, "\nHBPM: %s ......... ", ident );
     fprintf(outputFile, "%lf   %lf\n", ParticleCoords(0), ParticleCoords(3));
   }


//   cout << " X = " << ParticleCoords(0) << " Xprime = " 
//        << ParticleCoords(3) << endl;
//   cout << " Y = " << ParticleCoords(1) << " Yprime = " 
//        << ParticleCoords(4) << endl;
//   cout << " cdt= " << ParticleCoords(2) << " dp/p    = " 
//        << ParticleCoords(5) << endl;

   *p << ::form("update_plot %d %d %d %d %d %d %d\n",
	   atopixx(ParticleCoords(0),axmax,axmin),
	   atopixy(ParticleCoords(3),aymax,aymin),
	   atopixx(ParticleCoords(1),bxmax,bxmin),
	   atopixy(ParticleCoords(4),bymax,bymin),
	   atopixx(ParticleCoords(2),cxmax,cxmin),
	   atopixy(ParticleCoords(5),cymax,cymin),
	   color) << flush;

   // Block now to receive the return message and whether the user has 
   // interrupted on the plots to give new initial conditions.

   char  result[80];
   char* tmpbuf;
   tmpbuf = new char[80];
   char* strPtr;

   if(p->getline(result,80) == 0) exit(0); 
   //   cout << "Wish sent back: " << result;
   if(strncmp(result,"done",strlen("done"))) {
     if(!strncmp(result,"stop",strlen("stop"))) {
       char* returnMsg;
       returnMsg = new char[strlen(this->Type())+strlen(this->Name()) +
			    strlen("/:stop; ")];
       sprintf(returnMsg,"%s//$s:stop;",this->Type(),this->Name());
       delete [] returnMsg;
       onOff = 0;
     } else {

       strncpy(tmpbuf,result,80);
       strPtr = strtok(tmpbuf," ");
       if(!strcmp(strPtr,".fr.x.plot")) {
	 strPtr = strtok(0," ");
	 axmax = atof(strPtr);
	 axmin = -axmax;
	 strPtr = strtok(0," ");
	 aymax = atof(strPtr);
	 aymin = -aymax;
	 cout << "new x limits " << axmax << " " << aymax << endl;
       } else if(!strcmp(strPtr,".fr.y.plot")) {
	 strPtr = strtok(0," ");
	 axmax = atof(strPtr);
	 bxmin = -bxmax;
	 strPtr = strtok(0," ");
	 bymax = atof(strPtr);
	 bymin = -bymax;
	 cout << "new y limits " << bxmax << " " << bymax << endl;
       } else if(!strcmp(strPtr,"x")) {
	 strPtr = strPtr + strlen("x")+1;              // +1 to skip '\0'
	 strPtr = strtok(0," ");
	 cout << "Got x as: " << strPtr;
	 ParticleCoords(0) = apixtox(atoi(strPtr),axmax,axmin);
	 strPtr = strtok(0," ");
	 cout << " and x' as: " << strPtr << endl;
	 ParticleCoords(3) = apixtoy(atoi(strPtr),aymax,aymin);
       } else if (!strcmp(strPtr,"y")) {
	 strPtr = strPtr + strlen("y")+1;              // +1 to skip '\0'
	 strPtr = strtok(0," ");
	 cout << "Got y as: " << strPtr;
	 ParticleCoords(1) = apixtox(atoi(strPtr),bxmax,bxmin);
	 strPtr = strtok(0," ");
	 cout << " and y' as: " << strPtr << endl;
	 ParticleCoords(4) = apixtoy(atoi(strPtr),bymax,bymin);
       } else if (!strcmp(strPtr,"cdt")) {
	 strPtr = strPtr + strlen("cdt")+1;             // +1 to skip '\0'
	 strPtr = strtok(0," ");
	 cout << "Got cdt as: " << strPtr;
	 ParticleCoords(2) = apixtox(atoi(strPtr),cxmax,cxmin);
	 strPtr = strtok(0," ");
	 cout << " and dp/p as: " << strPtr << endl;
	 ParticleCoords(5) = apixtoy(atoi(strPtr),cymax,cymin);
       }
// Must handle this better.
//       if(strlen(strPtr) != 0) {
//	 cout << "Undecipherable message: " << result << endl;
//       } else {
       color++;
       if(color >= MAX_COLOR) color = 0;
       cout << "New values from canvas widget are: " << endl;
       cout << " X = " << ParticleCoords(0) << " Xprime = " 
	 << ParticleCoords(3) << endl;
       cout << " Y = " << ParticleCoords(1) << " Yprime = " 
	 << ParticleCoords(4) << endl;
       cout << " cdt= " << ParticleCoords(2) << " dp/p    = " 
	 << ParticleCoords(5) << endl;
       part.setState( origin + ParticleCoords );
       //       }
     }
   }
   delete [] tmpbuf;
 }
}

void tclMonitor::localPropagate( JetParticle& jpart) {
//******************************************************
//
//  This should update the display of the phase-space plot if 
//  the tclmonitor is turned on.
//
//******************************************************

 if ( onOff ) {
   if(!windowInitialized) {
     windowInitialized = 1;
     p = new iopipestream("wish");             // Start the child process.
     *p << "source $env(FNALROOT)/tcl/scripts/tclmonitor.tcl\n" 
        << flush; // init script to read.
     // Give the main window the name of the monitor.
     *p << "wm title . " << Name() << " \n" << flush; 
     // Place coordinates on the windows.
     *p << ::form("place_limits %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f\n",
	     axmax,aymax,bxmax,bymax,cxmax,cymax) << flush;
   }

   // Pull off the value of the particle position and send to the interpreter.
   Jet    JetParticleCoords[6];
   double Coords[6];
   jpart.getState(JetParticleCoords);

   if(outputFile != 0) {
     fprintf( outputFile, "\nHBPM: %s ......... ", ident );
     fprintf( outputFile, "%lf   %lf\n", (JetParticleCoords[0]).standardPart(),
             (JetParticleCoords[3]).standardPart() );
   }
//   cout << " X = " << (JetParticleCoords[0]).standardPart() 
//        << " Xprime = " << (JetParticleCoords[3]).standardPart() << endl;
//   cout << " Y = " << (JetParticleCoords[1]).standardPart() 
//        << " Yprime = " << (JetParticleCoords[4]).standardPart() << endl;
//   cout << " ct= " << (JetParticleCoords[2]).standardPart() 
//        << " E      = " << (JetParticleCoords[5]).standardPart() << endl;

   *p << ::form("update_plot %d %d %d %d %d %d %d\n",
	   atopixx((JetParticleCoords[0]).standardPart(),axmax,axmin),
	   atopixy((JetParticleCoords[3]).standardPart(),aymax,aymin),
	   atopixx((JetParticleCoords[1]).standardPart(),bxmax,bxmin),
	   atopixy((JetParticleCoords[4]).standardPart(),bymax,bymin),
	   atopixx((JetParticleCoords[2]).standardPart(),cxmax,cxmin),
	   atopixy((JetParticleCoords[5]).standardPart(),cymax,cymin),
	   color) << flush;

   // Block now to receive the return message and whether the user has 
   // interrupted on the plots to give new initial conditions.

   char  result[80];
   char* tmpbuf = new char[80];
   char* strPtr;
   if(p->getline(result,80) == 0) exit(0); 
   //   cout << "Wish sent back: " << result;
   if(strncmp(result,"done",strlen("done"))) {
     strcpy(tmpbuf,result);
     strPtr = strtok(tmpbuf," ");
     if(!strcmp(strPtr,"x")) {
       strPtr = strPtr + strlen("x")+1;              // +1 to skip '\0'
       strPtr = strtok(0," ");
       cout << "Got x as: " << strPtr;
       Coords[0] = apixtox(atoi(strPtr),axmax,axmin);
       strPtr = strtok(0," ");
       cout << " and x' as: " << strPtr << endl;
       Coords[3] = apixtoy(atoi(strPtr),aymax,aymin);
     }  
     if (!strcmp(strPtr,"y")) {
       strPtr = strPtr + strlen("y")+1;              // +1 to skip '\0'
       strPtr = strtok(0," ");
       cout << "Got y as: " << strPtr;
       Coords[1] = apixtox(atoi(strPtr),bxmax,bxmin);
       strPtr = strtok(0," ");
       cout << " and y' as: " << strPtr << endl;
       Coords[4] = apixtoy(atoi(strPtr),bymax,bymin);
     }
     if (!strcmp(strPtr,"cdt")) {
       strPtr = strPtr + strlen("cdt")+1;             // +1 to skip '\0'
       strPtr = strtok(0," ");
       cout << "Got cdt as: " << strPtr;
       Coords[2] = apixtox(atoi(strPtr),cxmax,cxmin);
       strPtr = strtok(0," ");
       cout << " and dp/p as: " << strPtr << endl;
       Coords[5] = apixtoy(atoi(strPtr),cymax,cymin);
     }
// Must handle this better.
//     if(strlen(strPtr) != 0) {
//       cout << "Undecipherable message: " << result << endl;
//     } else {
     color++;
     if(color >= MAX_COLOR) color = 0;
       cout << "New values from canvas widget are: " << endl;
       cout << " X = " << Coords[0] << " Xprime = " << Coords[3] << endl;
       cout << " Y = " << Coords[1] << " Yprime = " << Coords[4] << endl;
       cout << " cdt= " << Coords[2] << " dp/p    = " << Coords[5] << endl;
       jpart.setState(Coords);
//     }
   }
   delete [] tmpbuf;
 }
}

int tclMonitor::getColor() {
  return color;
}

void tclMonitor::setColor( int newcolor) {
  if(newcolor >= MAX_COLOR) {
    cout << "Requested new color is beyond maximum color values; set to zero" 
         << endl;
    newcolor = 0;
  }
  color = newcolor;
}

char* tclMonitor::Type() const 
{ 
return "tclMonitor"; 
}


#endif // Exclude under Visual C++ and Borland builds.
