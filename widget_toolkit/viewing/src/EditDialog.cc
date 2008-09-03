/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  File:      EditDialog.cc
****** 
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
******                All Rights Reserved                             
******
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S.and Foreign Copyright Laws. 
******
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
****** REVISION HISTORY
****** Mar 2007   ostiguy@fnal.gov
****** - support for reference-counted elements and beamlines 
******
**************************************************************************
*************************************************************************/
/* Creation Date:  July 26, 2005                                        */


#include <EditDialog.h>

#include <qlabel.h>
#include <qdialog.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qradiobutton.h>

#include <iosetup.h>

#include <GenericException.h>
#include <beamline/beamline.h>
#include <beamline/drift.h>
#include <beamline/Slot.h>
#include <beamline/marker.h>
#include <beamline/sextupole.h>
#include <beamline/quadrupole.h>
#include <beamline/monitor.h>
#include <beamline/CF_rbend.h>
#include <beamline/CF_sbend.h>
#include <beamline/sbend.h>
#include <beamline/rbend.h>
#include <beamline/sector.h>
#include <beamline/Alignment.h>

using FNAL::pcerr;
using FNAL::pcout;


using std::ostringstream;
using namespace std;

void EditDialog::visit( bmlnElmnt& x )
{
  QMessageBox::warning( 0, "BeamlineBrowser", 
                        "Sorry. You may not edit this element." );
}


void EditDialog::visit( beamline& x )
{
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );
      QWidget* qwa = new QWidget( qvb );
        QGridLayout* qgl = new QGridLayout( qwa, 1, 2, 5 );
          QString sts( x.Name() );
          QLineEdit* qlePtr = new QLineEdit( sts, qwa );
        qgl->addWidget( new QLabel( QString("Name: "), qwa ), 0, 0 );
        qgl->addWidget( qlePtr, 0, 1 );

      qwa->adjustSize();

      QHBox* qhb = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "Okay", qhb );
          QObject::connect( okayBtn, SIGNAL(pressed()),
                                wpu,    SLOT(accept()) );
        QPushButton* cancelBtn = new QPushButton( "Cancel", qhb );
          cancelBtn->setDefault( true );
          QObject::connect( cancelBtn,  SIGNAL(pressed()),
                                  wpu,     SLOT(reject()) );
      qhb->setMargin(5);
      qhb->setSpacing(3);
      qhb->adjustSize();

    qvb->adjustSize();

  // Note: when reject is activated, wpu and all its subwidgets
  //       will be deleted, if the flag Qt::WDestructiveClose is used.
  //       This is confirmed by changing these from pointers to objects.
  //       A warning message is issued, when exiting this scope, that
  //       the objects are deleted twice.

  wpu->setCaption( QString(x.Type())+QString(": ")+QString(x.Name()) );
  wpu->adjustSize();

  int returnCode = wpu->exec();

  if( returnCode == QDialog::Accepted ) {
    if( sts != qlePtr->text() ) {
      x.rename( (qlePtr->text()).latin1() );
    }
  }

  delete wpu;
}


void EditDialog::visit( rbend& x )
{
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );
      QWidget* qwa = new QWidget( qvb );
         QGridLayout* qgl = new QGridLayout( qwa, 2, 3, 5 );

         qgl->addWidget( new QLabel( QString("Length"), qwa ), 0, 0 );
         qgl->addWidget( new QLabel( QString("[m]"), qwa ), 0, 1 );
           QString stlen;
           stlen.setNum( x.Length() );
           // QLineEdit* qle1 = new QLineEdit( stlen, qwa );
         // qgl->addWidget( qle1, 0, 2 );
         qgl->addWidget( new QLabel( stlen, qwa ), 0, 2 );

         qgl->addWidget( new QLabel( QString("Magnetic field"), qwa ), 1, 0 );
         qgl->addWidget( new QLabel( QString("[T]"), qwa ), 1, 1 );
           QString sts;
           sts.setNum( x.Strength() );
           QLineEdit* qle2 = new QLineEdit( sts, qwa );
         qgl->addWidget( qle2, 1, 2 );

       qwa->adjustSize();

      QHBox* qhb = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "Okay", qhb );
          QObject::connect( okayBtn, SIGNAL(pressed()),
                                wpu,     SLOT(accept()) );
        QPushButton* cancelBtn = new QPushButton( "Cancel", qhb );
          cancelBtn->setDefault( true );
          QObject::connect( cancelBtn,  SIGNAL(pressed()),
                                  wpu,        SLOT(reject()) );
      qhb->setMargin(5);
      qhb->setSpacing(3);
      qhb->adjustSize();

    qvb->adjustSize();

  // Note: when reject is activated, wpu and all its subwidgets
  //       will be deleted, if the flag Qt::WDestructiveClose is used.
  //       This is confirmed by changing these from pointers to objects.
  //       A warning message is issued, when exiting this scope, that
  //       the objects are deleted twice.

  wpu->setCaption( QString(x.Type())+QString(": ")+QString(x.Name()) );
  wpu->adjustSize();

  int returnCode = wpu->exec();

  if( returnCode == QDialog::Accepted ) {
    // if( stlen != qle1->text() ) {
    //   bool ok;
    //   double newLength = (qle1->text()).toDouble( &ok );
    //   if( ok ) {
    //     if( 0 != _contextPtr->setLength( (rbend*) x, newLength ) ) { 
    //       (*pcerr) << "*** WARNING *** File "
    //            << __FILE__ 
    //            << ", Line "
    //            << __LINE__
    //            << ": rbend not found in context."
    //            << std::endl;
    //     }
    //   }
    // }
    if( sts != qle2->text() ) {
      bool ok;
      double newStrength = (qle2->text()).toDouble( &ok );
      if( ok ) {
        x.setStrength(newStrength);
      }
    }
  }

  delete wpu;
}


void EditDialog::visit( quadrupole& x )
{
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );

      QWidget* qwa = new QWidget( qvb );
         QGridLayout* qgl = new QGridLayout( qwa, 3, 3, 5 );

         qgl->addWidget( new QLabel( QString("Gradient"), qwa ), 0, 0 );
         qgl->addWidget( new QLabel( QString(" [T/m]  "), qwa ), 0, 1 );
           QString st1;
           st1.setNum( x.Strength() );
           QLineEdit* qle1 = new QLineEdit( st1, qwa );
         qgl->addWidget( qle1, 0, 2 );

         qgl->addWidget( new QLabel( QString("Length"), qwa ), 1, 0 );
         qgl->addWidget( new QLabel( QString(" [m]  "), qwa ), 1, 1 );
           QString stl;
           stl.setNum( x.Length() );
         qgl->addWidget( new QLabel( QString(stl), qwa ), 1, 2 );

         qgl->addWidget( new QLabel( QString("Roll angle"), qwa ), 2, 0 );
         qgl->addWidget( new QLabel( QString(" [mrad]  "), qwa ), 2, 1 );
           QString st2;
           alignmentData ad(x.Alignment());
           st2.setNum( 1000.*(ad.roll /*[rad]*/) );
           QLineEdit* qle2 = new QLineEdit( st2, qwa );
         qgl->addWidget( qle2, 2, 2 );
      qwa->adjustSize();

      QHBox* qhb = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "Okay", qhb );
          QObject::connect( okayBtn, SIGNAL(pressed()),
                                wpu,     SLOT(accept()) );
        QPushButton* cancelBtn = new QPushButton( "Cancel", qhb );
          cancelBtn->setDefault( true );
          QObject::connect( cancelBtn,  SIGNAL(pressed()),
                                  wpu,     SLOT(reject()) );
      qhb->setMargin(5);
      qhb->setSpacing(3);
      qhb->adjustSize();

    qvb->adjustSize();

  wpu->setCaption( QString(x.Type())+QString(": ")+QString(x.Name()) );
  wpu->adjustSize();

  int returnCode = wpu->exec();
  // Note: when reject is activated, wpu and all its subwidgets
  //       will be deleted, if the flag Qt::WDestructiveClose is used.
  //       This is confirmed by changing these from pointers to objects.
  //       A warning message is issued, when exiting this scope, that
  //       the objects are deleted twice.

  if( returnCode == QDialog::Accepted ) {
    if( st1 != qle1->text() ) {
      bool ok;
      double newStrength = (qle1->text()).toDouble( &ok );
      if( ok ) {
        x.setStrength(newStrength);
     }
    }
    if( st2 != qle2->text() ) {
      bool ok;
      ad.roll /*[rad]*/ = 0.001*((qle2->text()).toDouble( &ok ) /*[mrad]*/);
      if( ok ) {
        x.setAlignment( ad );
      }
    }
  }

  delete wpu;
}


void EditDialog::visit( drift& x )
{
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );

      QWidget* qwa = new QWidget( qvb );
         QGridLayout* qgl = new QGridLayout( qwa, 2, 3, 5 );

         qgl->addWidget( new QLabel( QString("Length"), qwa ), 0, 0 );
         qgl->addWidget( new QLabel( QString(" [m]  "), qwa ), 0, 1 );
           QString stl;
           stl.setNum( x.Length() );
           QLineEdit* qle2 = new QLineEdit( stl, qwa );
         qgl->addWidget( qle2, 0, 2 );

           QRadioButton* qrb = new QRadioButton(qwa);
         qgl->addWidget( qrb, 1, 0 );
         qgl->addMultiCellWidget( new QLabel( QString("Convert to Slot"), qwa )
                                  , 1, 1, 1, 2 );
      qwa->adjustSize();

      QHBox* qhb = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "Okay", qhb );
          QObject::connect( okayBtn, SIGNAL(pressed()),
                                wpu,     SLOT(accept()) );
        QPushButton* cancelBtn = new QPushButton( "Cancel", qhb );
          cancelBtn->setDefault( true );
          QObject::connect( cancelBtn,  SIGNAL(pressed()),
                                  wpu,        SLOT(reject()) );
      qhb->setMargin(5);
      qhb->setSpacing(3);
      qhb->adjustSize();

    qvb->adjustSize();

  wpu->setCaption( QString(x.Type())+QString(": ")+QString(x.Name()) );
  wpu->adjustSize();

  int returnCode = wpu->exec();
  // Note: when reject is activated, wpu and all its subwidgets
  //       will be deleted, if the flag Qt::WDestructiveClose is used.
  //       This is confirmed by changing these from pointers to objects.
  //       A warning message is issued, when exiting this scope, that
  //       the objects are deleted twice.

  if( returnCode == QDialog::Accepted ) {
    if( qrb->isOn() ) {

      bool ok;
      double newLength = (qle2->text()).toDouble( &ok );
      if( !ok ) { newLength = x.Length(); }

      Vector origin(3);
      origin[Particle::cdtIndex]  = newLength; 
      // This is a phenomenally stupid and convoluted
      // way to do this.
      Frame outFrame;
      outFrame.setOrigin( origin );

      
    }
    else {
      // Drift remains a drift
      if( stl != qle2->text() ) {
        bool ok;
        double newLength = (qle2->text()).toDouble( &ok );
        if( ok ) {
          x.setLength(newLength);
        }
      }
    }
  }

  delete wpu;
}


void EditDialog::visit( Slot& x )
{

  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );

      QWidget* qwa = new QWidget( qvb );
         QGridLayout* qgl = new QGridLayout( qwa, 6, 3, 5, 10 );

         QString xstr, ystr, zstr;
         QString lparen("( ");
         QString rparen(" )");
         QString comma(", ");
  
         qgl->addWidget( new QLabel( QString("Upstream"),   qwa ), 0, 1 );
         qgl->addWidget( new QLabel( QString("Downstream"), qwa ), 0, 2 );
         
         qgl->addWidget( new QLabel( QString("Origin   "), qwa ), 1, 0 );
         qgl->addWidget( new QLabel( QString("U   "), qwa ), 2, 0 );
         qgl->addWidget( new QLabel( QString("V   "), qwa ), 3, 0 );
         qgl->addWidget( new QLabel( QString("W   "), qwa ), 4, 0 );

         xstr.setNum( (x.getInFrame().getOrigin())[0] );
         ystr.setNum( (x.getInFrame().getOrigin())[1] );
         zstr.setNum( (x.getInFrame().getOrigin())[2] );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 1, 1 );
         xstr.setNum( (x.getOutFrame().getOrigin())[0] );
         ystr.setNum( (x.getOutFrame().getOrigin())[1] );
         zstr.setNum( (x.getOutFrame().getOrigin())[2] );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 1, 2 );

         xstr.setNum( (x.getInFrame().getxAxis()) [0] );
         ystr.setNum( (x.getInFrame().getxAxis()) [1] );
         zstr.setNum( (x.getInFrame().getxAxis()) [2] );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 2, 1 );
         xstr.setNum( (x.getInFrame().getyAxis()) [0] );
         ystr.setNum( (x.getInFrame().getyAxis()) [1] );
         zstr.setNum( (x.getInFrame().getyAxis()) [2] );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 3, 1 );
         xstr.setNum( (x.getInFrame().getzAxis()) [0] );
         ystr.setNum( (x.getInFrame().getzAxis()) [1] );
         zstr.setNum( (x.getInFrame().getzAxis()) [2] );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 4, 1 );
        
         xstr.setNum( (x.getOutFrame().getxAxis()) [0] );
         ystr.setNum( (x.getOutFrame().getxAxis()) [1] );
         zstr.setNum( (x.getOutFrame().getxAxis()) [2] );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 2, 2 );
         xstr.setNum( (x.getOutFrame().getyAxis())[0] );
         ystr.setNum( (x.getOutFrame().getyAxis())[1] );
         zstr.setNum( (x.getOutFrame().getyAxis())[2] );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 3, 2 );
         xstr.setNum( (x.getOutFrame().getzAxis()) [0] );
         ystr.setNum( (x.getOutFrame().getzAxis()) [1] );
         zstr.setNum( (x.getOutFrame().getzAxis()) [2] );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 4, 2 );

         QRadioButton* qrb = new QRadioButton(qwa);
         qgl->addWidget( qrb, 5, 0 );
         qgl->addMultiCellWidget( new QLabel( QString("Zero out-state"), qwa )
                                              , 5, 5, 1, 2 );
      qwa->adjustSize();
        
      QHBox* qhb = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "Okay", qhb );
          QObject::connect( okayBtn, SIGNAL(pressed()),
                                wpu,     SLOT(accept()) );
        QPushButton* cancelBtn = new QPushButton( "Cancel", qhb );
          cancelBtn->setDefault( true );
          QObject::connect( cancelBtn,  SIGNAL(pressed()),
                                  wpu,        SLOT(reject()) );
      qhb->setMargin(5);
      qhb->setSpacing(3);
      qhb->adjustSize();

    qvb->adjustSize();

  wpu->setCaption( QString(x.Type())+QString(": ")+QString(x.Name()) );
  wpu->adjustSize();

  int returnCode = wpu->exec();
  // Note: when reject is activated, wpu and all its subwidgets
  //       will be deleted, if the flag Qt::WDestructiveClose is used.
  //       This is confirmed by changing these from pointers to objects.
  //       A warning message is issued, when exiting this scope, that
  //       the objects are deleted twice.

  if( returnCode == QDialog::Accepted ) {

#if 0
    if( qrb->isOn() ) {

      Particle* particlePtr = (_contextPtr->getParticle()).Clone();
      // **** FIXME *particlePtr = _contextPtr->getReferenceParticle();

      beamline::const_deep_iterator dbi  = _contextPtr->deep_begin();
      ElmPtr q;
      for ( ; dbi != _contextPtr->deep_end(); ++dbi) {
       q = *dbi;

       if( q.get() == &x ) {   
        break; 
       }
         else  { 
         q->propagate( *particlePtr ); 
       }
      }
      
      if( q.get() == &x ) {
        q->propagate( *particlePtr );
        double offset   = particlePtr->get_x();
        double yawAngle = atan( particlePtr->get_npx()/particlePtr->get_npz() );
        Frame f(x.getOutFrame());
        f.translate( offset*f.getxAxis() );
        f.rotate( yawAngle, f.getyAxis(), false );
        x.setOutFrame(f);
      }
    }
#endif
  }

  delete wpu;
}


void EditDialog::visit( thinQuad& x )
{
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );

      QWidget* qwa = new QWidget( qvb );
         QGridLayout* qgl = new QGridLayout( qwa, 2, 3, 5 );

         qgl->addWidget( new QLabel( QString("Integrated gradient"), qwa ), 0, 0 );
         qgl->addWidget( new QLabel( QString(" [T]  "), qwa ), 0, 1 );
           QString st1;
           st1.setNum( x.Strength() );
           QLineEdit* qle1 = new QLineEdit( st1, qwa );
         qgl->addWidget( qle1, 0, 2 );

         qgl->addWidget( new QLabel( QString("Roll angle"), qwa ), 1, 0 );
         qgl->addWidget( new QLabel( QString(" [mrad]  "), qwa ), 1, 1 );
           QString st2;
           alignmentData ad(x.Alignment());
           st2.setNum( 1000.*(ad.roll /*[rad]*/) );
           QLineEdit* qle2 = new QLineEdit( st2, qwa );
         qgl->addWidget( qle2, 1, 2 );
      qwa->adjustSize();

      QHBox* qhb = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "Okay", qhb );
          QObject::connect( okayBtn, SIGNAL(pressed()),
                                wpu,     SLOT(accept()) );
        QPushButton* cancelBtn = new QPushButton( "Cancel", qhb );
          cancelBtn->setDefault( true );
          QObject::connect( cancelBtn,  SIGNAL(pressed()),
                                  wpu,        SLOT(reject()) );
      qhb->setMargin(5);
      qhb->setSpacing(3);
      qhb->adjustSize();

    qvb->adjustSize();

  wpu->setCaption( QString(x.Type())+QString(": ")+QString(x.Name()) );
  wpu->adjustSize();

  int returnCode = wpu->exec();
  // Note: when reject is activated, wpu and all its subwidgets
  //       will be deleted, if the flag Qt::WDestructiveClose is used.
  //       This is confirmed by changing these from pointers to objects.
  //       A warning message is issued, when exiting this scope, that
  //       the objects are deleted twice.

  if( returnCode == QDialog::Accepted ) {
    if( st1 != qle1->text() ) {
      bool ok;
      double newStrength = (qle1->text()).toDouble( &ok );
      if( ok ) {
        x.setStrength(newStrength);
      }
    }

    if( st2 != qle2->text() ) {
      bool ok;
      ad.roll /*[rad]*/ = 0.001*((qle2->text()).toDouble( &ok ) /*[mrad]*/);
      if( ok ) {
        x.setAlignment( ad );
      }
    }
  }

  delete wpu;
}
