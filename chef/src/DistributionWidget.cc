#include <iostream>
#include <fstream>

#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>

#include "DistributionWidget.h"

#include "bmlnElmnt.h"
#include "Particle.h"
#include "ParticleBunch.h"
#include "BeamlineContext.h"

#include <qapplication.h>
#include <qbuttongroup.h>
#include <qevent.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qvbox.h>

using namespace std;

DistributionWidget::DistributionWidget( const DistributionWidget& )
{
  QMessageBox::critical( this, "ERROR", "Program will crash."
                                        "\nDistributionWidget copy constructor called.");
}


DistributionWidget::DistributionWidget()
: QDialog( 0, 0, true )
{
  QMessageBox::warning( this, "DistributionWidget", 
                              "You must specify a ProtonBunch"
                              "\nin the constructor.");
}


DistributionWidget::DistributionWidget( ProtonBunch& x )
: QDialog( 0, 0, true )
  , _kind(uniform)
  , _theSamplePtr(&x)
  , _average(6)
  , _covariance(6,6)
  , _statsGiven(false)
{
  _qvbPtr = new QVBox( this );

    _selPtr = new QWidget( _qvbPtr );
       _selQglPtr = new QGridLayout( _selPtr, 1, 2, 5, 10 );

       _leftStuff = new QVBox( _selPtr );

         _qbgPtr = new QButtonGroup(3,Qt::Vertical,QString("Choices"),_leftStuff,0);
             _aPtr = new QRadioButton( QString("Uniform"), _qbgPtr );
             _bPtr = new QRadioButton( QString("Gaussian"), _qbgPtr );
             _cPtr = new QRadioButton( QString("Equilibrium"), _qbgPtr );

             connect( _aPtr, SIGNAL(pressed()),
                       this,  SLOT(changeLabels()) );
             connect( _bPtr, SIGNAL(pressed()),
                       this,  SLOT(changeLabels()) );
             connect( _cPtr, SIGNAL(pressed()),
                       this,  SLOT(changeLabels()) );

         // _qbgPtr->setFrameStyle(QFrame::NoFrame);
         _qbgPtr->setRadioButtonExclusive(true);
         _qbgPtr->setButton( _qbgPtr->id(_aPtr) );
         _qbgPtr->adjustSize();

         new QLabel( "Number of\nprotons", _leftStuff );
         _populationPtr = new QLineEdit( "256", _leftStuff );
         _populationPtr->setMaxLength(7);

         QWidget* space = new QWidget(_leftStuff);
         space->setSizePolicy( QSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored) );

         // QSpacerItem space(   200, 200
         //                    , QSizePolicy::Minimum
         //                    , QSizePolicy::MinimumExpanding );

       _leftStuff->adjustSize();
       _leftStuff->show();

       _unifPtr  = new QWidget( _selPtr );
         QGridLayout* unifLayoutPtr = new QGridLayout( _unifPtr, 7, 4, 5, 10 );
         unifLayoutPtr->addWidget( new QLabel("Coord",_unifPtr), 0, 0 );
         unifLayoutPtr->addWidget( new QLabel("Unit", _unifPtr), 0, 1 );
         // unifLayoutPtr->addWidget( new QLabel("Min",  _unifPtr,0,Qt::AlignHCenter)
         //                                                       , 0, 2 );
         // unifLayoutPtr->addWidget( new QLabel("Max",  _unifPtr,0,Qt::AlignHCenter)
         //                                                       , 0, 3 );
         unifLayoutPtr->addWidget( new QLabel("Min",  _unifPtr), 0, 2 );
         unifLayoutPtr->addWidget( new QLabel("Max",  _unifPtr), 0, 3 );

         _unif_xMin = new QLineEdit( "-1.000", _unifPtr );
         _unif_xMax = new QLineEdit( "1.000", _unifPtr );
         _unif_xMin->setMaxLength(7);
         _unif_xMax->setMaxLength(7);
         unifLayoutPtr->addWidget( new QLabel("x",     _unifPtr), 1, 0 );
         unifLayoutPtr->addWidget( new QLabel("[mm]",  _unifPtr), 1, 1 );
         unifLayoutPtr->addWidget( _unif_xMin,                    1, 2 );
         unifLayoutPtr->addWidget( _unif_xMax,                    1, 3 );

         _unif_yMin = new QLineEdit( "-1.000", _unifPtr );
         _unif_yMax = new QLineEdit( "1.000", _unifPtr );
         _unif_yMin->setMaxLength(7);
         _unif_yMax->setMaxLength(7);
         unifLayoutPtr->addWidget( new QLabel("y",     _unifPtr), 2, 0 );
         unifLayoutPtr->addWidget( new QLabel("[mm]",  _unifPtr), 2, 1 );
         unifLayoutPtr->addWidget( _unif_yMin,                    2, 2 );
         unifLayoutPtr->addWidget( _unif_yMax,                    2, 3 );

         _unif_cdtMin = new QLineEdit( "-1.000", _unifPtr );
         _unif_cdtMax = new QLineEdit( "1.000", _unifPtr );
         _unif_cdtMin->setMaxLength(7);
         _unif_cdtMax->setMaxLength(7);
         unifLayoutPtr->addWidget( new QLabel("cdt",   _unifPtr), 3, 0 );
         unifLayoutPtr->addWidget( new QLabel("[mm]",  _unifPtr), 3, 1 );
         unifLayoutPtr->addWidget( _unif_cdtMin,                  3, 2 );
         unifLayoutPtr->addWidget( _unif_cdtMax,                  3, 3 );

         _unif_pxMin = new QLineEdit( "-1.000", _unifPtr );
         _unif_pxMax = new QLineEdit( "1.000", _unifPtr );
         _unif_pxMin->setMaxLength(7);
         _unif_pxMax->setMaxLength(7);
         unifLayoutPtr->addWidget( new QLabel("px/p",  _unifPtr), 4, 0 );
         unifLayoutPtr->addWidget( new QLabel("[mrad]",_unifPtr), 4, 1 );
         unifLayoutPtr->addWidget( _unif_pxMin,                   4, 2 );
         unifLayoutPtr->addWidget( _unif_pxMax,                   4, 3 );

         _unif_pyMin = new QLineEdit( "-1.000", _unifPtr );
         _unif_pyMax = new QLineEdit( "1.000", _unifPtr );
         _unif_pyMin->setMaxLength(7);
         _unif_pyMax->setMaxLength(7);
         unifLayoutPtr->addWidget( new QLabel("py/p",  _unifPtr), 5, 0 );
         unifLayoutPtr->addWidget( new QLabel("[mrad]",_unifPtr), 5, 1 );
         unifLayoutPtr->addWidget( _unif_pyMin,                   5, 2 );
         unifLayoutPtr->addWidget( _unif_pyMax,                   5, 3 );

         _unif_dppMin = new QLineEdit( "-1.000", _unifPtr );
         _unif_dppMax = new QLineEdit( "1.000", _unifPtr );
         _unif_dppMin->setMaxLength(7);
         _unif_dppMax->setMaxLength(7);
         unifLayoutPtr->addWidget( new QLabel("dp/p",  _unifPtr), 6, 0 );
         unifLayoutPtr->addWidget( new QLabel("[.001]",_unifPtr), 6, 1 );
         unifLayoutPtr->addWidget( _unif_dppMin,                  6, 2 );
         unifLayoutPtr->addWidget( _unif_dppMax,                  6, 3 );

       _unifPtr->adjustSize();
       _unifPtr ->show();

       _gaussPtr = new QWidget( _selPtr );
         QGridLayout* gaussLayoutPtr = new QGridLayout( _gaussPtr, 7, 4, 5, 10 );
         gaussLayoutPtr->addWidget( new QLabel("Coord",_gaussPtr), 0, 0 );
         gaussLayoutPtr->addWidget( new QLabel("Unit", _gaussPtr), 0, 1 );
         // gaussLayoutPtr->addWidget( new QLabel("Mean"  _gaussPtr,0,Qt::AlignHCenter)
         //                                                       , 0, 2 );
         // gaussLayoutPtr->addWidget( new QLabel("Sigma",_gaussPtr,0,Qt::AlignHCenter)
         //                                                       , 0, 3 );
         gaussLayoutPtr->addWidget( new QLabel("Mean", _gaussPtr), 0, 2 );
         gaussLayoutPtr->addWidget( new QLabel("Sigma",_gaussPtr), 0, 3 );

         _gauss_xAvg = new QLineEdit( "0.000", _gaussPtr );
         _gauss_xSig = new QLineEdit( "1.000", _gaussPtr );
         _gauss_xAvg->setMaxLength(7);
         _gauss_xSig->setMaxLength(7);
         gaussLayoutPtr->addWidget( new QLabel("x",     _gaussPtr), 1, 0 );
         gaussLayoutPtr->addWidget( new QLabel("[mm]",  _gaussPtr), 1, 1 );
         gaussLayoutPtr->addWidget( _gauss_xAvg,                    1, 2 );
         gaussLayoutPtr->addWidget( _gauss_xSig,                    1, 3 );

         _gauss_yAvg = new QLineEdit( "0.000", _gaussPtr );
         _gauss_ySig = new QLineEdit( "1.000", _gaussPtr );
         _gauss_yAvg->setMaxLength(7);
         _gauss_ySig->setMaxLength(7);
         gaussLayoutPtr->addWidget( new QLabel("y",     _gaussPtr), 2, 0 );
         gaussLayoutPtr->addWidget( new QLabel("[mm]",  _gaussPtr), 2, 1 );
         gaussLayoutPtr->addWidget( _gauss_yAvg,                    2, 2 );
         gaussLayoutPtr->addWidget( _gauss_ySig,                    2, 3 );

         _gauss_cdtAvg = new QLineEdit( "0.000", _gaussPtr );
         _gauss_cdtSig = new QLineEdit( "1.000", _gaussPtr );
         _gauss_cdtAvg->setMaxLength(7);
         _gauss_cdtSig->setMaxLength(7);
         gaussLayoutPtr->addWidget( new QLabel("cdt",   _gaussPtr), 3, 0 );
         gaussLayoutPtr->addWidget( new QLabel("[mm]",  _gaussPtr), 3, 1 );
         gaussLayoutPtr->addWidget( _gauss_cdtAvg,                  3, 2 );
         gaussLayoutPtr->addWidget( _gauss_cdtSig,                  3, 3 );

         _gauss_pxAvg = new QLineEdit( "0.000", _gaussPtr );
         _gauss_pxSig = new QLineEdit( "1.000", _gaussPtr );
         _gauss_pxAvg->setMaxLength(7);
         _gauss_pxSig->setMaxLength(7);
         gaussLayoutPtr->addWidget( new QLabel("px/p",  _gaussPtr), 4, 0 );
         gaussLayoutPtr->addWidget( new QLabel("[mrad]",_gaussPtr), 4, 1 );
         gaussLayoutPtr->addWidget( _gauss_pxAvg,                   4, 2 );
         gaussLayoutPtr->addWidget( _gauss_pxSig,                   4, 3 );

         _gauss_pyAvg = new QLineEdit( "0.000", _gaussPtr );
         _gauss_pySig = new QLineEdit( "1.000", _gaussPtr );
         _gauss_pyAvg->setMaxLength(7);
         _gauss_pySig->setMaxLength(7);
         gaussLayoutPtr->addWidget( new QLabel("py/p",  _gaussPtr), 5, 0 );
         gaussLayoutPtr->addWidget( new QLabel("[mrad]",_gaussPtr), 5, 1 );
         gaussLayoutPtr->addWidget( _gauss_pyAvg,                   5, 2 );
         gaussLayoutPtr->addWidget( _gauss_pySig,                   5, 3 );

         _gauss_dppAvg = new QLineEdit( "0.000", _gaussPtr );
         _gauss_dppSig = new QLineEdit( "1.000", _gaussPtr );
         _gauss_dppAvg->setMaxLength(7);
         _gauss_dppSig->setMaxLength(7);
         gaussLayoutPtr->addWidget( new QLabel("dp/p",  _gaussPtr), 6, 0 );
         gaussLayoutPtr->addWidget( new QLabel("[.001]",_gaussPtr), 6, 1 );
         gaussLayoutPtr->addWidget( _gauss_dppAvg,                  6, 2 );
         gaussLayoutPtr->addWidget( _gauss_dppSig,                  6, 3 );

       _gaussPtr->adjustSize();
       _gaussPtr->hide();

       _equilPtr = new QWidget( _selPtr );
       _equilPtr->setMinimumSize( _gaussPtr->size() );
       _equilPtr->hide();

       _selQglPtr->addWidget( _leftStuff,  0, 0 );
       _selQglPtr->addWidget( _unifPtr, 0, 1 );

     _selPtr->adjustSize();

     // DGN: if( _selPtr->sizePolicy().mayGrowVertically() ) {
     // DGN:   cout << "Can grow." << endl;
     // DGN: }
     // DGN: else {
     // DGN:   cout << "Cannot grow." << endl;
     // DGN: }

     QWidget* spaceThing = new QWidget(_qvbPtr);
     spaceThing->setSizePolicy( QSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored) );

    QHBox* qhb99 = new QHBox( _qvbPtr );
      QPushButton* emptyBtn = new QPushButton( "Empty", qhb99 );
        connect( emptyBtn, SIGNAL(released()),
                      this,  SLOT(empty()) );
      QPushButton* sampleBtn = new QPushButton( "Sample", qhb99 );
        connect( sampleBtn, SIGNAL(released()),
                      this,  SLOT(sample()) );
      QPushButton* cancelBtn = new QPushButton( "Close", qhb99 );
        connect( cancelBtn, SIGNAL(pressed()),
                      this,  SLOT(reject()) );
        cancelBtn->setDefault(true);
    qhb99->setMargin(5);
    qhb99->setSpacing(3);
    qhb99->adjustSize();

  _qvbPtr->adjustSize();

  this->setCaption( "Specify distribution." );
  this->adjustSize();
}


DistributionWidget::DistributionWidget( beamline*, ProtonBunch&, QWidget*, const char*, WFlags )
: QDialog( 0, 0, true )
{
  QMessageBox::warning( this, "DistributionWidget", 
			"This constructor not written.");
}


DistributionWidget::DistributionWidget( BeamlineContext&, ProtonBunch&, QWidget*, const char*, WFlags )
: QDialog( 0, 0, true )
{
  QMessageBox::warning( this, "DistributionWidget", 
			"This constructor not written.");
}


DistributionWidget::~DistributionWidget()
{
}


void DistributionWidget::changeLabels()
{
  _unifPtr->hide();
  _gaussPtr->hide();
  _equilPtr->hide();
  _selQglPtr->remove( _unifPtr );
  _selQglPtr->remove( _gaussPtr );
  _selQglPtr->remove( _equilPtr );

  if( _aPtr->isDown() ) {
    _selQglPtr->addWidget( _unifPtr, 0, 1 );
    _unifPtr->show();
    _kind = uniform;
  }

  else if( _bPtr->isDown() ) {
    _selQglPtr->addWidget( _gaussPtr, 0, 1 );
    _gaussPtr->show();
    _kind = gaussian;
  }

  else if( _cPtr->isDown() ) {
    _selQglPtr->addWidget( _equilPtr, 0, 1 );
    _equilPtr->show();
    _kind = equilibrium;
  }

  this->update();
}


void DistributionWidget::resizeEvent( QResizeEvent* qrePtr )
{
  // DGN: cout << "Received resizeEvent "
  // DGN:      << qrePtr->size().width()
  // DGN:      << "  "
  // DGN:      << qrePtr->size().height()
  // DGN:      << endl;
  _qvbPtr->resize( qrePtr->size().width(), qrePtr->size().height() );
}


void DistributionWidget::_genUniform()
{
  bool keepGoing = true;
  bool ok = true;
  double xMin = _unif_xMin->text().toDouble(&ok);      keepGoing &= ok;
  double xMax = _unif_xMax->text().toDouble(&ok);      keepGoing &= ok;
  double yMin = _unif_yMin->text().toDouble(&ok);      keepGoing &= ok;
  double yMax = _unif_yMax->text().toDouble(&ok);      keepGoing &= ok;
  double cdtMin = _unif_cdtMin->text().toDouble(&ok);  keepGoing &= ok;
  double cdtMax = _unif_cdtMax->text().toDouble(&ok);  keepGoing &= ok;
  double pxMin = _unif_pxMin->text().toDouble(&ok);    keepGoing &= ok;
  double pxMax = _unif_pxMax->text().toDouble(&ok);    keepGoing &= ok;
  double pyMin = _unif_pyMin->text().toDouble(&ok);    keepGoing &= ok;
  double pyMax = _unif_pyMax->text().toDouble(&ok);    keepGoing &= ok;
  double dppMin = _unif_dppMin->text().toDouble(&ok);  keepGoing &= ok;
  double dppMax = _unif_dppMax->text().toDouble(&ok);  keepGoing &= ok;
  int n = _populationPtr->text().toInt(&ok);           keepGoing &= ok;

  if( keepGoing ) {
    // The following use of boost classes is based on boost 
    // random_demo.cpp profane demo
    // by Jens Maurer 2000
    // 
    // Define a random number generator and initialize it with a reproducible
    // seed.
    // (The seed is unsigned, otherwise the wrong overload may be selected
    // when using mt19937 as the base_generator_type.)
    // 
    // Whatever ...
    // 
    boost::minstd_rand generator(42u);
    // ??? Should be in constructor ???

    boost::uniform_real<double> xRan( xMin, xMax );
    boost::uniform_real<double> yRan( yMin, yMax );
    boost::uniform_real<double> cdtRan( cdtMin, cdtMax );
    boost::uniform_real<double> pxRan( pxMin, pxMax );
    boost::uniform_real<double> pyRan( pyMin, pyMax );
    boost::uniform_real<double> dppRan( dppMin, dppMax );

    boost::variate_generator<boost::minstd_rand&, boost::uniform_real<double> > 
    xDist(generator, xRan);
    boost::variate_generator<boost::minstd_rand&, boost::uniform_real<double> > 
    yDist(generator, yRan);
    boost::variate_generator<boost::minstd_rand&, boost::uniform_real<double> > 
    cdtDist(generator, cdtRan);
    boost::variate_generator<boost::minstd_rand&, boost::uniform_real<double> > 
    pxDist(generator, pxRan);
    boost::variate_generator<boost::minstd_rand&, boost::uniform_real<double> > 
    pyDist(generator, pyRan);
    boost::variate_generator<boost::minstd_rand&, boost::uniform_real<double> > 
    dppDist(generator, dppRan);

    if( n <= 0 ) { n = 16; }
    Proton p;
    for( int i = 0; i < n; i++ ) {
      p.set_x( xDist() );
      p.set_y( yDist() );
      p.set_cdt( cdtDist() );
      p.set_npx( pxDist() );
      p.set_npy( pyDist() );
      p.set_ndp( dppDist() );
      _theSamplePtr->append( p );
      // The proton is cloned by the ParticleBunch
    }

    ostringstream uic;
    uic << n
        << " uniformly distributed particles have been"
        << "\nadded to the bunch.It now contains "
        << _theSamplePtr->size()
        << " particles.";
    QMessageBox::information( 0, "DistributionWidget", uic.str().c_str() );
  }

  else {
    QMessageBox::warning( this, "Sorry", "Error reading data from widget." );
  }
}


void DistributionWidget::_genGaussian()
{
  bool keepGoing = true;
  bool ok = true;
  double xAvg = _gauss_xAvg->text().toDouble(&ok);      keepGoing &= ok;
  double xSig = _gauss_xSig->text().toDouble(&ok);      keepGoing &= ok;
  double yAvg = _gauss_yAvg->text().toDouble(&ok);      keepGoing &= ok;
  double ySig = _gauss_ySig->text().toDouble(&ok);      keepGoing &= ok;
  double cdtAvg = _gauss_cdtAvg->text().toDouble(&ok);  keepGoing &= ok;
  double cdtSig = _gauss_cdtSig->text().toDouble(&ok);  keepGoing &= ok;
  double pxAvg = _gauss_pxAvg->text().toDouble(&ok);    keepGoing &= ok;
  double pxSig = _gauss_pxSig->text().toDouble(&ok);    keepGoing &= ok;
  double pyAvg = _gauss_pyAvg->text().toDouble(&ok);    keepGoing &= ok;
  double pySig = _gauss_pySig->text().toDouble(&ok);    keepGoing &= ok;
  double dppAvg = _gauss_dppAvg->text().toDouble(&ok);  keepGoing &= ok;
  double dppSig = _gauss_dppSig->text().toDouble(&ok);  keepGoing &= ok;
  int n = _populationPtr->text().toInt(&ok);            keepGoing &= ok;

  if( keepGoing ) {
    // The following use of boost classes is based on boost 
    // random_demo.cpp profane demo
    // by Jens Maurer 2000
    // 
    // Define a random number generator and initialize it with a reproducible
    // seed.
    // (The seed is unsigned, otherwise the wrong overload may be selected
    // when using mt19937 as the base_generator_type.)
    // 
    // Whatever ...
    // 
    boost::minstd_rand generator(42u);
    // ??? Should be in constructor ???

    boost::normal_distribution<double> xRan( xAvg, xSig );
    boost::normal_distribution<double> yRan( yAvg, ySig );
    boost::normal_distribution<double> cdtRan( cdtAvg, cdtSig );
    boost::normal_distribution<double> pxRan( pxAvg, pxSig );
    boost::normal_distribution<double> pyRan( pyAvg, pySig );
    boost::normal_distribution<double> dppRan( dppAvg, dppSig );

    boost::variate_generator<boost::minstd_rand&, boost::normal_distribution<double> > 
    xDist(generator, xRan);
    boost::variate_generator<boost::minstd_rand&, boost::normal_distribution<double> > 
    yDist(generator, yRan);
    boost::variate_generator<boost::minstd_rand&, boost::normal_distribution<double> > 
    cdtDist(generator, cdtRan);
    boost::variate_generator<boost::minstd_rand&, boost::normal_distribution<double> > 
    pxDist(generator, pxRan);
    boost::variate_generator<boost::minstd_rand&, boost::normal_distribution<double> > 
    pyDist(generator, pyRan);
    boost::variate_generator<boost::minstd_rand&, boost::normal_distribution<double> > 
    dppDist(generator, dppRan);

    if( n <= 0 ) { n = 16; }
    Proton p;
    for( int i = 0; i < n; i++ ) {
      p.set_x( xDist() );
      p.set_y( yDist() );
      p.set_cdt( cdtDist() );
      p.set_npx( pxDist() );
      p.set_npy( pyDist() );
      p.set_ndp( dppDist() );
      _theSamplePtr->append( p );
      // The proton is cloned by the ParticleBunch
    }

    ostringstream uic;
    uic << n
        << " normally distributed particles have been"
        << "\nadded to the bunch.It now contains "
        << _theSamplePtr->size()
        << " particles.";
    QMessageBox::information( 0, "DistributionWidget", uic.str().c_str() );
  }

  else {
    QMessageBox::warning( this, "Sorry", "Error reading data from widget." );
  }
}


void DistributionWidget::_genEquilibrium()
{
}


void DistributionWidget::sample()
{
  // Note: if( _aPtr->isDown() ) ... etc. .. is not
  // going to work here. Don't know why. 
  // Probably because it violates QButtonBox integrity.

  // Uniform sampling
  if     ( uniform     == _kind ) { _genUniform(); }
  else if( gaussian    == _kind ) { _genGaussian(); }
  else if( equilibrium == _kind ) {
    QMessageBox::warning( this, "Sorry", "Nothing happened." );
  }
  else {
    cout << "DGN: whoops! Nothing is down." << endl;
    QMessageBox::warning( this, "Sorry", "Nothing happened." );
  }
}


void DistributionWidget::empty()
{
  if( _theSamplePtr->isEmpty() ) {
    QMessageBox::information( this, "WARNING", "A bunch with no particles"
                                               "\ncannot be emptied."
                                               "\n         -- Leo Tolstoy" );
    return;
  }

  int n = _theSamplePtr->size();
  _theSamplePtr->empty();

  ostringstream uic;
  uic << n
      << " particles have been removed from the bunch.";
  QMessageBox::information( 0, "DistributionWidget", uic.str().c_str() );
}


void DistributionWidget::setStatistics( const Vector& avg, const Matrix& cov )
{
  if( (6 != avg.Dim()) || (6 != cov.rows()) || (6 != cov.cols())  ) {
    QMessageBox::warning( this, "Invalid argument", 
                                "DistributionWidget set to handle"
                                "\nsix-dimensional phase space only." );
    return;
  }
  _average = avg;
  _covariance = cov;
  _statsGiven = true;
}


void DistributionWidget::getStatistics( Vector& avg, Matrix& cov )
{
  avg = _average;
  cov = _covariance;
}


