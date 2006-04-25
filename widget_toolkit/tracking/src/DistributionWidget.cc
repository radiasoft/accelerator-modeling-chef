/**********************************************************************/
/*                                                                    */
/* File:           DistributionWidget.cc                              */
/*                                                                    */ 
/* Authors:        Leo Michelotti                                     */
/*                 michelotti@fnal.gov                                */     
/*                                                                    */ 
/* Creation Date:  July 15, 2005                                      */
/* Current rev:    July 23, 2005                                      */
/*                                                                    */ 
/* Copyright:      (c) URA/Fermilab                                   */
/*                                                                    */
/**********************************************************************/

#include <iostream>
#include <fstream>
#include <typeinfo>

#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>

typedef boost::uniform_real<double>
        basUnifGen;
typedef boost::variate_generator<boost::minstd_rand&, boost::uniform_real<double> >
        varUnifGen;
typedef boost::normal_distribution<double>
        basGaussGen;
typedef boost::variate_generator<boost::minstd_rand&, boost::normal_distribution<double> >
        varGaussGen;

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
: QDialog( 0, 0, true )
{
  QMessageBox::critical( this, "ERROR", "Program will crash."
                                        "\nDistributionWidget copy constructor called.");
}


DistributionWidget::DistributionWidget()
: QDialog( 0, 0, true )
{
  QMessageBox::critical( this, "DistributionWidget", 
                               "You must specify a ParticleBunch"
                               "\nin the constructor."
                               "\nThis object will fail."       );
}


DistributionWidget::DistributionWidget(   ParticleBunch& x
                                        , QWidget*       parent
                                        , const char*    name
                                        , WFlags         atts   )
:   QDialog( parent, name, false, atts )
  , _bmlPtr(0)
  , _bmlConPtr(0)
  , _kind(uniform)
  , _theSamplePtr(&x)
  , _average(6)
  , _covariance(6,6)
  , _statsGiven(false)
{
  _finishConstructor();
}


DistributionWidget::DistributionWidget( beamline& x, ParticleBunch&, QWidget*, const char*, WFlags )
:   QDialog( 0, 0, true )
  , _bmlPtr(&x)
{
  QMessageBox::critical( this, "DistributionWidget", 
                        "This constructor not written.");
}


DistributionWidget::DistributionWidget( BeamlineContext& x, QWidget* parent, const char* name, WFlags atts )
:   QDialog( parent, name, false, atts )
  , _bmlPtr(0)
  , _bmlConPtr(&x)
  , _kind(uniform)
  , _theSamplePtr(x._particleBunchPtr)
  , _average(6)
  , _covariance(6,6)
  , _statsGiven(false)
{
  _finishConstructor();
}


void DistributionWidget::_finishConstructor()
{
  // Determine the particle type first
  if( typeid(*_theSamplePtr) == typeid(ProtonBunch) ) {
    _flavor = DistributionWidget::proton;
  }
  else if( typeid(*_theSamplePtr) == typeid(PositronBunch) ) {
    _flavor = DistributionWidget::positron;
  }
  else {
    ostringstream uic;
    uic << __FILE__ << ", line " << __LINE__
        << "\nvoid DistributionWidget::_finishConstructor()"
           "\nType of particle bunch not recognized."
           "\nIt should be either for protons or positrons."
           "\nConstruction aborted.";
    QMessageBox::critical( 0, "CHEF: ERROR", uic.str().c_str() );
  }

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

         _qbgPtr->setFrameStyle(QFrame::NoFrame);
         _qbgPtr->setRadioButtonExclusive(true);
         _qbgPtr->setButton( _qbgPtr->id(_aPtr) );
         _qbgPtr->adjustSize();

         new QLabel( "Number of\nparticles", _leftStuff );
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

         _unif_cdtMin = new QLineEdit( "0.000", _unifPtr );
         _unif_cdtMax = new QLineEdit( "0.000", _unifPtr );
         _unif_cdtMin->setMaxLength(7);
         _unif_cdtMax->setMaxLength(7);
         unifLayoutPtr->addWidget( new QLabel("cdt",   _unifPtr), 3, 0 );
         unifLayoutPtr->addWidget( new QLabel("[mm]",  _unifPtr), 3, 1 );
         unifLayoutPtr->addWidget( _unif_cdtMin,                  3, 2 );
         unifLayoutPtr->addWidget( _unif_cdtMax,                  3, 3 );

         _unif_pxMin = new QLineEdit( "0.000", _unifPtr );
         _unif_pxMax = new QLineEdit( "0.000", _unifPtr );
         _unif_pxMin->setMaxLength(7);
         _unif_pxMax->setMaxLength(7);
         unifLayoutPtr->addWidget( new QLabel("px/p",  _unifPtr), 4, 0 );
         unifLayoutPtr->addWidget( new QLabel("[mrad]",_unifPtr), 4, 1 );
         unifLayoutPtr->addWidget( _unif_pxMin,                   4, 2 );
         unifLayoutPtr->addWidget( _unif_pxMax,                   4, 3 );

         _unif_pyMin = new QLineEdit( "0.000", _unifPtr );
         _unif_pyMax = new QLineEdit( "0.000", _unifPtr );
         _unif_pyMin->setMaxLength(7);
         _unif_pyMax->setMaxLength(7);
         unifLayoutPtr->addWidget( new QLabel("py/p",  _unifPtr), 5, 0 );
         unifLayoutPtr->addWidget( new QLabel("[mrad]",_unifPtr), 5, 1 );
         unifLayoutPtr->addWidget( _unif_pyMin,                   5, 2 );
         unifLayoutPtr->addWidget( _unif_pyMax,                   5, 3 );

         _unif_dppMin = new QLineEdit( "0.000", _unifPtr );
         _unif_dppMax = new QLineEdit( "0.000", _unifPtr );
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
         _gauss_cdtSig = new QLineEdit( "0.000", _gaussPtr );
         _gauss_cdtAvg->setMaxLength(7);
         _gauss_cdtSig->setMaxLength(7);
         gaussLayoutPtr->addWidget( new QLabel("cdt",   _gaussPtr), 3, 0 );
         gaussLayoutPtr->addWidget( new QLabel("[mm]",  _gaussPtr), 3, 1 );
         gaussLayoutPtr->addWidget( _gauss_cdtAvg,                  3, 2 );
         gaussLayoutPtr->addWidget( _gauss_cdtSig,                  3, 3 );

         _gauss_pxAvg = new QLineEdit( "0.000", _gaussPtr );
         _gauss_pxSig = new QLineEdit( "0.000", _gaussPtr );
         _gauss_pxAvg->setMaxLength(7);
         _gauss_pxSig->setMaxLength(7);
         gaussLayoutPtr->addWidget( new QLabel("px/p",  _gaussPtr), 4, 0 );
         gaussLayoutPtr->addWidget( new QLabel("[mrad]",_gaussPtr), 4, 1 );
         gaussLayoutPtr->addWidget( _gauss_pxAvg,                   4, 2 );
         gaussLayoutPtr->addWidget( _gauss_pxSig,                   4, 3 );

         _gauss_pyAvg = new QLineEdit( "0.000", _gaussPtr );
         _gauss_pySig = new QLineEdit( "0.000", _gaussPtr );
         _gauss_pyAvg->setMaxLength(7);
         _gauss_pySig->setMaxLength(7);
         gaussLayoutPtr->addWidget( new QLabel("py/p",  _gaussPtr), 5, 0 );
         gaussLayoutPtr->addWidget( new QLabel("[mrad]",_gaussPtr), 5, 1 );
         gaussLayoutPtr->addWidget( _gauss_pyAvg,                   5, 2 );
         gaussLayoutPtr->addWidget( _gauss_pySig,                   5, 3 );

         _gauss_dppAvg = new QLineEdit( "0.000", _gaussPtr );
         _gauss_dppSig = new QLineEdit( "0.000", _gaussPtr );
         _gauss_dppAvg->setMaxLength(7);
         _gauss_dppSig->setMaxLength(7);
         gaussLayoutPtr->addWidget( new QLabel("dp/p",  _gaussPtr), 6, 0 );
         gaussLayoutPtr->addWidget( new QLabel("[.001]",_gaussPtr), 6, 1 );
         gaussLayoutPtr->addWidget( _gauss_dppAvg,                  6, 2 );
         gaussLayoutPtr->addWidget( _gauss_dppSig,                  6, 3 );

       _gaussPtr->adjustSize();
       _gaussPtr->hide();

       _equilPtr = new QWidget( _selPtr );
         QGridLayout* equilLayoutPtr = new QGridLayout( _equilPtr, 5, 4, 5, 10 );
         equilLayoutPtr->addWidget( new QLabel("Parameter",_equilPtr), 0, 0 );
         equilLayoutPtr->addWidget( new QLabel("Unit"     ,_equilPtr), 0, 1 );
         equilLayoutPtr->addWidget( new QLabel("Hor"      ,_equilPtr), 0, 2 );
         equilLayoutPtr->addWidget( new QLabel("Ver"      ,_equilPtr), 0, 3 );

         double beta_x = 10.0;
         double beta_y = 10.0;
         double alpha_x = 10.0;
         double alpha_y = 10.0;

         if( 0 != _bmlConPtr ) {
           if( _bmlConPtr->isTreatedAsRing() ) {
             const LBSage::Info* lbsPtr
               = _bmlConPtr->getLBFuncPtr( _bmlConPtr->countHowManyDeeply() - 1 );
             beta_x = lbsPtr->beta_1x;
             beta_y = lbsPtr->beta_2y;
             alpha_x = lbsPtr->alpha_1x;
             alpha_y = lbsPtr->alpha_2y;
           }
         }
 
         QString stl;
         stl.setNum( beta_x );
         _equil_xBeta = new QLineEdit( stl, _equilPtr );
         stl.setNum( beta_y );
         _equil_yBeta = new QLineEdit( stl, _equilPtr );
         _equil_xBeta->setMaxLength(7);
         _equil_yBeta->setMaxLength(7);
         equilLayoutPtr->addWidget( new QLabel("Beta" , _equilPtr), 1, 0 );
         equilLayoutPtr->addWidget( new QLabel("[m]"  , _equilPtr), 1, 1 );
         equilLayoutPtr->addWidget( _equil_xBeta                  , 1, 2 );
         equilLayoutPtr->addWidget( _equil_yBeta                  , 1, 3 );

         stl.setNum( alpha_x );
         _equil_xAlpha = new QLineEdit( stl, _equilPtr );
         stl.setNum( alpha_y );
         _equil_yAlpha = new QLineEdit( stl, _equilPtr );
         _equil_xAlpha->setMaxLength(7);
         _equil_yAlpha->setMaxLength(7);
         equilLayoutPtr->addWidget( new QLabel("Alpha" , _equilPtr), 2, 0 );
         equilLayoutPtr->addWidget( new QLabel("   "   , _equilPtr), 2, 1 );
         equilLayoutPtr->addWidget( _equil_xAlpha                  , 2, 2 );
         equilLayoutPtr->addWidget( _equil_yAlpha                  , 2, 3 );

         _equil_xEps = new QLineEdit( "10", _equilPtr );
         _equil_yEps = new QLineEdit( "10", _equilPtr );
         _equil_xEps->setMaxLength(7);
         _equil_yEps->setMaxLength(7);
         equilLayoutPtr->addWidget( new QLabel("Emittance"      , _equilPtr), 3, 0 );
         equilLayoutPtr->addWidget( new QLabel("[pi mm-mrad]"   , _equilPtr), 3, 1 );
         equilLayoutPtr->addWidget( _equil_xEps                             , 3, 2 );
         equilLayoutPtr->addWidget( _equil_yEps                             , 3, 3 );

         QWidget* fill0 = new QWidget(_equilPtr);
         QWidget* fill1 = new QWidget(_equilPtr);
         QWidget* fill2 = new QWidget(_equilPtr);
         QWidget* fill3 = new QWidget(_equilPtr);
         fill0->setSizePolicy( QSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored) );
         fill1->setSizePolicy( QSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored) );
         fill2->setSizePolicy( QSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored) );
         fill3->setSizePolicy( QSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored) );
         equilLayoutPtr->addWidget( fill0, 4, 0 );
         equilLayoutPtr->addWidget( fill1, 4, 1 );
         equilLayoutPtr->addWidget( fill2, 4, 2 );
         equilLayoutPtr->addWidget( fill3, 4, 3 );
   
       _equilPtr->adjustSize();
       _equilPtr->hide();

       _selQglPtr->addWidget( _leftStuff,  0, 0 );
       _selQglPtr->addWidget( _unifPtr, 0, 1 );

     _selPtr->adjustSize();

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
  _qvbPtr->resize( qrePtr->size().width(), qrePtr->size().height() );
}


void DistributionWidget::_genUniform()
{
  // Read the numbers from the interface
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

    // The boost random functors will only accept xMin < xMax,
    // throwing an exception when xMin = xMax. This is unacceptable.
    // To bypass this behavior, I use pointers.

    basUnifGen* xRanPtr   = 0;
    basUnifGen* yRanPtr   = 0;
    basUnifGen* cdtRanPtr = 0;
    basUnifGen* pxRanPtr  = 0;
    basUnifGen* pyRanPtr  = 0;
    basUnifGen* dppRanPtr = 0;

    varUnifGen* xDistPtr   = 0;
    varUnifGen* yDistPtr   = 0;
    varUnifGen* cdtDistPtr = 0;
    varUnifGen* pxDistPtr  = 0;
    varUnifGen* pyDistPtr  = 0;
    varUnifGen* dppDistPtr = 0;

    if( xMin < xMax )
    { xRanPtr  = new basUnifGen( 0.001*xMin,   0.001*xMax );
      xDistPtr = new varUnifGen(generator, *xRanPtr);
    }
    else
    { xMin = 0.001*xMin;
    }
    if( yMin < yMax )
    { yRanPtr  = new basUnifGen( 0.001*yMin,   0.001*yMax );
      yDistPtr = new varUnifGen(generator, *yRanPtr);
    }
    else
    { yMin = 0.001*yMin;
    }
    if( cdtMin < cdtMax )
    { cdtRanPtr  = new basUnifGen( 0.001*cdtMin, 0.001*cdtMax );
      cdtDistPtr = new varUnifGen(generator, *cdtRanPtr);
    }
    else
    { cdtMin = 0.001*cdtMin;
    }
    if( pxMin < pxMax )
    { pxRanPtr  = new basUnifGen( 0.001*pxMin,  0.001*pxMax );
      pxDistPtr = new varUnifGen(generator, *pxRanPtr);
    }
    else
    { pxMin = 0.001*pxMin;
    }
    if( pyMin < pyMax )
    { pyRanPtr  = new basUnifGen( 0.001*pyMin,  0.001*pyMax );
      pyDistPtr = new varUnifGen(generator, *pyRanPtr);
    }
    else
    { pyMin = 0.001*pyMin;
    }
    if( dppMin < dppMax )
    { dppRanPtr  = new basUnifGen( 0.001*dppMin, 0.001*dppMax );
      dppDistPtr = new varUnifGen(generator, *dppRanPtr);
    }
    else
    { dppMin = 0.001*dppMin;
    }

    // Finally, populate the ParticleBunch
    if( n <= 0 ) { n = 16; }

    Particle* pPtr = 0;
    if( DistributionWidget::proton == _flavor ) {
      pPtr = new Proton;
    }
    else if( DistributionWidget::positron == _flavor ) {
      pPtr = new Positron;
    }

    for( int i = 0; i < n; i++ ) {
      if(xDistPtr)   { pPtr->set_x( (*xDistPtr)() );     }
      else           { pPtr->set_x( xMin );              }
      if(yDistPtr)   { pPtr->set_y( (*yDistPtr)() );     }
      else           { pPtr->set_y( yMin );              }
      if(cdtDistPtr) { pPtr->set_cdt( (*cdtDistPtr)() ); }
      else           { pPtr->set_cdt( cdtMin );          }
      if(pxDistPtr)  { pPtr->set_npx( (*pxDistPtr)() );  }
      else           { pPtr->set_npx( pxMin );           }
      if(pyDistPtr)  { pPtr->set_npy( (*pyDistPtr)() );  }
      else           { pPtr->set_npy( pyMin );           }
      if(dppDistPtr) { pPtr->set_ndp( (*dppDistPtr)() ); }
      else           { pPtr->set_ndp( dppMin );          }

      _theSamplePtr->append( *pPtr );
      // Note: the particle is cloned by the ParticleBunch
    }

    ostringstream uic;
    uic << n
        << " uniformly distributed particles have been"
        << "\nadded to the bunch.It now contains "
        << _theSamplePtr->size()
        << " particles.";
    QMessageBox::information( 0, "DistributionWidget", uic.str().c_str() );

    // Clean up before returning.
                      delete pPtr;
    if( xRanPtr   ) { delete xRanPtr;   }
    if( yRanPtr   ) { delete yRanPtr;   }
    if( cdtRanPtr ) { delete cdtRanPtr; }
    if( pxRanPtr  ) { delete pxRanPtr;  }
    if( pyRanPtr  ) { delete pyRanPtr;  }
    if( dppRanPtr ) { delete dppRanPtr; }

    if( xDistPtr   ) { delete xDistPtr;   }
    if( yDistPtr   ) { delete yDistPtr;   }
    if( cdtDistPtr ) { delete cdtDistPtr; }
    if( pxDistPtr  ) { delete pxDistPtr;  }
    if( pyDistPtr  ) { delete pyDistPtr;  }
    if( dppDistPtr ) { delete dppDistPtr; }
  }

  else {
    QMessageBox::warning( this, "Sorry", "Error reading data from widget." );
  }
}


void DistributionWidget::_genGaussian()
{
  // Read the numbers from the interface
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

    basGaussGen xRan( 0.001*xAvg, 0.001*std::abs(xSig) );
    basGaussGen yRan( 0.001*yAvg, 0.001*std::abs(ySig) );
    basGaussGen cdtRan( 0.001*cdtAvg, 0.001*std::abs(cdtSig) );
    basGaussGen pxRan( 0.001*pxAvg, 0.001*std::abs(pxSig) );
    basGaussGen pyRan( 0.001*pyAvg, 0.001*std::abs(pySig) );
    basGaussGen dppRan( 0.001*dppAvg, 0.001*std::abs(dppSig) );

    varGaussGen xDist(generator, xRan);
    varGaussGen yDist(generator, yRan);
    varGaussGen cdtDist(generator, cdtRan);
    varGaussGen pxDist(generator, pxRan);
    varGaussGen pyDist(generator, pyRan);
    varGaussGen dppDist(generator, dppRan);

    if( n <= 0 ) { n = 16; }

    Particle* pPtr = 0;
    if( DistributionWidget::proton == _flavor ) {
      pPtr = new Proton;
    }
    else if( DistributionWidget::positron == _flavor ) {
      pPtr = new Positron;
    }

    for( int i = 0; i < n; i++ ) {
      pPtr->set_x( xDist() );
      pPtr->set_y( yDist() );
      pPtr->set_cdt( cdtDist() );
      pPtr->set_npx( pxDist() );
      pPtr->set_npy( pyDist() );
      pPtr->set_ndp( dppDist() );
      _theSamplePtr->append( *pPtr );
      // The particle is cloned by the ParticleBunch
    }

    delete pPtr;

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
  // Read the numbers from the interface
  bool keepGoing = true;
  bool ok = true;

  double beta_x  = _equil_xBeta->text().toDouble(&ok);  keepGoing &= ok;
  double beta_y  = _equil_yBeta->text().toDouble(&ok);  keepGoing &= ok;
  double alpha_x = _equil_xAlpha->text().toDouble(&ok); keepGoing &= ok;
  double alpha_y = _equil_yAlpha->text().toDouble(&ok); keepGoing &= ok;
  double eps_x   = _equil_xEps->text().toDouble(&ok);   keepGoing &= ok;
  double eps_y   = _equil_yEps->text().toDouble(&ok);   keepGoing &= ok;
  int n = _populationPtr->text().toInt(&ok);            keepGoing &= ok;

  if( keepGoing ) {
    if( beta_x <= 0 || beta_y <= 0 || eps_x <= 0 || eps_y <= 0 )
    { keepGoing = false; }
  }

  if( keepGoing ) {
    // Set up the boost random number generators
    boost::minstd_rand generator(42u);
    // ??? Should be in constructor ???

    basUnifGen psiRan(-M_PI, M_PI);
    basUnifGen x_epsRan(0.0,eps_x);
    basUnifGen y_epsRan(0.0,eps_y);

    varUnifGen psiDist(generator,psiRan);
    varUnifGen x_epsDist(generator,x_epsRan);
    varUnifGen y_epsDist(generator,y_epsRan);

    // Finally, populate the ParticleBunch
    if( n <= 0 ) { n = 16; }

    Particle* pPtr = 0;
    if( DistributionWidget::proton == _flavor ) {
      pPtr = new Proton;
    }
    else if( DistributionWidget::positron == _flavor ) {
      pPtr = new Positron;
    }

    double eps, psi, amp, pos;
    for( int i = 0; i < n; i++ ) {
      psi = psiDist();
      eps = (1.0e-6)*x_epsDist();

      amp = sqrt(beta_x*eps);
      pos = amp*sin(psi);
      pPtr->set_x( pos );
      pPtr->set_npx( (amp*cos(psi) - alpha_x*pos) / beta_x );

      psi = psiDist();
      eps = (1.0e-6)*y_epsDist();

      amp = sqrt(beta_y*eps);
      pos = amp*sin(psi);
      pPtr->set_y( pos );
      pPtr->set_npy( (amp*cos(psi) - alpha_y*pos) / beta_y );

      _theSamplePtr->append( *pPtr );
      // Note: the particle is cloned by the ParticleBunch
    }

    delete pPtr;

    ostringstream uic;
    uic << n
        << " particles have been added to the bunch."
        << "\nIt now contains "
        << _theSamplePtr->size()
        << " particles.";
    QMessageBox::information( 0, "DistributionWidget", uic.str().c_str() );
  }

  else {
    QMessageBox::warning( this, "Sorry", "Error reading data from widget." );
  }
}


void DistributionWidget::sample()
{
  // Note: if( _aPtr->isDown() ) ... etc. .. is not
  // going to work here. Don't know why. 
  // Probably because it violates QButtonBox integrity.

  // Uniform sampling
  if     ( uniform     == _kind ) { _genUniform(); }
  else if( gaussian    == _kind ) { _genGaussian(); }
  else if( equilibrium == _kind ) { _genEquilibrium(); }
  else { QMessageBox::warning( this, "Sorry", "Nothing happened." ); }
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


