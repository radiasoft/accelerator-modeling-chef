#include "BeamlineContext.h"
#include "beamline.h"
#include "BeamlineIterator.h"
#include "LattFuncSage.h"
#include "ClosedOrbitSage.h"
#include "ChromaticityAdjuster.h"
#include "TuneAdjuster.h"

extern void BeamlineSpitout( int, BeamlineIterator& );

BeamlineContext::BeamlineContext( bool doClone, beamline* x )
: _p_bml(x), 
  _p_lfs(0), _p_cos(0), 
  _p_ca(0), _p_ta(0),
  _isCloned(doClone),
  _p_bi(0), _p_dbi(0), _p_rbi(0), _p_drbi(0),
  _tunes(0), _p_jp(0), _normalLattFuncsCalcd(false)
{
  if( x == 0 ) {
    cerr << "\n*** ERROR *** " << __FILE__ << ", " << __LINE__ << ": "
         << "\n*** ERROR *** BeamlineContext::BeamlineContext"
            "\n*** ERROR *** Invoked with null beamline pointer."
            "\n*** ERROR *** \n"
         << endl;
    exit(9);
  }
  if( _isCloned && (x != 0) ) 
  { _p_bml = (beamline*) (x->Clone()); }
}

BeamlineContext::BeamlineContext( const BeamlineContext& )
{
  cerr << "*** ERROR *** BeamlineContext copy constructor called." << endl;
  exit(3);
}

BeamlineContext::~BeamlineContext()
{
  if( _isCloned && (_p_bml != 0) ) 
  { _p_bml->eliminate(); }

  if( _p_lfs ) delete _p_lfs;
  if( _p_cos ) delete _p_cos;
  if( _p_ca  ) delete _p_ca;
  if( _p_ta  ) delete _p_ta;

  if( _p_bi   )  delete _p_bi;
  if( _p_dbi  )  delete _p_dbi;
  if( _p_rbi  )  delete _p_rbi;
  if( _p_drbi )  delete _p_drbi;

  if( _p_jp   )  delete _p_jp;
}


int BeamlineContext::assign( beamline* x )
{
  static bool firstTime = true;
  if( 0 != _p_bml ) 
  { if( firstTime ) 
    { cerr << "\n*** WARNING *** BeamlineContext::assign "
              "invoked illegally."
              "\n*** WARNING *** This message appears only once.\n"
           << endl;
      firstTime = false;
    }
    return 1; 
  }
  
  if( 0 == x ) 
  { return 2;}

  if( _isCloned ) 
  { _p_bml = (beamline*) (x->Clone()); }
  else 
  { _p_bml = x; }

  return 0;
}


void BeamlineContext::accept( ConstBmlVisitor& x ) const
{
  _p_bml->accept(x);
}


void BeamlineContext::setClonedFlag( bool x )
{
  _isCloned = x;
}


bool BeamlineContext::getClonedFlag()
{
  return _isCloned;
}


void BeamlineContext::writeTree()
{
  BeamlineIterator bi( _p_bml );
  BeamlineSpitout( 0, bi );
}


// Beamline Functions

const char* BeamlineContext::name() const
{
  return _p_bml->Name();
}


void BeamlineContext::peekAt( double& s, Particle* p ) const
{
  _p_bml->peekAt( s, p );
}


double BeamlineContext::sumLengths() const
{
  double ret = 0.0;
  DeepBeamlineIterator dbi( _p_bml );
  bmlnElmnt* q;
  while( q = dbi++ ) { ret += q->Length(); }
  return ret;
}


int BeamlineContext::setLength( bmlnElmnt* w, double l )
{
  static bool notFound; notFound = true;
  static int ret;       ret = 0;

  DeepBeamlineIterator dbi( _p_bml );
  bmlnElmnt* q;
  while( notFound && (q = dbi++) ) 
  { if( q == w ) 
    { notFound = false;
      if( l != q->Length() ) 
      { q->setLength(l);
        this->_deleteLFS();
        if( _p_cos ) { delete _p_cos; _p_cos = 0; }
        if( _p_ca  ) { delete _p_ca; _p_ca = 0;   }
        if( _p_ta  ) { delete _p_ta; _p_ta = 0;   }
      }
    }
  }

  if(notFound) { ret = 1; }

  return ret;
}


double BeamlineContext::getEnergy() const
{
  return _p_bml->Energy();
}


int BeamlineContext::countHowManyDeeply() const
{
  return _p_bml->countHowManyDeeply();
}


const beamline* BeamlineContext::cheatBmlPtr() const
{
  return _p_bml;
}



// Sage operations

void BeamlineContext::_deleteLFS()
{
  if( 0 != _p_lfs )
  {
    _p_lfs->eraseAll();
    if( _tunes ) { 
      delete _tunes; 
      _tunes = 0;
    }
    delete _p_lfs;
    _p_lfs = 0;

    _normalLattFuncsCalcd = false;
  }
}


void BeamlineContext::_createLFS()
{
  this->_deleteLFS();
  _p_lfs = new LattFuncSage( _p_bml, false );
}


void BeamlineContext::_createTunes()
{
  if( _p_jp != 0 ) {
    delete _p_jp;
  }
  _p_jp = new JetProton( _p_bml->Energy() );

  int lfs_result = _p_lfs->TuneCalc( _p_jp );
  if( lfs_result != 0 ) {
    cerr << "\n*** ERROR *** " << __FILE__ << ", " << __LINE__ << ": "
         << "BeamlineContext::_createTunes()"
            "\n*** ERROR *** Something went wrong while calculating tune: error no. " 
         << lfs_result 
         << "\n*** ERROR *** \n"
         << endl;
    exit(0);
  }
    
  // At this point, _p_jp's state is that after one-turn on the
  // closed orbit, which has been found by LattFuncSage::TuneCalc.

  if( _tunes == 0 ) {
    _tunes = new LattFuncSage::tunes( *( dynamic_cast<LattFuncSage::tunes*>
                                         (_p_bml->dataHook.find( "Tunes" )) 
                                       ) 
                                    );
  }
  else {
    *_tunes = *( dynamic_cast<LattFuncSage::tunes*>(_p_bml->dataHook.find( "Tunes" )) );
  }

  _p_bml->dataHook.eraseAll( "Tunes" );
}


double BeamlineContext::getHorizontalFracTune()
{
  if( 0 == _p_lfs ) {
    cout << "DGN: " << __FILE__ << " line " << __LINE__ 
         << ": this->_createLFS();" << endl;
    this->_createLFS();
  }
  
  if( 0 == _tunes ) {
    cout << "DGN: " << __FILE__ << " line " << __LINE__ 
         << ": this->_createTunes();" << endl;
    this->_createTunes();
  }

  cout << "DGN: " << __FILE__ << " line " << __LINE__ 
       << ": return _tunes->hor;" << endl;
  cout << "DGN: " << __FILE__ << " line " << __LINE__ 
       << ": return "
       << _tunes->hor
       << ";" << endl;
  return _tunes->hor;
}


double BeamlineContext::getVerticalFracTune()
{
  if( 0 == _p_lfs ) {
    this->_createLFS();
  }
  
  if( 0 == _tunes ) {
    this->_createTunes();
  }

  return _tunes->ver;
}


const LattFuncSage::lattFunc* BeamlineContext::getLattFuncPtr( int i )
{
  if( 0 == _p_lfs ) {
    this->_createLFS();
  }
  
  if( 0 == _tunes ) {
    this->_createTunes();
  }

  if( !_normalLattFuncsCalcd ) {
    _p_lfs->NewSlow_CS_Calc( _p_jp );
    _normalLattFuncsCalcd = true;
  }

  return (_p_lfs->get_lattFuncPtr(i));
}


// Iterator functions

int BeamlineContext::beginIterator()
{
  if(_p_rbi||_p_dbi||_p_drbi)
  { 
    cerr << "*** WARNING *** "
         << __FILE__ 
         << ", line "
         << __LINE__
         << ": Only one BeamlineContext::Iterator at a time." 
         << endl;
    return -1;
  }

  if(_p_bi) { _p_bi->reset(); }
  else      { _p_bi = new BeamlineIterator(_p_bml); }

  return 0;
}


int BeamlineContext::beginDeepIterator()
{
  if(_p_bi||_p_rbi||_p_drbi)
  { 
    cerr << "*** WARNING *** "
         << __FILE__ 
         << ", line "
         << __LINE__
         << ": Only one BeamlineContext::Iterator at a time." 
         << endl;
    return -1;
  }

  if(_p_dbi) { _p_dbi->reset(); }
  else       { _p_dbi = new DeepBeamlineIterator(_p_bml); }

  return 0;
}


int BeamlineContext::beginReverseIterator()
{
  if(_p_bi||_p_dbi||_p_drbi)
  { 
    cerr << "*** WARNING *** "
         << __FILE__ 
         << ", line "
         << __LINE__
         << ": Only one BeamlineContext::Iterator at a time." 
         << endl;
    return -1;
  }

  if(_p_rbi) { _p_rbi->reset(); }
  else       { _p_rbi = new ReverseBeamlineIterator(_p_bml); }

  return 0;
}


int BeamlineContext::beginDeepReverseIterator()
{
  if(_p_bi||_p_rbi||_p_dbi)
  { 
    cerr << "*** WARNING *** "
         << __FILE__ 
         << ", line "
         << __LINE__
         << ": Only one BeamlineContext::Iterator at a time." 
         << endl;
    return -1;
  }

  if(_p_drbi) { _p_drbi->reset(); }
  else        { _p_drbi = new DeepReverseBeamlineIterator(_p_bml); }

  return 0;
}


void BeamlineContext::endIterator()
{
  if(_p_bi) 
  { delete _p_bi;
    _p_bi = 0;
  }
}


void BeamlineContext::endDeepIterator()
{
  if(_p_dbi) 
  { delete _p_dbi;
    _p_dbi = 0;
  }
}


void BeamlineContext::endReverseIterator()
{
  if(_p_rbi) 
  { delete _p_rbi;
    _p_rbi = 0;
  }
}


void BeamlineContext::endDeepReverseIterator()
{
  if(_p_drbi) 
  { delete _p_drbi;
    _p_drbi = 0;
  }
}


const bmlnElmnt* BeamlineContext::i_next()
{
  if(_p_bi) { return (*_p_bi)++; }
  else      { return 0; }
  return 0;
}


const bmlnElmnt* BeamlineContext::di_next()
{
  if(_p_dbi) { return (*_p_dbi)++; }
  else       { return 0; }
  return 0;
}


const bmlnElmnt* BeamlineContext::ri_next()
{
  if(_p_rbi) { return (*_p_rbi)++; }
  else       { return 0; }
  return 0;
}


const bmlnElmnt* BeamlineContext::dri_next()
{
  if(_p_drbi) { return (*_p_drbi)++; }
  else        { return 0; }
  return 0;
}


void BeamlineContext::i_reset()
{
  _p_bi->reset();
}


void BeamlineContext::di_reset()
{
  _p_dbi->reset();
}


void BeamlineContext::ri_reset()
{
  _p_rbi->reset();
}


void BeamlineContext::dri_reset()
{
  _p_drbi->reset();
}


ostream& operator<<( ostream& os, const BeamlineContext& x )
{
  os << "Begin BeamlineContext" << endl;
  if( x._p_bml ) 
  { os << "_p_bml" << endl;
    os << *(x._p_bml);
  }
  os << "End BeamlineContext" << endl;
  return os;
}


istream& operator>>( istream& is, BeamlineContext& x )
{
  static char charBuffer[128];
  is.getline( charBuffer, 128, '\n' );
  if( 0 != strcmp( charBuffer, "Begin BeamlineContext" ) ) 
  { cerr << "*** ERROR *** "
         << __FILE__
         << ", line "
         << __LINE__
         << ": operator>> : "
            "Expected beginning of BeamlineContext."
         << endl;
    exit(9);
  }

  is.getline( charBuffer, 128, '\n' );
  while( 0 != strcmp( charBuffer, "End BeamlineContext" ) )
  { 
    if( 0 == strcmp( charBuffer, "_p_bml" ) )
    {
      is >> *(x._p_bml);
    }

    else 
    { 
      cerr << "*** ERROR *** "
           << __FILE__
           << ", line "
           << __LINE__
           << ": operator>> : "
              "Unrecognized keyword "
           << charBuffer
           << endl;
      exit(8);
    }

    is.getline( charBuffer, 128, '\n' );
  }

  return is;
}


