// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator*( const Complex& y, const Jet& x ) {  
 static JetC z;
 static JLterm* p;
 static JLCterm* q;
 static JL* xPtr;
 static JLC* zPtr;
 static int testWeight;
 static Complex CZero( 0.0, 0.0 );
 
 z.Reconstruct( JetC::CreateEnvFrom( x->myEnv ) );

 p    = 0;
 q    = 0;
 xPtr = x.jl;
 zPtr = z.jl;
 testWeight = z->accuWgt = x->accuWgt;

 if( y == CZero ) {
   z.stacked = 1;
   return z;
 }
 
 // ??? REMOVE // If x is void, return it ..
 // ??? REMOVE if( xPtr->count < 1 ) {    // This is done in this way so that
 // ??? REMOVE   z.DeepCopy( x );         // what is returned does not own
 // ??? REMOVE   z.stacked = 1;           // the same data as x.
 // ??? REMOVE   return z;
 // ??? REMOVE }

 // If x is void, return z, 
 // which is itself void at this point in the program.
 if( xPtr->count < 1 ) {
   z.stacked = 1;
   return z;
 }

 dlist_iterator gx( *(dlist*) xPtr );

 while((  p = (JLterm*) gx()  )) {
   if( p->weight > testWeight ) break;
   q = new JLCterm( p );
   q->value *= y;
   zPtr->addTerm( q );
 }
 
 z.stacked = 1;
 return z;
}


