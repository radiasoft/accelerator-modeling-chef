#include "bmlnElmnt.h"
#include "PhysicsConstants.h"

InsertionList::InsertionList( double p )
: smax( -1.0e-20 ), prtn( sqrt( p*p + PH_NORM_mp*PH_NORM_mp ) )
{
}

InsertionList::InsertionList( const InsertionList& x )
: dlist( (dlist&) x )
{
  smax = x.smax;
}

InsertionList::~InsertionList()
{
}


void InsertionList::Append( const InsertionListElement& x )
{
  Append( &x  );
}

void InsertionList::Append( const InsertionListElement* x )
{
  if( x->q->OrbitLength( prtn ) != 0.0 ) {
    cerr << "\n"
            "*** ERROR ***                                          \n"
            "*** ERROR *** InsertionList::Append                    \n"
            "*** ERROR *** This version can only handle zero        \n"
            "*** ERROR *** length elements.                         \n"
            "*** ERROR ***                                          \n"
         << endl;
    exit(1);
  }

  if( x->s > smax ) {
    dlist::append( (ent) x );
    smax = x->s;
    return;
  }
  cerr << "\n"
          "*** ERROR ***                                          \n"
          "*** ERROR *** InsertionList::Append                    \n"
          "*** ERROR *** Ordering is not correct.                 \n"
          "*** ERROR *** " << smax << " < " << x->s << "          \n"
          "*** ERROR ***                                          \n"
       << endl;
  exit(1);
}

void InsertionList::Insert( const InsertionListElement& x )
{
  Insert( &x  );
}

void InsertionList::Insert( const InsertionListElement* x )
{
  if( x->q->OrbitLength( prtn ) != 0.0 ) {
    cerr << "\n"
            "*** ERROR ***                                          \n"
            "*** ERROR *** InsertionList::Insert                    \n"
            "*** ERROR *** This version can only handle zero        \n"
            "*** ERROR *** length elements.                         \n"
            "*** ERROR ***                                          \n"
         << endl;
    exit(1);
  }

  if( IsEmpty() ) {
    dlist::insert( (ent) x  );
    smax = x->s;
    return;
  }

  InsertionListElement* f = (InsertionListElement*) firstInfoPtr();
  if( f->s <= x->s ) {
    cerr << "\n"
            "*** ERROR ***                                          \n"
            "*** ERROR *** InsertionList::Insert                    \n"
            "*** ERROR *** Ordering is not correct.                 \n"
            "*** ERROR *** " << f->s << " <= " << x->s << "         \n"
            "*** ERROR ***                                          \n"
         << endl;
    exit(1);
  }

  dlist::insert( (ent) x  );
  return;
}

void InsertionList::MergeUnique( InsertionList& x )
{
  if( x.Size() <= 0 ) return;

  dlist_iterator gx(  (dlist&) x );
  InsertionListElement* p_ile_x;

  while((  p_ile_x = (InsertionListElement*) gx()  )) {
    dlist_iterator gt( *(dlist*) this );
    InsertionListElement* p_ile_t;
    char inserted = 0;
    while((  p_ile_t = (InsertionListElement*) gt()  )) {
      if( fabs( p_ile_t->s - p_ile_x->s ) < 0.5 /* meters */ ) {
        // Do not insert
        inserted = 1;
        break;
      }
      if( p_ile_t->s > p_ile_x->s ) {
	dlist::putAbove( (ent) p_ile_t, (ent) p_ile_x );
        inserted = 1;
        break;
      }
    }
    if( !inserted ) Append( p_ile_x );
  }
}

void InsertionList::MergeAll( InsertionList& x )
{
  if( x.Size() <= 0 ) return;

  dlist_iterator gx(  (dlist&) x );
  InsertionListElement* p_ile_x;

  while((  p_ile_x = (InsertionListElement*) gx()  )) {
    dlist_iterator gt( *(dlist*) this );
    InsertionListElement* p_ile_t;
    char inserted = 0;
    while((  p_ile_t = (InsertionListElement*) gt()  ))
      if( p_ile_t->s > p_ile_x->s ) {
	dlist::putAbove( (ent) p_ile_t, (ent) p_ile_x );
        inserted = 1;
        break;
      }
    if( !inserted ) {
      dlist::append( (ent) p_ile_x );
      smax = p_ile_x->s;
    }
  }
}

InsertionList& InsertionList::operator=( const InsertionList& x )
{
  smax = x.smax;
  dlist::operator=( (dlist&) x );
  return *this;
}

int InsertionList::Size()
{
  return dlist::size();
}

void InsertionList::Clear()
{
  dlist::clear();
  smax = -1.0e-20;
}
ostream& operator<<(ostream& os, const InsertionList& x)
{
  dlist_iterator getNext((dlist&)x);
  InsertionListElement* ple;
  os << "InsertionList Dump " << endl;
  while((  ple = (InsertionListElement*)getNext()  )) {
    os << "s = " << ple->s << " " << ple->q->Name();
    os << " " << ple->q->Type() << endl;
  }
  return (os << endl);
}
