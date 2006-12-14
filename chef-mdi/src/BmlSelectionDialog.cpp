/*************************************************************************
**************************************************************************
**************************************************************************
******
******  CHEF:      An application layered on the Beamline/mxyzptlk
******             class libraries.
******
******  File:      BmlSelectionDialog.cpp
******
******  Copyright (c) Universities Research Association, Inc.
******                All Rights Reserved
******
******  Author:    Jean-Francois Ostiguy
******             Fermilab
******             ostiguy@fnal.gov
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
******* URA/FNAL reserves all rights.
*******                                                                
**************************************************************************/
#include <BmlSelectionDialog.h>

#include <basic_toolkit/PhysicsConstants.h>
#include <basic_toolkit/GenericException.h>
#include <bmlfactory/bmlfactory.h>

#include <qlistbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qstring.h>
#include <qvalidator.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <iostream>
#include <cmath>

using namespace std;

BmlSelectionDialog::BmlSelectionDialog(QWidget* parent, const char* name, WFlags f):
  BmlSelectionDialogBase(parent,name,f), _bmllist(0), _mass(0)
{

  beamlines_listBox->clear();

  connect( buttonGroupParticle, SIGNAL( clicked(int) ), this, SLOT( particleCheck(int) ));

  connect( checkBoxMomentum, SIGNAL( toggled(bool) ), this, SLOT( momentumCheck(bool) ));
  connect( checkBoxET,       SIGNAL( toggled(bool) ), this, SLOT( ETCheck(bool)       ));
  connect( checkBoxEK,       SIGNAL( toggled(bool) ), this, SLOT( EKCheck(bool)       ));
  connect( checkBoxGamma,    SIGNAL( toggled(bool) ), this, SLOT( GammaCheck(bool)    ));
  connect( checkBoxBrho,     SIGNAL( toggled(bool) ), this, SLOT( BrhoCheck(bool)     ));

  connect( lineEditMomentum,     SIGNAL( returnPressed() ), this, SLOT( momentumChanged()   ));
  connect( lineEditET,           SIGNAL( returnPressed() ), this, SLOT( ETChanged()         ));
  connect( lineEditEK,           SIGNAL( returnPressed() ), this, SLOT( EKChanged()         ));
  connect( lineEditGamma,        SIGNAL( returnPressed() ), this, SLOT( GammaChanged()      ));
  connect( lineEditBrho,         SIGNAL( returnPressed() ), this, SLOT( BrhoChanged()       ));

  connect( buttonOk,             SIGNAL( clicked() ),       this, SLOT( updateBeamParameters() ));

  QDoubleValidator* v = new QDoubleValidator(this,0) ;
  v->setBottom(0.0);

  lineEditMomentum->setValidator( v );
  lineEditET->setValidator( v );
  lineEditEK->setValidator( v );
  lineEditGamma->setValidator( v );
  lineEditBrho->setValidator( v );

} 
  

void 
BmlSelectionDialog::setList( std::list<std::string>& bmllist, const char* use_name) {

  _bmllist = &bmllist;   
  
  beamlines_listBox->clear();

  
  list<string>::reverse_iterator it;

  QListBoxText* item          = 0;
  QListBoxText* selected_item = 0;

  for ( it = bmllist.rbegin(); it != bmllist.rend(); it++) {
     
     item = new QListBoxText( QString((*it).c_str() ));
     beamlines_listBox->insertItem( item , -1 );

     if ( QString( use_name ).upper() == item->text().upper() ) { // bml names are not case sensitive ! 
       selected_item = item;
     }   
  };
 
  if (selected_item) 
     beamlines_listBox->setSelected ( selected_item, true ); 
  else 
     beamlines_listBox->setSelected ( 0, true );             // the first item is the last defined beamline

  beamlines_textLabel->setText( QString("A total of %1 beamlines are defined in this file.\n Select one or more from the list below.").arg( _bmllist->size() ) );




}


void 
BmlSelectionDialog::setBeamParameters(const bmlfactory& bf) 
{
  // ------------------------------------
  // Determining the mass of the particle.
  // This assumes that a MAD file has already
  // been parsed.                 - lpjm
  // ------------------------------------
  if( 0 == strcmp( "POSITRON", bf.getParticleType() ) ) {
    _mass = PH_NORM_me;
  }
  else if( 0 == strcmp( "PROTON", bf.getParticleType() ) ) {
    _mass = PH_NORM_mp;
  }
  else {
    ostringstream uic;
    uic  << "Unrecognized particle type: "
         << bf.getParticleType()
         << "\nCurrent implementation allows "
            "\nonly proton or positron.";
    throw GenericException( __FILE__, __LINE__,
                            "BmlSelectionDialog::setBeamParameters",
                            uic.str().c_str() );
  }
  // ------------------------------------


  typedef  void (BmlSelectionDialog::* checkfptrtype)(bool);

  bool defined = false;

  struct varstructtype{
    const       char* name;
    int         type;
    QCheckBox*  cb;
  };


  const varstructtype vars[] = { {"MOMENTUM",       MOMENTUM,     checkBoxMomentum}, 
                                 {"TOTAL_ENERGY",   ET,           checkBoxET      },  
                                 {"KINETIC_ENERGY", EK,           checkBoxEK      },  
                                 {"GAMMA",          GAMMA,        checkBoxGamma   }, 
                                 {"BRHRO",          BRHO,         checkBoxBrho    },  
                                 {0,                0,            0               }
                               };

  const varstructtype* vp = &vars[0];

#if 0
  while ( vp->name ) 
  {
    vp->cb->setChecked(false);

    if (bf.variableIsDefined( vp->name ) ) // THIS NEEDS TO GO AWAY !
      {
        computeBeamParameters( bf.getVariableValue(vp->name), vp->type );
        vp->cb->setChecked(true);
        defined = true; 
     }
    
    vp++;
  };
#endif
  
  computeBeamParameters( bf.getBrho(), BRHO );
  vp[1].cb->setChecked(true);
  defined = true; 


  if (!defined) {

    checkBoxMomentum->setChecked(true);
    computeBeamParameters(0.0, MOMENTUM);
  };
     
  
  QString brho_str("0.0");
  QString momentum_str("0.0");
  QString et_str("0.0");
  QString ek_str("0.0");
  QString gamma_str("0.0");

  lineEditBrho->setText( brho_str.setNum(_brho) );
  lineEditMomentum->setText( momentum_str.setNum(_momentum) );
  lineEditET->setText( et_str.setNum(_et) );
  lineEditEK->setText( ek_str.setNum(_ek) );
  lineEditGamma->setText( gamma_str.setNum(_gamma) );

}



void 
BmlSelectionDialog::getSelected( ) {
  
  if (!_bmllist) return;

  list<string>::reverse_iterator it;

  int nbml = beamlines_listBox->count();

  for ( int i = 0; i < nbml; i++) {

    if  ( !beamlines_listBox->isSelected(i) ) {
      _bmllist->remove(  std::string( beamlines_listBox->item(i)->text().ascii() )  );
    }

  };

}


void 
BmlSelectionDialog::computeBeamParameters( double value, int datatype)
{

 switch (datatype) 
 {

 case MOMENTUM:

    _momentum  = value;
    _et        = std::sqrt( (_momentum*_momentum)+(_mass*_mass) );
    _ek        = _et - _mass;
    _gamma     = _et/_mass;
    _brho      = _momentum/PH_CNV_brho_to_p;

    break;

 case ET:
  
   _et        = value;
   _ek        = _et - _mass;
   _gamma     = _et/_mass;
   _momentum  = std::sqrt((_et*_et)-(_mass*_mass));
   _brho      = _momentum/PH_CNV_brho_to_p;

   break;
  
 case EK: 
 
   _ek        = value;
   _et        = _mass + _ek;
   _gamma     = _et/_mass;
   _momentum  = std::sqrt((_et*_et)-(_mass*_mass));
   _brho      = _momentum/PH_CNV_brho_to_p;
  
   break;

 case GAMMA: 
 
   _gamma     = value;
   _et        = _gamma*_mass;
   _ek        = _et-_mass;
   _momentum  = std::sqrt((_et*_et)-(_mass*_mass));
   _brho      = _momentum/PH_CNV_brho_to_p;

   break;
   
 case BRHO:

   _brho      =  value;
   _momentum  =  _brho*PH_CNV_brho_to_p;
   _et        =  std::sqrt( (_momentum*_momentum)+(_mass*_mass) );
   _ek        =  _et - _mass;
   _gamma     =  _et/_mass;

   break;

 }
}


void 
BmlSelectionDialog::momentumCheck(bool set) 
{

  if ( !set ) return;

  lineEditMomentum->setEnabled(true);
  lineEditET->setEnabled(false);
  lineEditEK->setEnabled(false);
  lineEditGamma->setEnabled(false);
  lineEditBrho->setEnabled(false);

}

 
void BmlSelectionDialog::ETCheck(bool set) 
{
  if ( !set ) return;

  lineEditMomentum->setEnabled(false);
  lineEditET->setEnabled(true);
  lineEditEK->setEnabled(false);
  lineEditGamma->setEnabled(false);
  lineEditBrho->setEnabled(false);


}

void 
BmlSelectionDialog::EKCheck(bool set) 
{

  if ( !set  ) return;

  lineEditMomentum->setEnabled(false);
  lineEditET->setEnabled(false);
  lineEditEK->setEnabled(true);
  lineEditGamma->setEnabled(false);
  lineEditBrho->setEnabled(false);

}

void 
BmlSelectionDialog::GammaCheck(bool set) 
{

  if ( !set ) return;

  lineEditMomentum->setEnabled(false);
  lineEditET->setEnabled(false);
  lineEditEK->setEnabled(false);
  lineEditGamma->setEnabled(true);
  lineEditBrho->setEnabled(false);

}

void 
BmlSelectionDialog::BrhoCheck(bool set) 
{

  if ( !set ) return;

  lineEditMomentum->setEnabled(false);
  lineEditET->setEnabled(false);
  lineEditEK->setEnabled(false);
  lineEditGamma->setEnabled(false);
  lineEditBrho->setEnabled(true);

}

void 
BmlSelectionDialog::momentumChanged()
{

  QString s = lineEditMomentum->text();

  double momentum = s.toDouble();
  computeBeamParameters( momentum, MOMENTUM);
  refreshBeamParameters();


}

void 
BmlSelectionDialog::ETChanged()
{

  QString s = lineEditET->text();

  double et = s.toDouble();
  computeBeamParameters( et, ET);
  refreshBeamParameters();

}

void 
BmlSelectionDialog::EKChanged()
{

  QString s = lineEditEK->text();

  double ek = s.toDouble();
  computeBeamParameters( ek, EK);
  refreshBeamParameters();


}

void 
BmlSelectionDialog::GammaChanged()
{

  QString s = lineEditGamma->text();
  double gamma = s.toDouble();
  computeBeamParameters( gamma, GAMMA);
  refreshBeamParameters();

}

void 
BmlSelectionDialog::BrhoChanged()
{

  QString s = lineEditBrho->text();
  double brho = s.toDouble();
  computeBeamParameters( brho, BRHO);
  refreshBeamParameters();

}

void 
BmlSelectionDialog::refreshBeamParameters()
{

  QString brho_str;
  QString momentum_str;
  QString et_str;
  QString ek_str;
  QString gamma_str;
  
  lineEditBrho->setText( brho_str.setNum(_brho) );
  lineEditMomentum->setText( momentum_str.setNum(_momentum) );
  lineEditET->setText( et_str.setNum(_et) );
  lineEditEK->setText( ek_str.setNum(_ek) );
  lineEditGamma->setText( gamma_str.setNum(_gamma) );

  return;

}

void 
BmlSelectionDialog::updateBeamParameters()
{

  QButton* b = buttonGroupReference->selected();

  if (!b) throw GenericException(__FILE__,__LINE__, "BmlSelectionDialog::updateBeamParameters()", 
                                "Reference momentum must be specified.");
  
  QString button_name(b->name());

  if ( button_name ==  QString("checkBoxMomentum"))
  {
    momentumChanged(); 
  } 
  else if ( button_name ==  QString("checkBoxET") )
  {
    ETChanged(); 
  }
  else if ( button_name ==  QString("checkBoxEK") )
  {
    EKChanged(); 
  }
  else if ( button_name ==  QString("checkBoxGamma")) 
  {
    GammaChanged(); 
  }
  else if ( button_name ==  QString("checkBoxBrho") )
  {
    BrhoChanged(); 
  
  }
  else
  {
   throw GenericException(__FILE__,__LINE__, "BmlSelectionDialog::updateBeamParameters()", 
                                "Invalid Reference parameter type.");
  }

  //showPage->();

}


void 
BmlSelectionDialog::_cancel( ){

   _bmllist->clear();
   hide();
}

void 
BmlSelectionDialog::particleCheck(int id) {


  std::cout<< "particle checked: id = " << id << std::endl; 


} 

