#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include "menu.h"
#include "mxyzptlk.h"

#define	NUMREG 4

Jet**             HPreg;
coord**           crds;
Jet__environment* pje;

int numberOfVar, topWeight;

main( int argc, char** argv ) 
{

  // --- Process input
  if( argc != 3 ) {
    cout << "Usage: " << argv[0] << " n  m      \n"
         << "where n = number of variables and  \n"
         << "      m = maximum order.           \n"
         << endl;
    exit(1);
  }
  
  numberOfVar = atoi( argv[1] );
  topWeight   = atoi( argv[2] );

  if( numberOfVar > 6 ) {
    cout << "Please .. have mercy!!\n";
    cout << "I have reduced the number of variables to 6.\n";
    cout << endl;
    numberOfVar = 6;
    }
  if( topWeight > 12 ) {
    cout << "Please .. have mercy!!\n";
    cout << "I have reduced the maximum order to 12.\n";
    cout << endl;
    topWeight = 12;
    }
  
  int i;
  crds = new coord* [ numberOfVar ];


  // --- Construct the environment
  Jet::BeginEnvironment( topWeight );
  for( i = 0; i < numberOfVar; i++ ) crds[i] = new coord( 0.0 );
  pje = Jet::EndEnvironment();
  
  HPreg = new Jet* [ NUMREG ];
  for( i = 0; i < NUMREG; i++ ) HPreg[i] = new Jet( pje );
  

  // --- Set up the menu and run
  ttMenu menu;
  
  void stvr(),
       rvl(),
       rgop(),
       rith(),
       dffr(),
       nlys(),
       clear(),
       eval();
  
  menu.setPrompt( "Jet TEST ..> " );
  menu.setItem( "Set variable   ", stvr );
  menu.setItem( "Peek           ", rvl  );
  menu.setItem( "Register op's  ", rgop );
  menu.setItem( "Arithmetic     ", rith );
  menu.setItem( "Evaluate       ", eval );
  menu.setItem( "Differential of", dffr );
  menu.setItem( "Analysis       ", nlys );
  menu.setItem( "Clear the stack", clear );
  menu.go();

  for( i = 0; i < NUMREG; i++ ) delete HPreg[i];
  delete [] HPreg;
  for( i = 0; i < numberOfVar; i++ ) delete crds[i];
  delete [] crds;
  delete pje;
}


void stvr(){
  double v;
  int i;
  
  for( i = NUMREG-1; i > 0; i-- ) *(HPreg[i]) = *(HPreg[i-1]);
  
  cout << "Enter the value of the new term ..>  ";
  cin  >> v;
  cout << "Enter one integer corresponding to the index ..>  ";
  cin >> i;
  
  HPreg[0]->setVariable( v, i, pje );
}

void rvl(){
  int j = NUMREG + 1;
  while( ( j < 0 ) || ( j > NUMREG ) ) {
    cout << "Which register?? ..>  ";
    cin >> j;
    }
  HPreg[j]->peekAt();
}

void rgop() {
  ttMenu rgopMenu;
  void entr(),
       xy(),
       rdown();
  rgopMenu.setPrompt( "Reg Ops ..> " );
  rgopMenu.setItem( "Set variable   ", stvr  );
  rgopMenu.setItem( "Peek           ", rvl   );
  rgopMenu.setItem( "Enter          ", entr  );
  rgopMenu.setItem( "Interchange    ", xy    );
  rgopMenu.setItem( "Rotate         ", rdown );
  rgopMenu.go();
}

void rith() {
  ttMenu rithMenu;
  void neg(),
       plus(),
       minus(),
       mltp(),
       dvd();
  rithMenu.setPrompt( "Arithmetic ..> " );
  rithMenu.setItem( "Negate         ", neg  );
  rithMenu.setItem( "Add            ", plus );
  rithMenu.setItem( "Subtract       ", minus );
  rithMenu.setItem( "Multiply       ", mltp );
  rithMenu.setItem( "Divide         ", dvd  );
  rithMenu.go();
}

void eval() {
  int i;
  double* z;
  
  z = new double[ numberOfVar ];
  
  cout <<  "Enter %d real numbers ..> " << numberOfVar ;
  for( i = 0; i < numberOfVar; i++ ) cin >> z[i];
  cout <<  "Confirming: You have entered  " ;
  for( i = 0; i < numberOfVar; i++ ) cout << z[i];
  cout << endl;
  
  cout <<  "\nThe value is " << (*(HPreg[0]))(z) << " \n";
  
  delete z;
}

void dffr() {
  int r;
  JLterm* p;
  cout << "Enter the register ..> ";
  cin >> r;
  p = HPreg[r]->get();
}

void nlys() {

  ttMenu nlysMenu;
  
  void drvt(),
       fltr(),
       xpon(),
       ssnn(),
       ccss(),
       ssqq(),
       llgg();
  
  nlysMenu.setPrompt( "Analysis ..> " );
  nlysMenu.setItem( "differentiate  ", drvt );
  nlysMenu.setItem( "filter         ", fltr );
  nlysMenu.setItem( "exp()          ", xpon );
  nlysMenu.setItem( "sin            ", ssnn );
  nlysMenu.setItem( "cos            ", ccss );
  nlysMenu.setItem( "sqrt           ", ssqq );
  nlysMenu.setItem( "log            ", llgg );
  nlysMenu.go();

}

void clear() {
  int i;
  for( i = 0; i < NUMREG; i++ ) HPreg[i]->setVariable( 1.0, -3, pje );
}


void neg(){
  Jet dummy;
  dummy = *(HPreg[0]);
  *(HPreg[0]) = - dummy;
}

void plus() {
  int i;
  *(HPreg[0]) += *(HPreg[1]);
  for( i = 1; i < NUMREG - 1; i++ ) *(HPreg[i]) = *(HPreg[i+1]);
}

void minus() {
  int i;
  *(HPreg[0]) = *(HPreg[1]) - *(HPreg[0]);
  for( i = 1; i < NUMREG - 1; i++ ) *(HPreg[i]) = *(HPreg[i+1]);
}

void mltp() {
  int i;
  *(HPreg[0]) = *(HPreg[1])*(*(HPreg[0]));
  for( i = 1; i < NUMREG - 1; i++ ) *(HPreg[i]) = *(HPreg[i+1]);
}

void dvd() {
  int i;
  *(HPreg[0]) = *(HPreg[1])/(*(HPreg[0]));
  for( i = 1; i < NUMREG - 1; i++ ) *(HPreg[i]) = *(HPreg[i+1]);
}


void fltr() {
  int source, sink, weightLo, weightHi;
  cout << "Enter source register ..>  ";
  cin >> source;
  cout << "Enter sink register ..>  ";
  cin >> sink;
  cout << "Enter desired weight range ..>  ";
  cin >> weightLo >> weightHi;
  *(HPreg[ sink ]) = HPreg[ source ]->filter( weightLo, weightHi );
}

void xpon() {
  *(HPreg[0]) = exp( *(HPreg[0]) );
}

void ssnn() {
  *(HPreg[0]) = sin( *(HPreg[0]) );
}

void ccss() {
  *(HPreg[0]) = cos( *(HPreg[0]) );
}

void ssqq() {
  *(HPreg[0]) = sqrt( *(HPreg[0]) );
}

void llgg() {
  *(HPreg[0]) = log( *(HPreg[0]) );
}

void drvt() {
  int i;
  int* n;
  
  n = new int[ numberOfVar ];
  
  cout <<  "Enter " << numberOfVar << "integers for the index set ..> ";
  for( i = 0; i < numberOfVar; i++ ) cin >> n[i];
  cout <<  "Confirming: You have entered  " ;
  for( i = 0; i < numberOfVar; i++ ) cout <<  n[i] << "  ";
  cout << endl;
  
  *(HPreg[0]) = HPreg[0]->D( n );
}


void entr() {
  int i;
  for( i = NUMREG - 1; i > 0; i-- ) {
    *(HPreg[i]) = *(HPreg[i-1]);
  }
}

void xy() {
  Jet z;
  z = *(HPreg[1]);
  *(HPreg[1]) = *(HPreg[0]);
  *(HPreg[0]) = z;
}

void rdown() {
  Jet z;
  int i;
  z = *(HPreg[0]);
  for( i = 0; i < NUMREG - 1; i++ ) *(HPreg[i]) = *(HPreg[i+1]);
  *(HPreg[NUMREG]) = z;
}
