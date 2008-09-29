/**************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      DataBrowser.cc                                    ****** 
******                                                               ******
****** Copyright (c) Fermi Research Alliance LLC                     ******   
******                All Rights Reserved                            ******
******                                                               ******
******  Usage, modification, and redistribution are subject to terms ******
******  of the License supplied with this software.                  ******
******                                                               ******
******  Software and documentation created under                     ******
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359     ******
******  The U.S. Government retains a world-wide non-exclusive,      ******
******  royalty-free license to publish or reproduce documentation   ******
******  and software for U.S. Government purposes. This software     ******
******  is protected under the U.S. and Foreign Copyright Laws.      ******
******                                                               ******
******  Author:   Jean-Francois Ostiguy                              ******
******            ostiguy@fnal.gov                                   ****** 
******            Fermilab                                           ****** 
******                                                               ******
***************************************************************************
***************************************************************************
**************************************************************************/

#include <DataBrowser.h>
#include <qtable.h>
#include <qspinbox.h>
#include <qpopupmenu.h>
#include <qfiledialog.h>
#include <qmenubar.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

#undef emit // Qt3 defines this as a macro ;-(

#include <sqlite/query.hpp>

class DataBrowser::FloatItem: public QTableItem { 

  public:

     FloatItem ( QTable* table, double const& value=0.0 );  
     void setPrecision( unsigned int digits);
     void     setValue( double const& value);

  private:

    double value_;
};


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

DataBrowser::FloatItem::FloatItem( QTable* table, double const& value )
  : QTableItem( table, Never, QString("%1").arg(value) ), value_(value)
{} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

void DataBrowser::FloatItem::setPrecision( unsigned int digits )
{ 
  setText( QString("%1").arg( value_, 10, 'g', digits ) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

void DataBrowser::FloatItem::setValue( double const& value)
{
  setText(  QString("%1").arg(value) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

DataBrowser::DataBrowser( char const* dbname, QWidget* parent, char const* name, WFlags fl )
: DataBrowserBase( parent, name, fl ), db_(dbname)
{
  menubar_ = new QMenuBar( this );

  QPopupMenu *file = new QPopupMenu( this );

  file->insertItem( "&SaveAs",  this, SLOT( saveAs() ), CTRL+Key_S );

  menubar_->insertItem( "&File", file );

  spinBox->setMinValue(2);
  spinBox->setValue(16);
  spinBox->setValue(8);

  connect( spinBox, SIGNAL( valueChanged (int) ), this, SLOT( handleSetPrecision(int) ) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

DataBrowser::~DataBrowser()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

void DataBrowser::init( sqlite::connection& db, std::string const& sql )
{ 
  
  sqlite::query q(db, sql);
  sqlite::result_type  res = q.emit_result();
 
  unsigned int colsize = res->get_column_count(); 

  table_widget->setNumRows( 0 );
  table_widget->setNumCols( colsize);

  if (!res)  return;  //  error ... no data  !

  get_rows( res );

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

void DataBrowser::get_rows( sqlite::result_type res)
{

  if (!res)  return;

  unsigned int colsize = table_widget->numCols();

  col_display_menu_ = new QPopupMenu( this );
  
  connect( col_display_menu_, SIGNAL( activated(int) ), this, SLOT( handleColumnSelection( int ) )); 

  col_display_menu_->setCheckable(true);
  menubar_->insertItem( "&Display", col_display_menu_   );

  for (unsigned int i=0; i<colsize; ++i) {
     table_widget->horizontalHeader()->setLabel( i,  res->get_column_name(i) );
     int id = col_display_menu_->insertItem( res->get_column_name(i), i );
     col_display_menu_->setItemChecked(id, true); 
  }  

  bool table_complete    = false;
  unsigned int row_end   = 0; 

  while ( !table_complete) {

    unsigned int row_start  =  (table_widget-> numRows() == 0 ) ? 0 : table_widget->numRows(); // the index of the old last row 
 
    table_widget->setNumRows(  (row_start == 0) ? 1000 : table_widget->numRows()*2  );
    
    row_end   = table_widget->numRows();              // the index of the new last row 

    for (int row=row_start; row<row_end; ++row ) {
      
         for ( unsigned int col=0; col < table_widget->numCols(); ++col) {
             switch ( res->get_column_type(col) ) {
               case (sqlite::text_t    ) : 
                                        table_widget->setText(row, col, res->get_string(col) );
                                        break;

               case (sqlite::integer_t ) :
	                         	table_widget->setText(row, col, QString("%1").arg( res->get_int(col), 6 ) );
                                        break;

               case ( sqlite::float_t  ) : 
		                        { FloatItem* item = new FloatItem( table_widget, res->get_double(col) ); 
                                          item->setPrecision( spinBox->value() ); 
	                                  table_widget->setItem( row, col, item ); 
                                        }
                                        break;
	       default:                 
                                        break; 
	     }
         }

         if ( !res->next_row() ) { table_complete=true; row_end = row;  break; }

    } 

  }

  table_widget->setNumRows( row_end+1);

  return;

 //table_widget->setText(row, col, QString("%1").arg( res->get_double(col), 10, 'g', spinBox->value() ) );

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

void  DataBrowser::saveAs( ) 
{
  QFileDialog fd( this, "SaveAs", TRUE );
  fd.setMode( QFileDialog::AnyFile );
  QString fileName;
  if ( fd.exec() == QDialog::Accepted ) {
        fileName = fd.selectedFile();
  }

  std::ofstream os( fileName.ascii() );
  os << *this; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

std::ostream& operator<<(  std::ostream& os, DataBrowser const& o)
{
  o.print(os);
  return os; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

void  DataBrowser::print ( std::ostream& os) const
{ 
  using namespace std;
  
  int const text_width   = 16;
  int const   int_width  = 6;
  int const float_width  = 12;

  sqlite::query q(db_, sql_ );
  sqlite::result_type  res = q.emit_result();
  
  if ( !res )  return;
 
  unsigned int colsize = res->get_column_count(); 

  //-----------------------------------------------------------------------------
  // table headings 
  //-----------------------------------------------------------------------------

  for (unsigned int i=0; i<colsize; ++i) {

    switch ( res->get_column_type(i) ) {
    
      case (sqlite::text_t    ) :  os << setw(text_width) 
                                      << res->get_column_name(i);
                                   break;

      case (sqlite::integer_t ) :  os << setw(int_width ) 
                                      << res->get_column_name(i);
                                   break;

      case ( sqlite::float_t  ) :  os << setw(float_width) 
                                      << res->get_column_name(i);
                                   break;

      case ( sqlite::blob_t   ) :  break;
      case ( sqlite::null_t   ) :  break;
      default                   :  break;
    }
  }   
  
  os << std::endl;

  //-----------------------------------------------------------------------------
  // table contents
  //-----------------------------------------------------------------------------

  do {   

    for (unsigned int i=0; i<colsize; ++i) {

      switch ( res->get_column_type(i) ) {
    
        case (sqlite::text_t    ) :  os << setw(text_width) 
                                               << res->get_string(i);
                                     break;

        case (sqlite::integer_t ) :  os << setw(int_width ) 
                                               << res->get_int(i);
                                     break;

        case ( sqlite::float_t  ) :  os << setw(float_width) 
                                               << res->get_double(i);
                                     break;

        case ( sqlite::blob_t   ) :  break;
        case ( sqlite::null_t   ) :  break;
        default                   :  break;
      }
    
    }
    os << std::endl;

  } while (  res->next_row() );
}    

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

void DataBrowser::closeEvent( QCloseEvent *event )
{
  // For some reason, QTable does not support the destructive close flag ... 
  // Therefore, we explicitly call delete on a close event.

    event->accept();
    delete this;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

void DataBrowser::handleColumnSelection( int id )
{
  col_display_menu_->setItemChecked(id, !col_display_menu_->isItemChecked(id) );
  table_widget->isColumnHidden(id) ? table_widget->showColumn(id) : table_widget->hideColumn(id);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

void DataBrowser::handleSetPrecision( int digits )
{
  unsigned int rows = table_widget->numRows();
  unsigned int cols = table_widget->numCols();

  for (unsigned int irow = 0; irow < rows; ++irow ) {
    for (unsigned int icol = 0; icol < cols; ++icol ) {

      FloatItem* fitem = dynamic_cast<FloatItem*>(table_widget->item(irow,icol));
      if (fitem) { 
          fitem->setPrecision(digits); 
          table_widget->updateCell(irow,icol);
      }
    }
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

CS4DDataBrowser::CS4DDataBrowser( char const* dbname, QWidget* parent, char const* name,  WFlags fl )
  : DataBrowser( dbname, parent, name, fl )
{
  setCaption( "Generalized Courant-Snyder Lattice Functions ( Coupled )" );
  sql_ = std::string("") 
       + "SELECT ELEMENTS.iseq, name, type, length, strength, arclength, beta1x, alpha1x, beta1y, alpha1y, psi1,"
       + "beta2x, alpha2x, beta2y, alpha2y, psi2,"
       + "eta_x, etap_x, eta_y, etap_y FROM " 
       + "ELEMENTS, REFERENCE_ORBIT, COURANT_SNYDER_4D, DISPERSION "
       +" WHERE (ELEMENTS.iseq =  COURANT_SNYDER_4D.iseq ) " 
       +"   AND (ELEMENTS.iseq =      REFERENCE_ORBIT.iseq ) "
       +"   AND (ELEMENTS.iseq =        DISPERSION.iseq ) ";

    
  init( db_, sql_ );  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

CS2DDataBrowser::CS2DDataBrowser(  char const* dbname, QWidget* parent, char const* name,  WFlags fl )
  : DataBrowser( dbname, parent, name, fl )
{

  setCaption("Courant-Snyder Lattice Functions ( Uncoupled )" );  
  sql_ = std::string("") +  
       + "SELECT ELEMENTS.iseq, name, type, length, strength, arclength, beta_x, alpha_x, psi_x, eta_x, etap_x, "
       + "beta_y, alpha_y, psi_y  eta_y, etap_y FROM " 
       + "ELEMENTS, REFERENCE_ORBIT, COURANT_SNYDER, DISPERSION "
       +" WHERE (ELEMENTS.iseq =  COURANT_SNYDER.iseq ) " 
       +"   AND (ELEMENTS.iseq =    REFERENCE_ORBIT.iseq ) "
       +"   AND (ELEMENTS.iseq =      DISPERSION.iseq ) ";
  init( db_, sql_ );  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

DispersionDataBrowser::DispersionDataBrowser( char const* dbname, QWidget* parent, char const* name, WFlags fl)
  : DataBrowser( dbname, parent, name, fl)
{

  sql_ = std::string("") 
       + "SELECT ELEMENTS.iseq, name, type, length, strength, arclength, x, xp, y,  yp, eta_x, etap_x "
       + "FROM ELEMENTS, REFERENCE_ORBIT, COURANT_SNYDER, DISPERSION "
       +" WHERE (ELEMENTS.iseq =    REFERENCE_ORBIT.iseq ) "
       +"   AND (ELEMENTS.iseq =      DISPERSION.iseq ) ";

  init( db_, sql_ );  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

CovarianceDataBrowser::CovarianceDataBrowser( char const* dbname, QWidget* parent, char const* name, WFlags fl )
  : DataBrowser( dbname, parent, name, fl)
{

   sql_= std::string("") 
       + "SELECT ELEMENTS.iseq, name, type, length, strength, arclength "
       + "FROM ELEMENTS, REFERENCE_ORBIT, COVARIANCE, DISPERSION "
       +" WHERE (ELEMENTS.iseq =      COVARIANCE.iseq ) " 
       +"   AND (ELEMENTS.iseq =    REFERENCE_ORBIT.iseq ) "
       +"   AND (ELEMENTS.iseq =      DISPERSION.iseq ) ";

  init( db_, sql_ );  
}

