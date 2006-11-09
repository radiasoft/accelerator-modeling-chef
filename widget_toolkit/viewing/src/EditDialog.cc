/**********************************************************************/
/*                                                                    */
/* File:           EditDialog.cc                                      */
/*                                                                    */ 
/* Authors:        Leo Michelotti                                     */
/*                 michelotti@fnal.gov                                */     
/*                                                                    */ 
/* Creation Date:  July 26, 2005                                      */
/*                                                                    */ 
/* Copyright:      (c) URA/Fermilab                                   */
/*                                                                    */
/**********************************************************************/


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
#include <beamline/BeamlineIterator.h>
#include <physics_toolkit/BeamlineContext.h>
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

void editDialog::visitBmlnElmnt( bmlnElmnt* x )
{
  QMessageBox::warning( 0, "BeamlineBrowser", 
                        "Sorry. You may not edit this element." );
}


void editDialog::visitBeamline( beamline* x )
{
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );
      QWidget* qwa = new QWidget( qvb );
        QGridLayout* qgl = new QGridLayout( qwa, 1, 2, 5 );
          QString sts( x->Name() );
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

  wpu->setCaption( QString(x->Type())+QString(": ")+QString(x->Name()) );
  wpu->adjustSize();

  int returnCode = wpu->exec();

  if( returnCode == QDialog::Accepted ) {
    if( sts != qlePtr->text() ) {
      x->rename( (qlePtr->text()).latin1() );
    }
  }

  delete wpu;
}


void editDialog::visitRbend( rbend* x )
{
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );
      QWidget* qwa = new QWidget( qvb );
         QGridLayout* qgl = new QGridLayout( qwa, 2, 3, 5 );

         qgl->addWidget( new QLabel( QString("Length"), qwa ), 0, 0 );
         qgl->addWidget( new QLabel( QString("[m]"), qwa ), 0, 1 );
           QString stlen;
           stlen.setNum( x->Length() );
           // QLineEdit* qle1 = new QLineEdit( stlen, qwa );
         // qgl->addWidget( qle1, 0, 2 );
         qgl->addWidget( new QLabel( stlen, qwa ), 0, 2 );

         qgl->addWidget( new QLabel( QString("Magnetic field"), qwa ), 1, 0 );
         qgl->addWidget( new QLabel( QString("[T]"), qwa ), 1, 1 );
           QString sts;
           sts.setNum( x->Strength() );
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

  wpu->setCaption( QString(x->Type())+QString(": ")+QString(x->Name()) );
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
        if( 0 != _contextPtr->setStrength( (rbend*) x, newStrength ) ) { 
          (*pcerr) << "*** WARNING *** File "
               << __FILE__ 
               << ", Line "
               << __LINE__
               << ": rbend not found in context."
               << std::endl;
        }
      }
    }
  }

  delete wpu;
}


void editDialog::visitQuadrupole( quadrupole* x )
{
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );

      QWidget* qwa = new QWidget( qvb );
         QGridLayout* qgl = new QGridLayout( qwa, 3, 3, 5 );

         qgl->addWidget( new QLabel( QString("Gradient"), qwa ), 0, 0 );
         qgl->addWidget( new QLabel( QString(" [T/m]  "), qwa ), 0, 1 );
           QString st1;
           st1.setNum( x->Strength() );
           QLineEdit* qle1 = new QLineEdit( st1, qwa );
         qgl->addWidget( qle1, 0, 2 );

         qgl->addWidget( new QLabel( QString("Length"), qwa ), 1, 0 );
         qgl->addWidget( new QLabel( QString(" [m]  "), qwa ), 1, 1 );
           QString stl;
           stl.setNum( x->Length() );
         qgl->addWidget( new QLabel( QString(stl), qwa ), 1, 2 );

         qgl->addWidget( new QLabel( QString("Roll angle"), qwa ), 2, 0 );
         qgl->addWidget( new QLabel( QString(" [mrad]  "), qwa ), 2, 1 );
           QString st2;
           alignmentData ad(x->Alignment());
           st2.setNum( 1000.*(ad.tilt /*[rad]*/) );
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

  wpu->setCaption( QString(x->Type())+QString(": ")+QString(x->Name()) );
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
        if( 0 != _contextPtr->setStrength( x, newStrength ) ) {
          ostringstream uic;
          uic  << "*** WARNING *** File "
               << __FILE__ 
               << ", Line "
               << __LINE__
               << ": "
               << x->Type()
               << " not found in context."
               << std::endl;
          QMessageBox::warning( 0, "BeamlineBrowser: ERROR", uic.str().c_str() );
        }
      }
    }

    if( st2 != qle2->text() ) {
      bool ok;
      ad.tilt /*[rad]*/ = 0.001*((qle2->text()).toDouble( &ok ) /*[mrad]*/);
      if( ok ) {
        if( 0 != _contextPtr->setAlignment( x, ad ) ) {
          ostringstream uic;
          uic  << "*** WARNING *** File "
               << __FILE__ 
               << ", Line "
               << __LINE__
               << ": "
               << x->Type()
               << " not found in context."
               << std::endl;
          QMessageBox::warning( 0, "BeamlineBrowser: ERROR", uic.str().c_str() );
        }
      }
    }

  }

  delete wpu;
}


void editDialog::visitDrift( drift* x )
{
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );

      QWidget* qwa = new QWidget( qvb );
         QGridLayout* qgl = new QGridLayout( qwa, 2, 3, 5 );

         qgl->addWidget( new QLabel( QString("Length"), qwa ), 0, 0 );
         qgl->addWidget( new QLabel( QString(" [m]  "), qwa ), 0, 1 );
           QString stl;
           stl.setNum( x->Length() );
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

  wpu->setCaption( QString(x->Type())+QString(": ")+QString(x->Name()) );
  wpu->adjustSize();

  int returnCode = wpu->exec();
  // Note: when reject is activated, wpu and all its subwidgets
  //       will be deleted, if the flag Qt::WDestructiveClose is used.
  //       This is confirmed by changing these from pointers to objects.
  //       A warning message is issued, when exiting this scope, that
  //       the objects are deleted twice.

  if( returnCode == QDialog::Accepted ) {
    if( qrb->isOn() ) {
      #if 0
      ostringstream uic;
      uic  << "*** WARNING *** File "
           << __FILE__ 
           << ", Line "
           << __LINE__
           << "\n*** WARNING *** This part routine needs to be rewritten. "
           << std::endl;
      QMessageBox::warning( 0, "BeamlineBrowser: WARNING", uic.str().c_str() );
      #endif

      #if 1
      // Drift will be converted to Slot
      ostringstream uic;
      uic  << "*** WARNING *** File "
           << __FILE__ 
           << ", Line "
           << __LINE__
           << "\n*** WARNING *** Am deleting "
           << x->Type()
           << " element "
           << x->Name()
           << "\n*** WARNING *** If the program aborts soon, this is"
              "\n*** WARNING *** probably the reason."
           << std::endl;
      QMessageBox::warning( 0, "BeamlineBrowser: WARNING", uic.str().c_str() );

      bool ok;
      double newLength = (qle2->text()).toDouble( &ok );
      if( !ok ) { newLength = x->Length(); }

      Vector origin(3);
      origin( Particle::_cdt() ) = newLength; 
      // This is a phenomenally stupid and convoluted
      // way to do this.
      Frame outFrame;
      outFrame.setOrigin( origin );
      Slot* slotPtr = new Slot( x->Name(), outFrame );
      _contextPtr->replaceElement( x, slotPtr );

      // START HERE. The Browser should not be in the business of editing
      // lines. This should be done within CHEF itself. How???

      // WARNING: This routine will delete x.
      // WARNING: Best of luck!
      #endif
    }

    else {
      // Drift remains a drift
      if( stl != qle2->text() ) {
        bool ok;
        double newLength = (qle2->text()).toDouble( &ok );
        if( ok ) {
          if( 0 != _contextPtr->setLength( x, newLength ) ) {
            ostringstream uic;
            uic  << "*** WARNING *** File "
                 << __FILE__ 
                 << ", Line "
                 << __LINE__
                 << ": "
                 << x->Type()
                 << " not found in context."
                 << std::endl;
            QMessageBox::warning( 0, "BeamlineBrowser: ERROR", uic.str().c_str() );
          }
        }
      }
    }
  }

  delete wpu;
}


void editDialog::visitSlot( Slot* x )
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

         xstr.setNum( (x->getInFrame().getOrigin())(0) );
         ystr.setNum( (x->getInFrame().getOrigin())(1) );
         zstr.setNum( (x->getInFrame().getOrigin())(2) );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 1, 1 );
         xstr.setNum( (x->getOutFrame().getOrigin())(0) );
         ystr.setNum( (x->getOutFrame().getOrigin())(1) );
         zstr.setNum( (x->getOutFrame().getOrigin())(2) );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 1, 2 );

         xstr.setNum( (x->getInFrame().getxAxis()) (0) );
         ystr.setNum( (x->getInFrame().getxAxis()) (1) );
         zstr.setNum( (x->getInFrame().getxAxis()) (2) );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 2, 1 );
         xstr.setNum( (x->getInFrame().getyAxis()) (0) );
         ystr.setNum( (x->getInFrame().getyAxis()) (1) );
         zstr.setNum( (x->getInFrame().getyAxis()) (2) );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 3, 1 );
         xstr.setNum( (x->getInFrame().getzAxis()) (0) );
         ystr.setNum( (x->getInFrame().getzAxis()) (1) );
         zstr.setNum( (x->getInFrame().getzAxis()) (2) );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 4, 1 );
        
         xstr.setNum( (x->getOutFrame().getxAxis()) (0) );
         ystr.setNum( (x->getOutFrame().getxAxis()) (1) );
         zstr.setNum( (x->getOutFrame().getxAxis()) (2) );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 2, 2 );
         xstr.setNum( (x->getOutFrame().getyAxis()) (0) );
         ystr.setNum( (x->getOutFrame().getyAxis()) (1) );
         zstr.setNum( (x->getOutFrame().getyAxis()) (2) );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 3, 2 );
         xstr.setNum( (x->getOutFrame().getzAxis()) (0) );
         ystr.setNum( (x->getOutFrame().getzAxis()) (1) );
         zstr.setNum( (x->getOutFrame().getzAxis()) (2) );
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

  wpu->setCaption( QString(x->Type())+QString(": ")+QString(x->Name()) );
  wpu->adjustSize();

  int returnCode = wpu->exec();
  // Note: when reject is activated, wpu and all its subwidgets
  //       will be deleted, if the flag Qt::WDestructiveClose is used.
  //       This is confirmed by changing these from pointers to objects.
  //       A warning message is issued, when exiting this scope, that
  //       the objects are deleted twice.

  if( returnCode == QDialog::Accepted ) {
    if( qrb->isOn() ) {
      if( !(_contextPtr->hasReferenceParticle()) ) {
        delete wpu;
  	QMessageBox::critical( 
  	        0, 
  	        "CHEF ERROR",
  	        "Reference particle must be established first."
  	        );
  	throw( GenericException( __FILE__, __LINE__, 
  	       "void editDialog::visitSlot( Slot* x )", 
  	       "Reference particle not established." ) );
      }


      Particle* particlePtr = (_contextPtr->getParticle()).Clone();
      _contextPtr->getReferenceParticle( *particlePtr );
      DeepBeamlineIterator dbi( const_cast<beamline&>(*_contextPtr->cheatBmlPtr()) );
      bmlnElmnt* q;
      while((  q = dbi++  )) {
        if( q == x ) { break; }
        else         { q->propagate( *particlePtr ); }
      }
      if( q == x ) {
        q->propagate( *particlePtr );
        double offset   = particlePtr->get_x();
        double yawAngle = atan( particlePtr->get_npx()/particlePtr->get_npz() );
        Frame f(x->getOutFrame());
        f.translate( offset*f.getxAxis() );
        f.rotate( yawAngle, f.getyAxis(), false );
        x->setOutFrame(f);
      }
    }
  }

  delete wpu;
}


void editDialog::visitThinQuad( thinQuad* x )
{
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );

      QWidget* qwa = new QWidget( qvb );
         QGridLayout* qgl = new QGridLayout( qwa, 2, 3, 5 );

         qgl->addWidget( new QLabel( QString("Integrated gradient"), qwa ), 0, 0 );
         qgl->addWidget( new QLabel( QString(" [T]  "), qwa ), 0, 1 );
           QString st1;
           st1.setNum( x->Strength() );
           QLineEdit* qle1 = new QLineEdit( st1, qwa );
         qgl->addWidget( qle1, 0, 2 );

         qgl->addWidget( new QLabel( QString("Roll angle"), qwa ), 1, 0 );
         qgl->addWidget( new QLabel( QString(" [mrad]  "), qwa ), 1, 1 );
           QString st2;
           alignmentData ad(x->Alignment());
           st2.setNum( 1000.*(ad.tilt /*[rad]*/) );
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

  wpu->setCaption( QString(x->Type())+QString(": ")+QString(x->Name()) );
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
        if( 0 != _contextPtr->setStrength( x, newStrength ) ) {
          (*pcerr) << "*** WARNING *** File "
               << __FILE__ 
               << ", Line "
               << __LINE__
               << ": thinQuad not found in context."
               << std::endl;
        }
      }
    }

    if( st2 != qle2->text() ) {
      bool ok;
      ad.tilt /*[rad]*/ = 0.001*((qle2->text()).toDouble( &ok ) /*[mrad]*/);
      if( ok ) {
        if( 0 != _contextPtr->setAlignment( x, ad ) ) {
          (*pcerr) << "*** WARNING *** File "
               << __FILE__ 
               << ", Line "
               << __LINE__
               << ": thinQuad not found in context."
               << std::endl;
        }
      }
    }

  }

  delete wpu;
}
